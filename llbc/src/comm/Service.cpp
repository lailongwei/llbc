// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), to deal in 
// the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
// the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/Packet.h"
#include "llbc/comm/PollerType.h"
#include "llbc/comm/protocol/IProtocol.h"
#include "llbc/comm/protocol/IProtocolFilter.h"
#include "llbc/comm/protocol/ProtocolStack.h"
#include "llbc/comm/protocol/RawProtocolFactory.h"
#include "llbc/comm/protocol/NormalProtocolFactory.h"
#include "llbc/comm/Service.h"
#include "llbc/comm/ServiceMgr.h"

namespace
{
    typedef LLBC_NS LLBC_Service This;
    typedef LLBC_NS LLBC_ProtocolStack _Stack;
}

__LLBC_INTERNAL_NS_BEGIN

static void __DeletePacket(void *data)
{
    LLBC_Delete(reinterpret_cast<LLBC_NS LLBC_Packet *>(data));
}

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

int LLBC_Service::_maxId = 1;

LLBC_Service::_EvHandler LLBC_Service::_evHandlers[LLBC_SvcEvType::End] = 
{
    &LLBC_Service::HandleEv_SessionCreate,
    &LLBC_Service::HandleEv_SessionDestroy,
    &LLBC_Service::HandleEv_AsyncConnResult,
    &LLBC_Service::HandleEv_DataArrival,
    &LLBC_Service::HandleEv_ProtoReport,

    &LLBC_Service::HandleEv_SubscribeEv,
    &LLBC_Service::HandleEv_UnsubscribeEv,
    &LLBC_Service::HandleEv_FireEv
};

// VS2005 and later version compiler support initialize array in construct list.
// In here, we disable C4351 warning to initialize it.
#if LLBC_CUR_COMP == LLBC_COMP_MSVC && LLBC_COMP_VER >= 1400
# pragma warning(disable:4351)
#endif

LLBC_Service::LLBC_Service(This::Type type, const LLBC_String &name, LLBC_IProtocolFactory *protoFactory)
: _id(LLBC_AtomicFetchAndAdd(&_maxId, 1))
, _type(type)
, _name(name.c_str(), name.length())
, _protoFactory(protoFactory)
, _sessionProtoFactory()
, _driveMode(This::SelfDrive)
, _suppressedCoderNotFoundWarning(false)

, _started(false)
, _stopping(false)
, _initingFacade(false)

, _lock()
, _protoLock()

, _fps(LLBC_CFG_COMM_DFT_SERVICE_FPS)
, _frameInterval(1000 / LLBC_CFG_COMM_DFT_SERVICE_FPS)
, _relaxTimes(0)
, _begHeartbeatTime(0)
, _sinkIntoLoop(false)
, _afterStop(false)

, _pollerMgr()
, _connectedSessionIds()
, _connectedSessionIdsLock()
#if !LLBC_CFG_COMM_USE_FULL_STACK
, _stack(LLBC_ProtocolStack::CodecStack)
#endif

, _facades()
, _facades2()
, _coders()
, _handlers()
, _preHandlers()
#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
, _unifyPreHandler(NULL)
#endif
#if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
, _statusHandlers()
#endif
#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
, _statusDescs()
#endif

#if LLBC_CUR_COMP == LLBC_COMP_MSVC && LLBC_COMP_VER >= 1400
, _filters()
#endif

, _beforeFrameTasks()
, _afterFrameTasks()

, _handlingBeforeFrameTasks(false)
, _handledBeforeFrameTasks(false)
, _handlingAfterFrameTasks(false)

#if LLBC_CFG_OBJBASE_ENABLED
, _releasePoolStack(NULL)
#endif // LLBC_CFG_OBJBASE_ENABLED

, _timerScheduler(NULL)

, _evManager()

, _svcMgr(*LLBC_ServiceMgrSingleton)
{
    // Create service name, if is empty.
    if (_name.empty())
        _name.format("S%d-%s", _id, LLBC_GUIDHelper::GenStr().c_str());

    // Create protocol stack, if type is not Custom.
    if (_type != This::Custom)
    {
        LLBC_XDelete(_protoFactory);
        if (_type == This::Raw)
            _protoFactory = LLBC_New(LLBC_RawProtocolFactory);
        else
            _protoFactory = LLBC_New(LLBC_NormalProtocolFactory);
    }
    else
    {
        ASSERT(_protoFactory != NULL && "Service type is Custom, but not pass Protocol Factory to Service!");
    }

    // Get the poller type from Config.h.
    const char *pollerModel = LLBC_CFG_COMM_POLLER_MODEL;
    const int pollerType = LLBC_PollerType::Str2Type(pollerModel);
    ASSERT(LLBC_PollerType::IsValid(pollerType) && "Invalid LLBC_CFG_COMM_POLLER_MODEL config!");

    _pollerMgr.SetService(this);
    _pollerMgr.SetPollerType(pollerType);

    // Force cleanup _filters array again.
    LLBC_MemSet(_filters, 0, sizeof(_filters));

    // Create protocol stack.
#if !LLBC_CFG_COMM_USE_FULL_STACK
    _stack.SetService(this);
    CreateCodecStack(0, 0, &_stack);
#endif
}

// If using MSVC compiler and version >= 1400(VS2005), Reset C4351 warning to default.
#if LLBC_CUR_COMP == LLBC_COMP_MSVC && LLBC_COMP_VER >= 1400
# pragma warning(default:4351)
#endif

LLBC_Service::~LLBC_Service()
{
    Stop();

    DestroyFacades();
    LLBC_STLHelper::DeleteContainer(_coders);
    LLBC_STLHelper::DeleteContainer(_handlers);
    LLBC_STLHelper::DeleteContainer(_preHandlers);
#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    LLBC_XDelete(_unifyPreHandler);
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

#if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
    for (_OpStatusHandlers::iterator it = _statusHandlers.begin();
         it != _statusHandlers.end();
         it++)
    {
        LLBC_STLHelper::DeleteContainer(*it->second);
        LLBC_Delete(it->second);
    }
#endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER

    for (int layer = LLBC_ProtocolLayer::Begin;
         layer != LLBC_ProtocolLayer::End;
         layer++)
        LLBC_XDelete(_filters[layer]);

    _handledBeforeFrameTasks = false;
    DestroyFrameTasks(_beforeFrameTasks, _handlingBeforeFrameTasks);
    DestroyFrameTasks(_afterFrameTasks, _handlingAfterFrameTasks);

    LLBC_STLHelper::DeleteContainer(_sessionProtoFactory);
    LLBC_XDelete(_protoFactory);
}

