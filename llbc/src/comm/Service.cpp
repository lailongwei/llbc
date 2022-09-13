/*
 * @Author: Your name
 * @Date:   2021-01-14 09:44:17
 * @Last Modified by:   Your name
 * @Last Modified time: 2021-01-14 09:51:23
 */
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

#include "llbc/comm/Packet.h"
#include "llbc/comm/PollerType.h"
#include "llbc/comm/protocol/IProtocol.h"
#include "llbc/comm/protocol/ProtocolStack.h"
#include "llbc/comm/protocol/RawProtocolFactory.h"
#include "llbc/comm/protocol/NormalProtocolFactory.h"
#include "llbc/comm/Component.h"
#include "llbc/comm/Service.h"
#include "llbc/comm/ServiceMgr.h"

namespace
{
    typedef LLBC_NS LLBC_Service This;
    typedef LLBC_NS LLBC_ProtocolStack _Stack;
}

__LLBC_NS_BEGIN

int LLBC_Service::_maxId = 1;

LLBC_Service::_EvHandler LLBC_Service::_evHandlers[LLBC_ServiceEventType::End] = 
{
    &LLBC_Service::HandleEv_SessionCreate,
    &LLBC_Service::HandleEv_SessionDestroy,
    &LLBC_Service::HandleEv_AsyncConnResult,
    &LLBC_Service::HandleEv_DataArrival,
    &LLBC_Service::HandleEv_ProtoReport,

    &LLBC_Service::HandleEv_SubscribeEv,
    &LLBC_Service::HandleEv_UnsubscribeEv,
    &LLBC_Service::HandleEv_FireEv,

    &LLBC_Service::HandleEv_AppCfgReload,
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
, _initingComp(false)

, _lock()
, _protoLock()

, _fps(LLBC_CFG_COMM_DFT_SERVICE_FPS)
, _frameInterval(1000 / LLBC_CFG_COMM_DFT_SERVICE_FPS)
#if LLBC_CFG_COMM_ENABLE_SERVICE_FRAME_TIMEOUT
, _frameTimeout(LLBC_INFINITE)
#endif // LLBC_CFG_COMM_ENABLE_SERVICE_FRAME_TIMEOUT
, _relaxTimes(0)
, _begHeartbeatTime(0)
, _sinkIntoLoop(false)
, _afterStop(false)

, _pollerMgr()
, _readySessionInfos()
, _readySessionInfosLock()

, _willRegComps()

, _compsInitFinished(0)
, _compsInitRet(LLBC_OK)
, _compsStartFinished(0)
, _compsStartRet(LLBC_OK)

, _comps()
, _comps2()
, _caredEventComps{}
, _coders()
, _handlers()
, _preHandlers()
#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
, _unifyPreHandler()
#endif
#if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
, _statusHandlers()
#endif
#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
, _statusDescs()
#endif

, _frameTaskIdx(0)
, _frameTasks{}

, _releasePoolStack(nullptr)

, _packetObjectPool(*_safetyObjectPool.GetPoolInst<LLBC_Packet>())
, _msgBlockObjectPool(*_safetyObjectPool.GetPoolInst<LLBC_MessageBlock>())

, _timerScheduler(nullptr)

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
        ASSERT(_protoFactory != nullptr && "Service type is Custom, but not pass Protocol Factory to Service!");
    }

    // Initialize cared event comps array.
    ::memset(_caredEventComps, 0, sizeof(_caredEventComps));

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

    DestroyComps();
    DestroyWillRegComps();
    CloseAllCompLibraries();

    LLBC_STLHelper::DeleteContainer(_coders);
    _handlers.clear();
    _preHandlers.clear();
    #if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    _unifyPreHandler = nullptr;
    #endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

    #if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
    _statusHandlers.clear();
    #endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER

    RemoveAllReadySessions();

    DestroyFrameTasks();

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
        // Waiting for all comp init & start finished.
        if (InitComps() != LLBC_OK || 
            StartComps() != LLBC_OK)
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

        // Waiting for all comps init finished.
        while (!_compsInitFinished)
            LLBC_Sleep(2);
        if (_compsInitRet != LLBC_OK)
        {
            Stop();
            LLBC_SetLastError(LLBC_ERROR_COMP_INIT);

            return LLBC_FAILED;
        }

        // Waiting for all comps start finished.
        while (!_compsStartFinished)
            LLBC_Sleep(2);
        if (_compsStartRet != LLBC_OK)
        {
            Stop();
            LLBC_SetLastError(LLBC_ERROR_COMP_START);

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
            // return;

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

        #if LLBC_CFG_COMM_ENABLE_SERVICE_FRAME_TIMEOUT
        const uint64 ivlInNanos = _frameInterval * LLBC_Time::NumOfNanoSecondsPerMilliSecond;
        if (_frameTimeout != LLBC_INFINITE)
            _frameTimeout = MAX(ivlInNanos, _frameTimeout);
        #endif // LLBC_CFG_COMM_ENABLE_SERVICE_FRAME_TIMEOUT
    }
    else
    {
        _relaxTimes = 0;
        _frameInterval = 0;
        #if LLBC_CFG_COMM_ENABLE_SERVICE_FRAME_TIMEOUT
        _frameTimeout = LLBC_INFINITE;
        #endif // LLBC_CFG_COMM_ENABLE_SERVICE_FRAME_TIMEOUT
    }

    return LLBC_OK;
}

