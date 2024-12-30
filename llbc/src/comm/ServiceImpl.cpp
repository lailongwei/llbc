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
#include "llbc/comm/protocol/NormalProtocolFactory.h"
#include "llbc/comm/ServiceImpl.h"
#include "llbc/comm/ServiceMgr.h"

#include "llbc/app/App.h"

namespace
{
    using _Stack = LLBC_NS LLBC_ProtocolStack;
}

#define __LLBC_INL_CHECK_RUNNING_PHASE_EQ(requirePhase, failedSetErr, failedRet) \
    LLBC_LockGuard guard(_lock);                                               \
    if (UNLIKELY(_runningPhase != LLBC_ServiceRunningPhase::requirePhase ||    \
        LLBC_ServiceRunningPhase::IsFailedOrStoppingPhase(_runningPhase))) {   \
        LLBC_SetLastError(failedSetErr);                                       \
        return failedRet;                                                      \
    }                                                                          \

#define __LLBC_INL_CHECK_RUNNING_PHASE_LE(requirePhase, failedSetErr, failedRet) \
    LLBC_LockGuard guard(_lock);                                               \
    if (UNLIKELY(_runningPhase > LLBC_ServiceRunningPhase::requirePhase ||     \
        LLBC_ServiceRunningPhase::IsFailedOrStoppingPhase(_runningPhase))) {   \
        LLBC_SetLastError(failedSetErr);                                       \
        return failedRet;                                                      \
    }                                                                          \

#define __LLBC_INL_CHECK_RUNNING_PHASE_GE(requirePhase, failedSetErr, failedRet) \
    LLBC_LockGuard guard(_lock);                                               \
    if (UNLIKELY(_runningPhase < LLBC_ServiceRunningPhase::requirePhase ||     \
        LLBC_ServiceRunningPhase::IsFailedOrStoppingPhase(_runningPhase))) {   \
        LLBC_SetLastError(failedSetErr);                                       \
        return failedRet;                                                      \
    }                                                                          \

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
    &LLBC_ServiceImpl::HandleEv_AppReloaded,

    &LLBC_ServiceImpl::HandleEv_ComponentEvent,
};

LLBC_ListenerStub LLBC_ServiceImpl::_evManagerMaxListenerStub = 0;

LLBC_ServiceImpl::LLBC_ServiceImpl(const LLBC_String &name,
                                   LLBC_IProtocolFactory *dftProtocolFactory,
                                   bool fullStack)
: _id(LLBC_AtomicFetchAndAdd(&_maxId, 1))

, _name(name.c_str(), name.length())
, _driveMode(LLBC_ServiceDriveMode::SelfDrive)
, _svcThreadId(LLBC_INVALID_NATIVE_THREAD_ID)
, _inCleaning(false)
, _sinkIntoOnSvcLoop(false)
, _svcMgr(*LLBC_ServiceMgrSingleton)
, _serviceBeginLoop(false)
, _runningPhase(LLBC_ServiceRunningPhase::NotStarted)

, _startErrNo(0)
, _startSubErrNo(0)

, _cfgType(LLBC_AppConfigType::End)

, _fullStack(fullStack)
, _pollerCount(0)
, _suppressedCoderNotFoundWarning(false)
, _dftProtocolFactory(dftProtocolFactory)

, _fps(LLBC_CFG_COMM_DFT_SERVICE_FPS)
, _begSvcTime(0)

// Service extend functions about members.
, _releasePoolStack(nullptr)

, _threadSafeObjPool(true)
, _threadUnsafeObjPool(false)

, _timerScheduler(nullptr)
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
    ASSERT(LLBC_PollerType::IsValid(pollerType) &&
           "Invalid LLBC_CFG_COMM_POLLER_MODEL config!");

    _pollerMgr.SetService(this);
    _pollerMgr.SetPollerType(pollerType);
}

LLBC_ServiceImpl::~LLBC_ServiceImpl()
{
    // Forbid destroy service in service thread(Self Drive mode).
    if (_driveMode == LLBC_ServiceDriveMode::SelfDrive)
        ASSERT(LLBC_GetCurrentThreadId() != _svcThreadId);

    // Forbid destroy service in service thread and not in <NotStarted> phase.
    if (_driveMode == LLBC_ServiceDriveMode::ExternalDrive)
        ASSERT(_runningPhase == LLBC_ServiceRunningPhase::NotStarted ||
               LLBC_GetCurrentThreadId() == _svcThreadId);

    // Stop service and destroy comps).
    Stop();
    DestroyComps(false);

    // Clear members.
    LLBC_STLHelper::DeleteContainer(_coderFactories);
    LLBC_STLHelper::DeleteContainer(_sessionProtoFactory);
    LLBC_XDelete(_dftProtocolFactory);
}