int LLBC_Service::GetId() const
{
    return _id;
}

This::Type LLBC_Service::GetType() const
{
    return _type;
}

const LLBC_String &LLBC_Service::GetName() const
{
    return _name;
}

This::DriveMode LLBC_Service::GetDriveMode() const
{
    return _driveMode;
}

int LLBC_Service::SetDriveMode(This::DriveMode mode)
{
    if (mode != This::SelfDrive && mode != This::ExternalDrive)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }
    else if (_started)
    {
        LLBC_SetLastError(LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);
    if (_started)
    {
        LLBC_SetLastError(LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }

    if ((_driveMode = mode) == This::ExternalDrive)
        GetTimerScheduler();

    return LLBC_OK;
}

int LLBC_Service::SuppressCoderNotFoundWarning()
{
    LLBC_LockGuard guard(_lock);
    if (_started)
    {
        LLBC_SetLastError(LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }

    _suppressedCoderNotFoundWarning = true;
#if !LLBC_CFG_COMM_USE_FULL_STACK
    _stack.SetIsSuppressedCoderNotFoundWarning(true);
#endif
    return LLBC_OK;
}

int LLBC_Service::Start(int pollerCount)
{
    if (pollerCount <= 0)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }
    else if (_svcMgr.GetService(_id) ||
        _svcMgr.GetService(_name))
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }
    else if (_started)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);
    if (_started)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    if (_pollerMgr.Start(pollerCount) != LLBC_OK)
        return LLBC_FAILED;

    if (_driveMode == This::ExternalDrive)
    {
        if (!IsCanContinueDriveService())
        {
            LLBC_SetLastError(LLBC_ERROR_LIMIT);
            _pollerMgr.Stop();

            return LLBC_FAILED;
        }

        AddServiceToTls();
    }
    else
    {
        if (Activate(1) != LLBC_OK)
        {
            _pollerMgr.Stop();
            return LLBC_FAILED;
        }
    }

    _started = true;

    if (_driveMode == This::ExternalDrive)
    {
        InitFacades();
        StartFacades();
    }

    return LLBC_OK;
}

bool LLBC_Service::IsStarted() const
{
    return _started;
}

void LLBC_Service::Stop()
{
	_lock.Lock();
	if (!_started || _stopping)
	{
		_lock.Unlock();
		return;
	}

    _stopping = true;
	_lock.Unlock();

    if (_driveMode == This::SelfDrive) // Stop self-drive service.
    {
        // TODO: How to stop sink into loop service???
        // if (_sinkIntoLoop) // Service sink into loop, direct return.
        //     return;

        while (_started) // Service not sink into loop, wait service stop(LLBC_Task mechanism will ensure Cleanup method called).
            LLBC_ThreadManager::Sleep(20);
    }
    else // Stop external-drive service.
    {
        if (_sinkIntoLoop) // Service sink into loop, set afterStop flag to true, and return.
        {
            _afterStop = true; //! After stop data member only use external-drive service.
            return;
        }

        // Service not sink into loop, direct cleanup.
        Cleanup();
    }
}

int LLBC_Service::GetFPS() const
{
    This *ncThis = const_cast<This *>(this);
    LLBC_LockGuard guard(ncThis->_lock);
    return _fps;
}

int LLBC_Service::SetFPS(int fps)
{
    if (fps != LLBC_INFINITE &&
            (fps < LLBC_CFG_COMM_MIN_SERVICE_FPS || fps > LLBC_CFG_COMM_MAX_SERVICE_FPS))
    {
        LLBC_SetLastError(LLBC_ERROR_LIMIT);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);

    _fps = fps;
    if (_fps != LLBC_INFINITE)
    {
        _frameInterval = 1000 / _fps;
    }
    else
    {
        _relaxTimes = 0;
        _frameInterval = 0;
    }

    return LLBC_OK;
}

int LLBC_Service::GetFrameInterval() const
{
    This *ncThis = const_cast<This *>(this);

    LLBC_LockGuard guard(ncThis->_lock);
    return _frameInterval;
}

int LLBC_Service::Listen(const char *ip, uint16 port, LLBC_IProtocolFactory *protoFactory)
{
    LLBC_LockGuard guard(_lock);
    const int sessionId = _pollerMgr.Listen(ip, port);
    if (sessionId != 0)
    {
        _connectedSessionIdsLock.Lock();
        _connectedSessionIds.insert(sessionId);
        _connectedSessionIdsLock.Unlock();

        if (protoFactory)
            AddSessionProtocolFactory(sessionId, protoFactory);
    }

    return sessionId;
}

int LLBC_Service::Connect(const char *ip, uint16 port, double timeout, LLBC_IProtocolFactory *protoFactory)
{
    LLBC_LockGuard guard(_lock);
    const int sessionId = _pollerMgr.Connect(ip, port);
    if (sessionId != 0)
    {
        _connectedSessionIdsLock.Lock();
        _connectedSessionIds.insert(sessionId);
        _connectedSessionIdsLock.Unlock();

        if (protoFactory)
            AddSessionProtocolFactory(sessionId, protoFactory);
    }

    return sessionId;
}

int LLBC_Service::AsyncConn(const char *ip, uint16 port, double timeout, LLBC_IProtocolFactory *protoFactory)
{
    LLBC_LockGuard guard(_lock);

    int pendingSessionId;
    const int ret = _pollerMgr.AsyncConn(ip, port, pendingSessionId);
    if (ret != LLBC_OK)
        return ret;

    if (protoFactory)
        AddSessionProtocolFactory(pendingSessionId, protoFactory);

    return ret;
}

bool LLBC_Service::IsSessionValidate(int sessionId)
{
    if (UNLIKELY(sessionId == 0))
        return false;

    _connectedSessionIdsLock.Lock();
    const bool valid = 
        _connectedSessionIds.find(sessionId) != _connectedSessionIds.end();
    _connectedSessionIdsLock.Unlock();

    return valid;
}

