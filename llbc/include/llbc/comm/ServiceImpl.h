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

#pragma once

#include "llbc/comm/Service.h"
#include "llbc/comm/ServiceEvent.h"
#include "llbc/comm/ServiceEventFirer.h"
#include "llbc/comm/PollerMgr.h"
#include "llbc/comm/protocol/ProtocolStack.h"

__LLBC_NS_BEGIN

/**
 * Previous declare some classes.
 */
class LLBC_ServiceMgr;

__LLBC_NS_END

__LLBC_NS_BEGIN

class LLBC_HIDDEN LLBC_ServiceImpl final : public LLBC_Service
{
public:
    /**
     * Create specified type service.
     * @param[in] name               - type service name.
     * @param[in] dftProtocolFactory - the default protocol factory.
     * @param[in] fullStack          - the full stack option, default is true.
     */
    LLBC_ServiceImpl(const LLBC_String &name = "",
                     LLBC_IProtocolFactory *dftProtocolFactory = nullptr,
                     bool fullStack = true);

    /**
     * Service destructor.
     */
    ~LLBC_ServiceImpl() override;

public:
    /**
     * Get the service Id.
     * @return int - the service Id.
     */
    int GetId() const override;

    /**
     * Get the service name.
     * @return const LLBC_String & - the service name.
     */
    const LLBC_String &GetName() const override;

    /**
     * Get config type.
     * @return int - the config type, see LLBC_AppConfigType enum.
     */
    int GetConfigType() const override;

    /**
     * Get service config.
     * @return const LLBC_Variant & - the service config.
     */
    const LLBC_Variant &GetConfig() const override;

    /**
     * Get full stack option.
     * @return bool - the full stack option.
     */
    bool IsFullStack() const override;

    /**
     * Get the service drive mode.
     * @return DriveMode - the service drive mode.
     */
    LLBC_ServiceDriveMode::ENUM GetDriveMode() const override;

    /**
     * Set the service drive mode.
     * @param[in] mode - the service drive mode.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetDriveMode(LLBC_ServiceDriveMode::ENUM mode) override;

public:
    /**
     * Suppress coder not found warning in protocol-stack.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SuppressCoderNotFoundWarning() override;

public:
    /**
     * Startup service, default will startup one poller to work.
     * @param[in] pollerCount - the poller count.
     * @return int - return 0 if startup successful, otherwise return -1.
     */
    int Start(int pollerCount = 1) override;

    /**
     * Check service is started or not.
     * @return bool - return true if running, otherwise return false.
     */
    bool IsStarted() const override;

    /**
     * Stop the service.
     * @param[in] destroyComp - destroy service components or not.
     * @return int - return 0 if success, otherwise return failed.
     */
    int Stop(bool destroyComp = false) override;

public:
    /**
     * Get service FPS.
     * @return int - the service FPS.
     */
    int GetFPS() const override;

    /**
     * Set service FPS.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetFPS(int fps) override;

    /**
     * Get service frame interval, in milli-seconds.
     * @return int - return 0 if success, otherwise return -1.
     */
    int GetFrameInterval() const override;

public:
    /**
     * Create a session and listening.
     * Note:
     *      If service not start when call this method, connection operation will
     *      create a pending-operation and recorded in service, your maybe could not get error.
     * @param[in] ip           - the ip address.
     * @param[in] port         - the port number.
     * @param[in] protoFactory - the protocol factory, default use service protocol factory.
     *                           if use custom protocol factory, when Listen failed, the factory will delete by framework.
     * @param[in] sessionOpts  - the session options.
     * @return int - the new session Id, if return 0, means failed, see LLBC_GetLastError().
     */
    int Listen(const char *ip,
               uint16 port,
               LLBC_IProtocolFactory *protoFactory = nullptr,
               const LLBC_SessionOpts &sessionOpts = LLBC_DftSessionOpts) override;

    /**
     * Establishes a connection to a specified address.
     * Note:
     *      If service not start when call this method, connection operation will
     *      create a pending-operation and recorded in service, your maybe could not get error.
     * @param[in] ip           - the ip address.
     * @param[in] port         - the port number.
     * @param[in] timeout      - the timeout value on connect operation, default use OS setting.
     * @param[in] protoFactory - the protocol factory, default use service protocol factory.
     *                           if use custom protocol factory, when Connect failed, the factory will delete by framework.
     * @param[in] sessionOpts  - the session options.
     * @return int - the new session Id, if return 0, means failed, see LBLC_GetLastError().
     */
    int Connect(const char *ip,
                uint16 port,
                double timeout = -1.0,
                LLBC_IProtocolFactory *protoFactory = nullptr,
                const LLBC_SessionOpts &sessionOpts = LLBC_DftSessionOpts) override;