int LLBC_ServiceImpl::SetDriveMode(LLBC_ServiceDriveMode::ENUM driveMode)
{
    // Drive mode validate check.
    if (UNLIKELY(!LLBC_ServiceDriveMode::IsValid(driveMode)))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

     // If service not in <NotStarted> phase, could not change drive mode.
    __LLBC_INL_CHECK_RUNNING_PHASE_EQ(
        NotStarted, LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    // Set driveMode to _driveMode.
    _driveMode = driveMode;

    // If drive mode is external drive, Reinit object-pools, timer-scheduler, ...
    if (_driveMode == LLBC_ServiceDriveMode::ExternalDrive)
    {
        ClearTimerScheduler();
        ClearAutoReleasePool();

        InitAutoReleasePool();
        InitTimerScheduler();
    }

    return LLBC_OK;
}

int LLBC_ServiceImpl::SuppressCoderNotFoundWarning()
{
    __LLBC_INL_CHECK_RUNNING_PHASE_LE(
        StartingComps, LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);
    _suppressedCoderNotFoundWarning = true;

    return LLBC_OK;
}

int LLBC_ServiceImpl::Start(int pollerCount)
{
    // Normalize pollerCount.
    _pollerCount = MAX(1, pollerCount);

    // Reentry check.
    _lock.Lock();
    if (_runningPhase != LLBC_ServiceRunningPhase::NotStarted)
    {
        _lock.Unlock();
        LLBC_SetLastError(LLBC_ERROR_REENTRY);

        return LLBC_FAILED;
    }

    // Service repeat start check.
    if (_svcMgr.GetService(_id) ||
        _svcMgr.GetService(_name))
    {
        _lock.Unlock();
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    // Defer reset _startErrNo/_startSubErrNo.
    LLBC_Defer(
        _startErrNo = 0;
        _startSubErrNo = 0;
    );

    // PreStart -> InitComps -> StartComps.
    if (_driveMode == LLBC_ServiceDriveMode::ExternalDrive)
    {
        // PreStart:
        if (PreStart() != LLBC_OK ||
            InitComps() != LLBC_OK ||
            StartComps() != LLBC_OK)
        {
            _runningPhase = LLBC_ServiceRunningPhase::NotStarted;
            _lock.Unlock();

            return LLBC_FAILED;
        }
    }
    else
    {
        // Activate service task(thread).
        if (Activate(1) != LLBC_OK)
        {
            _lock.Unlock();
            return LLBC_FAILED;
        }

        // Set _runningPhase to <PreStarting>, and then unlock service lock.
        _runningPhase = LLBC_ServiceRunningPhase::PreStarting;
        _lock.Unlock();

        // Waiting for all service start finished or failed.
        while (!LLBC_ServiceRunningPhase::IsFailedPhase(_runningPhase) &&
               _runningPhase != LLBC_ServiceRunningPhase::CompsStarted)
            LLBC_Sleep(1);

        // Process startup failed.
        if (_runningPhase != LLBC_ServiceRunningPhase::CompsStarted)
        {
            LLBC_SetLastError(_startErrNo);
            LLBC_SetSubErrorNo(_startSubErrNo);
            _runningPhase = LLBC_ServiceRunningPhase::NotStarted;

            return LLBC_FAILED;
        }
    }

    // Update _runningPhase to Started phase.
    _runningPhase = LLBC_ServiceRunningPhase::Started;
    // Waiting for _serviceBeginLoop flag set to true.
    if (_driveMode == LLBC_ServiceDriveMode::SelfDrive)
    {
        while (!_serviceBeginLoop)
            LLBC_Sleep(1);
    }

    return LLBC_OK;
}

int LLBC_ServiceImpl::Stop()
{
    // If service not in <Started> phase, return failed.
    _lock.Lock();
    if (_runningPhase != LLBC_ServiceRunningPhase::Started)
    {
        _lock.Unlock();
        LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);

        return LLBC_FAILED;
    }

    // Get service driveMode/threadId.
    const int driveMode = _driveMode;
    const LLBC_ThreadId svcThreadId = _svcThreadId;

    // Update service phase to <Stopping>.
    _runningPhase = LLBC_ServiceRunningPhase::StoppingComps;
    _lock.Unlock();

    // Exec stop logic.
    const auto curThreadId = LLBC_GetCurrentThreadId();
    if (driveMode == LLBC_ServiceDriveMode::ExternalDrive)
    {
        if (curThreadId == svcThreadId && _sinkIntoOnSvcLoop)
        {
            LLBC_SetLastError(LLBC_ERROR_PENDING);
            return LLBC_FAILED;
        }

        Cleanup();
        return LLBC_OK;
    }
    else // SelfDrive
    {
        if (curThreadId == svcThreadId)
        {
            LLBC_SetLastError(LLBC_ERROR_PENDING);
            return LLBC_FAILED;
        }

        return Wait();
    }
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

    return LLBC_OK;
}

int LLBC_ServiceImpl::Listen(const char *ip,
                             uint16 port,
                             LLBC_IProtocolFactory *protoFactory,
                             const LLBC_SessionOpts &sessionOpts)
{
    __LLBC_INL_CHECK_RUNNING_PHASE_LE(
        LLBC_ServiceRunningPhase::StoppingComps, LLBC_ERROR_NOT_ALLOW, 0);

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
    __LLBC_INL_CHECK_RUNNING_PHASE_LE(
        LLBC_ServiceRunningPhase::StoppingComps, LLBC_ERROR_NOT_ALLOW, 0);

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
    __LLBC_INL_CHECK_RUNNING_PHASE_LE(
        LLBC_ServiceRunningPhase::StoppingComps, LLBC_ERROR_NOT_ALLOW, 0);

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
    // checkRunningPhase = true
    // checkSessionValidity = true
    return LockableSend(packet, true, true, true);
}

int LLBC_ServiceImpl::Multicast(const LLBC_SessionIds &sessionIds,
                                int opcode,
                                const void *bytes,
                                size_t len,
                                int status,
                                uint32 flags)
{
    // Argument check.
    if (UNLIKELY(sessionIds.empty()))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    // Lock & Check running phase.
    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_runningPhase < LLBC_ServiceRunningPhase::StartingComps ||
        LLBC_ServiceRunningPhase::IsFailedOrStoppingPhase(_runningPhase)))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);
        return LLBC_FAILED;
    }

    // Foreach to call internal method LockableSend() to complete.
    // lock = false
    // checkRunningPhase = false
    // checkSessionValidity = true
    const auto sessionIdsEndIt = sessionIds.end();
    for (auto sessionIt = sessionIds.begin();
         sessionIt != sessionIdsEndIt;
         ++sessionIt)
        LockableSend(*sessionIt, // sessionId
                     opcode, // opcode
                     bytes, // bytes
                     len, // len
                     status, // status
                     flags, // flags
                     false, // lock
                     false, // checkRunningPhase
                     true); // checkSessionValidity

    return LLBC_OK;
}