int LLBC_Service::Send(LLBC_Packet *packet)
{
    // Call internal Lockable() to complete.
    // lock = true
    // validCheck = true
    return LockableSend(packet);
}

int LLBC_Service::Multicast(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, LLBC_ICoder *coder, int status)
{
    // Call internal MulticastSendCoder<> template method to complete.
    // validCheck = true
    return MulticastSendCoder<LLBC_SessionIdList>(svcId, sessionIds, opcode, coder, status);
}

int LLBC_Service::Multicast(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, const void *bytes, size_t len, int status)
{
    LLBC_LockGuard guard(_lock);

    // Foreach to call internal method LockableSend() method to complete.
    // lock = false
    // validCheck = true
    for (LLBC_SessionIdListCIter sessionIt = sessionIds.begin();
         sessionIt != sessionIds.end();
         sessionIt++)
        LockableSend(svcId, *sessionIt, opcode, bytes, len, status, false);

    return LLBC_OK;
}

int LLBC_Service::Broadcast(int svcId, int opcode, LLBC_ICoder *coder, int status)
{
    // Copy all connected session Ids.
    int idx = 0;

    _connectedSessionIdsLock.Lock();
    LLBC_SessionIdList connectedSessionIds(_connectedSessionIds.size());
    for (LLBC_SessionIdSetCIter sessionIdIt = _connectedSessionIds.begin();
         sessionIdIt != _connectedSessionIds.end();
         idx++, sessionIdIt++)
         connectedSessionIds[idx] = *sessionIdIt;

    _connectedSessionIdsLock.Unlock();

    // Call internal template method MulticastSendCoder<>() to complete.
    // validCheck = false
    return MulticastSendCoder<LLBC_SessionIdList>(svcId, connectedSessionIds, opcode, coder, status, false);
}

int LLBC_Service::Broadcast(int svcId, int opcode, const void *bytes, size_t len , int status)
{
    LLBC_LockGuard guard(_lock);

    // Foreach to call internal method LockableSend() method to complete.
    // lock = false
    // validCheck = false
    LLBC_LockGuard connSIdsGuard(_connectedSessionIdsLock);
    for (LLBC_SessionIdSetCIter sessionIt = _connectedSessionIds.begin();
         sessionIt != _connectedSessionIds.end();
         sessionIt++)
        LockableSend(svcId, *sessionIt, opcode, bytes, len, status, false, false);

    return LLBC_OK;
}

int LLBC_Service::RemoveSession(int sessionId, const char *reason)
{
    LLBC_LockGuard guard(_lock);
    if (!_started)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    LLBC_LockGuard connSIdsGuard(_connectedSessionIdsLock);

    LLBC_SessionIdSetIter sessionIdIt = _connectedSessionIds.find(sessionId);
    if (sessionIdIt == _connectedSessionIds.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    _pollerMgr.Close(sessionId, reason);
    _connectedSessionIds.erase(sessionIdIt);

    return LLBC_OK;
}

LLBC_IFacade *LLBC_Service::RegisterFacade(LLBC_IFacadeFactory *facadeFactory)
{
    if (UNLIKELY(!facadeFactory))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return NULL;
    }

    LLBC_IFacade *facade = facadeFactory->Create();
    LLBC_Delete(facadeFactory);

    int ret = RegisterFacade(facade);
    if (UNLIKELY(ret != LLBC_OK))
    {
        LLBC_Delete(facade);
        return NULL;
    }

    return facade;
}

int LLBC_Service::RegisterFacade(LLBC_IFacade *facade)
{
    if (UNLIKELY(!facade))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_started))
    {
        LLBC_SetLastError(LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }
    else if (std::find(_facades.begin(), 
            _facades.end(), facade) != _facades.end())
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    facade->SetService(this);
    _facades.push_back(facade);

    const LLBC_String facadeName = LLBC_GetTypeName(*facade);
    _Facades2::iterator it = _facades2.find(facadeName);
    if (it == _facades2.end())
        it = _facades2.insert(std::make_pair(facadeName, _Facades())).first;
    it->second.push_back(facade);

    return LLBC_OK;
}

LLBC_IFacade *LLBC_Service::GetFacade(const LLBC_String &facadeName)
{
    LLBC_LockGuard guard(_lock);

    _Facades2::iterator it = _facades2.find(facadeName);
    if (it == _facades2.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return NULL;
    }

    _Facades &facades = it->second;
    return facades[0];
}

std::vector<LLBC_IFacade *> LLBC_Service::GetFacades(const LLBC_String &facadeName)
{
    static const std::vector<LLBC_IFacade *> emptyFacades;

    LLBC_LockGuard guard(_lock);

    _Facades2::iterator it = _facades2.find(facadeName);
    if (it == _facades2.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return emptyFacades;
    }

    return it->second;
}

int LLBC_Service::RegisterCoder(int opcode, LLBC_ICoderFactory *coderFactory)
{
    if (UNLIKELY(!coderFactory))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_started && !_initingFacade))
    {
        LLBC_SetLastError(LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }
    else if (!_coders.insert(std::make_pair(opcode, coderFactory)).second)
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
int LLBC_Service::RegisterStatusDesc(int status, const LLBC_String &desc)
{
    if (status == 0 || desc.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_started && !_initingFacade))
    {
        LLBC_SetLastError(LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }

    if (!_statusDescs.insert(std::make_pair(
        status, LLBC_String(desc.data(), desc.size()))).second)
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}
#endif // LLBC_CFG_COMM_ENABLE_STATUS_DESC

int LLBC_Service::Subscribe(int opcode, LLBC_IDelegate1<void, LLBC_Packet &> *deleg)
{
    if (UNLIKELY(!deleg))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_started && !_initingFacade))
    {
        LLBC_SetLastError(LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }
    else if (_type == This::Raw && opcode != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }
    else if (!_handlers.insert(std::make_pair(opcode, deleg)).second)
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_Service::PreSubscribe(int opcode, LLBC_IDelegate1<bool, LLBC_Packet &> *deleg)
{
    if (UNLIKELY(!deleg))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_started && !_initingFacade))
    {
        LLBC_SetLastError(LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }
    else if (_type == This::Raw && opcode != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }
    else if (!_preHandlers.insert(std::make_pair(opcode, deleg)).second)
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
int LLBC_Service::UnifyPreSubscribe(LLBC_IDelegate1<bool, LLBC_Packet &> *deleg)
{
    if (UNLIKELY(!deleg))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_started && !_initingFacade))
    {
        LLBC_SetLastError(LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }
    else if (_unifyPreHandler)
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    _unifyPreHandler = deleg;

    return LLBC_OK;
}
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

