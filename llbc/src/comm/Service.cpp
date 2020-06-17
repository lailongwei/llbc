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
    &LLBC_Service::HandleEv_FireEv,

    &LLBC_Service::HandleEv_AppCfgReloaded,
};

// VS2005 and later version compiler support initialize array in construct list.
// In here, we disable C4351 warning to initialize it.
#if LLBC_CUR_COMP == LLBC_COMP_MSVC && LLBC_COMP_VER >= 1400
# pragma warning(disable:4351)
#endif

LLBC_Service::LLBC_Service(This::Type type,
                           const LLBC_String &name,
                           LLBC_IProtocolFactory *protoFactory,
                           bool fullStack)
: _id(LLBC_AtomicFetchAndAdd(&_maxId, 1))
, _type(type)
, _fullStack(fullStack)
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
, _readySessionInfos()
, _readySessionInfosLock()

, _willRegFacades()

, _facadesInitFinished(0)
, _facadesInitRet(LLBC_OK)
, _facadesStartFinished(0)
, _facadesStartRet(LLBC_OK)

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

, _beforeFrameTasks()
, _afterFrameTasks()

, _handlingBeforeFrameTasks(false)
, _handledBeforeFrameTasks(false)
, _handlingAfterFrameTasks(false)

, _releasePoolStack(NULL)

, _packetObjectPool(*_safetyObjectPool.GetPoolInst<LLBC_Packet>())
, _msgBlockObjectPool(*_safetyObjectPool.GetPoolInst<LLBC_MessageBlock>())

, _timerScheduler(NULL)

, _evManager()
, _evManagerMaxListenerStub(0)

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

    // Initialize cared event facades array.
    LLBC_MemSet(_caredEventFacades, 0, sizeof(_caredEventFacades));

    // Get the poller type from Config.h.
    const char *pollerModel = LLBC_CFG_COMM_POLLER_MODEL;
    const int pollerType = LLBC_PollerType::Str2Type(pollerModel);
    ASSERT(LLBC_PollerType::IsValid(pollerType) && "Invalid LLBC_CFG_COMM_POLLER_MODEL config!");

    _pollerMgr.SetService(this);
    _pollerMgr.SetPollerType(pollerType);
}

// If using MSVC compiler and version >= 1400(VS2005), Reset C4351 warning to default.
#if LLBC_CUR_COMP == LLBC_COMP_MSVC && LLBC_COMP_VER >= 1400
# pragma warning(default:4351)
#endif