    /**
     * Asynchronous establishes a connection to a specified address.
     * Note:
     *      If service not start when call this method, connection operation will
     *      create a pending-operation and recorded in service, your maybe could not get error.
     * @param[in] ip           - the ip address.
     * @param[in] port         - the port number.
     * @param[in] timeout      - the timeout value on connect operation, default use OS setting.
     * @param[in] protoFactory - the protocol factory, default use service protocol factory.
     *                           if use custom protocol factory, when AsyncConn failed, the factory will delete by framework.
     * @param[in] sessionOpts  - the session options.
     * @return int - the new session Id(not yet connected), if return 0 means failed, see LLBC_GetLastError().
     */
    int AsyncConn(const char *ip,
                  uint16 port,
                  double timeout = -1.0,
                  LLBC_IProtocolFactory *protoFactory = nullptr,
                  const LLBC_SessionOpts &sessionOpts = LLBC_DftSessionOpts) override;

    /**
     * Check given sessionId is legal or not.
     * @param[in] sessionId - the given session Id.
     * @return bool - return true is given session Id validate, otherwise return false.
     */
    bool IsSessionValidate(int sessionId) override;

    /**
     * Send packet.
     * Note:
     *      no matter this method success or not, packet will be managed by this call,
     *      it means no matter this call success or not, delete packet operation will
     *      execute by llbc framework.
     * @param[in] packet - the packet.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Send(LLBC_Packet *packet) override;

    /**
     * Multicast bytes.
     * @param[in] sessionIds - the session Ids.
     * @param[in] opcode     - the opcode.
     * @param[in] bytes      - the bytes data.
     * @param[in] len        - the bytes length.
     * @param[in] status     - the status, default is 0.
     * @param[in] flags      - the flags, default is 0.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Multicast(const LLBC_SessionIds &sessionIds,
                  int opcode,
                  const void *bytes,
                  size_t len,
                  int status = 0,
                  uint32 flags = 0) override;

   /**
     * Broadcast bytes.
     * @param[in] opcode     - the opcode.
     * @param[in] bytes      - bytes to multi cast.
     * @param[in] len   `    - will send bytes len, in bytes.
     * @param[in] status     - the status, default is 0.
     * @param[in] flags      - the flags, default is 0.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Broadcast(int opcode,
                  const void *bytes,
                  size_t len,
                  int status = 0,
                  uint32 flags = 0) override;

    /**
     * Remove session, always success.
     * @param[in] sessionId - the will close session Id.
	 * @param[in] reason    - the close reason string, use to describe session close reason.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveSession(int sessionId, const char *reason = nullptr) override;

    /**
     * Control session protocol stack.
     * @param[in] sessionId - the sessionId.
     * @param[in] ctrlCmd   - the stack control command(user defined).
     * @param[in] ctrlData  - the stack control data(user defined).
     * @return int - return 0 if success, otherwise return -1.
     */
    int CtrlProtocolStack(int sessionId,
                          int ctrlCmd,
                          const LLBC_Variant &ctrlData) override;

public:
    /**
     * Register component.
     */
    int AddComponent(LLBC_Component *comp) override;

    int AddComponent(const LLBC_String &compSharedLibPath,
                     const LLBC_String &compName,
                     LLBC_Component *&comp) override;

    /**
     * Get component.
     */
    LLBC_Component *GetComponent(const LLBC_CString &compName) override;


public:
    /**
     * Add coder factory.
     */
    int AddCoderFactory(int opcode, LLBC_CoderFactory *coderFactory) override;

public:
    /**
     * Subscribe message to specified delegate.
     */
    int Subscribe(int opcode, const LLBC_Delegate<void(LLBC_Packet &)> &deleg) override;

    /**
     * Previous subscribe message to specified delegate, if method return nullptr, will stop packet process flow.
     */
    int PreSubscribe(int opcode, const LLBC_Delegate<bool(LLBC_Packet &)> &deleg) override;

    #if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    /**
     * Unify previous subscribe message to specified delegate, if method return false, will stop packet process flow.
     */
    int UnifyPreSubscribe(const LLBC_Delegate<bool(LLBC_Packet &)> &deleg) override;
    #endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