#if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
int LLBC_Service::SubscribeStatus(int opcode, int status, LLBC_IDelegate1<void, LLBC_Packet &> *deleg)
{
    if (UNLIKELY(!deleg))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_started && !_initingFacade))
    {
        LLBC_SetLastError(LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }
    else if (_type == This::Raw)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }
    else if (status == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    _OpStatusHandlers::iterator it = _statusHandlers.find(opcode);
    if (it == _statusHandlers.end())
        it = _statusHandlers.insert(std::make_pair(opcode, LLBC_New(_StatusHandlers))).first;
    
    _StatusHandlers &stHandlers = *it->second;
    if (!stHandlers.insert(std::make_pair(status, deleg)).second)
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}
#endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER

int LLBC_Service::SetProtocolFilter(LLBC_IProtocolFilter *filter, int toLayer)
{
    if (UNLIKELY(!filter) || !LLBC_ProtocolLayer::IsValid(toLayer))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_started && !_initingFacade))
    {
        LLBC_SetLastError(LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }
    else if (_filters[toLayer])
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    _filters[toLayer] = filter;
#if !LLBC_CFG_COMM_USE_FULL_STACK
    _stack.SetFilter(filter, toLayer);
#endif

    return LLBC_OK;
}

int LLBC_Service::EnableTimerScheduler()
{
    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_started && !_initingFacade))
    {
        LLBC_SetLastError(LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }
    else if (_driveMode != ExternalDrive)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);
        return LLBC_FAILED;
    }

    _timerScheduler->SetEnabled(true);
    return LLBC_OK;
}

int LLBC_Service::DisableTimerScheduler()
{
    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_started && !_initingFacade))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }
    else if (_driveMode != ExternalDrive)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);
        return LLBC_FAILED;
    }

    _timerScheduler->SetEnabled(false);
    return LLBC_OK;
}

LLBC_ListenerStub LLBC_Service::SubscribeEvent(int event, LLBC_IDelegate1<void, LLBC_Event *> *deleg)
{
    if (UNLIKELY(!deleg))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_INVALID_LISTENER_STUB;
    }

    const LLBC_ListenerStub stub = LLBC_GUIDHelper::GenStr();
    Push(LLBC_SvcEvUtil::BuildSubscribeEvEv(event, stub, deleg));

    return stub;
}

void LLBC_Service::UnsubscribeEvent(int event)
{
    Push(LLBC_SvcEvUtil::
        BuildUnsubscribeEvEv(event, LLBC_INVALID_LISTENER_STUB));
}

void LLBC_Service::UnsubscribeEvent(const LLBC_ListenerStub &stub)
{
    Push(LLBC_SvcEvUtil::
        BuildUnsubscribeEvEv(0, stub));
}

void LLBC_Service::FireEvent(LLBC_Event *ev)
{
    Push(LLBC_SvcEvUtil::BuildFireEvEv(ev));
}

int LLBC_Service::Post(LLBC_IDelegate1<void, LLBC_Service::Base *> *deleg)
{
    if (UNLIKELY(!deleg))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_handlingBeforeFrameTasks && _handlingAfterFrameTasks))
    {
        LLBC_SetLastError(LLBC_ERROR_UNKNOWN);
        return LLBC_FAILED;
    }

    if (_beforeFrameTasks.find(deleg) != _beforeFrameTasks.end() ||
        _afterFrameTasks.find(deleg) != _afterFrameTasks.end())
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    if (_handlingBeforeFrameTasks)
    {
        _afterFrameTasks.insert(deleg);
    }
    else
    {
        if (!_handledBeforeFrameTasks)
        {
            _beforeFrameTasks.insert(deleg);
        }
        else
        {
            if (!_handlingAfterFrameTasks)
                _afterFrameTasks.insert(deleg);
            else
                _beforeFrameTasks.insert(deleg);
        }
    }

    return LLBC_OK;
}

void LLBC_Service::OnSvc(bool fullFrame)
{
    if (fullFrame && _frameInterval == 0)
        fullFrame = false;

    if (UNLIKELY(!_started))
    {
        if (fullFrame)
            LLBC_Sleep(_frameInterval);

        return;
    }

    _sinkIntoLoop = true;

    // Record begin heartbeat time.
    if (fullFrame)
        _begHeartbeatTime = LLBC_GetMilliSeconds();

    // Handle before frame-tasks.
    HandleFrameTasks(_beforeFrameTasks, _handlingBeforeFrameTasks);
    _handledBeforeFrameTasks = true;

    // Process queued events.
    HandleQueuedEvents();

    // Update all components.
    UpdateFacades();
    UpdateTimers();
#if LLBC_CFG_OBJBASE_ENABLED
    UpdateAutoReleasePool();
#endif // LLBC_CFG_OBJBASE_ENABLED

    // Handle after frame-tasks.
    HandleFrameTasks(_afterFrameTasks, _handlingAfterFrameTasks);
    _handledBeforeFrameTasks = false;

    // Process Idle.
    ProcessIdle(fullFrame);

    // Sleep FrameInterval - ElapsedTime milli-seconds, if need.
    if (fullFrame)
    {
        const sint64 elapsed = LLBC_GetMilliSeconds() - _begHeartbeatTime;
        if (elapsed >= 0 && elapsed < _frameInterval)
            LLBC_Sleep(static_cast<int>(_frameInterval - elapsed));
    }
    else
    {
        LLBC_CPURelax();
        if ((++_relaxTimes) % 10000 == 0)
            LLBC_Sleep(0);
    }

    _sinkIntoLoop = false;
    if (UNLIKELY(_afterStop))
        Cleanup();
}