LLBC_Service::~LLBC_Service()
{
    Stop();

    DestroyFacades();
    DestroyWillRegFacades();

    LLBC_STLHelper::DeleteContainer(_coders);
    LLBC_STLHelper::DeleteContainer(_handlers);
    LLBC_STLHelper::DeleteContainer(_preHandlers);
#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    LLBC_XDelete(_unifyPreHandler);
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

#if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
    for (_OpStatusHandlers::iterator it = _statusHandlers.begin();
         it != _statusHandlers.end();
         ++it)
    {
        LLBC_STLHelper::DeleteContainer(*it->second);
        LLBC_Delete(it->second);
    }
#endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER

    RemoveAllReadySessions();

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

bool LLBC_Service::IsFullStack() const
{
    return _fullStack;
}

This::DriveMode LLBC_Service::GetDriveMode() const
{
    return _driveMode;
}

int LLBC_Service::SetDriveMode(This::DriveMode mode)
{
    // Drive mode vlidate check.
    if (mode != This::SelfDrive && mode != This::ExternalDrive)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }
    else if (_started) // If service started, could not change drive mode.
    {
        LLBC_SetLastError(LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }

    // Service started recheck(after locked).
    LLBC_LockGuard guard(_lock);
    if (_started)
    {
        LLBC_SetLastError(LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }

    // If drive mode is external drive, Reinit object-pools, timer-scheduler, ...
    if ((_driveMode = mode) == This::ExternalDrive)
    {
        ClearHoldedObjectPools();
        ClearHoldedTimerScheduler();

        InitObjectPools();
        InitTimerScheduler();
    }

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

    _lock.Lock();
    if (_started)
    {
        _lock.Unlock();
        LLBC_SetLastError(LLBC_ERROR_REENTRY);

        return LLBC_FAILED;
    }

    if (_pollerMgr.Start(pollerCount) != LLBC_OK)
    {
        _lock.Unlock();
        return LLBC_FAILED;
    }

    if (_driveMode == This::ExternalDrive)
    {
        if (!IsCanContinueDriveService())
        {
            _pollerMgr.Stop();
            LLBC_SetLastError(LLBC_ERROR_LIMIT);

            _lock.Unlock();

            return LLBC_FAILED;
        }

        AddServiceToTls();
    }
    else
    {
        if (Activate(1) != LLBC_OK)
        {
            _pollerMgr.Stop();
            _lock.Unlock();

            return LLBC_FAILED;
        }
    }

    _started = true;

    if (_driveMode == This::ExternalDrive)
    {
        // Waiting for all facade init & start finished.
        if (InitFacades() != LLBC_OK || 
            StartFacades() != LLBC_OK)
        {
            int errNo = LLBC_GetLastError();

            Stop();
            LLBC_SetLastError(errNo);

            _lock.Unlock();

            return LLBC_FAILED;
        }

        _lock.Unlock();
    }
    else
    {
        // Unlock first.
        _lock.Unlock();

        // Waiting for all facades init finished.
        while (!_facadesInitFinished)
            LLBC_Sleep(2);
        if (_facadesInitRet != LLBC_OK)
        {
            Stop();
            LLBC_SetLastError(LLBC_ERROR_FACADE_INIT);

            return LLBC_FAILED;
        }

        // Waiting for all facades start finished.
        while (!_facadesStartFinished)
            LLBC_Sleep(2);
        if (_facadesStartRet != LLBC_OK)
        {
            Stop();
            LLBC_SetLastError(LLBC_ERROR_FACADE_START);

            return LLBC_FAILED;
        }
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
    if (fps != static_cast<int>(LLBC_INFINITE) &&
            (fps < LLBC_CFG_COMM_MIN_SERVICE_FPS || fps > LLBC_CFG_COMM_MAX_SERVICE_FPS))
    {
        LLBC_SetLastError(LLBC_ERROR_LIMIT);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);

    _fps = fps;
    if (_fps != static_cast<int>(LLBC_INFINITE))
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

int LLBC_Service::Listen(const char *ip,
                         uint16 port,
                         LLBC_IProtocolFactory *protoFactory,
                         const LLBC_SessionOpts &sessionOpts)
{
    LLBC_LockGuard guard(_lock);
    const int sessionId = _pollerMgr.Listen(ip, port, protoFactory, sessionOpts);
    if (sessionId != 0)
        AddReadySession(sessionId, 0, true);
    else
        LLBC_XDelete(protoFactory);

    return sessionId;
}

int LLBC_Service::Connect(const char *ip,
                          uint16 port,
                          double timeout,
                          LLBC_IProtocolFactory *protoFactory,
                          const LLBC_SessionOpts &sessionOpts)
{
    LLBC_LockGuard guard(_lock);
    const int sessionId = _pollerMgr.Connect(ip, port, protoFactory, sessionOpts);
    if (sessionId != 0)
        AddReadySession(sessionId, 0, false);
    else
        LLBC_XDelete(protoFactory);

    return sessionId;
}

int LLBC_Service::AsyncConn(const char *ip,
                            uint16 port,
                            double timeout,
                            LLBC_IProtocolFactory *protoFactory,
                            const LLBC_SessionOpts &sessionOpts)
{
    LLBC_LockGuard guard(_lock);

    int pendingSessionId;
    const int asyncConnRet = _pollerMgr.AsyncConn(ip,
                                                  port,
                                                  pendingSessionId,
                                                  protoFactory,
                                                  sessionOpts);
    if (asyncConnRet != LLBC_OK)
    {
        LLBC_XDelete(protoFactory);
        return 0;
    }

    return pendingSessionId;
}

bool LLBC_Service::IsSessionValidate(int sessionId)
{
    if (UNLIKELY(sessionId == 0))
        return false;

    _readySessionInfosLock.Lock();
    const bool valid = _readySessionInfos.find(sessionId) != _readySessionInfos.end();
    _readySessionInfosLock.Unlock();

    return valid;
}

int LLBC_Service::Send(LLBC_Packet *packet)
{
    // Call internal LockableSend() method to send packet.
    // lock = true
    // validCheck = true
    return LockableSend(packet);
}

int LLBC_Service::Broadcast(int svcId, int opcode, LLBC_ICoder *coder, int status)
{
    // Copy all connected session Ids.
    _readySessionInfosLock.Lock();
    LLBC_SessionIdList connSIds;
    for (_ReadySessionInfosCIter readySInfoIt = _readySessionInfos.begin();
         readySInfoIt != _readySessionInfos.end();
         ++readySInfoIt)
    {
        const _ReadySessionInfo * const &sessionInfo = readySInfoIt->second;
        if (sessionInfo->isListenSession)
            continue;

        connSIds.push_back(sessionInfo->sessionId);
    }
    _readySessionInfosLock.Unlock();

    // Call internal template method MulticastSendCoder<>() to complete.
    // validCheck = false
    return MulticastSendCoder<LLBC_SessionIdList>(svcId, connSIds, opcode, coder, status, false);
}

int LLBC_Service::Broadcast(int svcId, int opcode, const void *bytes, size_t len , int status)
{
    LLBC_LockGuard guard(_lock);

    // Foreach to call internal method LockableSend() method to complete.
    // lock = false
    // validCheck = false
    LLBC_LockGuard readySInfosGuard(_readySessionInfosLock);
    for (_ReadySessionInfosCIter readySInfoIt = _readySessionInfos.begin();
         readySInfoIt != _readySessionInfos.end();
         ++readySInfoIt)
    {
        const _ReadySessionInfo * const &readySInfo = readySInfoIt->second;
        if (readySInfo->isListenSession)
            continue;

        LockableSend(svcId, readySInfo->sessionId, opcode, bytes, len, status, false, false);
    }

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

    LLBC_LockGuard readySInfosGuard(_readySessionInfosLock);
    _ReadySessionInfosIter readySInfoIt = _readySessionInfos.find(sessionId);
    if (readySInfoIt == _readySessionInfos.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    _pollerMgr.Close(sessionId, reason);

    LLBC_Delete(readySInfoIt->second);
    _readySessionInfos.erase(readySInfoIt);

    return LLBC_OK;
}

int LLBC_Service::CtrlProtocolStack(int sessionId, int ctrlCmd, const LLBC_Variant &ctrlData, LLBC_IDelegate3<void, int, int, const LLBC_Variant &> *ctrlDataClearDeleg)
{
    LLBC_LockGuard guard(_lock);
    if (!_started)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    _readySessionInfosLock.Lock();
    _ReadySessionInfosCIter readySInfoIt = _readySessionInfos.find(sessionId);
    if (readySInfoIt == _readySessionInfos.end())
    {
        _readySessionInfosLock.Unlock();
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);

        return LLBC_FAILED;
    }

    if (!_fullStack)
    {
        bool removeSession = false;
        const _ReadySessionInfo * const &readySInfo = readySInfoIt->second;
        if (!readySInfo->codecStack->CtrlStackCodec(ctrlCmd, ctrlData, removeSession))
        {
            _readySessionInfosLock.Unlock();
            if (removeSession)
                RemoveSession(sessionId, "Protocol stack ctrl finished, business logic require remove this session(Half-Stack mode only)");

            return LLBC_OK;
        }
    }

    _readySessionInfosLock.Unlock();

    _pollerMgr.CtrlProtocolStack(sessionId, ctrlCmd, ctrlData, ctrlDataClearDeleg);

    return LLBC_OK;
}

int LLBC_Service::RegisterFacade(LLBC_IFacadeFactory *facadeFactory)
{
    if (UNLIKELY(!facadeFactory))
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

    for (_WillRegFacades::iterator it = _willRegFacades.begin();
         it != _willRegFacades.end();
         ++it)
    {
        if (it->facadeFactory != NULL && it->facadeFactory == facadeFactory)
        {
            LLBC_SetLastError(LLBC_ERROR_REPEAT);
            return LLBC_FAILED;
        }
    }

    _willRegFacades.push_back(_WillRegFacade(facadeFactory));

    return LLBC_OK;
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

    for (_WillRegFacades::iterator regIt = _willRegFacades.begin();
         regIt != _willRegFacades.end();
         ++regIt)
    {
        if (regIt->facade != NULL && regIt->facade == facade)
        {
            LLBC_SetLastError(LLBC_ERROR_REPEAT);
            return LLBC_FAILED;
        }
    }

    _willRegFacades.push_back(_WillRegFacade(facade));

    return LLBC_OK;
}

int LLBC_Service::RegisterFacade(const LLBC_String &libPath, const LLBC_String &facadeName)
{
    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_started))
    {
        LLBC_SetLastError(LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }

    if (facadeName.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }
    else if (!LLBC_File::Exists(libPath))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    _willRegFacades.push_back(_WillRegFacade(LLBC_Directory::AbsPath(libPath), facadeName));

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

    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_started && !_initingFacade))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    Push(LLBC_SvcEvUtil::BuildSubscribeEvEv(event, ++_evManagerMaxListenerStub, deleg));

    return _evManagerMaxListenerStub;
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

int LLBC_Service::Post(LLBC_IDelegate2<void, LLBC_Service::Base *, const LLBC_Variant *> *deleg, LLBC_Variant *data)
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
        _afterFrameTasks.insert(std::make_pair(deleg, data));
    }
    else
    {
        if (!_handledBeforeFrameTasks)
        {
            _beforeFrameTasks.insert(std::make_pair(deleg, data));
        }
        else
        {
            if (!_handlingAfterFrameTasks)
                _afterFrameTasks.insert(std::make_pair(deleg, data));
            else
                _beforeFrameTasks.insert(std::make_pair(deleg, data));
        }
    }

    return LLBC_OK;
}