int LLBC_ServiceImpl::Broadcast(int opcode,
                                const void *bytes,
                                size_t len,
                                int status,
                                uint32 flags)
{
    // Lock & Check running phase.
    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(_runningPhase < LLBC_ServiceRunningPhase::StartingComps ||
        LLBC_ServiceRunningPhase::IsFailedOrStoppingPhase(_runningPhase)))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);
        return LLBC_FAILED;
    }

    // Get all non-listen sessionIds.
    static thread_local LLBC_SessionIds sessionIds;

    _readySessionInfosLock.Lock();
    const auto readySessionInfosEndIt = _readySessionInfos.end();
    for (auto it = _readySessionInfos.begin();
         it != readySessionInfosEndIt;
         ++it)
    {
        const _ReadySessionInfo &readySessionInfo = *it->second;
        if (!readySessionInfo.isListenSession)
            sessionIds.push_back(readySessionInfo.sessionId);
    }
    _readySessionInfosLock.Unlock();

    // return LLBC_OK, if is empty.
    if (sessionIds.empty())
        return LLBC_OK;

    // Foreach to call internal method LockableSend() to compolete.
    // lock = false
    // checkRunningPhase = false
    // checkSessionValidity = false
    const auto sessionIdsEndIt = sessionIds.end();
    for (auto sessionIt = sessionIds.begin();
         sessionIt != sessionIdsEndIt;
         ++sessionIt)
        LockableSend(*sessionIt, // sessionId
                     opcode, // opcode
                     bytes, // bytes
                     len, // len
                     status, // status
                     flags, // flags
                     false, // lock
                     false, // checkRunningPhase
                     false); // checkSessionValidity

    return LLBC_OK;
}

