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
#include "llbc/comm/Component.h"
#include "llbc/comm/ServiceImpl.h"
#include "llbc/comm/ServiceMgr.h"

#include "llbc/app/App.h"

namespace
{
    typedef LLBC_NS LLBC_ProtocolStack _Stack;
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
    &LLBC_ServiceImpl::HandleEv_AppCfgReload,
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
, _runningPhase(LLBC_ServiceRunningPhase::NotStarted)

, _startErrNo(0)
, _startSubErrNo(0)

, _cfgType(LLBC_AppConfigType::End)

, _fullStack(fullStack)
, _pollerCount(0)
, _suppressedCoderNotFoundWarning(false)
, _dftProtocolFactory(dftProtocolFactory)

, _fps(LLBC_CFG_COMM_DFT_SERVICE_FPS)
, _frameInterval(1000 / LLBC_CFG_COMM_DFT_SERVICE_FPS)
, _begSvcTime(0)

, _caredEventComps{}

// Service extend functions about members.
, _releasePoolStack(nullptr)

, _packetObjectPool(*_safeObjectPool.GetPoolInst<LLBC_Packet>())
, _msgBlockObjectPool(*_safeObjectPool.GetPoolInst<LLBC_MessageBlock>())
, _eventFirerPool(*_safeObjectPool.GetPoolInst<LLBC_ServiceEventFirer>())

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
               LLBC_GetCurrentThreadId() != _svcThreadId);

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
        while (_runningPhase != LLBC_ServiceRunningPhase::PreStartFailed &&
               _runningPhase != LLBC_ServiceRunningPhase::InitCompsFailed &&
               _runningPhase != LLBC_ServiceRunningPhase::StartCompsFailed &&
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

int LLBC_ServiceImpl::GetFPS() const
{
    LLBC_LockGuard guard(_lock);
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
        _frameInterval = 0;
    }

    return LLBC_OK;
}

int LLBC_ServiceImpl::GetFrameInterval() const
{
    LLBC_LockGuard guard(_lock);
    return _frameInterval;
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
    size_t compNameLen;
    char compName[LLBC_CFG_COMM_MAX_COMP_NAME_LEN + 1];
    GetCompName(typeid(*comp).name(), compName, compNameLen);

    // Define component find lambda.
    const auto findLambda = [comp, compName, compNameLen](LLBC_Component *regComp) {
        if (comp == regComp)
            return true;

        char regCompName[LLBC_CFG_COMM_MAX_COMP_NAME_LEN + 1];
        size_t regCompNameLen;
        GetCompName(typeid(*regComp).name(), regCompName, regCompNameLen);

        return regCompNameLen == compNameLen &&
            memcmp(compName, regCompName, compNameLen) == 0;
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
    auto realCompName = LLBC_GetTypeName(*comp);
    const auto realCompNSNameEnd = strrchr(realCompName, ':');
    if (realCompNSNameEnd != nullptr)
        realCompName = realCompNSNameEnd + 1;
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

LLBC_ServiceEventFirer &LLBC_ServiceImpl::BeginFireEvent(int eventId)
{
    LLBC_Event *ev = LLBC_GetObjectFromSafePool<LLBC_Event>();
    ev->SetId(eventId);

    auto *eventServiceFirer = _eventFirerPool.GetReferencableObject();
    LLBC_AutoRelease(eventServiceFirer);
    SetEventInfo(eventServiceFirer, ev);

    return *eventServiceFirer;
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
    if (fullFrame && _frameInterval == 0)
        fullFrame = false;

    while (UNLIKELY(_runningPhase != LLBC_ServiceRunningPhase::Started &&
                    _runningPhase != LLBC_ServiceRunningPhase::StoppingComps))
    {
        LLBC_DoIf(fullFrame, LLBC_Sleep(_frameInterval));
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
        if (elapsed >= 0 && elapsed < _frameInterval)
            LLBC_Sleep(static_cast<int>(_frameInterval - elapsed));
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

    // Update _runningPhase to <NotStarted>.
    _runningPhase = LLBC_ServiceRunningPhase::NotStarted;
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
    UpdateServiceCfg();

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
    _propCfg.RemoveAllProperties();
    _nonPropCfg.BecomeNil();

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
    // Set _runningPhase to InitingComps
    _runningPhase = LLBC_ServiceRunningPhase::InitingComps;

    // Use while() for support repeatly add components operation.
    bool compsInitSucc = true;
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

        // If comp don't care <OnInit> event, set to inited, and than continue.
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

                _startErrNo = LLBC_GetLastError();
                _startSubErrNo = LLBC_GetSubErrorNo();

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
            compsInitSucc = false;
            break;
        }
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

void LLBC_ServiceImpl::DestroyComps(bool onlyCallEvMeth)
{
    // Call components OnDestroy() event method.
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

    for (auto &evComps: _caredEventComps)
        evComps.clear();

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

                _startErrNo = LLBC_GetLastError();
                _startSubErrNo = LLBC_GetSubErrorNo();
                break;
            }

            if (startFinished)
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

        _runningPhase = LLBC_ServiceRunningPhase::CompsStarted;
        return LLBC_OK;
    }
    else // Otherwise stop all comps.
    {
        StopComps();
        DestroyComps(true);
        PostStop();

        LLBC_SetLastError(_startErrNo);
        LLBC_SetSubErrorNo(_startSubErrNo);

        _runningPhase = LLBC_ServiceRunningPhase::StartCompsFailed;

        return LLBC_OK;
    }
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

    // Update _runningPhase to <Stopping> phase, if in <StoppingComps> phase.
    if (_runningPhase >= LLBC_ServiceRunningPhase::StoppingComps &&
        _runningPhase <= LLBC_ServiceRunningPhase::Stopping)
        _runningPhase = LLBC_ServiceRunningPhase::Stopping;

    if (_runningPhase)

    // Stop poller mgr.
    _pollerMgr.Stop();
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

void LLBC_ServiceImpl::LateUpdateComps()
{
    auto &caredComps = _caredEventComps[LLBC_ComponentEventIndex::OnLateUpdate];
    if (caredComps.empty())
        return;

    const size_t compsSize = caredComps.size();
    for (size_t compIdx = 0; compIdx != compsSize; ++compIdx)
    {
        LLBC_Component *&comp = caredComps[compIdx];
        if (LIKELY(comp->_started))
            comp->OnLateUpdate();
    }
}

void LLBC_ServiceImpl::AddComp(LLBC_Component *comp)
{
    _compList.push_back(comp);

    auto compName = LLBC_GetTypeName(*comp);
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
    auto &caredComps = _caredEventComps[LLBC_ComponentEventIndex::OnIdle];
    LLBC_ReturnIf(caredComps.empty(), void());

    const size_t compsSize = caredComps.size();
    for (size_t compIdx = 0; compIdx != compsSize; ++compIdx)
    {
        sint64 elapsed = LLBC_GetMilliseconds() - _begSvcTime;
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
    LLBC_Packet *packet = _packetObjectPool.GetObject();
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