LLBC_ProtocolStack *LLBC_Service::CreatePackStack(int sessionId, int acceptSessionId, LLBC_ProtocolStack *stack)
{
    if (!stack)
    {
        stack = LLBC_New1(_Stack, _Stack::PackStack);
        stack->SetService(this);
    }

    // Find protocol factory.
    LLBC_IProtocolFactory *protoFactory = 
        FindSessionProtocolFactory(acceptSessionId != 0 ? acceptSessionId : sessionId);

    // Create PackLayer protocol.
    LLBC_IProtocol *packProtocol = protoFactory->Create(LLBC_ProtocolLayer::PackLayer);
    ASSERT(packProtocol && "Protocol stack require PackLayer protocol exist!");
    ASSERT(packProtocol->GetLayer() == LLBC_ProtocolLayer::PackLayer && "Invalid protocol layer!");
    stack->AddProtocol(packProtocol);

    // Create CompressLayer protocol.
    LLBC_IProtocol *compressProtocol = protoFactory->Create(LLBC_ProtocolLayer::CompressLayer);
    if (compressProtocol)
    {
        ASSERT(compressProtocol->GetLayer() == LLBC_ProtocolLayer::CompressLayer && "Invalid protocol layer!");
        stack->AddProtocol(compressProtocol);
    }

    return stack;
}

LLBC_ProtocolStack *LLBC_Service::CreateCodecStack(int sessionId, int acceptSessionId, LLBC_ProtocolStack *stack)
{
    if (!stack)
    {
        stack = LLBC_New1(_Stack, _Stack::CodecStack);
        stack->SetService(this);
        stack->SetIsSuppressedCoderNotFoundWarning(_suppressedCoderNotFoundWarning);
    }

    // Find protocol factory.
    LLBC_IProtocolFactory *protoFactory = 
        FindSessionProtocolFactory(acceptSessionId != 0 ? acceptSessionId : sessionId);

    // Create CodecLayer protocol.
    LLBC_IProtocol *codecLayer = protoFactory->Create(LLBC_ProtocolLayer::CodecLayer);
    if (codecLayer)
    {
        ASSERT(codecLayer->GetLayer() == 
            LLBC_ProtocolLayer::CodecLayer && "Invalid protocol layer!");

        stack->AddProtocol(codecLayer);
        stack->SetCoders(&_coders);
    }

    return stack;
}

LLBC_ProtocolStack *LLBC_Service::CreateFullStack(int sessionId, int acceptSessionId)
{
    _Stack *stack = LLBC_New1(_Stack, _Stack::FullStack);
    stack->SetService(this);
    stack->SetIsSuppressedCoderNotFoundWarning(_suppressedCoderNotFoundWarning);

    return CreatePackStack(sessionId, acceptSessionId,
            CreateCodecStack(sessionId, acceptSessionId, stack));
}

void LLBC_Service::Svc()
{
    while (!_started)
        LLBC_Sleep(20);

    _lock.Lock();
    AddServiceToTls();
    GetTimerScheduler();
#if LLBC_CFG_OBJBASE_ENABLED
    CreateAutoReleasePool();
#endif // LLBC_CFG_OBJBASE_ENABLED
    InitFacades();
    StartFacades();
    _lock.Unlock();

    _svcMgr.OnServiceStart(this);

    while (!_stopping)
        OnSvc();
}

void LLBC_Service::Cleanup()
{
    // Stop poller manager.
    _pollerMgr.Stop();

    // If drivemode is external-drive, cancel all timers first.
    if (_driveMode == This::ExternalDrive)
        _timerScheduler->CancelAll();

    // Cleanup connected-sessionIds set.
    _connectedSessionIdsLock.Lock();
    _connectedSessionIds.clear();
    _connectedSessionIdsLock.Unlock();

    // Stop facades, destroy release-pool, and remove service from TLS.
    StopFacades();
#if LLBC_CFG_OBJBASE_ENABLED
    DestroyAutoReleasePool();
#endif // LLBC_CFG_OBJBASE_ENABLED
    RemoveServiceFromTls();

    // Popup & Destroy all not-process events.
    LLBC_MessageBlock *block;
    while (TryPop(block) == LLBC_OK)
        LLBC_SvcEvUtil::DestroyEvBlock(block);

    // If is self-drive servie, notify service manager self stopped.
    if (_driveMode == This::SelfDrive)
    {
        _timerScheduler = NULL;
        _svcMgr.OnServiceStop(this);
    }

    // Reset some variables.
    _relaxTimes = 0;

    _started = false;
    _stopping = false;
}

void LLBC_Service::AddServiceToTls()
{
    __LLBC_LibTls *tls = __LLBC_GetLibTls();

    int idx = 0;
    const int lmt = LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT;
    for (; idx <= lmt; idx++)
        if (!tls->commTls.services[idx])
            break;

    tls->commTls.services[idx] = this;
}

void LLBC_Service::RemoveServiceFromTls()
{
    __LLBC_LibTls *tls = __LLBC_GetLibTls();

    int idx = 0;
    const int lmt = LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT;
    for (; idx <= lmt; idx++)
        if (tls->commTls.services[idx] == this)
            break;

    LLBC_MemCpy(&tls->commTls.services[idx],
                &tls->commTls.services[idx + 1],
                sizeof(tls->commTls.services[0]) * (lmt + 1 - (idx + 1)));
}

bool LLBC_Service::IsCanContinueDriveService()
{
    __LLBC_LibTls *tls = __LLBC_GetLibTls();

    int checkIdx = 0;
    const int lmt = LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT;
    for (; checkIdx <= lmt; checkIdx++)
        if (tls->commTls.services[checkIdx] == NULL)
            break;

    return checkIdx < lmt ? true : false;
}

void LLBC_Service::HandleFrameTasks(LLBC_Service::_FrameTasks &tasks, bool &usingFlag)
{
    if (tasks.size() == 0)
        return;

    usingFlag = true;
    for (_FrameTasks::iterator it = tasks.begin();
         it != tasks.end();
         it++)
        (*it)->Invoke(this);

    DestroyFrameTasks(tasks, usingFlag);
}