    #if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
    /**
     * Subscribe message status to specified delegate, if subscribed, service will not call default opcode handler.
     */
    int SubscribeStatus(int opcode, int status, const LLBC_Delegate<void(LLBC_Packet &)> &deleg) override;
    #endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER

public:
    /**
     * Enable/Disable timer scheduler, only use external-drive type service.
     * @return int - return 0 if success, otherwise return -1.
     */
    int EnableTimerScheduler() override;

    int DisableTimerScheduler() override;

public:
    /**
     * Subscribe event to specified delegate.
     */
    LLBC_ListenerStub SubscribeEvent(int event, const LLBC_Delegate<void(LLBC_Event &)> &deleg) override;

    /**
     * Subscribe event to specified event listener.
     */
    LLBC_ListenerStub SubscribeEvent(int event, LLBC_EventListener *listener) override;

    /**
     * Unsubscribe event.
     * @param[in] event - the event Id, all the event's handler will delegate.
     */
    void UnsubscribeEvent(int event) override;

    /**
     * Unsubscribe event.
     * @param[in] stub - the event stub.
     */
    void UnsubscribeEvent(const LLBC_ListenerStub &stub) override;

    /**
     * Fire event(asynchronous operation).
     * @param[in] ev             - the will fire event pointer.
     * @param[in] enqueueHandler - the event enqueue handler.
     * @param[in] dequeueHandler - the event dequeue handler.
     */
    void FireEvent(LLBC_Event *ev,
                   const LLBC_Delegate<void(LLBC_Event *)> &enqueueHandler = nullptr,
                   const LLBC_Delegate<void(LLBC_Event *)> &dequeueHandler = nullptr) override;

    /**
     * Begin fire event(asynchronous operation).
     * @param[in] eventId - the event id.
     * @return LLBC_Event & - the event firer object.
     */
    /**
     * Begin fire event(asynchronous operation).
     * @param[in] eventId - the event id.
     * @return LLBC_ServiceEventFirer - the event firer object.
     */
    LLBC_ServiceEventFirer BeginFireEvent(int eventId) override;

    /**
     * Get event manager.
     * @return LLBC_EventMgr & - the event manager.
     */
    LLBC_EventMgr &GetEventManager() override;

public:
    /**
     * Add component event into service. Operated in the next service drive loop.
     * @param[in] eventType   - the event type, see LLBC_ComponentEventType enum.
     * @param[in] eventParams - the event params.
     * @return int - return 0 if success, otherwise return -1.
     */
    int AddComponentEvent(int compEvent, const LLBC_Variant &eventParams) override;

public:
    /**
     * Post lazy task to service.
     * @param[in] runnable - the runnable obj.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Post(const LLBC_Delegate<void(LLBC_Service *)> &runnable) override;

    /**
     * Get service protocol stack, only full-stack option disabled available.
     * Warning: This is a danger method, only use in user-defined protocol.
     * @param[in] sessionId - the session Id.
     * @return const LLBC_ProtocolStack * - the protocol stack.
     */
    const LLBC_ProtocolStack *GetCodecProtocolStack(int sessionId) const override;

public:
    /**
     * Get service thread-safe object pool.
     * @return LLBC_ObjPool & - the service thread-safe object pool.
     */
    LLBC_ObjPool &GetThreadSafeObjPool() override;

    /**
     * Get service thread-unsafe object pool.
     * @return LLBC_ObjPool * - the service thread-unsafe object pool.
     */
    LLBC_ObjPool &GetThreadUnsafeObjPool() override;

public:
    /**
     * One time service call routine, if service drive mode is ExternalDrive, you must manual call this method.
     * @param[in] fullFrame - the full frame flag.
     */
    void OnSvc(bool fullFrame = true) override;

protected:
    /**
     * Lock service.
     */
    void LockService() override;

    /**
     * Unlock service.
     */
    void UnlockService() override;

protected:
    /**
     * Stack create helper method(call by service and session class).
     */
    LLBC_ProtocolStack *CreatePackStack(int sessionId,
                                        int acceptSessionId = 0,
                                        LLBC_ProtocolStack *stack = nullptr) override;

    LLBC_ProtocolStack *CreateCodecStack(int sessionId,
                                         int acceptSessionId = 0,
                                         LLBC_ProtocolStack *stack = nullptr) override;

