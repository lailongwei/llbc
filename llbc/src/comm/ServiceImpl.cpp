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
#include "llbc/comm/ServiceImpl.h"
#include "llbc/comm/ServiceMgr.h"

#include "llbc/app/App.h"

namespace
{
    typedef LLBC_NS LLBC_ServiceImpl This;
    typedef LLBC_NS LLBC_ProtocolStack _Stack;
}

__LLBC_NS_BEGIN

int LLBC_ServiceImpl::_maxId = 1;

LLBC_ServiceImpl::_EvHandler LLBC_ServiceImpl::_evHandlers[LLBC_ServiceEventType::End] = 
{
    &LLBC_ServiceImpl::HandleEv_SessionCreate,
    &LLBC_ServiceImpl::HandleEv_SessionDestroy,
    &LLBC_ServiceImpl::HandleEv_AsyncConnResult,
    &LLBC_ServiceImpl::HandleEv_DataArrival,
    &LLBC_ServiceImpl::HandleEv_ProtoReport,

    &LLBC_ServiceImpl::HandleEv_SubscribeEv,
    &LLBC_ServiceImpl::HandleEv_UnsubscribeEv,
    &LLBC_ServiceImpl::HandleEv_FireEv,

    &LLBC_ServiceImpl::HandleEv_AppPhaseEv,
    &LLBC_ServiceImpl::HandleEv_AppCfgReload,
};

// VS2005 and later version compiler support initialize array in construct list.
// In here, we disable C4351 warning to initialize it.
#if LLBC_CUR_COMP == LLBC_COMP_MSVC && LLBC_COMP_VER >= 1400
# pragma warning(disable:4351)
#endif

LLBC_ServiceImpl::LLBC_ServiceImpl(const LLBC_String &name,
                           LLBC_IProtocolFactory *dftProtocolFactory,
                           bool fullStack)
: _id(LLBC_AtomicFetchAndAdd(&_maxId, 1))
, _name(name.c_str(), name.length())
, _svcThreadId(LLBC_INVALID_NATIVE_THREAD_ID)

, _cfgType(LLBC_AppConfigType::End)

, _fullStack(fullStack)
, _dftProtocolFactory(dftProtocolFactory)
, _sessionProtoFactory()
, _driveMode(SelfDrive)
, _suppressedCoderNotFoundWarning(false)

, _started(false)
, _stopping(false)
, _initingComp(false)

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

, _willRegComps()

, _compsInitFinished(false)
, _compsInitRet(LLBC_ERROR_SUCCESS)
, _compsStartFinished(false)
, _compsStartRet(LLBC_ERROR_SUCCESS)

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
, _eventFirerPool(*_safetyObjectPool.GetPoolInst<LLBC_ServiceEventFirer>())

, _timerScheduler(nullptr)

, _evManager()
, _evManagerMaxListenerStub(0)

, _svcMgr(*LLBC_ServiceMgrSingleton)
{
    // Create service name, if is empty.
    if (_name.empty())
        _name.format("Svc_%d_%s", _id, LLBC_GUIDHelper::GenStr().c_str());

    // Create protocol stack, if is null.
    if (!_dftProtocolFactory)
        _dftProtocolFactory = new LLBC_NormalProtocolFactory;

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

LLBC_ServiceImpl::~LLBC_ServiceImpl()
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
    LLBC_XDelete(_dftProtocolFactory);
}

bool LLBC_ServiceImpl::IsFullStack() const
{
    return _fullStack;
}

auto LLBC_ServiceImpl::GetDriveMode() const -> DriveMode
{
    return _driveMode;
}

int LLBC_ServiceImpl::SetDriveMode(DriveMode mode)
{
    // Drive mode vlidate check.
    if (mode != SelfDrive && mode != ExternalDrive)
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
    if ((_driveMode = mode) == ExternalDrive)
    {
        ClearHoldedObjectPools();
        ClearHoldedTimerScheduler();

        InitObjectPools();
        InitTimerScheduler();
    }

    return LLBC_OK;
}

int LLBC_ServiceImpl::SuppressCoderNotFoundWarning()
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

int LLBC_ServiceImpl::Start(int pollerCount)
{
    // Normalize pollerCount.
    pollerCount = MAX(1, pollerCount);

    // Service repeat start check.
    if (_svcMgr.GetService(_id) ||
        _svcMgr.GetService(_name))
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }
    // Reentry check.
    else if (_started)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    // Reentry check again.
    _lock.Lock();
    if (_started)
    {
        _lock.Unlock();
        LLBC_SetLastError(LLBC_ERROR_REENTRY);

        return LLBC_FAILED;
    }

    // Init PollerMgr.
    if (_pollerMgr.Init(pollerCount) != LLBC_OK)
    {
        _lock.Unlock();
        return LLBC_FAILED;
    }

    // Update service config(extract from the app config).
    UpdateServiceCfg();

    // Add to service tls or activate.
    if (_driveMode == ExternalDrive)
    {
        if (!IsCanContinueDriveService())
        {
            _pollerMgr.Finalize();
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
            _pollerMgr.Finalize();
            _lock.Unlock();

            return LLBC_FAILED;
        }
    }

    // Mask started.
    _started = true;

    // Init&Start comps.
    if (_driveMode == ExternalDrive)
    {
        // Start pollermgr.
        _pollerMgr.Start();

        // Add service to service mgr.
        _svcMgr.OnServiceStart(this);

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
    else // Self-Drive
    {
        // Unlock first.
        _lock.Unlock();

        // Waiting for all comps init finished.
        while (!_compsInitFinished)
            LLBC_Sleep(1);
        if (_compsInitRet != LLBC_ERROR_SUCCESS)
        {
            const int compsInitRet = _compsInitRet;
            Stop();

            LLBC_SetLastError(compsInitRet);

            return LLBC_FAILED;
        }

        // Waiting for all comps start finished.
        while (!_compsStartFinished)
            LLBC_Sleep(1);
        if (_compsStartRet != LLBC_ERROR_SUCCESS)
        {
            const int compsStartRet = _compsStartRet;
            Stop();

            LLBC_SetLastError(compsStartRet);

            return LLBC_FAILED;
        }
    }

    return LLBC_OK;
}

