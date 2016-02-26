/**
 * @file    Service.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/16
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/ICoder.h"
#include "llbc/comm/Packet.h"
#include "llbc/comm/PollerType.h"
#include "llbc/comm/protocol/IProtocol.h"
#include "llbc/comm/protocol/IProtocolFilter.h"
#include "llbc/comm/protocol/ProtocolStack.h"
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

LLBC_Service::LLBC_Service(This::Type type)
: _type(type)
, _id(0)
, _driveMode(This::SelfDrive)

, _started(false)
, _stopping(false)

, _lock()

, _fps(LLBC_CFG_COMM_DFT_SERVICE_FPS)
, _frameInterval(1000 / LLBC_CFG_COMM_DFT_SERVICE_FPS)
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

, _releasePoolStack(NULL)

, _timerScheduler(NULL)

, _evManager()

, _svcMgr(*LLBC_ServiceMgrSingleton)
{
    // Get the poller type from Config.h
    const char *pollerModel = LLBC_CFG_COMM_POLLER_MODEL;
    const int pollerType = LLBC_PollerType::Str2Type(pollerModel);
    ASSERT (LLBC_PollerType::IsValid(pollerType) && "Invalid LLBC_CFG_COMM_POLLER_MODEL config!");

    _pollerMgr.SetService(this);
    _pollerMgr.SetPollerType(pollerType);

    // MSVC earlier compiler may not support initialize array in construct list.
#if LLBC_CUR_COMP == LLBC_COMP_MSVC && LLBC_COMP_VER < 1400
    LLBC_MemSet(_filters, 0, sizeof(_filters));
#endif

    // Create protocol stack.
#if !LLBC_CFG_COMM_USE_FULL_STACK
    this->CreateCodecStack(&_stack);
#endif
}

// If using MSVC compiler and version >= 1400(VS2005), Reset C4351 warning to default.
#if LLBC_CUR_COMP == LLBC_COMP_MSVC && LLBC_COMP_VER >= 1400
# pragma warning(default:4351)
#endif

LLBC_Service::~LLBC_Service()
{
    this->Stop();

    LLBC_STLHelper::DeleteContainer(_facades);
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
    this->DestroyFrameTasks(_beforeFrameTasks, _handlingBeforeFrameTasks);
    this->DestroyFrameTasks(_afterFrameTasks, _handlingAfterFrameTasks);
}

int LLBC_Service::GetId() const
{
    return _id;
}

int LLBC_Service::SetId(int id)
{
    if (_svcMgr.GetService(id))
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }
    else if (_started)
    {
        LLBC_SetLastError(LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }

    LLBC_Guard guard(_lock);
    if (_started)
    {
        LLBC_SetLastError(LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }

    _id = id;
    return LLBC_OK;
}

This::Type LLBC_Service::GetType() const
{
    return _type;
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

    LLBC_Guard guard(_lock);
    if (_started)
    {
        LLBC_SetLastError(LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }

    if ((_driveMode = mode) == This::ExternalDrive)
        this->GetTimerScheduler();

    return LLBC_FAILED;
}

int LLBC_Service::Start(int pollerCount)
{
    if (pollerCount <= 0)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }
    else if (_svcMgr.GetService(_id))
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }
    else if (_started)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    LLBC_Guard guard(_lock);
    if (_started)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    if (_pollerMgr.Start(pollerCount) != LLBC_OK)
        return LLBC_FAILED;

    if (_driveMode == This::ExternalDrive)
    {
        if (!this->IsCanContinueDriveService())
        {
            LLBC_SetLastError(LLBC_ERROR_LIMIT);
            _pollerMgr.Stop();

            return LLBC_FAILED;
        }

        this->AddServiceToTls();
    }
    else
    {
        if (this->Activate(1) != LLBC_OK)
        {
            _pollerMgr.Stop();
            return LLBC_FAILED;
        }
    }

    _started = true;

    if (_driveMode == This::ExternalDrive)
        this->InitFacades();

    return LLBC_OK;
}

bool LLBC_Service::IsStarted() const
{
    return _started;
}

void LLBC_Service::Stop()
{
    LLBC_Guard guard(_lock);

    if (!_started || _stopping)
        return;

    _stopping = true;
    if (_driveMode == This::SelfDrive)
        while (_started)
            LLBC_ThreadManager::Sleep(20);
    else
    {
        if (_sinkIntoLoop)
        {
            _afterStop = true;
            return;
        }
        else
        {
            this->Cleanup();
        }
    }

    _stopping = false;
}

int LLBC_Service::GetFPS() const
{
    This *ncThis = const_cast<This *>(this);
    LLBC_Guard guard(ncThis->_lock);
    return _fps;
}

int LLBC_Service::SetFPS(int fps)
{
    if (LLBC_CFG_COMM_MIN_SERVICE_FPS <= fps &&
            fps <= LLBC_CFG_COMM_MAX_SERVICE_FPS)
    {
        LLBC_Guard guard(_lock);

        _fps = fps;
        _frameInterval = 1000 / _fps;
        return LLBC_OK;
    }

    LLBC_SetLastError(LLBC_ERROR_LIMIT);
    return LLBC_FAILED;
}

int LLBC_Service::GetFrameInterval() const
{
    This *ncThis = const_cast<This *>(this);

    LLBC_Guard guard(ncThis->_lock);
    return _frameInterval;
}

int LLBC_Service::Listen(const char *ip, uint16 port)
{
    LLBC_Guard guard(_lock);
    return _pollerMgr.Listen(ip, port);
}

int LLBC_Service::Connect(const char *ip, uint16 port)
{
    LLBC_Guard guard(_lock);
    const int sessionId = _pollerMgr.Connect(ip, port);
    if (sessionId != 0)
    {
        _connectedSessionIdsLock.Lock();
        _connectedSessionIds.insert(sessionId);
        _connectedSessionIdsLock.Unlock();
    }

    return sessionId;
}

int LLBC_Service::AsyncConn(const char *ip, uint16 port)
{
    LLBC_Guard guard(_lock);
    return _pollerMgr.AsyncConn(ip, port);
}

int LLBC_Service::Send(LLBC_Packet *packet)
{
    // Call internal Lockable() to complete.
    // lock = true
    // validCheck = true
    return this->LockableSend(packet);
}

int LLBC_Service::Send2(int sessionId, int opcode, LLBC_ICoder *coder, int status, LLBC_PacketHeaderParts *parts)
{
    return this->Send2(0, sessionId, opcode, coder, status, parts);
}

int LLBC_Service::Send2(int svcId, int sessionId, int opcode, LLBC_ICoder *coder, int status, LLBC_PacketHeaderParts *parts)
{
    LLBC_Packet *packet = LLBC_New(LLBC_Packet);
    packet->SetHeader(svcId, sessionId, opcode, status);
    if (parts)
    {
        if (_type != This::Raw)
            parts->SetToPacket(*packet);

        LLBC_Delete(parts);
    }

    packet->SetEncoder(coder);

    // Call internal LockableSend() to complete.
    // lock = true
    // validCheck = true
    return this->LockableSend(packet);
}

int LLBC_Service::Send2(int sessionId, int opcode, const void *bytes, size_t len, int status, LLBC_PacketHeaderParts *parts)
{
    // Call internal LockableSend() to complete.
    // lock = true
    // validCheck = true
    const int ret = this->LockableSend(0, sessionId, opcode, bytes, len, status, parts);
    if (parts)
        LLBC_Delete(parts);

    return ret;
}

int LLBC_Service::Send2(int svcId, int sessionId, int opcode, const void *bytes, size_t len, int status, LLBC_PacketHeaderParts *parts)
{
    // Call internal LockableSend() to complete.
    // lock = true
    // validCheck = true
    const int ret = this->LockableSend(svcId, sessionId, opcode, bytes, len, status, parts);
    if (parts)
        LLBC_Delete(parts);

    return ret;
}

int LLBC_Service::Multicast2(const LLBC_SessionIdList &sessionIds, int opcode, LLBC_ICoder *coder, int status, LLBC_PacketHeaderParts *parts)
{
    return this->Multicast2(0, sessionIds, opcode, coder, status, parts);
}

int LLBC_Service::Multicast2(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, LLBC_ICoder *coder, int status, LLBC_PacketHeaderParts *parts)
{
    // Call internal MulticastSendCoder<> template method to complete.
    // validCheck = true
    const int ret = this->MulticastSendCoder<LLBC_SessionIdList>(svcId, sessionIds, opcode, coder, status, parts);
    if (parts)
        LLBC_Delete(parts);

    return ret;
}

int LLBC_Service::Multicast2(const LLBC_SessionIdList &sessionIds, int opcode, const void *bytes, size_t len, int status, LLBC_PacketHeaderParts *parts)
{
    return this->Multicast2(0, sessionIds, opcode, bytes, len, status, parts);
}

int LLBC_Service::Multicast2(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, const void *bytes, size_t len, int status, LLBC_PacketHeaderParts *parts)
{
    LLBC_Guard guard(_lock);

    // Foreach to call internal method LockableSend() method to complete.
    // lock = false
    // validCheck = true
    for (LLBC_SessionIdListCIter sessionIt = sessionIds.begin();
         sessionIt != sessionIds.end();
         sessionIt++)
        this->LockableSend(svcId, *sessionIt, opcode, bytes, len, status, parts, false);

    if (parts)
        LLBC_Delete(parts);

    return LLBC_OK;
}

int LLBC_Service::Broadcast2(int opcode, LLBC_ICoder *coder, int status, LLBC_PacketHeaderParts *parts)
{
    return this->Broadcast2(0, opcode, coder, status, parts);
}

int LLBC_Service::Broadcast2(int svcId, int opcode, LLBC_ICoder *coder, int status, LLBC_PacketHeaderParts *parts)
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
    const int ret = this->MulticastSendCoder<LLBC_SessionIdList>(svcId, connectedSessionIds, opcode, coder, status, parts, false);
    if (parts)
        LLBC_Delete(parts);

    return ret;
}

int LLBC_Service::Broadcast2(int opcode, const void *bytes, size_t len, int status, LLBC_PacketHeaderParts *parts)
{
    return this->Broadcast2(0, opcode, bytes, len ,status, parts);
}

int LLBC_Service::Broadcast2(int svcId, int opcode, const void *bytes, size_t len , int status, LLBC_PacketHeaderParts *parts)
{
    LLBC_Guard guard(_lock);

    // Foreach to call internal method LockableSend() method to complete.
    // lock = false
    // validCheck = false
    LLBC_Guard connSIdsGuard(_connectedSessionIdsLock);
    for (LLBC_SessionIdSetCIter sessionIt = _connectedSessionIds.begin();
         sessionIt != _connectedSessionIds.end();
         sessionIt++)
        this->LockableSend(svcId, *sessionIt, opcode, bytes, len, status, parts, false, false);

    if (parts)
        LLBC_Delete(parts);

    return LLBC_OK;
}

int LLBC_Service::RemoveSession(int sessionId)
{
    LLBC_Guard guard(_lock);
    if (!_started)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    LLBC_Guard connSIdsGuard(_connectedSessionIdsLock);

    LLBC_SessionIdSetIter sessionIdIt = _connectedSessionIds.find(sessionId);
    if (sessionIdIt == _connectedSessionIds.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    _pollerMgr.Close(sessionId);
    _connectedSessionIds.erase(sessionIdIt);

    return LLBC_OK;
}

int LLBC_Service::RegisterFacade(LLBC_IFacade *facade)
{
    if (UNLIKELY(!facade))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_Guard guard(_lock);
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

    return LLBC_OK;
}

int LLBC_Service::RegisterCoder(int opcode, LLBC_ICoderFactory *coder)
{
    if (UNLIKELY(!coder))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_Guard guard(_lock);
    if (UNLIKELY(_started))
    {
        LLBC_SetLastError(LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }
    else if (!_coders.insert(std::make_pair(opcode, coder)).second)
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

#if !LLBC_CFG_COMM_USE_FULL_STACK
    _stack.AddCoder(opcode, coder);
#endif // !LLBC_CFG_COMM_USE_FULL_STACK

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

    LLBC_Guard guard(_lock);
    if (UNLIKELY(_started))
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

int LLBC_Service::Subscribe(int opcode, LLBC_IDelegate1<LLBC_Packet &> *deleg)
{
    if (UNLIKELY(!deleg))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_Guard guard(_lock);
    if (UNLIKELY(_started))
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

int LLBC_Service::PreSubscribe(int opcode, LLBC_IDelegateEx<LLBC_Packet &> *deleg)
{
    if (UNLIKELY(!deleg))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_Guard guard(_lock);
    if (UNLIKELY(_started))
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
int LLBC_Service::UnifyPreSubscribe(LLBC_IDelegateEx<LLBC_Packet &> *deleg)
{
    if (UNLIKELY(!deleg))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_Guard guard(_lock);
    if (UNLIKELY(_started))
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
int LLBC_Service::SubscribeStatus(int opcode, int status, LLBC_IDelegate1<LLBC_Packet &> *deleg)
{
    if (UNLIKELY(!deleg))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_Guard guard(_lock);
    if (UNLIKELY(_started))
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

    LLBC_Guard guard(_lock);
    if (_started)
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
    LLBC_Guard guard(_lock);
    if (_started)
    {
        LLBC_SetLastError(LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }

    _timerScheduler->SetEnabled(true);
    return LLBC_OK;
}

int LLBC_Service::DisableTimerScheduler()
{
    LLBC_Guard guard(_lock);
    if (_started)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    _timerScheduler->SetEnabled(false);
    return LLBC_OK;
}

LLBC_ListenerStub LLBC_Service::SubscribeEvent(int event, LLBC_IDelegate1<LLBC_Event *> *deleg)
{
    if (UNLIKELY(!deleg))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_INVALID_LISTENER_STUB;
    }

    const LLBC_ListenerStub stub = LLBC_GUIDHelper::GenStr();
    this->Push(LLBC_SvcEvUtil::BuildSubscribeEvEv(event, stub, deleg));

    return stub;
}

void LLBC_Service::UnsubscribeEvent(int event)
{
    this->Push(LLBC_SvcEvUtil::
        BuildUnsubscribeEvEv(event, LLBC_INVALID_LISTENER_STUB));
}

void LLBC_Service::UnsubscribeEvent(const LLBC_ListenerStub &stub)
{
    this->Push(LLBC_SvcEvUtil::
        BuildUnsubscribeEvEv(0, stub));
}

void LLBC_Service::FireEvent(LLBC_Event *ev)
{
    this->Push(LLBC_SvcEvUtil::BuildFireEvEv(ev));
}

int LLBC_Service::Post(LLBC_IDelegate1<LLBC_Service::Base *> *deleg)
{
    if (UNLIKELY(!deleg))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_Guard guard(_lock);
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
    _sinkIntoLoop = true;

    // Record begin heartbeat time.
    _begHeartbeatTime = LLBC_GetMilliSeconds();

    // Handle before frame-tasks.
    this->HandleFrameTasks(_beforeFrameTasks, _handlingBeforeFrameTasks);
    _handledBeforeFrameTasks = true;

    // Process queued events.
    this->HandleQueuedEvents();

    // Update all compoments.
    this->UpdateFacades();
    this->UpdateTimers();
    this->UpdateAutoReleasePool();

    // Handle after frame-tasks.
    this->HandleFrameTasks(_afterFrameTasks, _handlingAfterFrameTasks);
    _handledBeforeFrameTasks = false;

    // Process Idle.
    this->ProcessIdle();

    // Sleep FrameInterval - ElapsedTime milli-seconds, if need.
    if (fullFrame)
    {
        const sint64 elapsed = LLBC_GetMilliSeconds() - _begHeartbeatTime;
        if (elapsed >= 0 && elapsed < _frameInterval)
            LLBC_Sleep(static_cast<int>(_frameInterval - elapsed));
    }

    _sinkIntoLoop = false;
    if (_afterStop)
    {
        this->Cleanup();
        _stopping = false;
    }
}

LLBC_ProtocolStack *LLBC_Service::CreateRawStack(LLBC_ProtocolStack *stack)
{
    if (!stack)
    {
        stack = LLBC_New1(_Stack, _Stack::RawStack);
        stack->SetService(this);
    }

    if (_type == This::Raw)
    {
        stack->AddProtocol(LLBC_IProtocol::Create<LLBC_RawProtocol>(_filters[LLBC_ProtocolLayer::PackLayer]));
    }
    else
    {
        stack->AddProtocol(LLBC_IProtocol::Create<LLBC_PacketProtocol>(_filters[LLBC_ProtocolLayer::PackLayer]));
        stack->AddProtocol(LLBC_IProtocol::Create<LLBC_CompressProtocol>(_filters[LLBC_ProtocolLayer::CompressLayer]));
    }

    return stack;
}

LLBC_ProtocolStack *LLBC_Service::CreateCodecStack(LLBC_ProtocolStack *stack)
{
    if (!stack)
    {
        stack = LLBC_New1(_Stack, _Stack::CodecStack);
        stack->SetService(this);
    }

    if (_type != This::Raw)
    {
        stack->AddProtocol(LLBC_IProtocol::Create<LLBC_CodecProtocol>(_filters[LLBC_ProtocolLayer::CodecLayer]));
        for (_Coders::iterator it = _coders.begin();
             it != _coders.end();
             it++)
            stack->AddCoder(it->first, it->second);
    }

    return stack;
}

LLBC_ProtocolStack *LLBC_Service::CreateFullStack()
{
    _Stack *stack = LLBC_New1(_Stack, _Stack::FullStack);
    stack->SetService(this);

    return this->CreateRawStack(
            this->CreateCodecStack(stack));
}

void LLBC_Service::Svc()
{
    while (!_started)
        LLBC_Sleep(20);

    _lock.Lock();
    this->AddServiceToTls();
    this->GetTimerScheduler();
    this->CreateAutoReleasePool();
    this->InitFacades();
    _lock.Unlock();

    _svcMgr.OnServiceStart(this);

    while (!_stopping)
        this->OnSvc();
}

void LLBC_Service::Cleanup()
{
    _pollerMgr.Stop();

    LLBC_ServiceEvent *ev;
    LLBC_MessageBlock *block;
    while (this->TryPop(block) == LLBC_OK)
    {
        // Skip event type.
        block->ShiftReadPos(sizeof(int));
        block->Read(&ev, sizeof(ev));

        LLBC_Delete(ev);
        LLBC_Delete(block);
    }

    _connectedSessionIdsLock.Lock();
    _connectedSessionIds.clear();
    _connectedSessionIdsLock.Unlock();

    this->DestroyFacades();
    this->DestroyAutoReleasePool();
    this->RemoveServiceFromTls();

    if (_driveMode == This::SelfDrive)
        _svcMgr.OnServiceStop(this);

    _started = false;
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
    usingFlag = true;
    for (_FrameTasks::iterator it = tasks.begin();
         it != tasks.end();
         it++)
        (*it)->Invoke(this);

    this->DestroyFrameTasks(tasks, usingFlag);
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
    while (this->TryPop(block) == LLBC_OK)
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
    info.SetSessionId(ev.sessionId);
    info.SetIsListenSession(ev.isListen);
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

    _connectedSessionIdsLock.Lock();
    _connectedSessionIds.erase(ev.sessionId);
    _connectedSessionIdsLock.Unlock();

    for (_Facades::iterator it = _facades.begin();
         it != _facades.end();
         it++)
        (*it)->OnSessionDestroy(ev.sessionId);
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
    if (UNLIKELY(_stack.RecvCodec(packet, packet) != LLBC_OK))
    {
        this->RemoveSession(sessionId);
        return;
    }
#endif

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
            {
                this->RemoveSession(sessionId);
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
            this->RemoveSession(sessionId);
            return;
        }
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
            (*facadeIt)->OnUnHandledPacket(packet->GetOpcode());
    }
}

void LLBC_Service::HandleEv_ProtoReport(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_ProtoReport _Ev;
    _Ev &ev = static_cast<_Ev &>(_);

    LLBC_ProtoReport report;
    report.SetSessionId(ev.sessionId);
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
    for (_Facades::iterator it = _facades.begin();
         it != _facades.end();
         it++)
        (*it)->OnInitialize();
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
        (*it)->OnDestroy();
}

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

void LLBC_Service::ProcessIdle()
{
    for (_Facades::iterator it = _facades.begin();
         it != _facades.end();
         it++)
    {
        sint64 elapsed = LLBC_GetMilliSeconds() - _begHeartbeatTime;
        if (LIKELY(elapsed >= 0))
        {
            if (elapsed >= _frameInterval)
                break;

            (*it)->OnIdle(static_cast<int>(_frameInterval - elapsed));
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
        LLBC_Guard connSIdsGuard(_connectedSessionIdsLock);
        if (_connectedSessionIds.find(sessionId) == _connectedSessionIds.end())
        {
            if (lock)
                _lock.Unlock();

            LLBC_Delete(packet);

            LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
            return LLBC_FAILED;
        }
    }

#if !LLBC_CFG_COMM_USE_FULL_STACK
    LLBC_Packet *encoded;
    if (_stack.SendCodec(packet, encoded) != LLBC_OK)
    {
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
                               const LLBC_PacketHeaderParts *parts,
                               bool lock,
                               bool validCheck)
{
    LLBC_Packet *packet = LLBC_New(LLBC_Packet);
    packet->SetHeader(svcId, sessionId, opcode, status);
    if (parts && _type != This::Raw)
        parts->SetToPacket(*packet);

    int ret = packet->Write(bytes, len);
    if (UNLIKELY(ret != LLBC_OK))
    {
        LLBC_Delete(packet);
        return ret;
    }

    return this->LockableSend(packet, lock, validCheck);
}

template <typename SessionIds>
int LLBC_Service::MulticastSendCoder(int svcId,
                                     const SessionIds &sessionIds,
                                     int opcode,
                                     LLBC_ICoder *coder,
                                     int status,
                                     const LLBC_PacketHeaderParts *parts,
                                     bool validCheck)
{
    LLBC_Guard guard(_lock);
    if (UNLIKELY(!_started))
    {
        if (LIKELY(coder))
            LLBC_Delete(coder);

        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    if (sessionIds.empty())
    {
        if(LIKELY(coder))
            LLBC_Delete(coder);

        return LLBC_OK;
    }

    typename SessionIds::const_iterator sessionIt = sessionIds.begin();

    LLBC_Packet *firstPacket = LLBC_New(LLBC_Packet);
    firstPacket->SetHeader(svcId, *sessionIt++, opcode, status);
    if (parts && _type != This::Raw)
        parts->SetToPacket(*firstPacket);

    bool hasCoder = true;
    if (LIKELY(coder))
    {
        coder->Encode(*firstPacket);
        LLBC_Delete(coder);
    }
    else
    {
        hasCoder = false;
    }

    typename SessionIds::size_type sessionCnt = sessionIds.size();
    if (sessionCnt == 1)
        return this->LockableSend(firstPacket, false, validCheck);

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
            otherPacket->SetHeader(svcId, sessionId, opcode, status);
            if (parts && _type != This::Raw)
                parts->SetToPacket(*otherPacket);

            otherPackets[i - 1] = otherPacket;
        }

        if (validCheck)
            _connectedSessionIdsLock.Unlock();
    }

    this->LockableSend(firstPacket, false, validCheck); // Use pass "validCheck" argument to call LockableSend().

    const size_t otherPacketCnt = sessionCnt - 1;
    for (register size_t i = 0; i < otherPacketCnt; i++)
    {
        LLBC_Packet *otherPacket = otherPackets[i];
        if (!otherPacket)
            continue;

        this->LockableSend(otherPacket, false, false); // Don't need vaildate check.
    }

    LLBC_Free(otherPackets);

    return LLBC_OK;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