int LLBC_Service::GetFrameInterval() const
{
    This *ncThis = const_cast<This *>(this);

    LLBC_LockGuard guard(ncThis->_lock);
    return _frameInterval;
}

#if LLBC_CFG_COMM_ENABLE_SERVICE_FRAME_TIMEOUT
LLBC_TimeSpan LLBC_Service::GetFrameTimeout() const
{
    return LLBC_TimeSpan::FromMicros(_frameTimeout / static_cast<uint64>(LLBC_Time::NumOfNanoSecondsPerMicroSecond));
}

void LLBC_Service::SetFrameTimeout(const LLBC_TimeSpan &frameTimeout)
{
    const uint64 ivlInNanoSecs = static_cast<uint64>(LLBC_Time::NumOfNanoSecondsPerMilliSecond * _frameInterval);
    _frameTimeout = frameTimeout.GetTotalMicroSeconds() * static_cast<uint64>(LLBC_Time::NumOfNanoSecondsPerMicroSecond);
    _frameTimeout = MAX(ivlInNanoSecs, _frameTimeout);
}
#endif // LLBC_CFG_COMM_ENABLE_SERVICE_FRAME_TIMEOUT

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

int LLBC_Service::Broadcast(int svcId, int opcode, LLBC_Coder *coder, int status)
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

int LLBC_Service::CtrlProtocolStack(int sessionId,
                                    int ctrlCmd,
                                    const LLBC_Variant &ctrlData)
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
                RemoveSession(sessionId,
                              "Protocol stack ctrl finished, business logic require remove this session(Half-Stack mode only)");

            return LLBC_OK;
        }
    }

    _readySessionInfosLock.Unlock();

    _pollerMgr.CtrlProtocolStack(sessionId, ctrlCmd, ctrlData);

    return LLBC_OK;
}

int LLBC_Service::RegisterComponent(LLBC_ComponentFactory *compFactory)
{
    if (UNLIKELY(!compFactory))
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

    for (_WillRegComps::iterator it = _willRegComps.begin();
         it != _willRegComps.end();
         ++it)
    {
        if (it->compFactory != nullptr && it->compFactory == compFactory)
        {
            LLBC_SetLastError(LLBC_ERROR_REPEAT);
            return LLBC_FAILED;
        }
    }

    _willRegComps.push_back(_WillRegComp(compFactory));

    return LLBC_OK;
}

int LLBC_Service::RegisterComponent(LLBC_Component *comp)
{
    if (UNLIKELY(!comp))
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
    else if (std::find(_comps.begin(), 
            _comps.end(), comp) != _comps.end())
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    for (_WillRegComps::iterator regIt = _willRegComps.begin();
         regIt != _willRegComps.end();
         ++regIt)
    {
        if (regIt->comp != nullptr && regIt->comp == comp)
        {
            LLBC_SetLastError(LLBC_ERROR_REPEAT);
            return LLBC_FAILED;
        }
    }

    _willRegComps.push_back(_WillRegComp(comp));

    return LLBC_OK;
}