void LLBC_Service::DestroyFrameTasks(_FrameTasks &tasks, bool &usingFlag)
{
    usingFlag = false;
    LLBC_STLHelper::DeleteContainer(tasks, true);
}

void LLBC_Service::HandleQueuedEvents()
{
    int type;
    LLBC_ServiceEvent *ev;
    LLBC_MessageBlock *block;
    while (TryPop(block) == LLBC_OK)
    {
        block->Read(&type, sizeof(int));
        block->Read(&ev, sizeof(LLBC_ServiceEvent *));

        (this->*_evHandlers[type])(*ev);

        LLBC_Delete(ev);
        LLBC_Delete(block);
    }
}

void LLBC_Service::HandleEv_SessionCreate(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_SessionCreate _Ev;
    _Ev &ev = static_cast<_Ev &>(_);

    {
        _connectedSessionIdsLock.Lock();
        _connectedSessionIds.insert(ev.sessionId);
        _connectedSessionIdsLock.Unlock();
    }

    LLBC_SessionInfo info;
    info.SetIsListenSession(ev.isListen);
    info.SetSessionId(ev.sessionId);
    info.SetAcceptSessionId(ev.acceptSessionId);
    info.SetLocalAddr(ev.local);
    info.SetPeerAddr(ev.peer);
    info.SetSocket(ev.handle);

    for (_Facades::iterator it = _facades.begin();
         it != _facades.end();
         it++)
        (*it)->OnSessionCreate(info);
}

void LLBC_Service::HandleEv_SessionDestroy(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_SessionDestroy _Ev;
    _Ev &ev = static_cast<_Ev &>(_);

    // Erase session from connected sessionIds set.
    _connectedSessionIdsLock.Lock();
    _connectedSessionIds.erase(ev.sessionId);
    _connectedSessionIdsLock.Unlock();

    // Build session info.
    LLBC_SessionInfo *sessionInfo = LLBC_New(LLBC_SessionInfo);
    sessionInfo->SetIsListenSession(ev.isListen);
    sessionInfo->SetSessionId(ev.sessionId);
    sessionInfo->SetAcceptSessionId(ev.acceptSessionId);
    sessionInfo->SetLocalAddr(ev.local);
    sessionInfo->SetPeerAddr(ev.peer);
    sessionInfo->SetSocket(ev.handle);

    // Build session destroy info.
    LLBC_SessionDestroyInfo destroyInfo(sessionInfo, ev.closeInfo);
    ev.closeInfo = NULL;

    // Dispatch destroy event to all facades.
    for (_Facades::iterator it = _facades.begin();
         it != _facades.end();
         it++)
        (*it)->OnSessionDestroy(destroyInfo);

    // Remove session protocol factory.
    if (ev.acceptSessionId == 0)
        RemoveSessionProtocolFactory(ev.sessionId);
}

void LLBC_Service::HandleEv_AsyncConnResult(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_AsyncConn _Ev;
    _Ev &ev = static_cast<_Ev &>(_);

    LLBC_AsyncConnResult result;
    result.SetIsConnected(ev.connected);
    result.SetReason(ev.reason);
    result.SetPeerAddr(ev.peer);

    for (_Facades::iterator it = _facades.begin();
         it != _facades.end();
         it++)
        (*it)->OnAsyncConnResult(result);

    if (!ev.connected)
        RemoveSessionProtocolFactory(ev.sessionId);
}

void LLBC_Service::HandleEv_DataArrival(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_DataArrival _Ev;
    _Ev &ev = static_cast<_Ev &>(_);
    LLBC_Packet *packet = ev.packet;

    // Makesure session in connected sessionId set.
    const int sessionId = packet->GetSessionId();
    _connectedSessionIdsLock.Lock();
    if (_connectedSessionIds.find(sessionId) == 
        _connectedSessionIds.end())
    {
        _connectedSessionIdsLock.Unlock();
        return;
    }
    _connectedSessionIdsLock.Unlock();

    ev.packet = NULL;

#if !LLBC_CFG_COMM_USE_FULL_STACK
    bool removeSession;
    if (UNLIKELY(_stack.RecvCodec(packet, packet, removeSession) != LLBC_OK))
    {
        if (removeSession)
            RemoveSession(sessionId);

        return;
    }
#endif

    // Packet receiver service Id set or dispatch to another service.
    const int recverSvcId = packet->GetRecverServiceId();
    if (recverSvcId == 0) // Set receiver service Id, if the packet's receiver service Id is 0.
    {
        packet->SetRecverServiceId(_id);
    }
    else if (recverSvcId != _id)
    {
        LLBC_IService *recverSvc = _svcMgr.GetService(recverSvcId);
        if (recverSvc == NULL || !recverSvc->IsStarted())
        {
            LLBC_Delete(packet);
            return;
        }

        recverSvc->Push(LLBC_SvcEvUtil::BuildDataArrivalEv(packet));
        return;
    }

    // Create invoke-guard to delete packet.
    LLBC_InvokeGuard delPacketGuard(&LLBC_INL_NS __DeletePacket, packet);

    const int opcode = packet->GetOpcode();

#if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER || LLBC_CFG_COMM_ENABLE_STATUS_DESC
    const int status = packet->GetStatus();
    if (status != 0)
    {
# if LLBC_CFG_COMM_ENABLE_STATUS_DESC
        _StatusDescs::const_iterator statusDescIt = _statusDescs.find(status);
        if (statusDescIt != _statusDescs.end())
            packet->SetStatusDesc(statusDescIt->second);
# endif // LLBC_CFG_COMM_ENABLE_STATUS_DESC
# if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
        _OpStatusHandlers::iterator stHandlersIt = _statusHandlers.find(opcode);
        if (stHandlersIt != _statusHandlers.end())
        {
            _StatusHandlers &stHandlers = *stHandlersIt->second;
            _StatusHandlers::iterator stHandlerIt = stHandlers.find(status);
            if (stHandlerIt != stHandlers.end())
            {
                stHandlerIt->second->Invoke(*packet);
                return;
            }
        }
# endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
    }
#endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER || LLBC_CFG_COMM_ENABLE_STATUS_DESC

    // Firstly, we recognize specified opcode's pre-handler, if registered, call it(Non-RAW service type).
    bool preHandled = false;
    if (_type != This::Raw)
    {
        _PreHandlers::iterator preIt = _preHandlers.find(opcode);
        if (preIt != _preHandlers.end())
        {
            if (!preIt->second->Invoke(*packet))
                return;

            preHandled = true;
        }
    }
#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    // Secondary, we recognize generalized pre-handler, if registered, call it(all service type available).
    if (!preHandled && _unifyPreHandler)
    {
        if (!_unifyPreHandler->Invoke(*packet))
            return;
    }
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

    _Handlers::iterator it = _handlers.find(opcode);
    if (it != _handlers.end())
    {
        it->second->Invoke(*packet);
    }
    else
    {
        for (_Facades::iterator facadeIt = _facades.begin();
             facadeIt != _facades.end();
             facadeIt++)
            (*facadeIt)->OnUnHandledPacket(*packet);
    }
}