const LLBC_ProtocolStack *LLBC_Service::GetCodecProtocolStack(int sessionId) const
{
    // If enabled full-stack option, return NULL.
    if (_fullStack)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return NULL;
    }

    // Not enabled full-stack option, return session codec protocol-stack.
    LLBC_Service *ncThis = const_cast<LLBC_Service *>(this);
    ncThis->_readySessionInfosLock.Lock();
    _ReadySessionInfosCIter it = _readySessionInfos.find(sessionId);
    const LLBC_ProtocolStack *codecStack = it != _readySessionInfos.end() ? it->second->codecStack : NULL;
    ncThis->_readySessionInfosLock.Unlock();

    if (!codecStack)
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);

    return codecStack;
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
    UpdateAutoReleasePool();

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

void LLBC_Service::NtyApplicationConfigReloaded(bool iniReloaded, bool propReloaded)
{
    LLBC_LockGuard guard(_lock);
    if (!IsStarted())
        return;

    Push(LLBC_SvcEvUtil::BuildAppCfgReloadedEv(iniReloaded, propReloaded));
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

void LLBC_Service::AddReadySession(int sessionId, int acceptSessionId, bool isListenSession, bool repeatCheck)
{
    if (repeatCheck)
    {
        _readySessionInfosLock.Lock();
        const _ReadySessionInfosCIter readySInfoIt = _readySessionInfos.find(sessionId);
        if (readySInfoIt != _readySessionInfos.end())
        {
            _readySessionInfosLock.Unlock();
            return;
        }

        _ReadySessionInfo *readySInfo = new _ReadySessionInfo(sessionId,
                                                              0,
                                                              isListenSession,
                                                              _fullStack ? NULL : CreateCodecStack(sessionId, acceptSessionId, NULL));
        _readySessionInfos.insert(std::make_pair(sessionId, readySInfo));
        _readySessionInfosLock.Unlock();
    }
    else
    {
        _ReadySessionInfo *readySInfo = new _ReadySessionInfo(sessionId,
                                                              0,
                                                              isListenSession,
                                                              _fullStack ? NULL : CreateCodecStack(sessionId, acceptSessionId, NULL));
        _readySessionInfosLock.Lock();
        _readySessionInfos.insert(std::make_pair(sessionId, readySInfo));
        _readySessionInfosLock.Unlock();
    }
}

void LLBC_Service::RemoveReadySession(int sessionId)
{
    // Lock.
    _readySessionInfosLock.Lock();

    // Find ready session info, if not found, return.
    _ReadySessionInfosIter readySInfoIt = _readySessionInfos.find(sessionId);
    if (readySInfoIt == _readySessionInfos.end())
    {
        _readySessionInfosLock.Unlock();
        return;
    }

    // Erase from dict.
    _ReadySessionInfo *readySInfo = readySInfoIt->second;
    _readySessionInfos.erase(readySInfoIt);

    // Unlock.
    _readySessionInfosLock.Unlock();

    // At last, delete ready session info.
    LLBC_Delete(readySInfo);
}

void LLBC_Service::RemoveAllReadySessions()
{
    _readySessionInfosLock.Lock();
    LLBC_STLHelper::DeleteContainer(_readySessionInfos, true, false);
    _readySessionInfosLock.Unlock();
}

void LLBC_Service::Svc()
{
    while (!_started)
        LLBC_Sleep(20);

    _lock.Lock();

    AddServiceToTls();
    InitObjectPools();
    InitTimerScheduler();
    InitAutoReleasePool();
    _facadesInitRet = InitFacades();
    _facadesInitFinished = 1;
    if (UNLIKELY(_facadesInitRet != LLBC_OK))
    {
        _lock.Unlock();
        return;
    }

    _facadesStartRet = StartFacades();
    _facadesStartFinished = 1;
    if (UNLIKELY(_facadesStartRet != LLBC_OK))
    {
        _lock.Unlock();
        return;
    }

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

    // Cleanup ready-sessionInfos map.
    RemoveAllReadySessions();

    // Stop facades, destroy release-pool.
    StopFacades();
    ClearAutoReleasePool();

    // Clear holded timer-scheduler & object-pools.
    ClearHoldedObjectPools();
    ClearHoldedTimerScheduler();

    // Remove service from TLS.
    RemoveServiceFromTls();

    // Popup & Destroy all not-process events.
    LLBC_MessageBlock *block;
    while (TryPop(block) == LLBC_OK)
        LLBC_SvcEvUtil::DestroyEvBlock(block);

    // If is self-drive servie, notify service manager self stopped.
    if (_driveMode == This::SelfDrive)
        _svcMgr.OnServiceStop(this);

    // Reset some variables.
    _relaxTimes = 0;

    _facadesInitFinished = 0;
    _facadesInitRet = LLBC_OK;
    _facadesStartFinished = 0;
    _facadesStartRet = LLBC_OK;

    _started = false;
    _stopping = false;
}

void LLBC_Service::AddServiceToTls()
{
    __LLBC_LibTls *tls = __LLBC_GetLibTls();

    int idx = 0;
    const int lmt = LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT;
    for (; idx <= lmt; ++idx)
    {
        if (!tls->commTls.services[idx])
            break;
    }

    tls->commTls.services[idx] = this;
}

void LLBC_Service::RemoveServiceFromTls()
{
    __LLBC_LibTls *tls = __LLBC_GetLibTls();

    int idx = 0;
    const int lmt = LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT;
    for (; idx <= lmt; ++idx)
    {
        if (tls->commTls.services[idx] == this)
            break;
    }

    LLBC_MemCpy(&tls->commTls.services[idx],
                &tls->commTls.services[idx + 1],
                sizeof(tls->commTls.services[0]) * (lmt + 1 - (idx + 1)));
}

bool LLBC_Service::IsCanContinueDriveService()
{
    __LLBC_LibTls *tls = __LLBC_GetLibTls();

    int checkIdx = 0;
    const int lmt = LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT;
    for (; checkIdx <= lmt; ++checkIdx)
    {
        if (tls->commTls.services[checkIdx] == NULL)
            break;
    }

    return checkIdx < lmt ? true : false;
}

void LLBC_Service::HandleFrameTasks(LLBC_Service::_FrameTasks &tasks, bool &usingFlag)
{
    if (tasks.size() == 0)
        return;

    usingFlag = true;
    for (_FrameTasks::iterator it = tasks.begin();
         it != tasks.end();
         ++it)
        (it->first)->Invoke(this, it->second);

    DestroyFrameTasks(tasks, usingFlag);
}

void LLBC_Service::DestroyFrameTasks(_FrameTasks &tasks, bool &usingFlag)
{
    usingFlag = false;
    for (_FrameTasks::iterator it = tasks.begin();
         it != tasks.end();
         ++it)
    {
        LLBC_Delete(it->first);
        if (it->second)
            LLBC_Delete(it->second);
    }

    tasks.clear();
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

    // Add session to connected sessionIds set.
    {
        AddReadySession(ev.sessionId, ev.acceptSessionId, ev.isListen, true);
    }

    // Check has care session-create ev facades or not, if has cared event facades, dispatch event.
    if (_caredEventFacades[LLBC_FacadeEventsOffset::OnSessionCreate])
    {
        // Build session info.
        LLBC_SessionInfo info;
        info.SetIsListenSession(ev.isListen);
        info.SetSessionId(ev.sessionId);
        info.SetAcceptSessionId(ev.acceptSessionId);
        info.SetLocalAddr(ev.local);
        info.SetPeerAddr(ev.peer);
        info.SetSocket(ev.handle);

        // Dispatch session-create event to all facades.
        _Facades &caredEvFacades = *_caredEventFacades[LLBC_FacadeEventsOffset::OnSessionCreate];
        const size_t facadesSize = caredEvFacades.size();
        for (size_t facadeIdx = 0; facadeIdx != facadesSize; ++facadeIdx)
            caredEvFacades[facadeIdx]->OnSessionCreate(info);
    }
}

void LLBC_Service::HandleEv_SessionDestroy(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_SessionDestroy _Ev;
    _Ev &ev = static_cast<_Ev &>(_);

    // Erase session from connected sessionIds set.
    {
        RemoveReadySession(ev.sessionId);
    }

    // Check has care session-destroy ev facades or not, if has cared event facades, dispatch event.
    if (_caredEventFacades[LLBC_FacadeEventsOffset::OnSessionDestroy])
    {
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

        // Dispatch session-destroy event to all facades.
        _Facades &caredEvFacades = *_caredEventFacades[LLBC_FacadeEventsOffset::OnSessionDestroy];
        const size_t facadesSize = caredEvFacades.size();
        for (size_t facadeIdx = 0; facadeIdx != facadesSize; ++facadeIdx)
             caredEvFacades[facadeIdx]->OnSessionDestroy(destroyInfo);
    }

    // Remove session protocol factory.
    if (ev.acceptSessionId == 0)
        RemoveSessionProtocolFactory(ev.sessionId);
}

void LLBC_Service::HandleEv_AsyncConnResult(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_AsyncConn _Ev;
    _Ev &ev = static_cast<_Ev &>(_);


    // Check has care asyncconn-result ev facades or not, if has cared event facades, dispatch event.
    if (_caredEventFacades[LLBC_FacadeEventsOffset::OnAsyncConnResult])
    {
        LLBC_AsyncConnResult result;
        result.SetIsConnected(ev.connected);
        result.SetSessionId(ev.sessionId);
        result.SetReason(ev.reason);
        result.SetPeerAddr(ev.peer);

        _Facades &caredFacades = *_caredEventFacades[LLBC_FacadeEventsOffset::OnAsyncConnResult];
        const size_t facadesSize = caredFacades.size();
        for (size_t facadeIdx = 0; facadeIdx != facadesSize; ++facadeIdx)
             caredFacades[facadeIdx]->OnAsyncConnResult(result);
    }

    // Remove session protocol factory, if connect failed.
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

    _readySessionInfosLock.Lock();
    _ReadySessionInfosCIter readySInfoIt = _readySessionInfos.find(sessionId);
    if (readySInfoIt == _readySessionInfos.end())
    {
        _readySessionInfosLock.Unlock();
        return;
    }

    ev.packet = NULL;

    if (!_fullStack)
    {
        bool removeSession;
        const _ReadySessionInfo * const &readySInfo = readySInfoIt->second;
        if (UNLIKELY(readySInfo->codecStack->RecvCodec(packet, packet, removeSession) != LLBC_OK))
        {
            _readySessionInfosLock.Unlock();
            if (removeSession)
                RemoveSession(sessionId);

            return;
        }
    }

    _readySessionInfosLock.Unlock();

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
            LLBC_Recycle(packet);
            return;
        }

        recverSvc->Push(LLBC_SvcEvUtil::BuildDataArrivalEv(packet));
        return;
    }

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
                LLBC_Recycle(packet);
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
            {
                LLBC_Recycle(packet);
                return;
            }

            preHandled = true;
        }
    }