int LLBC_Service::RegisterComponent(const LLBC_String &libPath, const LLBC_String &compName, LLBC_Component *&comp)
{
    // Force reset out parameter: comp.
    comp = nullptr;

    // Service started or not check.
    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_started))
    {
        LLBC_SetLastError(LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }

    // Argument check.
    if (compName.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }
    else if (!LLBC_File::Exists(libPath))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    // Open comp library(if cached, reuse it).
    bool existingLib;
    LLBC_Library *lib = OpenCompLibrary(libPath, existingLib);
    if (!lib)
        return LLBC_FAILED;

    // Get comp create entry function.
    LLBC_String compCreateFuncName;
    compCreateFuncName.format("%s%s", LLBC_CFG_COMM_CREATE_COMP_FROM_LIB_FUNC_PREFIX, compName.c_str());
    LLBC_CompDynamicCreateFunc compCreateFunc = reinterpret_cast<
        LLBC_CompDynamicCreateFunc>(lib->GetProcAddress(compCreateFuncName.c_str()));
    if (!compCreateFunc)
    {
        if (!existingLib)
            CloseCompLibrary(libPath);

        return LLBC_FAILED;
    }

    // Create comp.
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    comp = reinterpret_cast<LLBC_Component *>(compCreateFunc());
    if (!comp)
    {
        if (!existingLib)
            CloseCompLibrary(libPath);

        if (LLBC_GetLastError() == LLBC_ERROR_SUCCESS)
            LLBC_SetLastError(LLBC_ERROR_UNKNOWN);

        return LLBC_FAILED;
    }

    // Validate comp class name and giving compName is same or not.
    if (UNLIKELY(LLBC_GetTypeName(*comp) != compName))
    {
        LLBC_XDelete(comp);
        if (!existingLib)
            CloseCompLibrary(libPath);

        LLBC_SetLastError(LLBC_ERROR_ILLEGAL);

        return LLBC_FAILED;
    }

    // Call normalize register comp method to register.
    int ret = RegisterComponent(comp);
    if (ret != LLBC_OK)
    {
        LLBC_XDelete(comp);
        if (!existingLib)
            CloseCompLibrary(libPath);
    }

    return ret;
}

LLBC_Component *LLBC_Service::GetComponent(const char *compName)
{
    LLBC_LockGuard guard(_lock);

    _compNameKey.assign(compName);
    _Comps2::iterator it = _comps2.find(_compNameKey);
    if (it != _comps2.end())
        return it->second[0];

    if (_compNameKey.size() > 1 &&
        _compNameKey[0] == 'I')
    {
        _compNameKey.erase(_compNameKey.begin());
        if ((it = _comps2.find(_compNameKey)) != _comps2.end())
            return it->second[0];
    }

    LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
    return nullptr;
}

LLBC_Component *LLBC_Service::GetComponent(const LLBC_String &compName)
{
    LLBC_LockGuard guard(_lock);

    _Comps2::iterator it = _comps2.find(compName);
    if (it == _comps2.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return nullptr;
    }

    _Comps &comps = it->second;
    return comps[0];
}

const std::vector<LLBC_Component *> &LLBC_Service::GetComponents(const LLBC_String &compName)
{
    static const std::vector<LLBC_Component *> emptyComps;

    LLBC_LockGuard guard(_lock);

    _Comps2::iterator it = _comps2.find(compName);
    if (it == _comps2.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return emptyComps;
    }

    return it->second;
}

int LLBC_Service::RegisterCoder(int opcode, LLBC_CoderFactory *coderFactory)
{
    if (UNLIKELY(!coderFactory))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_started && !_initingComp))
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
    if (UNLIKELY(_started && !_initingComp))
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

int LLBC_Service::Subscribe(int opcode, const LLBC_Delegate<void(LLBC_Packet &)> &deleg)
{
    if (UNLIKELY(!deleg))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_started && !_initingComp))
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

int LLBC_Service::PreSubscribe(int opcode, const LLBC_Delegate<bool(LLBC_Packet &)> &deleg)
{
    if (UNLIKELY(!deleg))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_started && !_initingComp))
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
int LLBC_Service::UnifyPreSubscribe(const LLBC_Delegate<bool(LLBC_Packet &)> &deleg)
{
    if (UNLIKELY(!deleg))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_started && !_initingComp))
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
int LLBC_Service::SubscribeStatus(int opcode, int status, const LLBC_Delegate<void(LLBC_Packet &)> &deleg)
{
    if (UNLIKELY(!deleg))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_started && !_initingComp))
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
    if (UNLIKELY(_started && !_initingComp))
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
    if (UNLIKELY(_started && !_initingComp))
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

LLBC_ListenerStub LLBC_Service::SubscribeEvent(int event, const LLBC_Delegate<void(LLBC_Event &)> &deleg)
{
    if (UNLIKELY(!deleg))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return 0;
    }

    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_started && !_initingComp))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return 0;
    }

    Push(LLBC_SvcEvUtil::BuildSubscribeEventEv(event, ++_evManagerMaxListenerStub, deleg, nullptr));

    return _evManagerMaxListenerStub;
}

LLBC_ListenerStub LLBC_Service::SubscribeEvent(int event, LLBC_EventListener *listener)
{
    if (!listener)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return 0;
    }

    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_started && !_initingComp))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return 0;
    }

    Push(LLBC_SvcEvUtil::BuildSubscribeEventEv(event, ++_evManagerMaxListenerStub, nullptr, listener));

    return _evManagerMaxListenerStub;
}