void LLBC_Service::HandleEv_ProtoReport(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_ProtoReport _Ev;
    _Ev &ev = static_cast<_Ev &>(_);

    LLBC_ProtoReport report;
    report.SetSessionId(ev.sessionId);
    report.SetOpcode(ev.opcode);
    report.SetLayer(ev.layer);
    report.SetLevel(ev.level);
    report.SetReport(ev.report);

    for (_Facades::iterator it = _facades.begin();
         it != _facades.end();
         it++)
        (*it)->OnProtoReport(report);
}

void LLBC_Service::HandleEv_SubscribeEv(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_SubscribeEv _Ev;
    _Ev &ev = static_cast<_Ev &>(_);

    _evManager.AddListener(ev.id, ev.deleg, ev.stub);
    ev.deleg = NULL;
}

void LLBC_Service::HandleEv_UnsubscribeEv(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_UnsubscribeEv _Ev;
    _Ev &ev = static_cast<_Ev &>(_);

    if (ev.stub.empty())
        _evManager.RemoveListener(ev.id);
    else
        _evManager.RemoveListener(ev.stub);
}

void LLBC_Service::HandleEv_FireEv(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_FireEv _Ev;
    _Ev &ev = static_cast<_Ev &>(_);

    _evManager.FireEvent(ev.ev);
    ev.ev = NULL;
}

void LLBC_Service::InitFacades()
{
    _initingFacade = true;

    for (_Facades::iterator it = _facades.begin();
         it != _facades.end();
         it++)
    {
        LLBC_IFacade *facade = (*it);
        if (facade->_inited)
            continue;

        facade->OnInitialize();
        facade->_inited = true;
    }

    _initingFacade = false;
}

void LLBC_Service::StartFacades()
{
    for (_Facades::iterator it = _facades.begin();
         it != _facades.end();
         it++)
        (*it)->OnStart();
}

void LLBC_Service::StopFacades()
{
    for (_Facades::reverse_iterator it = _facades.rbegin();
         it != _facades.rend();
         it++)
        (*it)->OnStop();
}

void LLBC_Service::UpdateFacades()
{
    for (_Facades::iterator it = _facades.begin();
         it != _facades.end();
         it++)
        (*it)->OnUpdate();
}

void LLBC_Service::DestroyFacades()
{
    for (_Facades::reverse_iterator it = _facades.rbegin();
         it != _facades.rend();
         it++)
    {
        LLBC_IFacade *facade = *it;
        if (facade->_inited)
            facade->OnDestroy();
    }

    LLBC_STLHelper::DeleteContainer(_facades, true, true);
    _facades2.clear();
}

void LLBC_Service::AddSessionProtocolFactory(int sessionId, LLBC_IProtocolFactory *protoFactory)
{
    _protoLock.Lock();

    std::map<int, LLBC_IProtocolFactory *>::iterator it = _sessionProtoFactory.find(sessionId);
    if (it != _sessionProtoFactory.end())
    {
        LLBC_Delete(it->second);
        _sessionProtoFactory.erase(it);
    }

    _sessionProtoFactory.insert(std::make_pair(sessionId, protoFactory));

    _protoLock.Unlock();
}

LLBC_IProtocolFactory *LLBC_Service::FindSessionProtocolFactory(int sessionId)
{
    if (sessionId == 0)
        return _protoFactory;

    LLBC_LockGuard guard(_protoLock);
    std::map<int, LLBC_IProtocolFactory *>::iterator it = _sessionProtoFactory.find(sessionId);
    if (it != _sessionProtoFactory.end())
        return it->second;

    return _protoFactory;
}

void LLBC_Service::RemoveSessionProtocolFactory(int sessionId)
{
    LLBC_LockGuard guard(_protoLock);
    std::map<int, LLBC_IProtocolFactory *>::iterator it = _sessionProtoFactory.find(sessionId);
    if (it != _sessionProtoFactory.end())
    {
        LLBC_Delete(it->second);
        _sessionProtoFactory.erase(it);
    }
}

#if LLBC_CFG_OBJBASE_ENABLED
void LLBC_Service::CreateAutoReleasePool()
{
    __LLBC_LibTls *tls = __LLBC_GetLibTls();

    _releasePoolStack = LLBC_New(LLBC_AutoReleasePoolStack);
    tls->objbaseTls.poolStack = _releasePoolStack;

    LLBC_New(LLBC_AutoReleasePool);
}

void LLBC_Service::UpdateAutoReleasePool()
{
    if (_driveMode == This::SelfDrive)
        _releasePoolStack->Purge();
}

void LLBC_Service::DestroyAutoReleasePool()
{
    LLBC_XDelete(_releasePoolStack);

    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    tls->objbaseTls.poolStack = NULL;
}
#endif // LLBC_CFG_OBJBASE_ENABLED

void LLBC_Service::GetTimerScheduler()
{
    if (!_timerScheduler)
    {
        __LLBC_LibTls *tls = __LLBC_GetLibTls();
        _timerScheduler = reinterpret_cast<LLBC_TimerScheduler *>(tls->coreTls.timerScheduler);
    }
}

void LLBC_Service::UpdateTimers()
{
    _timerScheduler->Update();
}