#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    // Secondary, we recognize generalized pre-handler, if registered, call it(all service type available).
    if (!preHandled && _unifyPreHandler)
    {
        if (!_unifyPreHandler->Invoke(*packet))
        {
            LLBC_Recycle(packet);
            return;
        }
    }
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

    // Finally, search packet handler to handle, if not found any packet handler, dispatch unhandled-packet event to all facades.
    _Handlers::iterator it = _handlers.find(opcode);
    if (it != _handlers.end())
    {
        it->second->Invoke(*packet);
    }
    else
    {
        if (_caredEventFacades[LLBC_FacadeEventsOffset::OnUnHandledPacket])
        {
            _Facades &caredFacades = *_caredEventFacades[LLBC_FacadeEventsOffset::OnUnHandledPacket];
            const size_t facadesSize = caredFacades.size();
            for (size_t facadeIdx = 0; facadeIdx != facadesSize; ++facadeIdx)
                 caredFacades[facadeIdx]->OnUnHandledPacket(*packet);
        }
    }

    LLBC_Recycle(packet);
}

void LLBC_Service::HandleEv_ProtoReport(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_ProtoReport _Ev;
    _Ev &ev = static_cast<_Ev &>(_);

    // Check has care proto-report ev facades or not, if has cared event facades, dispatch event.
    if (_caredEventFacades[LLBC_FacadeEventsOffset::OnProtoReport])
    {
        LLBC_ProtoReport report;
        report.SetSessionId(ev.sessionId);
        report.SetOpcode(ev.opcode);
        report.SetLayer(ev.layer);
        report.SetLevel(ev.level);
        report.SetReport(ev.report);

        _Facades &caredFacades = *_caredEventFacades[LLBC_FacadeEventsOffset::OnProtoReport];
        const size_t facadesSize = caredFacades.size();
        for (size_t facadeIdx = 0; facadeIdx != facadesSize; ++facadeIdx)
             caredFacades[facadeIdx]->OnProtoReport(report);
    }
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

    if (ev.stub == LLBC_INVALID_LISTENER_STUB)
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

void LLBC_Service::HandleEv_AppCfgReloaded(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_AppCfgReloadedEv _Ev;
    _Ev &ev = static_cast<_Ev &>(_);

    // Check has care application config reloaded ev facades or not, if has cared event facades, dispatch event.
    if (_caredEventFacades[LLBC_FacadeEventsOffset::OnAppCfgReloaded])
    {
        // Dispatch application config reloaded event to all facades.
        _Facades &caredEvFacades = *_caredEventFacades[LLBC_FacadeEventsOffset::OnAppCfgReloaded];
        const size_t facadesSize = caredEvFacades.size();
        for (size_t facadeIdx = 0; facadeIdx != facadesSize; ++facadeIdx)
        {
            if (ev.iniReloaded)
                caredEvFacades[facadeIdx]->OnApplicationIniConfigReload();
            if (ev.propReloaded)
                caredEvFacades[facadeIdx]->OnApplicationPropertyConfigReload();
        }
    }
}

int LLBC_Service::InitFacades()
{
    _initingFacade = true;

    bool initSuccess = true;
    for (_WillRegFacades::iterator regIt = _willRegFacades.begin();
         regIt != _willRegFacades.end();
         ++regIt)
    {
        LLBC_IFacade *facade = NULL;
        _WillRegFacade &willRegFacade = *regIt;
        if (willRegFacade.facadeFactory != NULL) // Create facade from facade factory.
        {
            facade = willRegFacade.facadeFactory->Create();
            LLBC_XDelete(willRegFacade.facadeFactory);
        }
        else if (willRegFacade.facade != NULL) // Create facade from giving facade(borrow).
        {
            facade = willRegFacade.facade;
            willRegFacade.facade = NULL;
        }
        else if (!willRegFacade.libPath.empty() &&
                 !willRegFacade.facadeName.empty()) // Create facade from dynamic library.
        {
            // Open facade library(if cached, reuse it).
            bool existingLib;
            const LLBC_String &libPath = willRegFacade.libPath;
            LLBC_Library *lib = OpenFacadeLibrary(libPath, existingLib);
            if (!lib)
            {
                ClearFacadesWhenInitFacadeFailed();
                initSuccess = false;

                break;
            }

            // Get facade create entry function, create it.
            LLBC_String facadeCreateFuncName;
            const LLBC_String &facadeName = willRegFacade.facadeName;
            facadeCreateFuncName.format("%s%s", LLBC_CFG_COMM_CREATE_FACADE_FROM_LIB_FUNC_PREFIX, facadeName.c_str());
            LLBC_FacadeDynamicCreateFunc facadeCreateFunc = reinterpret_cast<
                LLBC_FacadeDynamicCreateFunc>(lib->GetProcAddress(facadeCreateFuncName.c_str()));
            if (!facadeCreateFunc ||
                !(facade = reinterpret_cast<LLBC_IFacade *>(facadeCreateFunc())))
            {
                ClearFacadesWhenInitFacadeFailed();
                initSuccess = false;

                break;
            }

            // Validate facade class name and giving facadeName is same or not.
            if (UNLIKELY(LLBC_GetTypeName(*facade) != facadeName))
            {
                LLBC_Delete(facade);

                ClearFacadesWhenInitFacadeFailed();
                initSuccess = false;

                LLBC_SetLastError(LLBC_ERROR_ILLEGAL);

                break;
            }
        }

        facade->SetService(this);
        AddFacade(facade);

        if (facade->IsCaredEvents(LLBC_FacadeEvents::OnInitialize) &&
            UNLIKELY(!facade->OnInitialize()))
        {
            ClearFacadesWhenInitFacadeFailed();
            initSuccess = false;

            LLBC_SetLastError(LLBC_ERROR_FACADE_INIT);

            break;
        }

        facade->_inited = true;
    }

    if (initSuccess)
        _willRegFacades.clear();

    _initingFacade = false;

    return initSuccess ? LLBC_OK : LLBC_FAILED;
}

int LLBC_Service::StartFacades()
{
    size_t startIndex = 0;
    const size_t facadesSize = _facades.size();
    for (; startIndex < facadesSize; ++startIndex)
    {
        LLBC_IFacade *facade = _facades[startIndex];
        if (facade->_started)
            continue;

        if (facade->IsCaredEvents(LLBC_FacadeEvents::OnStart) && 
            !facade->OnStart())
            break;

        facade->_started = true;
    }

    if (startIndex == facadesSize)
        return LLBC_OK;

    StopFacades();
    LLBC_SetLastError(LLBC_ERROR_FACADE_START);

    return LLBC_FAILED;
}

void LLBC_Service::StopFacades()
{
    for (_Facades::reverse_iterator it = _facades.rbegin();
         it != _facades.rend();
         ++it)
    {
        LLBC_IFacade *facade = *it;
        if (!facade->_started)
            continue;

        if (facade->IsCaredEvents(LLBC_FacadeEvents::OnStop))
            facade->OnStop();

        facade->_started = false;
    }
}

void LLBC_Service::UpdateFacades()
{
    _Facades *&caredFacadesPtr = _caredEventFacades[LLBC_FacadeEventsOffset::OnUpdate];
    if (!caredFacadesPtr)
        return;

    _Facades &caredFacades = *caredFacadesPtr;
    const size_t facadesSize = caredFacades.size();
    for (size_t facadeIdx = 0; facadeIdx != facadesSize; ++facadeIdx)
        caredFacades[facadeIdx]->OnUpdate();
}

void LLBC_Service::DestroyFacades()
{
    for (_Facades::reverse_iterator it = _facades.rbegin();
         it != _facades.rend();
         ++it)
    {
        LLBC_IFacade *facade = *it;
        if (!facade->_inited)
            continue;

        if (facade->IsCaredEvents(LLBC_FacadeEventsOffset::OnDestroy))
            facade->OnDestroy();
    }

    LLBC_STLHelper::DeleteContainer(_facades, true, true);
    _facades2.clear();

    for (int evOffset = LLBC_FacadeEventsOffset::Begin;
         evOffset != LLBC_FacadeEventsOffset::End;
         ++evOffset)
    {
        _Facades *&evFacades = _caredEventFacades[evOffset];
        LLBC_XDelete(evFacades);
    }

    LLBC_STLHelper::DeleteContainer(_facadeLibraries);
}

void LLBC_Service::DestroyWillRegFacades()
{
    for (_WillRegFacades::iterator it = _willRegFacades.begin();
         it != _willRegFacades.end();
         ++it)
    {
        LLBC_XDelete((*it).facade);
        LLBC_XDelete((*it).facadeFactory);
    }

    _willRegFacades.clear();
}

void LLBC_Service::AddFacade(LLBC_IFacade *facade)
{
    // Add facade to _facades(vector)
    _facades.push_back(facade);

    // Add facade to _facades2(map<type, vector>)
    const LLBC_String facadeName = LLBC_GetTypeName(*facade);
    _Facades2::iterator facadesIt = _facades2.find(facadeName);
    if (facadesIt == _facades2.end())
        facadesIt = _facades2.insert(std::make_pair(facadeName, _Facades())).first;
    _Facades2::mapped_type &typeFacades = facadesIt->second;
    typeFacades.push_back(facade);

    // Add facade to cared events array
    AddFacadeToCaredEventsArray(facade);
}

void LLBC_Service::AddFacadeToCaredEventsArray(LLBC_IFacade *facade)
{
    for (int evOffset = LLBC_FacadeEventsOffset::Begin;
         evOffset != LLBC_FacadeEventsOffset::End;
         ++evOffset)
    {
        if (!facade->IsCaredEventOffset(evOffset))
            continue;

        _Facades *&evFacades = _caredEventFacades[evOffset];
        if (evFacades == NULL)
            evFacades = new _Facades();
        evFacades->push_back(facade);
    }
}

LLBC_Library *LLBC_Service::OpenFacadeLibrary(const LLBC_String &libPath, bool &existingLib)
{
    existingLib = false;

    LLBC_Library *lib = NULL;
    _FacadeLibraries::iterator libIt = _facadeLibraries.find(libPath);
    if (libIt != _facadeLibraries.end())
    {
        existingLib = true;
        return libIt->second;
    }

    lib = LLBC_New(LLBC_Library);
    if (lib->Open(libPath.c_str()) != LLBC_OK)
    {
        LLBC_Delete(lib);
        return NULL;
    }

    _facadeLibraries.insert(std::make_pair(libPath, lib));

    return lib;
}

void LLBC_Service::ClearFacadesWhenInitFacadeFailed()
{
    StopFacades();
    DestroyFacades();
    DestroyWillRegFacades();
}

 void LLBC_Service::InitAutoReleasePool()
{
    _releasePoolStack = LLBC_AutoReleasePoolStack::GetCurrentThreadReleasePoolStack();
}

void LLBC_Service::UpdateAutoReleasePool()
{
    if (_driveMode == This::SelfDrive)
        _releasePoolStack->Purge();
}

void LLBC_Service::ClearAutoReleasePool()
{
    _releasePoolStack = NULL;
}

void LLBC_Service::InitObjectPools()
{
}

void LLBC_Service::UpdateObjectPools()
{
}

void LLBC_Service::ClearHoldedObjectPools()
{
}

void LLBC_Service::InitTimerScheduler()
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

void LLBC_Service::ClearHoldedTimerScheduler()
{
    _timerScheduler = NULL;
}

void LLBC_Service::ProcessIdle(bool fullFrame)
{
    _Facades *&caredEvsPtr = _caredEventFacades[LLBC_FacadeEventsOffset::OnIdle];
    if (!caredEvsPtr)
        return;

    _Facades &caredEvs = *caredEvsPtr;
    const size_t facadesSize = caredEvs.size();
    for (size_t facadeIdx = 0; facadeIdx != facadesSize; ++facadeIdx)
    {
        if (fullFrame)
        {
            sint64 elapsed = LLBC_GetMilliSeconds() - _begHeartbeatTime;
            if (LIKELY(elapsed >= 0))
            {
                if (elapsed >= _frameInterval)
                    break;

                caredEvs[facadeIdx]->OnIdle(static_cast<int>(_frameInterval - elapsed));
            }
        }
        else
        {
            caredEvs[facadeIdx]->OnIdle(0);
        }
    }
}

int LLBC_Service::LockableSend(LLBC_Packet *packet,
                               bool lock,
                               bool validCheck)
{
    // Lock if need.
    if (lock)
        _lock.Lock();

    // Started or not check.
    if (UNLIKELY(!_started))
    {
        if (lock)
            _lock.Unlock();

        LLBC_Recycle(packet);

        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    // Validate check, if need.
    const _ReadySessionInfo *readySInfo;
    const int sessionId = packet->GetSessionId();
    if (!_fullStack || validCheck)
    {
        _ReadySessionInfosCIter readySInfoIt;

        // Check _ReadySessionInfo exist or not.
        _readySessionInfosLock.Lock();
        if ((readySInfoIt = _readySessionInfos.find(sessionId)) == _readySessionInfos.end())
        {
            _readySessionInfosLock.Unlock();

            if (lock)
                _lock.Unlock();

            LLBC_Recycle(packet);
            LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);

            return LLBC_FAILED;
        }

        // Listen session check(not allow send packet to listen session).
        readySInfo = readySInfoIt->second;
        if (UNLIKELY(readySInfo->isListenSession))
        {
            _readySessionInfosLock.Unlock();

            if (lock)
                _lock.Unlock();

            LLBC_Recycle(packet);
            LLBC_SetLastError(LLBC_ERROR_IS_LISTEN_SOCKET);

            return LLBC_FAILED;
        }

        // If enabled full-stack option, unlock _readySessionInfosLock.
        if (_fullStack)
            _readySessionInfosLock.Unlock();
    }

    // Set sender service Id.
    packet->SetSenderServiceId(_id);

    // If enabled full-stack option, send packet and return.
    if (_fullStack)
    {
        const int ret = _pollerMgr.Send(packet);
        if (lock)
            _lock.Unlock();

        return ret;
    }

    // Not enabled full-stack option, call codec protocol-stack to encode packet.
    bool removeSession;
    LLBC_Packet *encoded;
    if (readySInfo->codecStack->SendCodec(packet, encoded, removeSession) != LLBC_OK)
    {
        _readySessionInfosLock.Unlock();
        if (removeSession)
            RemoveSession(sessionId, LLBC_FormatLastError());

        if (lock)
            _lock.Unlock();

        return LLBC_FAILED;
    }

    // Unlock _readySessionInfosLock.
    _readySessionInfosLock.Unlock();

    // Send encoded packet.
    const int ret = _pollerMgr.Send(encoded);
    if (lock)
        _lock.Unlock();

    return ret;
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
    // Create packet(from object pool) and send.
    LLBC_Packet *packet = _packetObjectPool.GetObject();
    // packet->SetSenderServiceId(_id); // LockableSend(LLBC_Packet *, bool bool) function will set sender service Id.
    packet->SetHeader(svcId, sessionId, opcode, status);
    int ret = packet->Write(bytes, len);
    if (UNLIKELY(ret != LLBC_OK))
    {
        LLBC_Recycle(packet);
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
            LLBC_Recycle(coder);

        return LLBC_OK;
    }

    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(!_started))
    {
        if (LIKELY(coder))
            LLBC_Recycle(coder);

        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    typename SessionIds::const_iterator sessionIt = sessionIds.begin();

    LLBC_Packet *firstPacket = _packetObjectPool.GetObject();
    // firstPacket->SetSenderServiceId(_id); // LockableSend(LLBC_Packet *, bool, bool) function will set sender service Id.
    firstPacket->SetHeader(svcId, *sessionIt++, opcode, status);

    bool hasCoder = true;
    if (LIKELY(coder))
    {
        firstPacket->SetEncoder(coder);
        if (!firstPacket->Encode())
        {
            LLBC_Recycle(firstPacket);
            LLBC_SetLastError(LLBC_ERROR_ENCODE);

            return LLBC_FAILED;
        }
    }
    else
    {
        hasCoder = false;
    }

    typename SessionIds::size_type sessionCnt = sessionIds.size();
    if (sessionCnt == 1)
        return LockableSend(firstPacket, false, validCheck);

    _multicastOtherPackets.resize(sessionCnt - 1);
    if (LIKELY(hasCoder))
    {
        const void *payload = firstPacket->GetPayload();
        const size_t payloadLen = firstPacket->GetPayloadLength();

        if (validCheck)
            _readySessionInfosLock.Lock();

        _ReadySessionInfosCIter readySInfoIt;
        for (typename SessionIds::size_type i = 1;
             i < sessionCnt;
             ++i)
        {
            const int sessionId = *sessionIt++;
            if (validCheck &&
                (readySInfoIt = _readySessionInfos.find(sessionId)) == _readySessionInfos.end())
                    continue;

            const _ReadySessionInfo * const &readySInfo = readySInfoIt->second;
            if (readySInfo->isListenSession)
                continue;

            LLBC_Packet *otherPacket = _packetObjectPool.GetObject();
            // otherPacket->SetSenderServiceId(_id); // LockableSend(LLBC_Packet *, bool, bool) function will set sender service Id.
            otherPacket->SetHeader(svcId, sessionId, opcode, status);
            otherPacket->Write(payload, payloadLen);

            _multicastOtherPackets[i - 1] = otherPacket;
        }

        if (validCheck)
            _readySessionInfosLock.Unlock();
    }
    else
    {
        if (validCheck)
            _readySessionInfosLock.Lock();

        _ReadySessionInfosCIter readySInfoIt;
        for (typename SessionIds::size_type i = 1;
             i < sessionCnt;
             ++i)
        {
            const int sessionId = *sessionIt++;
            if (validCheck &&
                (readySInfoIt = _readySessionInfos.find(sessionId)) == _readySessionInfos.end())
                continue;

            const _ReadySessionInfo *const &readySInfo = readySInfoIt->second;
            if (readySInfo->isListenSession)
                continue;

            LLBC_Packet *otherPacket = _packetObjectPool.GetObject();
            // otherPacket->SetSenderServiceId(_id); // LockableSend(LLBC_Packet *, bool, bool) function will set sender service Id.
            otherPacket->SetHeader(svcId, sessionId, opcode, status);

            _multicastOtherPackets[i - 1] = otherPacket;
        }

        if (validCheck)
            _readySessionInfosLock.Unlock();
    }

    LockableSend(firstPacket, false, validCheck); // Use pass "validCheck" argument to call LockableSend().

    const typename SessionIds::size_type otherPacketCnt = sessionCnt - 1;
    for (typename SessionIds::size_type i = 0; i != otherPacketCnt; ++i)
    {
        LLBC_Packet *&otherPacket = _multicastOtherPackets[i];
        if (UNLIKELY(!otherPacket))
            continue;

        LockableSend(otherPacket, false, false); // Don't need vaildate check.
    }

    _multicastOtherPackets.clear();

    return LLBC_OK;
}

LLBC_Service::_ReadySessionInfo::_ReadySessionInfo(int sessionId, int acceptSessionId, bool isListenSession, LLBC_ProtocolStack *codecStack)
{
    this->sessionId = sessionId;
    this->acceptSessionId = acceptSessionId;
    this->isListenSession = isListenSession;
    this->codecStack = codecStack;
}

LLBC_Service::_ReadySessionInfo::~_ReadySessionInfo()
{
    if (codecStack)
        LLBC_Delete(codecStack);
}

LLBC_Service::_WillRegFacade::_WillRegFacade(LLBC_IFacade *facade)
{
    this->facade = facade;
    facadeFactory = NULL;
}

LLBC_Service::_WillRegFacade::_WillRegFacade(LLBC_IFacadeFactory *facadeFactory)
{
    facade = NULL;
    this->facadeFactory = facadeFactory;
}

LLBC_Service::_WillRegFacade::_WillRegFacade(const LLBC_String &libPath, const LLBC_String &facadeName)
{
    this->facade = NULL;
    this->facadeFactory = NULL;

    this->libPath = libPath;
    this->facadeName = facadeName;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