void LLBC_Service::UnsubscribeEvent(int event)
{
    Push(LLBC_SvcEvUtil::
        BuildUnsubscribeEventEv(event, 0));
}

void LLBC_Service::UnsubscribeEvent(const LLBC_ListenerStub &stub)
{
    Push(LLBC_SvcEvUtil::
        BuildUnsubscribeEventEv(0, stub));
}

void LLBC_Service::FireEvent(LLBC_Event *ev,
                             const LLBC_Delegate<void(LLBC_Event *)> &enqueueHandler,
                             const LLBC_Delegate<void(LLBC_Event *)> &dequeueHandler)
{
    Push(LLBC_SvcEvUtil::BuildFireEventEv(ev, dequeueHandler));
    if (enqueueHandler)
        enqueueHandler(ev);
}

int LLBC_Service::Post(const LLBC_Delegate<void(Base *, const LLBC_Variant &)> &runnable, const LLBC_Variant &data)
{
    if (UNLIKELY(!runnable))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);
    _frameTasks[_frameTaskIdx].emplace_back(runnable, data);

    return LLBC_OK;
}

const LLBC_ProtocolStack *LLBC_Service::GetCodecProtocolStack(int sessionId) const
{
    // If enabled full-stack option, return nullptr.
    if (_fullStack)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return nullptr;
    }

    // Not enabled full-stack option, return session codec protocol-stack.
    LLBC_Service *ncThis = const_cast<LLBC_Service *>(this);
    ncThis->_readySessionInfosLock.Lock();
    _ReadySessionInfosCIter it = _readySessionInfos.find(sessionId);
    const LLBC_ProtocolStack *codecStack = it != _readySessionInfos.end() ? it->second->codecStack : nullptr;
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

    // Handle frame-tasks.
    HandleFrameTasks();

    // Process queued events.
    HandleQueuedEvents();

    // Update all components.
    UpdateComps();
    UpdateTimers();
    UpdateAutoReleasePool();

    // Handle frame-tasks.
    HandleFrameTasks();

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
        stack = LLBC_New(_Stack, _Stack::PackStack);
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
        stack = LLBC_New(_Stack, _Stack::CodecStack);
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
    _Stack *stack = LLBC_New(_Stack, _Stack::FullStack);
    stack->SetService(this);
    stack->SetIsSuppressedCoderNotFoundWarning(_suppressedCoderNotFoundWarning);

    return CreatePackStack(sessionId, acceptSessionId,
            CreateCodecStack(sessionId, acceptSessionId, stack));
}

void LLBC_Service::ProcessAppConfigReload()
{
    if (!IsStarted())
        return;

    LLBC_LockGuard guard(_lock);
    if (!IsStarted())
        return;

    Push(LLBC_SvcEvUtil::BuildAppCfgReloadEv());
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
                                                              _fullStack ? nullptr : CreateCodecStack(sessionId, acceptSessionId, nullptr));
        _readySessionInfos.insert(std::make_pair(sessionId, readySInfo));
        _readySessionInfosLock.Unlock();
    }
    else
    {
        _ReadySessionInfo *readySInfo = new _ReadySessionInfo(sessionId,
                                                              0,
                                                              isListenSession,
                                                              _fullStack ? nullptr : CreateCodecStack(sessionId, acceptSessionId, nullptr));
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
    _compsInitRet = InitComps();
    _compsInitFinished = 1;
    if (UNLIKELY(_compsInitRet != LLBC_OK))
    {
        _lock.Unlock();
        return;
    }

    _compsStartRet = StartComps();
    _compsStartFinished = 1;
    if (UNLIKELY(_compsStartRet != LLBC_OK))
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

    // Stop components, destroy release-pool.
    StopComps();
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

    _compsInitFinished = 0;
    _compsInitRet = LLBC_OK;
    _compsStartFinished = 0;
    _compsStartRet = LLBC_OK;

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

    ::memmove(&tls->commTls.services[idx],
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
        if (tls->commTls.services[checkIdx] == nullptr)
            break;
    }

    return checkIdx < lmt ? true : false;
}

void LLBC_Service::HandleFrameTasks()
{
    int frameTaskIdx = _frameTaskIdx;
    _frameTaskIdx = (_frameTaskIdx + 1) % 2;

    _FrameTasks &tasks = _frameTasks[frameTaskIdx];
    if (tasks.empty())
        return;

    const _FrameTasks::const_iterator endIt = tasks.end();    for (_FrameTasks::const_iterator it = tasks.begin();
         it != endIt;
         ++it)
        (it->first)(this, it->second);

    tasks.clear();
}