void LLBC_Service::ProcessIdle(bool fullFrame)
{
    for (_Facades::iterator it = _facades.begin();
         it != _facades.end();
         it++)
    {
        if (fullFrame)
        {
            sint64 elapsed = LLBC_GetMilliSeconds() - _begHeartbeatTime;
            if (LIKELY(elapsed >= 0))
            {
                if (elapsed >= _frameInterval)
                    break;

                (*it)->OnIdle(static_cast<int>(_frameInterval - elapsed));
            }
        }
        else
        {
            (*it)->OnIdle(0);
        }
    }
}

int LLBC_Service::LockableSend(LLBC_Packet *packet,
                               bool lock,
                               bool validCheck)
{
    if (lock)
        _lock.Lock();

    if (UNLIKELY(!_started))
    {
        if (lock)
            _lock.Unlock();

        LLBC_Delete(packet);

        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    const int sessionId = packet->GetSessionId();
    if (validCheck)
    {
        LLBC_LockGuard connSIdsGuard(_connectedSessionIdsLock);
        if (_connectedSessionIds.find(sessionId) == _connectedSessionIds.end())
        {
            if (lock)
                _lock.Unlock();

            LLBC_Delete(packet);

            LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
            return LLBC_FAILED;
        }
    }

    packet->SetSenderServiceId(_id);

#if !LLBC_CFG_COMM_USE_FULL_STACK
    bool removeSession;
    LLBC_Packet *encoded;
    if (_stack.SendCodec(packet, encoded, removeSession) != LLBC_OK)
    {
        if (removeSession)
            RemoveSession(sessionId, LLBC_FormatLastError());

        if (lock)
            _lock.Unlock();

        return LLBC_FAILED;
    }

    const int ret = _pollerMgr.Send(encoded);
    if (lock)
        _lock.Unlock();

    return ret;
#else
    const int ret = _pollerMgr.Send(packet);
    if (lock)
        _lock.Unlock();

    return ret;
#endif
}

int LLBC_Service::LockableSend(int svcId,
                               int sessionId,
                               int opcode,
                               const void *bytes,
                               size_t len,
                               int status,
                               bool lock,
                               bool validCheck)
{
    LLBC_Packet *packet = LLBC_New(LLBC_Packet);
    // packet->SetSenderServiceId(_id); // LockableSend(LLBC_Packet *, bool bool) function will set sender service Id.
    packet->SetHeader(svcId, sessionId, opcode, status);

    int ret = packet->Write(bytes, len);
    if (UNLIKELY(ret != LLBC_OK))
    {
        LLBC_Delete(packet);
        return ret;
    }

    return LockableSend(packet, lock, validCheck);
}

template <typename SessionIds>
int LLBC_Service::MulticastSendCoder(int svcId,
                                     const SessionIds &sessionIds,
                                     int opcode,
                                     LLBC_ICoder *coder,
                                     int status,
                                     bool validCheck)
{
    if (sessionIds.empty())
    {
        if(LIKELY(coder))
            LLBC_Delete(coder);

        return LLBC_OK;
    }

    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(!_started))
    {
        if (LIKELY(coder))
            LLBC_Delete(coder);

        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    typename SessionIds::const_iterator sessionIt = sessionIds.begin();

    LLBC_Packet *firstPacket = LLBC_New(LLBC_Packet);
    // firstPacket->SetSenderServiceId(_id); // LockableSend(LLBC_Packet *, bool, bool) function will set sender service Id.
    firstPacket->SetHeader(svcId, *sessionIt++, opcode, status);

    bool hasCoder = true;
    if (LIKELY(coder))
    {
        if (!coder->Encode(*firstPacket))
        {
            LLBC_SetLastError(LLBC_ERROR_ENCODE);
            return LLBC_FAILED;
        }

        LLBC_Delete(coder);
    }
    else
    {
        hasCoder = false;
    }

    typename SessionIds::size_type sessionCnt = sessionIds.size();
    if (sessionCnt == 1)
        return LockableSend(firstPacket, false, validCheck);

    LLBC_Packet **otherPackets =
        LLBC_Calloc(LLBC_Packet *, (sizeof(LLBC_Packet *) * (sessionCnt - 1)));
    if (LIKELY(hasCoder))
    {
        const void *payload = firstPacket->GetPayload();
        const size_t payloadLen = firstPacket->GetPayloadLength();

        if (validCheck)
            _connectedSessionIdsLock.Lock();

        for (register typename SessionIds::size_type i = 1;
             i < sessionCnt;
             i++)
        {
            const int sessionId = *sessionIt++;
            if (validCheck &&
                _connectedSessionIds.find(sessionId) == _connectedSessionIds.end())
                    continue;

            LLBC_Packet *otherPacket = LLBC_New(LLBC_Packet);
            // otherPacket->SetSenderServiceId(_id); // LockableSend(LLBC_Packet *, bool, bool) function will set sender service Id.
            otherPacket->SetHeader(svcId, sessionId, opcode, status);
            otherPacket->Write(payload, payloadLen);

            otherPackets[i - 1] = otherPacket;
        }

        if (validCheck)
            _connectedSessionIdsLock.Unlock();
    }
    else
    {
        if (validCheck)
            _connectedSessionIdsLock.Lock();

        for (register typename SessionIds::size_type i = 1;
             i < sessionCnt;
             i++)
        {
            const int sessionId = *sessionIt++;
            if (validCheck &&
                _connectedSessionIds.find(sessionId) == _connectedSessionIds.end())
                continue;

            LLBC_Packet *otherPacket = LLBC_New(LLBC_Packet);
            // otherPacket->SetSenderServiceId(_id); // LockableSend(LLBC_Packet *, bool, bool) function will set sender service Id.
            otherPacket->SetHeader(svcId, sessionId, opcode, status);

            otherPackets[i - 1] = otherPacket;
        }

        if (validCheck)
            _connectedSessionIdsLock.Unlock();
    }

    LockableSend(firstPacket, false, validCheck); // Use pass "validCheck" argument to call LockableSend().

    const size_t otherPacketCnt = sessionCnt - 1;
    for (register size_t i = 0; i < otherPacketCnt; i++)
    {
        LLBC_Packet *otherPacket = otherPackets[i];
        if (!otherPacket)
            continue;

        LockableSend(otherPacket, false, false); // Don't need vaildate check.
    }

    LLBC_Free(otherPackets);

    return LLBC_OK;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