    LLBC_ProtocolStack *CreateFullStack(int sessionId, int acceptSessionId = 0) override;

protected:
    /**
     * Get component list.
     */
    const std::vector<LLBC_Component *> &GetComponentList() const override;

protected:
    /**
     * Declare friend class: LLBC_pollerMgr.
     *  Access method list:
     *      AddSessionProtocolFactory()
     */
    friend class LLBC_PollerMgr;

    /**
     * Session protocol factory operation methods.
     */
    void AddSessionProtocolFactory(int sessionId, LLBC_IProtocolFactory *protoFactory) override;

    LLBC_IProtocolFactory *FindSessionProtocolFactory(int sessionId) override;

    void RemoveSessionProtocolFactory(int sessionId) override;

protected:
    /**
     * Ready session operation methods.
     */
    void AddReadySession(int sessionId,
                         int acceptSessionId,
                         bool isListenSession,
                         bool repeatCheck = false) override;
    void RemoveReadySession(int sessionId);
    void RemoveAllReadySessions();

protected:
    /**
     * Task entry method.
     */
    void Svc() override;

    /**
     * Task cleanup method.
     */
    void Cleanup() override;

private:
    /**
     * Service config operation methods.
     * @param[in] appCfgType    - the application config type, see LLBC_AppConfigType.
     * @param[in] appCfg        - the application config.
     */
    void UpdateServiceCfg(int appCfgType, const LLBC_Variant &appCfg);

    /**
     * Service TLS operation methods.
     */
    void AddServiceToTls();
    void RemoveServiceFromTls();
    bool IsCanContinueDriveService();

    /**
     * Service phase support methods.
     */
    int PreStart();
    void PostStop();

    /**
     * Post operation methods.
     */
    void HandlePosts();

    /**
     * Queued event operation methods.
     */
    void HandleQueuedEvents();
    void HandleEv_SessionCreate(LLBC_ServiceEvent &ev);
    void HandleEv_SessionDestroy(LLBC_ServiceEvent &ev);
    void HandleEv_AsyncConnResult(LLBC_ServiceEvent &ev);
    void HandleEv_DataArrival(LLBC_ServiceEvent &ev);
    void HandleEv_ProtoReport(LLBC_ServiceEvent &ev);
    void HandleEv_SubscribeEv(LLBC_ServiceEvent &ev);
    void HandleEv_UnsubscribeEv(LLBC_ServiceEvent &ev);
    void HandleEv_FireEv(LLBC_ServiceEvent &ev);
    void HandleEv_AppPhaseEv(LLBC_ServiceEvent &ev);
    void HandleEv_AppReloaded(LLBC_ServiceEvent &ev);
    void HandleEv_ComponentEvent(LLBC_ServiceEvent &ev);

    /**
     * Component operation methods.
     */
    int InitComps();
    void DestroyComps(bool onlyCallEvMeth = true);
    int StartComps();
    void StopComps();
    void UpdateComps();
    void LateUpdateComps();
    void AddComp(LLBC_Component *comp);
    LLBC_Library *OpenCompLibrary(const LLBC_String &libPath, bool &existingLib);
    void CloseCompLibrary(const LLBC_String &libPath);

    /**
     * Auto-Release pool operation methods.
     */
    void InitAutoReleasePool();
    void UpdateAutoReleasePool();
    void ClearAutoReleasePool();

    /**
     * Timer-Scheduler operation methods.
     */
    void InitTimerScheduler();
    void UpdateTimerScheduler();
    void ClearTimerScheduler();

    /**
     * Idle process method.
     */
    void ProcessIdle();

private:
    /**
     * Internal helper methods.
     */
    int LockableSend(LLBC_Packet *packet,
                     bool lock = true,
                     bool checkRunningPhase = true,
                     bool checkSessionValidity = true);
    int LockableSend(int sessionId,
                     int opcode,
                     const void *bytes,
                     size_t len,
                     int status,
                     uint32 flags,
                     bool lock = true,
                     bool checkRunningPhase = true,
                     bool checkSessionValidity = true);

private:
    static int _maxId; // Max service Id.