int LLBC_ServiceImpl::RemoveSession(int sessionId, const char *reason)
{
    __LLBC_INL_CHECK_RUNNING_PHASE_GE(
        InitingComps, LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

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
    __LLBC_INL_CHECK_RUNNING_PHASE_GE(
        InitingComps, LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

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
    // Argument check.
    if (UNLIKELY(!comp))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    // Check service running phase.
    __LLBC_INL_CHECK_RUNNING_PHASE_EQ(
        NotStarted, LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    // Get component name.
    const LLBC_String compName = LLBC_GetCompName(*comp);

    // Define component find lambda.
    const auto findLambda = [comp, compName](LLBC_Component *regComp) {
        return comp == regComp ||
            compName == LLBC_GetCompName(*regComp);
    };

    // Repeat add check.
    if (std::find_if(_willRegComps.begin(),
                     _willRegComps.end(),
                     findLambda) != _willRegComps.end() ||
        std::find_if(_compList.begin(),
                     _compList.end(),
                     findLambda) != _compList.end())
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    // Add to _willRegComps.
    _willRegComps.push_back(comp);

    return LLBC_OK;
}

int LLBC_ServiceImpl::AddComponent(const LLBC_String &compSharedLibPath,
                                   const LLBC_String &compName,
                                   LLBC_Component *&comp)
{
    // Force reset out parameter: comp.
    comp = nullptr;

    // Check service phase.
    __LLBC_INL_CHECK_RUNNING_PHASE_EQ(
        NotStarted, LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

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
    const LLBC_CString realCompName = LLBC_GetCompName(*comp);
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

    // Strict match.
    const auto compsEnd = _name2Comps.end();
    LLBC_LockGuard guard(_lock);
    auto it = _name2Comps.find(compName);
    if (it != compsEnd)
        return it->second;

    // Match IXxxYyy.
    if (compName.size() > 1 && compName[0] == 'I')
    {
        if ((it = _name2Comps.find(compName + 1)) != compsEnd)
            return it->second;
    }

    // Match BaseXxxYyy.
    if (compName.size() > 4 &&
        compName[0] == 'B' &&
        compName[1] == 'a' &&
        compName[2] == 's' &&
        compName[3] == 'e')
    {
        if ((it = _name2Comps.find(compName + 4)) != compsEnd)
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

    __LLBC_INL_CHECK_RUNNING_PHASE_LE(
        InitingComps, LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    if (!_coderFactories.insert(std::make_pair(opcode, coderFactory)).second)
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_ServiceImpl::Subscribe(int opcode, const LLBC_Delegate<void(LLBC_Packet &)> &deleg)
{
    if (UNLIKELY(!deleg))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    __LLBC_INL_CHECK_RUNNING_PHASE_LE(
        InitingComps, LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    if (!_handlers.insert(std::make_pair(opcode, deleg)).second)
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

    __LLBC_INL_CHECK_RUNNING_PHASE_LE(
        InitingComps, LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    if (!_preHandlers.insert(std::make_pair(opcode, deleg)).second)
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

    __LLBC_INL_CHECK_RUNNING_PHASE_LE(
        InitingComps, LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    if (_unifyPreHandler)
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
    if (UNLIKELY(!deleg || status == 0))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    __LLBC_INL_CHECK_RUNNING_PHASE_LE(
        InitingComps, LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

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
    __LLBC_INL_CHECK_RUNNING_PHASE_LE(
        InitingComps, LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    if (_driveMode != LLBC_ServiceDriveMode::ExternalDrive)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);
        return LLBC_FAILED;
    }

    _timerScheduler->SetEnabled(true);
    return LLBC_OK;
}

int LLBC_ServiceImpl::DisableTimerScheduler()
{
    __LLBC_INL_CHECK_RUNNING_PHASE_LE(
        InitingComps, LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    if (_driveMode != LLBC_ServiceDriveMode::ExternalDrive)
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

LLBC_ServiceEventFirer LLBC_ServiceImpl::BeginFireEvent(int eventId)
{
    LLBC_Event *ev = _threadSafeObjPool.Acquire<LLBC_Event>();
    ev->SetId(eventId);

    return LLBC_ServiceEventFirer(this, ev);
}

int LLBC_ServiceImpl::AddComponentEvent(int eventType, const LLBC_Variant &eventParams)
{
    if (eventType < LLBC_ComponentEventType::LogicBegin ||
        eventType >= LLBC_ComponentEventType::LogicEnd)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    return Push(LLBC_SvcEvUtil::BuildComponentEventEv(eventType, eventParams));
}

int LLBC_ServiceImpl::Post(const LLBC_Delegate<void(LLBC_Service *)> &runnable)
{
    if (UNLIKELY(!runnable))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    _lock.Lock();
    _posts.push_back(runnable);
    _lock.Unlock();

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
    _readySessionInfosLock.Lock();
    auto it = _readySessionInfos.find(sessionId);
    const LLBC_ProtocolStack *codecStack =
        it != _readySessionInfos.end() ? it->second->codecStack : nullptr;
    _readySessionInfosLock.Unlock();

    if (!codecStack)
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);

    return codecStack;
}

void LLBC_ServiceImpl::OnSvc(bool fullFrame)
{
    const auto frameInterval = GetFrameInterval();
    if (fullFrame && frameInterval == 0)
        fullFrame = false;

    while (UNLIKELY(_runningPhase != LLBC_ServiceRunningPhase::Started &&
                    _runningPhase != LLBC_ServiceRunningPhase::StartingComps &&
                    _runningPhase != LLBC_ServiceRunningPhase::StoppingComps))
    {
        LLBC_DoIf(fullFrame, LLBC_Sleep(frameInterval));
        return;
    }

    bool needResetSinkIntoFlag = false;
    if (!_sinkIntoOnSvcLoop)
    {
        _sinkIntoOnSvcLoop = true;
        needResetSinkIntoFlag = true;
    }

    // Record begin svc time, if fullFrame is true.
    if (fullFrame)
        _begSvcTime = LLBC_GetMilliseconds();

    // Handle posts.
    HandlePosts();

    // Process queued events.
    HandleQueuedEvents();

    // Update all components.
    UpdateComps();
    LateUpdateComps();
    UpdateTimerScheduler();
    UpdateAutoReleasePool();

    // Handle frame-tasks.
    HandlePosts();

    // Process Idle.
    if (fullFrame)
        ProcessIdle();

    // Sleep FrameInterval - ElapsedTime milli-seconds, if need.
    if (fullFrame)
    {
        const sint64 elapsed = LLBC_GetMilliseconds() - _begSvcTime;
        if (elapsed >= 0 && elapsed < frameInterval)
            LLBC_Sleep(static_cast<int>(frameInterval - elapsed));
    }

    // If in stopping phases(StoppingComp/Stopping) and is ExternalDrive mode, Exec cleanup.
    if (UNLIKELY(_runningPhase >= LLBC_ServiceRunningPhase::StoppingComps &&
        _driveMode == LLBC_ServiceDriveMode::ExternalDrive))
        Cleanup();

    // Reset sink into OnSvc() loop flag.
    if (needResetSinkIntoFlag)
        _sinkIntoOnSvcLoop = false;

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
        stack->SetCoders(&_coderFactories);
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
    LLBC_STLHelper::DeleteContainer(_readySessionInfos);
    _readySessionInfosLock.Unlock();
}

void LLBC_ServiceImpl::Svc()
{
    // Startup(lock service).
    _lock.Lock();
    if (PreStart() != LLBC_OK ||
        InitComps() != LLBC_OK ||
        StartComps() != LLBC_OK)
    {
        _lock.Unlock();
        return;
    }

    // Unlock.
    _lock.Unlock();

    // Waiting for _runningPhase updated to Started phase.
    while (_runningPhase != LLBC_ServiceRunningPhase::Started)
        LLBC_Sleep(1);

    // Do Svc.
    _serviceBeginLoop = true;
    while (LIKELY(_runningPhase == LLBC_ServiceRunningPhase::Started))
        OnSvc();
}

void LLBC_ServiceImpl::Cleanup()
{
    if (_inCleaning)
        return;

    _inCleaning = true;
    LLBC_Defer(_inCleaning = false);

    // Force handle queued events.
    if (_runningPhase >= LLBC_ServiceRunningPhase::StoppingComps &&
        _runningPhase <= LLBC_ServiceRunningPhase::Stopping)
        HandleQueuedEvents();

    // Stop comps.
    StopComps();

    // Post-Stop.
    PostStop();

    // Reset _serviceBeginLoop flag.
    _serviceBeginLoop = false;
    // Reset _runningPhase, if is not failed phase.
    if (!LLBC_ServiceRunningPhase::IsFailedPhase(_runningPhase))
        _runningPhase = LLBC_ServiceRunningPhase::NotStarted;
}

void LLBC_ServiceImpl::UpdateServiceCfg(int appCfgType, const LLBC_Variant &appCfg)
{
    // Update config type.
    _cfgType = appCfgType;
    _cfg.BecomeNil();

    if (_cfgType < LLBC_AppConfigType::Begin ||
        _cfgType >= LLBC_AppConfigType::End)
        return;

    // Update service config.
    bool isXmlCfg = false;
    if (_cfgType == LLBC_AppConfigType::Property)
    {
        // Service config prop name:
        // <svc_name>
        // <svc_name>.xxx
        // <svc_name>.xxx.xxx
        // <svc_name>.xxx.xxx.xxx....
        _cfg = appCfg[GetName()];
    }
    else if (_cfgType == LLBC_AppConfigType::Ini)
    {
        // Service config section: [<svc_name>]
        // Comp config section(s): [<svc_name>.<comp_name>]
        for (auto it = appCfg.DictBegin(); it != appCfg.DictEnd(); ++it)
        {
            const auto iniSectionName = it->first.AsStr();
            if (iniSectionName == GetName())
            {
                for (auto &iniCfgItem : it->second.AsDict())
                    _cfg.DictInsert(iniCfgItem.first, iniCfgItem.second);
            }
            else if (iniSectionName.startswith(GetName() + ".") && iniSectionName.size() > GetName().size() + 1)
            {
                auto &compCfg = _cfg[iniSectionName.substr(GetName().size() + 1)];
                for (auto &compCfgItem : it->second.AsDict())
                    compCfg.DictInsert(compCfgItem.first, compCfgItem.second);
            }
        }
    }
    else // if (_cfgType == LLBC_AppConfigType::Xml)
    {
        isXmlCfg = true;
        auto &svcCfgs = appCfg[LLBC_XMLKeys::Children];
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
                _cfg = svcCfg;
                break;
            }
        }
    }

    // After config update, read recognizable service config.
    // - Update service fps:
    for (auto cfgItem : _cfg.AsDict())
    {
        if (cfgItem.first.AsStr().tolower() == "fps")
        {
            const auto fps = isXmlCfg ? cfgItem.second[LLBC_XMLKeys::Value] : cfgItem.second;
            SetFPS(fps);
            break;
        }
    }

    // - Update other service configs.
    // ... ...
}

void LLBC_ServiceImpl::AddServiceToTls()
{
    __LLBC_LibTls *tls = __LLBC_GetLibTls();

    for (int idx = 0; idx < LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT; ++idx)
    {
        if (!tls->commTls.services[idx])
        {
            tls->commTls.services[idx] = this;
            return;
        }
    }

    ASSERT(false && "llbc framework internal error");
}

void LLBC_ServiceImpl::RemoveServiceFromTls()
{
    __LLBC_LibTls *tls = __LLBC_GetLibTls();

    int svcCnt = 0;
    int svcIdx = -1;
    const int lmt = LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT;
    for (; svcCnt < lmt; ++svcCnt)
    {
        if (!tls->commTls.services[svcCnt])
            break;

        if (tls->commTls.services[svcCnt] == this)
            svcIdx = svcCnt;
    }

    if (UNLIKELY(svcCnt == 0 || svcIdx == -1))
        return;

    if (svcIdx != svcCnt - 1)
    {
        memmove(&tls->commTls.services[svcIdx],
                &tls->commTls.services[svcIdx + 1],
                sizeof(tls->commTls.services[0]) * (svcCnt - svcIdx - 1));
    }

    tls->commTls.services[svcCnt - 1] = nullptr;
}

bool LLBC_ServiceImpl::IsCanContinueDriveService()
{
    __LLBC_LibTls *tls = __LLBC_GetLibTls();

    for (int idx = 0; idx < LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT; ++idx)
    {
        if (!tls->commTls.services[idx])
            return true;
    }

    return false;
}

int LLBC_ServiceImpl::PreStart()
{
    // Set _runningPhase to PreStarting.
    _runningPhase = LLBC_ServiceRunningPhase::PreStarting;

    // Defer execute fail process lambda.
    int ret = LLBC_FAILED;
    LLBC_Defer(if (ret != LLBC_OK) {
        // Fetch errNo/subErrNo first.
        _startErrNo = LLBC_GetLastError();
        _startSubErrNo = LLBC_GetSubErrorNo();

        // Rollback PreStart state.
        PostStop();

        // Set _runningPhase to PreStartFailed.
        _runningPhase = LLBC_ServiceRunningPhase::PreStartFailed;
    });

    // Get service thread Id.
    _svcThreadId = LLBC_GetCurrentThreadId();

    // Check current thread is can continue drive service ot not.
    if (!IsCanContinueDriveService())
    {
        LLBC_SetLastError(LLBC_ERROR_LIMIT);
        return LLBC_FAILED;
    }

    // Update service config.
    auto app = LLBC_App::ThisApp();
    if (app)
    {
        app->PreventReload();
        UpdateServiceCfg(app->GetConfigType(), app->GetConfigUnsafe());
        app->CancelPreventReload();
    }

    // Initialize PollerMgr.
    if (_pollerMgr.Init(_pollerCount) != LLBC_OK)
        return LLBC_FAILED;

    // Add service to TLS.
    AddServiceToTls();

    // Init timer-scheduler.
    InitTimerScheduler();
    // Init auto-release-pool.
    InitAutoReleasePool();
    // ... ...

    // Add service to ServiceMgr.
    _svcMgr.OnServiceStart(this);

    // Set _runningPhase to PreStarted.
    _runningPhase = LLBC_ServiceRunningPhase::PreStarted;

    return ret = LLBC_OK;
}

void LLBC_ServiceImpl::PostStop()
{
    // Remove service from ServiceMgr.
    _svcMgr.OnServiceStop(this);

    // ... ...
    // Clear auto-release-pool.
    ClearAutoReleasePool();
    // Clear timer-scheduler.
    ClearTimerScheduler();

    // Remove service from TLS.
    RemoveServiceFromTls();

    // Finalize PollerMgr.
    _pollerMgr.Finalize();

    // Popup & Destroy all not-process events.
    LLBC_MessageBlock *block;
    while (TryPop(block) == LLBC_OK)
        LLBC_SvcEvUtil::DestroyEvBlock(block);

    // Remove all ready sessions.
    RemoveAllReadySessions();

    // Cleanup service config.
    _cfgType = LLBC_AppConfigType::End;
    _cfg.BecomeNil();

    // Reset _svcThreadId.
    _svcThreadId = LLBC_INVALID_NATIVE_THREAD_ID;
    // Reset _pollerCount.
    _pollerCount = 0;
}

void LLBC_ServiceImpl::HandlePosts()
{
    LLBC_ReturnIf(_posts.empty(), void());

    std::vector<LLBC_Delegate<void(LLBC_Service *)> > posts;
    std::swap(posts, _posts);

    const auto endIt = posts.end();
    for (auto it = posts.begin(); it != endIt; ++it)
        (*it)(this);
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

    // Build session info.
    LLBC_SessionInfo info;
    info.SetIsListenSession(ev.isListen);
    info.SetSessionId(ev.sessionId);
    info.SetAcceptSessionId(ev.acceptSessionId);
    info.SetLocalAddr(ev.local);
    info.SetPeerAddr(ev.peer);
    info.SetSocket(ev.handle);
    LLBC_Variant eventParams(&info);

    // Dispatch session-create event to all comps.
    for(auto &comp : _compList)
    {
        if (comp->_runningPhase == _CompRunningPhase::LateStarted)
            comp->OnEvent(LLBC_ComponentEventType::SessionCreate, eventParams);
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
    LLBC_Variant eventParams(&destroyInfo);

    // Dispatch session-destroy event to all comps.
    for(auto &comp : _compList)
    {
        if (comp->_runningPhase == _CompRunningPhase::LateStarted)
            comp->OnEvent(LLBC_ComponentEventType::SessionDestroy, eventParams);
    }

    // Remove session protocol factory.
    if (ev.acceptSessionId == 0)
        RemoveSessionProtocolFactory(ev.sessionId);
}

void LLBC_ServiceImpl::HandleEv_AsyncConnResult(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_AsyncConn _Ev;
    _Ev &ev = static_cast<_Ev &>(_);

    // Build async-conn-result info.
    LLBC_AsyncConnResult result;
    result.SetIsConnected(ev.connected);
    result.SetSessionId(ev.sessionId);
    result.SetReason(ev.reason);
    result.SetPeerAddr(ev.peer);
    LLBC_Variant eventParams(&result);

    // Dispatch async-conn-result event to all comps.
    for(auto &comp : _compList)
    {
        if (comp->_runningPhase == _CompRunningPhase::LateStarted)
            comp->OnEvent(LLBC_ComponentEventType::AsyncConnResult, eventParams);
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

    const int opcode = packet->GetOpcode();
    #if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
    const int status = packet->GetStatus();
    if (status != 0)
    {
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
    #endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER

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
        // Dispatch unhandled-packet event to all comps.
        LLBC_Variant eventParams(packet);
        for(auto &comp : _compList)
        {
            if (comp->_runningPhase == _CompRunningPhase::LateStarted)
                comp->OnEvent(LLBC_ComponentEventType::UnHandledPacket, eventParams);
        }
    }

    LLBC_Recycle(packet);
}

void LLBC_ServiceImpl::HandleEv_ProtoReport(LLBC_ServiceEvent &_)
{
    typedef LLBC_SvcEv_ProtoReport _Ev;
    _Ev &ev = static_cast<_Ev &>(_);

    // Build proto-report.
    LLBC_ProtoReport report;
    report.SetSessionId(ev.sessionId);
    report.SetOpcode(ev.opcode);
    report.SetLayer(ev.layer);
    report.SetLevel(ev.level);
    report.SetReport(ev.report);
    LLBC_Variant eventParams(&report);

    // Dispatch proto-report event to all comps.
    for(auto &comp : _compList)
    {
        if (comp->_runningPhase == _CompRunningPhase::LateStarted)
            comp->OnEvent(LLBC_ComponentEventType::ProtoReport, eventParams);
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

    LLBC_Variant eventParams;
    if (ev.willStart)
    {
        for(auto &comp : _compList)
        {
            if (comp->_runningPhase == _CompRunningPhase::LateStarted)
                comp->OnEvent(LLBC_ComponentEventType::AppWillStart, eventParams);
        }
    }
    else if (ev.startFailed)
    {
        for(auto &comp : _compList)
        {
            if (comp->_runningPhase == _CompRunningPhase::LateStarted)
                comp->OnEvent(LLBC_ComponentEventType::AppStartFailed, eventParams);
        }
    }
    else if (ev.startFinished)
    {
        if (_cfgType == LLBC_AppConfigType::End)
            UpdateServiceCfg(ev.cfgType, ev.cfg);

        for(auto &comp : _compList)
        {
            if (comp->_runningPhase == _CompRunningPhase::LateStarted)
                comp->OnEvent(LLBC_ComponentEventType::AppStartFinished, eventParams);
        }
    }
    else if (ev.willStop)
    {
        for(auto &comp : _compList)
        {
            if (comp->_runningPhase == _CompRunningPhase::LateStarted)
                comp->OnEvent(LLBC_ComponentEventType::AppWillStop, eventParams);
        }
    }
}

void LLBC_ServiceImpl::HandleEv_AppReloaded(LLBC_ServiceEvent &_)
{
    // Update service config.
    auto &ev = static_cast<LLBC_SvcEv_AppReloadedEv &>(_);
    UpdateServiceCfg(ev.cfgType, ev.cfg);

    // Update all components config.
    for (auto &compItem : _name2Comps)
        compItem.second->UpdateComponentCfg();

    // Dispatch app-reload event to all comps.
    LLBC_Variant eventParams{};
    for(auto &comp : _compList)
    {
        if (comp->_runningPhase == _CompRunningPhase::LateStarted)
            comp->OnReload();
    }
}

void LLBC_ServiceImpl::HandleEv_ComponentEvent(LLBC_ServiceEvent &_)
{
    auto &ev = static_cast<LLBC_SvcEv_ComponentEventEv &>(_);
    for (auto &comp : _compList)
    {
        if (comp->_runningPhase == _CompRunningPhase::LateStarted)
            comp->OnEvent(ev.eventType, ev.eventParams);
    }
}

// Define comp init macro.
#define __LLBC_Inl_InitComp(comp, initMeth, toPhase, failSetErrno)        \
    {                                                                     \
        while (true) {                                                    \
            bool initFinished = true;                                     \
            LLBC_SetLastError(LLBC_ERROR_SUCCESS);                        \
            if (comp->initMeth(initFinished) != LLBC_OK) {                \
                if (LLBC_GetLastError() == LLBC_ERROR_SUCCESS)            \
                    LLBC_SetLastError(failSetErrno);                      \
                                                                          \
                _startErrNo = LLBC_GetLastError();                        \
                _startSubErrNo = LLBC_GetSubErrorNo();                    \
                                                                          \
                break;                                                    \
            }                                                             \
                                                                          \
            if (initFinished) {                                           \
                comp->_runningPhase = _CompRunningPhase::toPhase;         \
                break;                                                    \
            }                                                             \
                                                                          \
            if (_CompRunningPhase::toPhase >= _CompRunningPhase::Started) \
                OnSvc(false);                                             \
            LLBC_Sleep(LLBC_CFG_APP_TRY_START_INTERVAL);                  \
        }                                                                 \
                                                                          \
        if (comp->_runningPhase != _CompRunningPhase::toPhase) {          \
            compsInitSucc = false;                                        \
            break;                                                        \
        }                                                                 \
    }                                                                     \

int LLBC_ServiceImpl::InitComps()
{
    // Set _runningPhase to InitingComps
    _runningPhase = LLBC_ServiceRunningPhase::InitingComps;

    // Init comps.
    bool compsInitSucc = true;
    while (!_willRegComps.empty())
    {
        // Pop will-reg comp.
        auto willRegCompIt = _willRegComps.begin();
        auto comp = *willRegCompIt;
        _willRegComps.erase(willRegCompIt);

        // Set service to comp & update comp config & add to service's comps dict.
        comp->SetService(this);
        comp->UpdateComponentCfg();
        AddComp(comp);

        // Init comp.
        if (comp->_runningPhase == _CompRunningPhase::NotInit)
            __LLBC_Inl_InitComp(comp, OnInit, Inited, LLBC_ERROR_COMP_INIT_FAILED);
    }

    // Late-Init comps.
    for (auto &comp : _compList)
    {
        // Late-Init comp.
        if (comp->_runningPhase == _CompRunningPhase::Inited)
            __LLBC_Inl_InitComp(comp, OnLateInit, LateInited, LLBC_ERROR_COMP_LATE_INIT_FAILED);
    }

    if (!compsInitSucc)
    {
        DestroyComps(true);
        PostStop();

        LLBC_SetLastError(_startErrNo);
        LLBC_SetSubErrorNo(_startSubErrNo);

        _runningPhase = LLBC_ServiceRunningPhase::InitCompsFailed;

        return LLBC_FAILED;
    }

    _runningPhase = LLBC_ServiceRunningPhase::CompsInited;

    return LLBC_OK;
}

// Define component destroy macro.
#define __LLBC_Inl_DestoryComp(comp, destroyMeth, toPhase)    \
    while (true) {                                            \
        bool destroyFinished = true;                          \
        comp->destroyMeth(destroyFinished);                   \
        if (destroyFinished) {                                \
            comp->_runningPhase = _CompRunningPhase::toPhase; \
            break;                                            \
        }                                                     \
                                                              \
        LLBC_Sleep(LLBC_CFG_APP_TRY_STOP_INTERVAL);           \
    }                                                         \

void LLBC_ServiceImpl::DestroyComps(bool onlyCallEvMeth)
{
    // Early-Destroy comps.
    for (auto it = _compList.rbegin(); it != _compList.rend(); ++it)
    {
        LLBC_Component *comp = *it;
        if (comp->_runningPhase == _CompRunningPhase::LateInited)
            __LLBC_Inl_DestoryComp(comp, OnEarlyDestroy, Inited);
    }

    // Destroy comps.
    for (auto it = _compList.rbegin(); it != _compList.rend(); ++it)
    {
        LLBC_Component *comp = *it;
        if (comp->_runningPhase == _CompRunningPhase::Inited)
            __LLBC_Inl_DestoryComp(comp, OnDestroy, NotInit);
    }

    if (onlyCallEvMeth)
        return;

    // Delete all components.
    LLBC_STLHelper::DeleteContainer(_compList, true);
    while (!_name2Comps.empty())
    {
        auto it = _name2Comps.begin();
        auto compName = it->first;
        _name2Comps.erase(it);

        free(const_cast<char *>(compName.c_str()));
    }

    // Delete all will-register components.
    LLBC_STLHelper::RecycleContainer(_willRegComps, true);

    // Close all component libraries.
    LLBC_STLHelper::DeleteContainer(_compLibraries);
}

int LLBC_ServiceImpl::StartComps()
{
    // Set _runningPhase to StartingComps.
    _runningPhase = LLBC_ServiceRunningPhase::StartingComps;

    // Before start comps, startup poller mgr.
    if (_pollerMgr.Start() != LLBC_OK)
    {
        _startErrNo = LLBC_GetLastError();
        _startSubErrNo = LLBC_GetSubErrorNo();

        DestroyComps(true);
        PostStop();

        LLBC_SetLastError(_startErrNo);
        LLBC_SetSubErrorNo(_startSubErrNo);

        _runningPhase = LLBC_ServiceRunningPhase::StartCompsFailed;

        return LLBC_FAILED;
    }

    // Start comps.
    bool compsInitSucc = true;
    for (auto &comp : _compList)
    {
        if (comp->_runningPhase == _CompRunningPhase::LateInited)
            __LLBC_Inl_InitComp(comp, OnStart, Started, LLBC_ERROR_COMP_START_FAILED);
    }

    // Late-Start comps.
    if (compsInitSucc)
    {
        for (auto comp : _compList)
        {
            if (comp->_runningPhase == _CompRunningPhase::Started)
                __LLBC_Inl_InitComp(comp, OnLateStart, LateStarted, LLBC_ERROR_COMP_LATE_START_FAILED);
        }
    }

    if (!compsInitSucc)
    {
        StopComps();
        DestroyComps(true);
        PostStop();

        LLBC_SetLastError(_startErrNo);
        LLBC_SetSubErrorNo(_startSubErrNo);

        _runningPhase = LLBC_ServiceRunningPhase::StartCompsFailed;

        return LLBC_FAILED;
    }

    _runningPhase = LLBC_ServiceRunningPhase::CompsStarted;

    return LLBC_OK;
}

void LLBC_ServiceImpl::StopComps()
{
    // Before-Stop comps.
    for (auto it = _compList.rbegin(); it != _compList.rend(); ++it)
    {
        LLBC_Component *comp = *it;
        if (comp->_runningPhase == _CompRunningPhase::LateStarted)
            __LLBC_Inl_DestoryComp(comp, OnEarlyStop, Started);
    }

    // Stop comps.
    for (auto it = _compList.rbegin(); it != _compList.rend(); ++it)
    {
        LLBC_Component *comp = *it;
        if (comp->_runningPhase == _CompRunningPhase::Started)
            __LLBC_Inl_DestoryComp(comp, OnStop, LateInited);
    }

    // Update _runningPhase to <Stopping> phase, if in <StoppingComps> phase.
    if (_runningPhase == LLBC_ServiceRunningPhase::StoppingComps)
        _runningPhase = LLBC_ServiceRunningPhase::Stopping;

    // Stop poller mgr.
    _pollerMgr.Stop();
}

// Undef comp init macro.
#undef __LLBC_Inl_InitComp

// Undef component destroy macro.
#undef __LLBC_Inl_DestroyComp

void LLBC_ServiceImpl::UpdateComps()
{
    for(auto &comp : _compList)
    {
        if (comp->_runningPhase >= _CompRunningPhase::Started)
            comp->OnUpdate();
    }
}

void LLBC_ServiceImpl::LateUpdateComps()
{
    for(auto &comp : _compList)
    {
        if (comp->_runningPhase >= _CompRunningPhase::Started)
            comp->OnLateUpdate();
    }
}

void LLBC_ServiceImpl::AddComp(LLBC_Component *comp)
{
    _compList.push_back(comp);

    const LLBC_CString compName = LLBC_GetCompName(*comp);
    auto allocCompName = LLBC_Malloc(char, compName.size() + 1);
    memcpy(allocCompName, compName.c_str(), compName.size() + 1);

    _name2Comps.emplace(LLBC_CString(allocCompName, compName.size()), comp);
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

 void LLBC_ServiceImpl::InitAutoReleasePool()
{
    _releasePoolStack = LLBC_AutoReleasePoolStack::GetCurrentThreadReleasePoolStack();
}

void LLBC_ServiceImpl::UpdateAutoReleasePool()
{
    if (_driveMode == LLBC_ServiceDriveMode::SelfDrive)
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

void LLBC_ServiceImpl::ClearTimerScheduler()
{
    if (_timerScheduler)
    {
        _timerScheduler->CancelAll();
        _timerScheduler = nullptr;
    }
}

void LLBC_ServiceImpl::ProcessIdle()
{
    const auto frameInterval = GetFrameInterval();
    for(auto &comp : _compList)
    {
        sint64 elapsed = LLBC_GetMilliseconds() - _begSvcTime;
        if (LIKELY(elapsed >= 0))
        {
            if (elapsed >= frameInterval)
                break;

            if (comp->_runningPhase == _CompRunningPhase::LateStarted)
                comp->OnIdle(LLBC_TimeSpan::FromMillis(frameInterval - elapsed));
        }
    }
}

LLBC_FORCE_INLINE int LLBC_ServiceImpl::LockableSend(LLBC_Packet *packet,
                                                     bool lock,
                                                     bool checkRunningPhase,
                                                     bool checkSessionValidity)
{
    // Lock if need.
    if (lock)
        _lock.Lock();

    // Running phase check.
    if (checkRunningPhase)
    {
        if (UNLIKELY(_runningPhase < LLBC_ServiceRunningPhase::StartingComps ||
            LLBC_ServiceRunningPhase::IsFailedOrStoppingPhase(_runningPhase)))
        {
            if (lock)
                _lock.Unlock();

            LLBC_Recycle(packet);

            LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);
            return LLBC_FAILED;
        }
    }

    // Validate check, if need.
    const _ReadySessionInfo *readySInfo = nullptr;
    const int sessionId = packet->GetSessionId();
    if (!_fullStack || checkSessionValidity)
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

LLBC_FORCE_INLINE int LLBC_ServiceImpl::LockableSend(int sessionId,
                                                     int opcode,
                                                     const void *bytes,
                                                     size_t len,
                                                     int status,
                                                     uint32 flags,
                                                     bool lock,
                                                     bool checkRunningPhase,
                                                     bool checkSessionValidity)
{
    // Create packet(from object pool) and send.
    LLBC_Packet *packet = _threadSafeObjPool.Acquire<LLBC_Packet>();
    packet->SetHeader(sessionId, opcode, status, flags);
    int ret = packet->Write(bytes, len);
    if (UNLIKELY(ret != LLBC_OK))
    {
        LLBC_Recycle(packet);
        return ret;
    }

    return LockableSend(packet, lock, checkRunningPhase, checkSessionValidity);
}

LLBC_ServiceImpl::_ReadySessionInfo::_ReadySessionInfo(int sessionId,
                                                       int acceptSessionId,
                                                       bool isListenSession,
                                                       LLBC_ProtocolStack *codecStack)
: sessionId(sessionId)
, acceptSessionId(acceptSessionId)
, isListenSession(isListenSession)
, codecStack(codecStack)
{
}

LLBC_ServiceImpl::_ReadySessionInfo::~_ReadySessionInfo()
{
    if (codecStack)
        delete codecStack;
}

__LLBC_NS_END

#undef __LLBC_INL_CHECK_RUNNING_PHASE_GE
#undef __LLBC_INL_CHECK_RUNNING_PHASE_LE
#undef __LLBC_INL_CHECK_RUNNING_PHASE_EQ