bool LLBC_ServiceImpl::IsStarted() const
{
    return _started;
}

void LLBC_ServiceImpl::Stop()
{
    _lock.Lock();
    if (!_started || _stopping)
    {
        _lock.Unlock();
        return;
    }

    LLBC_ThreadId svcThreadId = LLBC_INVALID_NATIVE_THREAD_ID;
    if (_driveMode == SelfDrive)
        svcThreadId = _svcThreadId;
    _stopping = true;
    _lock.Unlock();

    if (_driveMode == SelfDrive) // Stop self-drive service.
    {
        // Call Stop() in self thread, do nothing.
        if (LLBC_GetCurrentThreadId() == svcThreadId)
            return;

        // Waiting for svc-thread stopped.
        Wait();
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

int LLBC_ServiceImpl::GetFPS() const
{
    This *ncThis = const_cast<This *>(this);

    LLBC_LockGuard guard(ncThis->_lock);
    return _fps;
}

int LLBC_ServiceImpl::SetFPS(int fps)
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

int LLBC_ServiceImpl::GetFrameInterval() const
{
    This *ncThis = const_cast<This *>(this);

    LLBC_LockGuard guard(ncThis->_lock);
    return _frameInterval;
}

int LLBC_ServiceImpl::Listen(const char *ip,
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

int LLBC_ServiceImpl::Connect(const char *ip,
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

int LLBC_ServiceImpl::AsyncConn(const char *ip,
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

bool LLBC_ServiceImpl::IsSessionValidate(int sessionId)
{
    if (UNLIKELY(sessionId == 0))
        return false;

    _readySessionInfosLock.Lock();
    const bool valid = _readySessionInfos.find(sessionId) != _readySessionInfos.end();
    _readySessionInfosLock.Unlock();

    return valid;
}

int LLBC_ServiceImpl::Send(LLBC_Packet *packet)
{
    // Call internal LockableSend() method to send packet.
    // lock = true
    // validCheck = true
    return LockableSend(packet);
}

int LLBC_ServiceImpl::Broadcast(int svcId, int opcode, LLBC_Coder *coder, int status)
{
    // Copy all connected session Ids.
    _readySessionInfosLock.Lock();
    LLBC_SessionIdList connSIds;
    for (auto readySInfoIt = _readySessionInfos.begin();
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

int LLBC_ServiceImpl::Broadcast(int svcId, int opcode, const void *bytes, size_t len , int status)
{
    LLBC_LockGuard guard(_lock);

    // Foreach to call internal method LockableSend() method to complete.
    // lock = false
    // validCheck = false
    LLBC_LockGuard readySInfosGuard(_readySessionInfosLock);
    for (auto readySInfoIt = _readySessionInfos.begin();
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

int LLBC_ServiceImpl::RemoveSession(int sessionId, const char *reason)
{
    LLBC_LockGuard guard(_lock);
    if (!_started)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    LLBC_LockGuard readySInfosGuard(_readySessionInfosLock);
    auto readySInfoIt = _readySessionInfos.find(sessionId);
    if (readySInfoIt == _readySessionInfos.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    _pollerMgr.Close(sessionId, reason);

    delete readySInfoIt->second;
    _readySessionInfos.erase(readySInfoIt);

    return LLBC_OK;
}

int LLBC_ServiceImpl::CtrlProtocolStack(int sessionId,
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
    auto readySInfoIt = _readySessionInfos.find(sessionId);
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
                              "Protocol stack ctrl finished, "
                              "business logic require remove this session(Half-Stack mode only)");

            return LLBC_OK;
        }
    }

    _readySessionInfosLock.Unlock();

    _pollerMgr.CtrlProtocolStack(sessionId, ctrlCmd, ctrlData);

    return LLBC_OK;
}

int LLBC_ServiceImpl::AddComponent(LLBC_Component *comp)
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

    char compName[512];
    const char *compRttiName = LLBC_GetTypeName(*comp);
    const size_t compNameLen = strlen(compRttiName);
    if (compNameLen >= sizeof(compName))
    {
        LLBC_SetLastError(LLBC_ERROR_LIMIT);
        return LLBC_FAILED;
    }

    memcpy(compName, compRttiName, compNameLen + 1);

    if (std::find_if(_willRegComps.begin(),
                _willRegComps.end(),
                [comp, compName, compNameLen](LLBC_Component *regComp) {
        if (comp == regComp)
        {
            return true;
        }

        const char *regCompName = LLBC_GetTypeName(*regComp);
        return strlen(regCompName) == compNameLen && memcmp(compName, regCompName, compNameLen) == 0;
    }) != _willRegComps.end())
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    _willRegComps.push_back(comp);

    return LLBC_OK;
}

int LLBC_ServiceImpl::AddComponent(const LLBC_String &compSharedLibPath,
                                   const LLBC_String &compName,
                                   LLBC_Component *&comp)
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
    else if (!LLBC_File::Exists(compSharedLibPath))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    // Open comp library(if cached, reuse it).
    bool existingLib;
    LLBC_Library *lib = OpenCompLibrary(compSharedLibPath, existingLib);
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
            CloseCompLibrary(compSharedLibPath);

        return LLBC_FAILED;
    }

    // Create comp.
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    comp = reinterpret_cast<LLBC_Component *>(compCreateFunc());
    if (!comp)
    {
        if (!existingLib)
            CloseCompLibrary(compSharedLibPath);

        if (LLBC_GetLastError() == LLBC_ERROR_SUCCESS)
            LLBC_SetLastError(LLBC_ERROR_UNKNOWN);

        return LLBC_FAILED;
    }

    // Validate comp class name and giving compName is same or not.
    auto realCompName = LLBC_GetTypeName(*comp);
    const auto colonPos = strrchr(realCompName, ':');
    if (colonPos != nullptr)
        realCompName = colonPos + 1;
    if (UNLIKELY(realCompName != compName))
    {
        LLBC_XDelete(comp);
        if (!existingLib)
            CloseCompLibrary(compSharedLibPath);

        LLBC_SetLastError(LLBC_ERROR_ILLEGAL);

        return LLBC_FAILED;
    }

    // Call normalize register comp method to register.
    int ret = AddComponent(comp);
    if (ret != LLBC_OK)
    {
        LLBC_XDelete(comp);
        if (!existingLib)
            CloseCompLibrary(compSharedLibPath);
    }

    return ret;
}

LLBC_Component *LLBC_ServiceImpl::GetComponent(const LLBC_CString &compName)
{
    if (UNLIKELY(compName.empty()))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return nullptr;
    }

    const auto compsEnd = _name2Comps.end();
    LLBC_LockGuard guard(_lock);
    auto it = _name2Comps.find(compName);
    if (it != compsEnd)
        return it->second;

    if (compName.size() > 1 && compName[0] == 'I')
    {
        if ((it = _name2Comps.find(compName + 1)) != compsEnd)
            return it->second;
    }

    LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
    return nullptr;
}

int LLBC_ServiceImpl::AddCoderFactory(int opcode, LLBC_CoderFactory *coderFactory)
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
int LLBC_ServiceImpl::AddStatusDesc(int status, const LLBC_String &desc)
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

int LLBC_ServiceImpl::Subscribe(int opcode, const LLBC_Delegate<void(LLBC_Packet &)> &deleg)
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
    else if (!_handlers.insert(std::make_pair(opcode, deleg)).second)
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_ServiceImpl::PreSubscribe(int opcode, const LLBC_Delegate<bool(LLBC_Packet &)> &deleg)
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
    else if (!_preHandlers.insert(std::make_pair(opcode, deleg)).second)
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
int LLBC_ServiceImpl::UnifyPreSubscribe(const LLBC_Delegate<bool(LLBC_Packet &)> &deleg)
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
int LLBC_ServiceImpl::SubscribeStatus(int opcode, int status, const LLBC_Delegate<void(LLBC_Packet &)> &deleg)
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
    else if (status == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    auto &stHandlers = _statusHandlers[opcode];
    if (!stHandlers.insert(std::make_pair(status, deleg)).second)
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}
#endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER

int LLBC_ServiceImpl::EnableTimerScheduler()
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

int LLBC_ServiceImpl::DisableTimerScheduler()
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

LLBC_ListenerStub LLBC_ServiceImpl::SubscribeEvent(int event, const LLBC_Delegate<void(LLBC_Event &)> &deleg)
{
    if (UNLIKELY(!deleg))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return 0;
    }

    Push(LLBC_SvcEvUtil::BuildSubscribeEventEv(event, ++_evManagerMaxListenerStub, deleg, nullptr));

    return _evManagerMaxListenerStub;
}

LLBC_ListenerStub LLBC_ServiceImpl::SubscribeEvent(int event, LLBC_EventListener *listener)
{
    if (!listener)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return 0;
    }

    Push(LLBC_SvcEvUtil::BuildSubscribeEventEv(event, ++_evManagerMaxListenerStub, nullptr, listener));

    return _evManagerMaxListenerStub;
}

void LLBC_ServiceImpl::UnsubscribeEvent(int event)
{
    Push(LLBC_SvcEvUtil::
        BuildUnsubscribeEventEv(event, 0));
}

void LLBC_ServiceImpl::UnsubscribeEvent(const LLBC_ListenerStub &stub)
{
    Push(LLBC_SvcEvUtil::
        BuildUnsubscribeEventEv(0, stub));
}

void LLBC_ServiceImpl::FireEvent(LLBC_Event *ev,
                                 const LLBC_Delegate<void(LLBC_Event *)> &enqueueHandler,
                                 const LLBC_Delegate<void(LLBC_Event *)> &dequeueHandler)
{
    Push(LLBC_SvcEvUtil::BuildFireEventEv(ev, dequeueHandler));
    if (enqueueHandler)
        enqueueHandler(ev);
}

LLBC_ServiceEventFirer &LLBC_ServiceImpl::BeginFireEvent(int eventId)
{
    LLBC_Event *ev = LLBC_GetObjectFromSafetyPool<LLBC_Event>();
    ev->SetId(eventId);

    auto *eventServiceFirer = _eventFirerPool.GetReferencableObject();
    LLBC_AutoRelease(eventServiceFirer);
    SetEventInfo(eventServiceFirer, ev);

    return *eventServiceFirer;
}

int LLBC_ServiceImpl::Post(const LLBC_Delegate<void(Base *, const LLBC_Variant &)> &runnable,
                           const LLBC_Variant &data)
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

const LLBC_ProtocolStack *LLBC_ServiceImpl::GetCodecProtocolStack(int sessionId) const
{
    // If enabled full-stack option, return nullptr.
    if (_fullStack)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return nullptr;
    }

    // Not enabled full-stack option, return session codec protocol-stack.
    LLBC_ServiceImpl *ncThis = const_cast<LLBC_ServiceImpl *>(this);
    ncThis->_readySessionInfosLock.Lock();
    auto it = _readySessionInfos.find(sessionId);
    const LLBC_ProtocolStack *codecStack =
        it != _readySessionInfos.end() ? it->second->codecStack : nullptr;
    ncThis->_readySessionInfosLock.Unlock();

    if (!codecStack)
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);

    return codecStack;
}