    int _id; // Service Id.
    LLBC_String _name; // Service Name.
    LLBC_ServiceDriveMode::ENUM _driveMode; // Drive mode(ExternalDrive or InternalDrive).
    LLBC_ThreadId _svcThreadId; // Service thread Id(which thread drive this service).
    volatile bool _inCleaning; // Cleaning flag.
    volatile bool _sinkIntoOnSvcLoop; // Sink into OnSvc() loop flag.
    LLBC_ServiceMgr &_svcMgr; // Owned service manager.
    mutable LLBC_RecursiveLock _lock; // Service lock.
    volatile bool _serviceBeginLoop; // Service begin loop flag.
    volatile LLBC_ServiceRunningPhase::ENUM _runningPhase; // Service running phase.
    bool _destroyCompWhenStop; // Destroy components when stop flag.
    typedef void (LLBC_ServiceImpl::*_EvHandler)(LLBC_ServiceEvent &);
    static _EvHandler _evHandlers[LLBC_ServiceEventType::End]; // Service event handlers slot.

    // Service startup assistant members.
    volatile int _startErrNo;
    volatile int _startSubErrNo;

    // Config about members.
    int _cfgType; // Config type, see LLBC_AppConfigType enum.
    LLBC_Variant _cfg; // Prop type config content.

    // Protocol/poller/session about members.
    bool _fullStack; // ProtocolStack running mode(full/half).
    int _pollerCount; // Poller count.
    LLBC_PollerMgr _pollerMgr; // Poller manager.
    LLBC_SpinLock _protoLock; // Protocol logic about lock.
    bool _suppressedCoderNotFoundWarning; // Suppress coder not found warning flag.
    LLBC_IProtocolFactory *_dftProtocolFactory; // Default protocol factory.
    std::map<int, LLBC_IProtocolFactory *> _sessionProtoFactory; // Specific protocol factory.
    class _ReadySessionInfo // Ready session information.
    {
    public:
        int sessionId;
        int acceptSessionId;
        bool isListenSession;
        LLBC_ProtocolStack *codecStack;

    public:
        _ReadySessionInfo(int sessionId,
                          int acceptSessionId,
                          bool isListenSession,
                          LLBC_ProtocolStack *codecStack = nullptr);
        ~_ReadySessionInfo();
    };
    std::map<int, _ReadySessionInfo *> _readySessionInfos; // Ready sessions set.
    mutable LLBC_SpinLock _readySessionInfosLock; // Ready session set lock.

    // FPS about members.
    volatile int _fps; // Service FPS.
    sint64 _begSvcTime; // Begin heartbeat time, update on every heartbeat begin.

private:
    // Components about members.
    using _CompRunningPhase = LLBC_NS LLBC_Component::_CompRunningPhase; // Component running phase.
    std::list<LLBC_Component *> _willRegComps; // Will register component list.
    std::vector<LLBC_Component *> _compList; // Component list.
    std::map<LLBC_CString, LLBC_Component *> _name2Comps; // Name->Component map.
    std::map<LLBC_String, LLBC_Library *> _compLibraries; // Component libraries(if is dynamic load component).

    // Coder & Handler about members.
    std::map<int, LLBC_CoderFactory *> _coderFactories; // Coder Factories.
    std::map<int, LLBC_Delegate<void(LLBC_Packet &)> > _handlers; // Packet handlers.
    std::map<int, LLBC_Delegate<bool(LLBC_Packet &)> > _preHandlers; // Packet pre-handlers.
    #if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    LLBC_Delegate<bool(LLBC_Packet &)> _unifyPreHandler; // Unify packet pre-handler.
    #endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    #if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
    std::map<int, std::map<int, LLBC_Delegate<void(LLBC_Packet &)> > > _statusHandlers; // Status handlers.
    #endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER

private:
    // Service extend functions about members.
    // - Post support members.
    std::vector<LLBC_Delegate<void(LLBC_Service *)> > _posts; // Post list.

    // - Obj-Base support members.
    LLBC_AutoReleasePoolStack *_releasePoolStack; // Auto-Release pool stack.

    // - ObjPool support members.
    LLBC_ObjPool _threadSafeObjPool; // Thread-safe object pool.
    LLBC_ObjPool _threadUnsafeObjPool; // Thread-unsafe object pool.

    // - Timer scheduler.
    LLBC_TimerScheduler *_timerScheduler; // Timer scheduler.

    // - Event support members.
    LLBC_EventMgr _evManager; // EventManager.
    static LLBC_ListenerStub _evManagerMaxListenerStub; // Max event listener stub.
    std::queue<std::pair<int, const LLBC_Variant &> > _compEvents; // Component events.
};

__LLBC_NS_END

#include "llbc/comm/ServiceImplInl.h"