void LLBC_Service::DestroyFrameTasks()
{
    for (size_t i = 0; i < sizeof(_frameTasks) / sizeof(_frameTasks[0]); ++i)
        _frameTasks[i].clear();
    _frameTaskIdx = 0;
}

void LLBC_Service::HandleQueuedEvents()
{
    #if LLBC_CFG_COMM_ENABLE_SERVICE_FRAME_TIMEOUT
    LLBC_CPUTime begTime;
    if (_frameTimeout != LLBC_INFINITE)
        begTime = LLBC_CPUTime::Current();
    #endif // LLBC_CFG_COMM_ENABLE_SERVICE_FRAME_TIMEOUT

    LLBC_ServiceEvent *ev;
    LLBC_MessageBlock *block, *blocks;
    if (PopAll(blocks) == LLBC_OK)
    {
        while (blocks)
        {
            block = blocks;
            blocks = blocks->GetNext();

            ev = reinterpret_cast<LLBC_ServiceEvent *>(block->GetDataStartWithReadPos());
            (this->*_evHandlers[ev->type])(*ev);

            LLBC_Delete(ev);
            LLBC_Delete(block);

            #if LLBC_CFG_COMM_ENABLE_SERVICE_FRAME_TIMEOUT
            if (_frameTimeout == LLBC_INFINITE)
                continue;

            if (UNLIKELY(static_cast<uint64>(
                (LLBC_CPUTime::Current() - begTime).ToNanoSeconds()) >= _frameTimeout))
                break;
            #endif  // LLBC_CFG_COMM_ENABLE_SERVICE_FRAME_TIMEOUT
        }
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

    // Check has care session-create ev comps or not, if has cared event comps, dispatch event.
    if (_caredEventComps[LLBC_ComponentEventIndex::OnSessionCreate])
    {
        // Build session info.
        LLBC_SessionInfo info;
        info.SetIsListenSession(ev.isListen);
        info.SetSessionId(ev.sessionId);
        info.SetAcceptSessionId(ev.acceptSessionId);
        info.SetLocalAddr(ev.local);
        info.SetPeerAddr(ev.peer);
        info.SetSocket(ev.handle);

        // Dispatch session-create event to all comps.
        _Comps &caredComps = *_caredEventComps[LLBC_ComponentEventIndex::OnSessionCreate];
        const size_t compsSize = caredComps.size();
        for (size_t compIdx = 0; compIdx != compsSize; ++compIdx)
            caredComps[compIdx]->OnSessionCreate(info);
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

    // Check has care session-destroy ev comps or not, if has cared event comps, dispatch event.
    if (_caredEventComps[LLBC_ComponentEventIndex::OnSessionDestroy])
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
        ev.closeInfo = nullptr;

        // Dispatch session-destroy event to all comps.
        _Comps &caredComps = *_caredEventComps[LLBC_ComponentEventIndex::OnSessionDestroy];
        const size_t compsSize = caredComps.size();
        for (size_t compIdx = 0; compIdx != compsSize; ++compIdx)
             caredComps[compIdx]->OnSessionDestroy(destroyInfo);
    }

    // Remove session protocol factory.
    if (ev.acceptSessionId == 0)
        RemoveSessionProtocolFactory(ev.sessionId);
}

void LLBC_Service::HandleEv_AsyncConnResult(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_AsyncConn _Ev;
    _Ev &ev = static_cast<_Ev &>(_);


    // Check has care asyncconn-result ev comps or not, if has cared event comps, dispatch event.
    if (_caredEventComps[LLBC_ComponentEventIndex::OnAsyncConnResult])
    {
        LLBC_AsyncConnResult result;
        result.SetIsConnected(ev.connected);
        result.SetSessionId(ev.sessionId);
        result.SetReason(ev.reason);
        result.SetPeerAddr(ev.peer);

        _Comps &caredComps = *_caredEventComps[LLBC_ComponentEventIndex::OnAsyncConnResult];
        const size_t compsSize = caredComps.size();
        for (size_t compIdx = 0; compIdx != compsSize; ++compIdx)
             caredComps[compIdx]->OnAsyncConnResult(result);
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

    ev.packet = nullptr;

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
        if (recverSvc == nullptr || !recverSvc->IsStarted())
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
        #if LLBC_CFG_COMM_ENABLE_STATUS_DESC
        _StatusDescs::const_iterator statusDescIt = _statusDescs.find(status);
        if (statusDescIt != _statusDescs.end())
            packet->SetStatusDesc(statusDescIt->second);
        #endif // LLBC_CFG_COMM_ENABLE_STATUS_DESC
        # if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
        _OpStatusHandlers::iterator stHandlersIt = _statusHandlers.find(opcode);
        if (stHandlersIt != _statusHandlers.end())
        {
            _StatusHandlers &stHandlers = *stHandlersIt->second;
            _StatusHandlers::iterator stHandlerIt = stHandlers.find(status);
            if (stHandlerIt != stHandlers.end())
            {
                stHandlerIt->second(*packet);
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
            if (!preIt->second(*packet))
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
        if (!_unifyPreHandler(*packet))
        {
            LLBC_Recycle(packet);
            return;
        }
    }
    #endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

    // Finally, search packet handler to handle, if not found any packet handler, dispatch unhandled-packet event to all comps.
    _Handlers::iterator it = _handlers.find(opcode);
    if (it != _handlers.end())
    {
        it->second(*packet);
    }
    else
    {
        if (_caredEventComps[LLBC_ComponentEventIndex::OnUnHandledPacket])
        {
            _Comps &caredComps = *_caredEventComps[LLBC_ComponentEventIndex::OnUnHandledPacket];
            const size_t compsSize = caredComps.size();
            for (size_t compIdx = 0; compIdx != compsSize; ++compIdx)
                 caredComps[compIdx]->OnUnHandledPacket(*packet);
        }
    }

    LLBC_Recycle(packet);
}

void LLBC_Service::HandleEv_ProtoReport(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_ProtoReport _Ev;
    _Ev &ev = static_cast<_Ev &>(_);

    // Check has care proto-report ev comps or not, if has cared event comps, dispatch event.
    if (_caredEventComps[LLBC_ComponentEventIndex::OnProtoReport])
    {
        LLBC_ProtoReport report;
        report.SetSessionId(ev.sessionId);
        report.SetOpcode(ev.opcode);
        report.SetLayer(ev.layer);
        report.SetLevel(ev.level);
        report.SetReport(ev.report);

        _Comps &caredComps = *_caredEventComps[LLBC_ComponentEventIndex::OnProtoReport];
        const size_t compsSize = caredComps.size();
        for (size_t compIdx = 0; compIdx != compsSize; ++compIdx)
             caredComps[compIdx]->OnProtoReport(report);
    }
}

void LLBC_Service::HandleEv_SubscribeEv(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_SubscribeEv _Ev;
    _Ev &ev = static_cast<_Ev &>(_);

    if (ev.deleg)
    {
        _evManager.AddListener(ev.id, ev.deleg, ev.stub);
    }
    else
    {
        _evManager.AddListener(ev.id, ev.listener, ev.stub);
        ev.listener = nullptr;
    }
}

void LLBC_Service::HandleEv_UnsubscribeEv(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_UnsubscribeEv _Ev;
    _Ev &ev = static_cast<_Ev &>(_);

    if (ev.stub == 0)
        _evManager.RemoveListener(ev.id);
    else
        _evManager.RemoveListener(ev.stub);
}

void LLBC_Service::HandleEv_FireEv(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_FireEv _Ev;
    _Ev &ev = static_cast<_Ev &>(_);

    const bool dontDelAfterFire = ev.ev->IsDontDelAfterFire();
    _evManager.Fire(ev.ev);

    if (ev.dequeueHandler &&
        dontDelAfterFire)
        ev.dequeueHandler(ev.ev);

    ev.ev = nullptr;
}

void LLBC_Service::HandleEv_AppCfgReload(LLBC_ServiceEvent &_)
{
    // Check has care application config reloaded ev comps or not, if has cared event comps, dispatch event.
    if (_caredEventComps[LLBC_ComponentEventIndex::OnApplicationConfigReload])
    {
        // Dispatch application config reloaded event to all comps.
        _Comps &caredComps = *_caredEventComps[LLBC_ComponentEventIndex::OnApplicationConfigReload];
        const size_t compsSize = caredComps.size();
        for (size_t compIdx = 0; compIdx != compsSize; ++compIdx)
            caredComps[compIdx]->OnApplicationConfigReload();
    }
}

int LLBC_Service::InitComps()
{
    _initingComp = true;

    bool initSuccess = true;
    for (_WillRegComps::iterator regIt = _willRegComps.begin();
         regIt != _willRegComps.end();
         ++regIt)
    {
        LLBC_Component *comp;
        _WillRegComp &willRegComp = *regIt;
        if (willRegComp.compFactory != nullptr) // Create comp from comp factory.
        {
            comp = willRegComp.compFactory->Create();
            LLBC_XDelete(willRegComp.compFactory);
        }
        else if (willRegComp.comp != nullptr) // Create comp from giving comp(borrow).
        {
            comp = willRegComp.comp;
            willRegComp.comp = nullptr;
        }
        else
        {
            continue;
        }

        comp->SetService(this);
        AddComp(comp);

        if (!comp->IsCaredEvents(LLBC_ComponentEvents::OnInitialize))
        {
            comp->_inited = true;
            continue;
        }

        while (true)
        {
            bool initFinished = true;
            if (!comp->OnInitialize(initFinished))
                break;

            if (initFinished)
            {
                comp->_inited = true;
                break;
            }

            LLBC_Sleep(LLBC_CFG_APP_TRY_START_INTERVAL);
        }

        if (!comp->_inited)
        {
            ClearCompsWhenInitCompFailed();
            initSuccess = false;

            LLBC_SetLastError(LLBC_ERROR_COMP_INIT);

            break;
        }
    }

    if (initSuccess)
        _willRegComps.clear();

    _initingComp = false;

    return initSuccess ? LLBC_OK : LLBC_FAILED;
}

int LLBC_Service::StartComps()
{
    size_t compIdx = 0;
    const size_t compsSize = _comps.size();
    for (; compIdx < compsSize; ++compIdx)
    {
        LLBC_Component *comp = _comps[compIdx];
        if (comp->_started)
            continue;

        if (!comp->IsCaredEvents(LLBC_ComponentEvents::OnStart))
        {
            comp->_started = true;
            continue;
        }

        while (true)
        {
            bool startFinished = true;
            if (!comp->OnStart(startFinished))
                break;

            if (startFinished)
            {
                comp->_started = true;
                break;
            }

            for (size_t upCompIdx = 0; upCompIdx < compIdx; ++upCompIdx)
            {
                auto &upComp = _comps[upCompIdx];
                if (upComp->_started &&
                    upComp->IsCaredEvents(LLBC_ComponentEvents::OnUpdate))
                    upComp->OnUpdate();
            }

            LLBC_Sleep(LLBC_CFG_APP_TRY_START_INTERVAL);
        }

        if (!comp->_started)
            break;
    }

    if (compIdx == compsSize)
        return LLBC_OK;

    StopComps();
    LLBC_SetLastError(LLBC_ERROR_COMP_START);

    return LLBC_FAILED;
}

void LLBC_Service::StopComps()
{
    for (int compIdx = static_cast<int>(_comps.size()) - 1; compIdx >= 0; --compIdx)
    {
        auto &comp = _comps[compIdx];
        if (!comp->_started)
            continue;

        if (!comp->IsCaredEvents(LLBC_ComponentEvents::OnStop))
        {
            comp->_started = false;
            continue;
        }

        while (true)
        {
            bool stopFinished = true;
            comp->OnStop(stopFinished);
            if (stopFinished)
            {
                comp->_started = false;
                break;
            }

            for (int upCompIdx = 0; upCompIdx < compIdx; ++upCompIdx)
            {
                auto &upComp = _comps[upCompIdx];
                if (upComp->_started &&
                    upComp->IsCaredEvents(LLBC_ComponentEvents::OnUpdate))
                    upComp->OnUpdate();
            }

            LLBC_Sleep(LLBC_CFG_APP_TRY_STOP_INTERVAL);
        }
    }
}

void LLBC_Service::UpdateComps()
{
    _Comps *&caredCompsPtr = _caredEventComps[LLBC_ComponentEventIndex::OnUpdate];
    if (!caredCompsPtr)
        return;

    _Comps &caredComps = *caredCompsPtr;
    const size_t compsSize = caredComps.size();
    for (size_t compIdx = 0; compIdx != compsSize; ++compIdx)
        caredComps[compIdx]->OnUpdate();
}

void LLBC_Service::DestroyComps()
{
    for (_Comps::reverse_iterator it = _comps.rbegin();
         it != _comps.rend();
         ++it)
    {
        LLBC_Component *comp = *it;
        if (!comp->_inited)
            continue;

        if (!comp->IsCaredEvents(LLBC_ComponentEvents::OnDestroy))
        {
            comp->_inited = false;
            continue;
        }

        while (true)
        {
            bool destroyFinished = true;
            comp->OnDestroy(destroyFinished);

            if (destroyFinished)
            {
                comp->_inited = false;
                break;
            }

            LLBC_Sleep(LLBC_CFG_APP_TRY_STOP_INTERVAL);
        }
    }

    LLBC_STLHelper::DeleteContainer(_comps, true, true);
    _comps2.clear();

    for (int evOffset = LLBC_ComponentEventIndex::Begin;
         evOffset != LLBC_ComponentEventIndex::End;
         ++evOffset)
    {
        _Comps *&evComps = _caredEventComps[evOffset];
        LLBC_XDelete(evComps);
    }
}

void LLBC_Service::DestroyWillRegComps()
{
    for (_WillRegComps::iterator it = _willRegComps.begin();
         it != _willRegComps.end();
         ++it)
    {
        LLBC_XDelete((*it).comp);
        LLBC_XDelete((*it).compFactory);
    }

    _willRegComps.clear();
}

void LLBC_Service::CloseAllCompLibraries()
{
    LLBC_STLHelper::DeleteContainer(_compLibraries);
}

void LLBC_Service::AddComp(LLBC_Component *comp)
{
    // Add comp to _comps(vector)
    _comps.push_back(comp);

    // Add comp to _comps2(map<type, vector>)
    const LLBC_String compName = LLBC_GetTypeName(*comp);
    _Comps2::iterator compsIt = _comps2.find(compName);
    if (compsIt == _comps2.end())
    {
        compsIt = _comps2.insert(std::make_pair(compName, _Comps())).first;
    }
    _Comps2::mapped_type &typeComps = compsIt->second;
    typeComps.push_back(comp);

    // Add comp to cared events array
    AddCompToCaredEventsArray(comp);
}

void LLBC_Service::AddCompToCaredEventsArray(LLBC_Component *comp)
{
    for (int evOffset = LLBC_ComponentEventIndex::Begin;
         evOffset != LLBC_ComponentEventIndex::End;
         ++evOffset)
    {
        if (!comp->IsCaredEventIndex(evOffset))
            continue;

        _Comps *&evComps = _caredEventComps[evOffset];
        if (evComps == nullptr)
            evComps = new _Comps();
        evComps->push_back(comp);
    }
}

LLBC_Library *LLBC_Service::OpenCompLibrary(const LLBC_String &libPath, bool &existingLib)
{
    existingLib = false;

    _CompLibraries::iterator libIt = _compLibraries.find(libPath);
    if (libIt != _compLibraries.end())
    {
        existingLib = true;
        return libIt->second;
    }

    LLBC_Library *lib = LLBC_New(LLBC_Library);
    if (lib->Open(libPath.c_str()) != LLBC_OK)
    {
        LLBC_Delete(lib);
        return nullptr;
    }

    _compLibraries.insert(std::make_pair(libPath, lib));

    return lib;
}

void LLBC_Service::CloseCompLibrary(const LLBC_String &libPath)
{
    _CompLibraries::iterator libIt = _compLibraries.find(libPath);
    if (libIt == _compLibraries.end())
        return;

    LLBC_Delete(libIt->second);
    _compLibraries.erase(libIt);
}

void LLBC_Service::ClearCompsWhenInitCompFailed()
{
    StopComps();
    DestroyComps();
    DestroyWillRegComps();

    CloseAllCompLibraries();
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
    if (_releasePoolStack)
    {
        _releasePoolStack->Purge();
        _releasePoolStack = nullptr;
    }
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
    if (_timerScheduler)
    {
        _timerScheduler->CancelAll();
        _timerScheduler = nullptr;
    }
}

void LLBC_Service::ProcessIdle(bool fullFrame)
{
    _Comps *&caredEvsPtr = _caredEventComps[LLBC_ComponentEventIndex::OnIdle];
    if (!caredEvsPtr)
        return;

    _Comps &caredEvs = *caredEvsPtr;
    const size_t compsSize = caredEvs.size();
    for (size_t compIdx = 0; compIdx != compsSize; ++compIdx)
    {
        if (fullFrame)
        {
            sint64 elapsed = LLBC_GetMilliSeconds() - _begHeartbeatTime;
            if (LIKELY(elapsed >= 0))
            {
                if (elapsed >= _frameInterval)
                    break;

                caredEvs[compIdx]->OnIdle(static_cast<int>(_frameInterval - elapsed));
            }
        }
        else
        {
            caredEvs[compIdx]->OnIdle(0);
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
    const _ReadySessionInfo *readySInfo = nullptr;
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
                                     LLBC_Coder *coder,
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

LLBC_Service::_WillRegComp::_WillRegComp(LLBC_Component *comp)
{
    this->comp = comp;
    compFactory = nullptr;
}

LLBC_Service::_WillRegComp::_WillRegComp(LLBC_ComponentFactory *compFactory)
{
    comp = nullptr;
    this->compFactory = compFactory;
}

__LLBC_NS_END