void LLBC_ServiceImpl::OnSvc(bool fullFrame)
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
    UpdateTimerScheduler();
    UpdateAutoReleasePool();

    // Handle frame-tasks.
    HandleFrameTasks();

    // Process Idle.
    if (fullFrame)
        ProcessIdle();

    // Sleep FrameInterval - ElapsedTime milli-seconds, if need.
    if (fullFrame)
    {
        const sint64 elapsed = LLBC_GetMilliSeconds() - _begHeartbeatTime;
        if (elapsed >= 0 && elapsed < _frameInterval)
            LLBC_Sleep(static_cast<int>(_frameInterval - elapsed));
    }

    _sinkIntoLoop = false;
    if (UNLIKELY(_afterStop))
        Cleanup();
}

LLBC_ProtocolStack *LLBC_ServiceImpl::CreatePackStack(int sessionId,
                                                      int acceptSessionId,
                                                      LLBC_ProtocolStack *stack)
{
    if (!stack)
    {
        stack = new _Stack(_Stack::PackStack);
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

LLBC_ProtocolStack *LLBC_ServiceImpl::CreateCodecStack(int sessionId,
                                                       int acceptSessionId,
                                                       LLBC_ProtocolStack *stack)
{
    if (!stack)
    {
        stack = new _Stack(_Stack::CodecStack);
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

LLBC_ProtocolStack *LLBC_ServiceImpl::CreateFullStack(int sessionId, int acceptSessionId)
{
    _Stack *stack = new _Stack(_Stack::FullStack);
    stack->SetService(this);
    stack->SetIsSuppressedCoderNotFoundWarning(_suppressedCoderNotFoundWarning);

    return CreatePackStack(sessionId, acceptSessionId,
            CreateCodecStack(sessionId, acceptSessionId, stack));
}

const std::vector<LLBC_Component *> &LLBC_ServiceImpl::GetComponentList() const
{
    return _compList;
}

void LLBC_ServiceImpl::ProcessAppConfigReload()
{
    if (!IsStarted())
        return;

    LLBC_LockGuard guard(_lock);
    if (!IsStarted())
        return;

    auto app = LLBC_App::ThisApp();
    Push(LLBC_SvcEvUtil::BuildAppCfgReloadEv(
        app->GetConfigType(), app->GetPropertyConfig(), app->GetConfig()));
}

void LLBC_ServiceImpl::AddSessionProtocolFactory(int sessionId, LLBC_IProtocolFactory *protoFactory)
{
    _protoLock.Lock();

    std::map<int, LLBC_IProtocolFactory *>::iterator it = _sessionProtoFactory.find(sessionId);
    if (it != _sessionProtoFactory.end())
    {
        delete it->second;
        _sessionProtoFactory.erase(it);
    }

    _sessionProtoFactory.insert(std::make_pair(sessionId, protoFactory));

    _protoLock.Unlock();
}

LLBC_IProtocolFactory *LLBC_ServiceImpl::FindSessionProtocolFactory(int sessionId)
{
    if (sessionId == 0)
        return _dftProtocolFactory;

    LLBC_LockGuard guard(_protoLock);
    std::map<int, LLBC_IProtocolFactory *>::iterator it = _sessionProtoFactory.find(sessionId);
    if (it != _sessionProtoFactory.end())
        return it->second;

    return _dftProtocolFactory;
}

void LLBC_ServiceImpl::RemoveSessionProtocolFactory(int sessionId)
{
    LLBC_LockGuard guard(_protoLock);
    std::map<int, LLBC_IProtocolFactory *>::iterator it = _sessionProtoFactory.find(sessionId);
    if (it != _sessionProtoFactory.end())
    {
        delete it->second;
        _sessionProtoFactory.erase(it);
    }
}

void LLBC_ServiceImpl::AddReadySession(int sessionId,
                                       int acceptSessionId,
                                       bool isListenSession,
                                       bool repeatCheck)
{
    if (repeatCheck)
    {
        _readySessionInfosLock.Lock();
        const auto readySInfoIt = _readySessionInfos.find(sessionId);
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

void LLBC_ServiceImpl::RemoveReadySession(int sessionId)
{
    // Lock.
    _readySessionInfosLock.Lock();

    // Find ready session info, if not found, return.
    auto readySInfoIt = _readySessionInfos.find(sessionId);
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
    delete readySInfo;
}

void LLBC_ServiceImpl::RemoveAllReadySessions()
{
    _readySessionInfosLock.Lock();
    LLBC_STLHelper::DeleteContainer(_readySessionInfos, true, false);
    _readySessionInfosLock.Unlock();
}

void LLBC_ServiceImpl::Svc()
{
    _svcThreadId = LLBC_GetCurrentThreadId();
    while (!_started)
        LLBC_Sleep(5);

    _lock.Lock();

    _pollerMgr.Start();

    AddServiceToTls();
    InitObjectPools();
    InitTimerScheduler();
    InitAutoReleasePool();

    _svcMgr.OnServiceStart(this);
    if (UNLIKELY(InitComps() != LLBC_OK))
    {
        _lock.Unlock();
        return;
    }

    if (UNLIKELY(StartComps() != LLBC_OK))
    {
        _lock.Unlock();
        return;
    }

    _lock.Unlock();

    while (!_stopping)
        OnSvc();
}

void LLBC_ServiceImpl::Cleanup()
{
    // Force handle queued events.
    if (_started &&
        _stopping &&
        _compsStartFinished &&
        _compsStartRet == LLBC_ERROR_SUCCESS)
        HandleQueuedEvents();

    // Stop && Finalize poller manager.
    _pollerMgr.Stop();
    _pollerMgr.Finalize();

    // If drivemode is external-drive, cancel all timers first.
    if (_driveMode == ExternalDrive)
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

    // notify service manager self stopped.
    _svcMgr.OnServiceStop(this);

    // Reset some variables.
    _relaxTimes = 0;

    _propCfg.RemoveAllProperties();
    _nonPropCfg = LLBC_Variant::nil;
    _cfgType = LLBC_AppConfigType::End;

    _compsInitFinished = false;
    _compsInitRet = LLBC_ERROR_SUCCESS;
    _compsStartFinished = false;
    _compsStartRet = LLBC_ERROR_SUCCESS;

    _started = false;
    _stopping = false;

    _svcThreadId = LLBC_INVALID_NATIVE_THREAD_ID;
}

void LLBC_ServiceImpl::UpdateServiceCfg(LLBC_SvcEv_AppCfgReloadedEv *ev)
{
    auto app = LLBC_App::ThisApp();
    if (UNLIKELY(!app))
        return;

    // If use app config, lock app config load.
    if (!ev)
        app->PreventConfigLoad();
    LLBC_Defer(if (!ev) app->CancelPreventConfigLoad());

    // Get config type.
    _cfgType = ev ? ev->cfgType : app->GetConfigType();

    // Reset service config.
    _propCfg.RemoveAllProperties();
    _nonPropCfg = LLBC_Variant::nil;

    // Update service config.
    if (_cfgType == LLBC_AppConfigType::Property)
    {
        // Service config prop name:
        // <svc_name>
        // <svc_name>.xxx
        // <svc_name>.xxx.xxx
        // <svc_name>.xxx.xxx.xxx....
        auto &appPropCfg = ev ? ev->propCfg : app->GetPropertyConfig();
        auto svcCfg = appPropCfg.GetProperty(GetName());
        LLBC_DoIf(svcCfg, _propCfg = *svcCfg);
    }
    else
    {
        auto &appNonPropCfg = ev ? ev->nonPropCfg : app->GetConfig();
        if (_cfgType == LLBC_AppConfigType::Ini)
        {
            // Service config section: [<svc_name>]
            // Comp config section(s): [<svc_name>.<comp_name>]
            for (auto it = appNonPropCfg.DictBegin(); it != appNonPropCfg.DictEnd(); ++it)
            {
                const auto iniSectionName = it->first.AsStr();
                if (iniSectionName == GetName() ||
                    (iniSectionName.startswith(GetName() + ".") && iniSectionName.size() > GetName().size() + 1))
                    _nonPropCfg[iniSectionName] = it->second;
            }
        }
        else if (_cfgType == LLBC_AppConfigType::Xml)
        {
            auto &svcCfgs = appNonPropCfg[LLBC_XMLKeys::Children];
            for (auto &svcCfg : svcCfgs.AsSeq())
            {
                const auto svcCfgName = svcCfg[LLBC_XMLKeys::Name].AsStr();
                if (svcCfgName != "Service" &&
                    svcCfgName != "service" &&
                    svcCfgName != "Svc" &&
                    svcCfgName != "svc")
                    continue;

                const auto &svcCfgAttrs = svcCfg[LLBC_XMLKeys::Attrs];
                if (svcCfgAttrs["Name"] == GetName() ||
                    svcCfgAttrs["name"] == GetName())
                {
                    _nonPropCfg = svcCfg;
                    break;
                }
            }
        }
    }
}

void LLBC_ServiceImpl::AddServiceToTls()
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

void LLBC_ServiceImpl::RemoveServiceFromTls()
{
    __LLBC_LibTls *tls = __LLBC_GetLibTls();

    int idx = 0;
    const int lmt = LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT;
    for (; idx <= lmt; ++idx)
    {
        if (tls->commTls.services[idx] == this)
            break;
    }

    memmove(&tls->commTls.services[idx],
            &tls->commTls.services[idx + 1],
            sizeof(tls->commTls.services[0]) * (lmt + 1 - (idx + 1)));
}

bool LLBC_ServiceImpl::IsCanContinueDriveService()
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

void LLBC_ServiceImpl::HandleFrameTasks()
{
    auto &tasks = _frameTasks[_frameTaskIdx];
    if (tasks.empty())
        return;

    _frameTaskIdx ^= 1;
    const auto endIt = tasks.end();
    for (auto it = tasks.begin(); it != endIt; ++it)
        (it->first)(this, it->second);

    tasks.clear();
}

void LLBC_ServiceImpl::DestroyFrameTasks()
{
    for (size_t i = 0; i < sizeof(_frameTasks) / sizeof(_frameTasks[0]); ++i)
        _frameTasks[i].clear();
    _frameTaskIdx = 0;
}

void LLBC_ServiceImpl::HandleQueuedEvents()
{
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

            delete ev;
            delete block;
        }
    }
}

void LLBC_ServiceImpl::HandleEv_SessionCreate(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_SessionCreate _Ev;
    _Ev &ev = static_cast<_Ev &>(_);

    // Add session to connected sessionIds set.
    {
        AddReadySession(ev.sessionId, ev.acceptSessionId, ev.isListen, true);
    }

    // Check has care session-create ev comps or not, if has cared event comps, dispatch event.
    auto &caredComps = _caredEventComps[LLBC_ComponentEventIndex::OnSessionCreate];
    if (!caredComps.empty())
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
        const size_t compsSize = caredComps.size();
        for (size_t compIdx = 0; compIdx != compsSize; ++compIdx)
        {
            LLBC_Component *&comp = caredComps[compIdx];
            if (LIKELY(comp->_started))
                comp->OnSessionCreate(info);
        }
    }
}

void LLBC_ServiceImpl::HandleEv_SessionDestroy(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_SessionDestroy _Ev;
    _Ev &ev = static_cast<_Ev &>(_);

    // Erase session from connected sessionIds set.
    {
        RemoveReadySession(ev.sessionId);
    }

    // Check has care session-destroy ev comps or not, if has cared event comps, dispatch event.
    auto &caredComps = _caredEventComps[LLBC_ComponentEventIndex::OnSessionDestroy];
    if (!caredComps.empty())
    {
        // Build session info.
        LLBC_SessionInfo *sessionInfo = new LLBC_SessionInfo;
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
        const size_t compsSize = caredComps.size();
        for (size_t compIdx = 0; compIdx != compsSize; ++compIdx)
        {
            LLBC_Component *&comp = caredComps[compIdx];
            if (LIKELY(comp->_started))
                comp->OnSessionDestroy(destroyInfo);
        }
    }

    // Remove session protocol factory.
    if (ev.acceptSessionId == 0)
        RemoveSessionProtocolFactory(ev.sessionId);
}

void LLBC_ServiceImpl::HandleEv_AsyncConnResult(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_AsyncConn _Ev;
    _Ev &ev = static_cast<_Ev &>(_);


    // Check has care asyncconn-result ev comps or not, if has cared event comps, dispatch event.
    auto &caredComps = _caredEventComps[LLBC_ComponentEventIndex::OnAsyncConnResult];
    if (!caredComps.empty())
    {
        LLBC_AsyncConnResult result;
        result.SetIsConnected(ev.connected);
        result.SetSessionId(ev.sessionId);
        result.SetReason(ev.reason);
        result.SetPeerAddr(ev.peer);

        const size_t compsSize = caredComps.size();
        for (size_t compIdx = 0; compIdx != compsSize; ++compIdx)
        {
            LLBC_Component *&comp = caredComps[compIdx];
            if (LIKELY(comp->_started))
                comp->OnAsyncConnResult(result);
        }
    }

    // Remove session protocol factory, if connect failed.
    if (!ev.connected)
        RemoveSessionProtocolFactory(ev.sessionId);
}

void LLBC_ServiceImpl::HandleEv_DataArrival(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_DataArrival _Ev;
    _Ev &ev = static_cast<_Ev &>(_);
    LLBC_Packet *packet = ev.packet;

    // Makesure session in connected sessionId set.
    const int sessionId = packet->GetSessionId();

    _readySessionInfosLock.Lock();
    auto readySInfoIt = _readySessionInfos.find(sessionId);
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
        LLBC_Service *recverSvc = _svcMgr.GetService(recverSvcId);
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
        auto statusDescIt = _statusDescs.find(status);
        if (statusDescIt != _statusDescs.end())
            packet->SetStatusDesc(statusDescIt->second);
        #endif // LLBC_CFG_COMM_ENABLE_STATUS_DESC
        # if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
        auto stHandlersIt = _statusHandlers.find(opcode);
        if (stHandlersIt != _statusHandlers.end())
        {
            auto &stHandlers = stHandlersIt->second;
            auto stHandlerIt = stHandlers.find(status);
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

    // Firstly, we recognize specified opcode's pre-handler, if registered, call it.
    bool preHandled = false;
    auto preIt = _preHandlers.find(opcode);
    if (preIt != _preHandlers.end())
    {
        if (!preIt->second(*packet))
        {
            LLBC_Recycle(packet);
            return;
        }

        preHandled = true;
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

    // Finally, search packet handler to handle,
    // if not found any packet handler, dispatch unhandled-packet event to all comps.
    auto it = _handlers.find(opcode);
    if (it != _handlers.end())
    {
        it->second(*packet);
    }
    else
    {
        auto &caredComps = _caredEventComps[LLBC_ComponentEventIndex::OnUnHandledPacket];
        if (!caredComps.empty())
        {
            const size_t compsSize = caredComps.size();
            for (size_t compIdx = 0; compIdx != compsSize; ++compIdx)
            {
                LLBC_Component *&comp = caredComps[compIdx];
                if (LIKELY(comp->_started))
                    comp->OnUnHandledPacket(*packet);
            }
        }
    }

    LLBC_Recycle(packet);
}

void LLBC_ServiceImpl::HandleEv_ProtoReport(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_ProtoReport _Ev;
    _Ev &ev = static_cast<_Ev &>(_);

    // Check has care proto-report ev comps or not, if has cared event comps, dispatch event.
    auto &caredComps = _caredEventComps[LLBC_ComponentEventIndex::OnProtoReport];
    if (!caredComps.empty())
    {
        LLBC_ProtoReport report;
        report.SetSessionId(ev.sessionId);
        report.SetOpcode(ev.opcode);
        report.SetLayer(ev.layer);
        report.SetLevel(ev.level);
        report.SetReport(ev.report);

        const size_t compsSize = caredComps.size();
        for (size_t compIdx = 0; compIdx != compsSize; ++compIdx)
        {
            LLBC_Component *&comp = caredComps[compIdx];
            if (LIKELY(comp->_started))
                comp->OnProtoReport(report);
        }
    }
}

void LLBC_ServiceImpl::HandleEv_SubscribeEv(LLBC_ServiceEvent &_)
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

void LLBC_ServiceImpl::HandleEv_UnsubscribeEv(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_UnsubscribeEv _Ev;
    _Ev &ev = static_cast<_Ev &>(_);

    if (ev.stub == 0)
        _evManager.RemoveListener(ev.id);
    else
        _evManager.RemoveListener(ev.stub);
}

void LLBC_ServiceImpl::HandleEv_FireEv(LLBC_ServiceEvent &_)
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

void LLBC_ServiceImpl::HandleEv_AppPhaseEv(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_AppPhaseEv _Ev;
    auto &ev = static_cast<_Ev &>(_);
    if (ev.earlyStart)
    {
        for (auto &comp : _caredEventComps[LLBC_ComponentEventIndex::OnAppEarlyStart])
        {
            if (LIKELY(comp->_started))
                comp->OnAppEarlyStart();
        }
    }
    else if (ev.startFail)
    {
        for (auto &comp : _caredEventComps[LLBC_ComponentEventIndex::OnAppStartFail])
        {
            if (LIKELY(comp->_started))
                comp->OnAppStartFail();
        }
    }
    else if (ev.startFinish)
    {
        if (_cfgType == LLBC_AppConfigType::End)
            UpdateServiceCfg();

        for (auto &comp : _caredEventComps[LLBC_ComponentEventIndex::OnAppStartFinish])
        {
            if (LIKELY(comp->_started))
                comp->OnAppStartFinish();
        }
    }
    else if (ev.earlyStop)
    {
        for (auto &comp : _caredEventComps[LLBC_ComponentEventIndex::OnAppWillStop])
        {
            if (LIKELY(comp->_started))
                comp->OnAppEarlyStop();
        }
    }
}

void LLBC_ServiceImpl::HandleEv_AppCfgReload(LLBC_ServiceEvent &_)
{
    // Update service config.
    UpdateServiceCfg(&static_cast<LLBC_SvcEv_AppCfgReloadedEv &>(_));
    // Update all components config.
    for (auto &compItem : _name2Comps)
        compItem.second->UpdateComponentCfg();

    // Check has care application config reloaded ev comps or not, if has cared event comps, dispatch event.
    auto &caredComps = _caredEventComps[LLBC_ComponentEventIndex::OnAppConfigReload];
    if (caredComps.empty())
        return;

    // Dispatch application config reloaded event to all comps.
    const size_t compsSize = caredComps.size();
    for (size_t compIdx = 0; compIdx != compsSize; ++compIdx)
    {
        LLBC_Component *&comp = caredComps[compIdx];
        if (LIKELY(comp->_started))
            comp->OnAppConfigReload();
    }
}

int LLBC_ServiceImpl::InitComps()
{
    _initingComp = true;
    LLBC_Defer(_initingComp = false);

    _compsInitRet = LLBC_ERROR_SUCCESS;
    while (!_willRegComps.empty())
    {
        // Pop will-reg comp.
        const std::list<LLBC_Component *>::iterator willRegCompIt = _willRegComps.begin();
        LLBC_Component * const willRegComp = *willRegCompIt;
        _willRegComps.erase(willRegCompIt);

        // Set service to comp & update component config & add to service's comps dict.
        willRegComp->SetService(this);
        willRegComp->UpdateComponentCfg();
        AddComp(willRegComp);

        // If inited, continue.
        if (willRegComp->_inited)
            continue;

        // If comp uncare <OnInit> event, set to inited, and than continue.
        if (!willRegComp->IsCaredEvents(LLBC_ComponentEvents::OnInit))
        {
            willRegComp->_inited = true;
            continue;
        }

        // Init comp.
        while (true)
        {
            bool initFinished = true;
            LLBC_SetLastError(LLBC_ERROR_SUCCESS);
            if (!willRegComp->OnInit(initFinished))
            {
                if (LLBC_GetLastError() == LLBC_ERROR_SUCCESS)
                    LLBC_SetLastError(LLBC_ERROR_COMP_INIT);
                _compsInitRet = LLBC_GetLastError();

                break;
            }

            if (initFinished)
            {
                willRegComp->_inited = true;
                break;
            }

            LLBC_Sleep(LLBC_CFG_APP_TRY_START_INTERVAL);
        }

        if (!willRegComp->_inited)
        {
            ClearCompsWhenInitCompFailed();
            break;
        }
    }

    LLBC_SetLastError(_compsInitRet);
    _compsInitFinished = true;

    return _compsInitRet == LLBC_ERROR_SUCCESS ? LLBC_OK : LLBC_FAILED;
}

int LLBC_ServiceImpl::StartComps()
{
    _compsStartRet = LLBC_ERROR_SUCCESS;

    // Call comps OnStart() event method.
    size_t compIdx = 0;
    const size_t compsSize = _compList.size();
    for (; compIdx < compsSize; ++compIdx)
    {
        auto &comp = _compList[compIdx];
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
            LLBC_SetLastError(LLBC_ERROR_SUCCESS);
            if (!comp->OnStart(startFinished))
            {
                if (LLBC_GetLastError() == LLBC_ERROR_SUCCESS)
                    LLBC_SetLastError(LLBC_ERROR_COMP_START);
                _compsStartRet = LLBC_GetLastError();
                break;
            }
            else if (startFinished)
            {
                comp->_started = true;
                break;
            }

            OnSvc(false);
            LLBC_Sleep(LLBC_CFG_APP_TRY_START_INTERVAL);
        }

        // If comp start failed, stop all comps stop process.
        if (!comp->_started)
            break;
    }

    // If all comps start finished, call comps OnLateStart() event method.
    // TODO: will remove async-waiting for Late-Start finish design in the future.
    if (compIdx == compsSize)
    {
        for (compIdx = 0; compIdx != compsSize; ++compIdx)
        {
            auto &comp = _compList[compIdx];
            if (comp->IsCaredEvents(LLBC_ComponentEvents::OnLateStart))
            {
                while (true)
                {
                    bool lateStartFinish = true;
                    comp->OnLateStart(lateStartFinish);
                    if (lateStartFinish)
                        break;

                    OnSvc(false);
                    LLBC_Sleep(LLBC_CFG_APP_TRY_START_INTERVAL);
                }
            }
        }
    }
    else // Otherwise stop all comps.
    {
        StopComps();
    }

    // Set comps start errno, mask start finished.
    LLBC_SetLastError(_compsStartRet);
    _compsStartFinished = true;

    return _compsStartRet == LLBC_ERROR_SUCCESS ? LLBC_OK : LLBC_FAILED;
}

void LLBC_ServiceImpl::StopComps()
{
    // Call comps OnEarlyStop() event method.
    // TODO: will remove async-waiting for Early-Stop finish design in the future.
    for (int compIdx = static_cast<int>(_compList.size() - 1); compIdx >= 0; --compIdx)
    {
        auto &comp = _compList[compIdx];
        if (!comp->_started ||
            !comp->IsCaredEvents(LLBC_ComponentEventIndex::OnEarlyStop))
            continue;

        while (true)
        {
            bool earlyStopFinish = true;
            comp->OnEarlyStop(earlyStopFinish);
            if (earlyStopFinish)
                break;

            OnSvc(false);
            LLBC_Sleep(LLBC_CFG_APP_TRY_STOP_INTERVAL);
        }
    }

    // Call comps OnStop() event method.
    for (int compIdx = static_cast<int>(_compList.size()) - 1; compIdx >= 0; --compIdx)
    {
        auto &comp = _compList[compIdx];
        if (!comp->_started)
            continue;

        if (comp->IsCaredEvents(LLBC_ComponentEvents::OnStop))
        {
            while (true)
            {
                bool stopFinished = true;
                comp->OnStop(stopFinished);
                if (stopFinished)
                    break;

                OnSvc(false);
                LLBC_Sleep(LLBC_CFG_APP_TRY_STOP_INTERVAL);
            }
        }

        comp->_started = false;
    }
}

void LLBC_ServiceImpl::UpdateComps()
{
    auto &caredComps = _caredEventComps[LLBC_ComponentEventIndex::OnUpdate];
    if (caredComps.empty())
        return;

    const size_t compsSize = caredComps.size();
    for (size_t compIdx = 0; compIdx != compsSize; ++compIdx)
    {
        LLBC_Component *&comp = caredComps[compIdx];
        if (LIKELY(comp->_started))
            comp->OnUpdate();
    }
}

void LLBC_ServiceImpl::DestroyComps()
{
    for (auto it = _compList.rbegin(); it != _compList.rend(); ++it)
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

    LLBC_STLHelper::DeleteContainer(_compList, true, true);
    while (!_name2Comps.empty())
    {
        auto it = _name2Comps.begin();
        auto compName = it->first;
        _name2Comps.erase(it);

        free(const_cast<char *>(compName.c_str()));
    }

    for (auto &evComps: _caredEventComps)
        evComps.clear();
}

void LLBC_ServiceImpl::DestroyWillRegComps()
{
    LLBC_STLHelper::RecycleContainer(_willRegComps, true, true);
}

void LLBC_ServiceImpl::CloseAllCompLibraries()
{
    LLBC_STLHelper::DeleteContainer(_compLibraries);
}

void LLBC_ServiceImpl::AddComp(LLBC_Component *comp)
{
    _compList.push_back(comp);

    #if LLBC_TARGET_PLATFORM_WIN32
    auto compName = typeid(*comp).name();
    #else
    auto compName = LLBC_GetTypeName(*comp);
    #endif
    const auto colonPos = strrchr(compName, ':');
    if (colonPos)
        compName = colonPos + 1;
    const auto compNameLen = strlen(compName);

    auto allocCompName = LLBC_Malloc(char, compNameLen + 1);
    memcpy(allocCompName, compName, compNameLen + 1);
    _name2Comps.emplace(allocCompName, comp);

    AddCompToCaredEventsArray(comp);
}

void LLBC_ServiceImpl::AddCompToCaredEventsArray(LLBC_Component *comp)
{
    for (int evOffset = LLBC_ComponentEventIndex::Begin;
         evOffset != LLBC_ComponentEventIndex::End;
         ++evOffset)
    {
        if (comp->IsCaredEventIndex(evOffset))
            _caredEventComps[evOffset].push_back(comp);
    }
}

LLBC_Library *LLBC_ServiceImpl::OpenCompLibrary(const LLBC_String &libPath, bool &existingLib)
{
    existingLib = false;

    auto libIt = _compLibraries.find(libPath);
    if (libIt != _compLibraries.end())
    {
        existingLib = true;
        return libIt->second;
    }

    LLBC_Library *lib = new LLBC_Library;
    if (lib->Open(libPath.c_str()) != LLBC_OK)
    {
        delete lib;
        return nullptr;
    }

    _compLibraries.insert(std::make_pair(libPath, lib));

    return lib;
}

void LLBC_ServiceImpl::CloseCompLibrary(const LLBC_String &libPath)
{
    auto libIt = _compLibraries.find(libPath);
    if (libIt == _compLibraries.end())
        return;

    delete libIt->second;
    _compLibraries.erase(libIt);
}

void LLBC_ServiceImpl::ClearCompsWhenInitCompFailed()
{
    StopComps();
    DestroyComps();
    DestroyWillRegComps();

    CloseAllCompLibraries();
}

 void LLBC_ServiceImpl::InitAutoReleasePool()
{
    _releasePoolStack = LLBC_AutoReleasePoolStack::GetCurrentThreadReleasePoolStack();
}

void LLBC_ServiceImpl::UpdateAutoReleasePool()
{
    if (_driveMode == SelfDrive)
        _releasePoolStack->Purge();
}

void LLBC_ServiceImpl::ClearAutoReleasePool()
{
    if (_releasePoolStack)
    {
        _releasePoolStack->Purge();
        _releasePoolStack = nullptr;
    }
}

void LLBC_ServiceImpl::InitObjectPools()
{
}

void LLBC_ServiceImpl::UpdateObjectPools()
{
}

void LLBC_ServiceImpl::ClearHoldedObjectPools()
{
}

void LLBC_ServiceImpl::InitTimerScheduler()
{
    if (!_timerScheduler)
    {
        __LLBC_LibTls *tls = __LLBC_GetLibTls();
        _timerScheduler = reinterpret_cast<LLBC_TimerScheduler *>(tls->coreTls.timerScheduler);
    }
}

void LLBC_ServiceImpl::UpdateTimerScheduler()
{
    _timerScheduler->Update();
}

void LLBC_ServiceImpl::ClearHoldedTimerScheduler()
{
    if (_timerScheduler)
    {
        _timerScheduler->CancelAll();
        _timerScheduler = nullptr;
    }
}

void LLBC_ServiceImpl::ProcessIdle()
{
    auto &caredComps = _caredEventComps[LLBC_ComponentEventIndex::OnIdle];
    LLBC_ReturnIf(caredComps.empty(), void());

    const size_t compsSize = caredComps.size();
    for (size_t compIdx = 0; compIdx != compsSize; ++compIdx)
    {
        sint64 elapsed = LLBC_GetMilliSeconds() - _begHeartbeatTime;
        if (LIKELY(elapsed >= 0))
        {
            if (elapsed >= _frameInterval)
                break;

            LLBC_Component *&comp = caredComps[compIdx];
            if (LIKELY(comp->_started))
                comp->OnIdle(LLBC_TimeSpan::FromMillis(_frameInterval - elapsed));
        }
    }
}

int LLBC_ServiceImpl::LockableSend(LLBC_Packet *packet,
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
        decltype(_readySessionInfos)::const_iterator readySInfoIt;

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

int LLBC_ServiceImpl::LockableSend(int svcId,
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
int LLBC_ServiceImpl::MulticastSendCoder(int svcId,
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
    // firstPacket->SetSenderServiceId(_id);
        // LockableSend(LLBC_Packet *, bool, bool) function will set sender service Id.
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

        decltype(_readySessionInfos)::const_iterator readySInfoIt;
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
            // otherPacket->SetSenderServiceId(_id);
                // LockableSend(LLBC_Packet *, bool, bool) function will set sender service Id.
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

        decltype(_readySessionInfos)::const_iterator readySInfoIt;
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
            // otherPacket->SetSenderServiceId(_id);
                // LockableSend(LLBC_Packet *, bool, bool) function will set sender service Id.
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

LLBC_ServiceImpl::_ReadySessionInfo::_ReadySessionInfo(int sessionId,
                                                       int acceptSessionId,
                                                       bool isListenSession,
                                                       LLBC_ProtocolStack *codecStack)
{
    this->sessionId = sessionId;
    this->acceptSessionId = acceptSessionId;
    this->isListenSession = isListenSession;
    this->codecStack = codecStack;
}

LLBC_ServiceImpl::_ReadySessionInfo::~_ReadySessionInfo()
{
    if (codecStack)
        delete codecStack;
}

__LLBC_NS_END
