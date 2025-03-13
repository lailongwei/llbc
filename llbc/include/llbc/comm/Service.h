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

#include "llbc/comm/SessionOpts.h"
#include "llbc/comm/Coder.h"
#include "llbc/comm/Component.h"

__LLBC_NS_BEGIN
 /**
  * Previous declare some classes.
  */
class LLBC_Packet;
class LLBC_Session;
class LLBC_PollerMgr;
class LLBC_ComponentFactory;
class LLBC_IProtocolFactory;
class LLBC_ProtocolStack;
class LLBC_ServiceEventFirer;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The service running phase enumeration.
 */
class LLBC_ServiceRunningPhase
{
public:
    enum ENUM
    {
        Begin = 0,

        // Service has not been started.
        NotStarted = Begin,

        // Service pre-starting(Perform some preparatory work before initializing components).
        PreStarting,
        // Specific service pre-start failed.
        PreStartFailed,
        // Service has been pre-started.
        PreStarted,

        // Service is currently initializing components.
        InitingComps,
        // Specific service initialize components failed.
        InitCompsFailed,
        // Service has been initialized all components.
        CompsInited,

        // Service is currently starting components.
        StartingComps,
        // Specific service start components failed.
        StartCompsFailed,
        // Service has been started all components.
        CompsStarted,

        // Service has been started.
        Started,

        // Service is currently stopping components.
        StoppingComps,

        // Service is currently stopping.
        Stopping,

        End,
    };

    /**
     * Check given running is failed phase or not.
     * Failed phases: XXXFailed.
     * @param[in] runningPhase - the running phase.
     * @return bool - failed phase flag.
     */
    static constexpr bool IsFailedPhase(int runningPhase);

    /**
     * Check given running phase is stopping phase or not.
     * @param[in] runningPhase - the running phase.
     * @return bool - stopping phase flag.
     */
    static constexpr bool IsStoppingPhase(int runningPhase);

    /**
     * Check given running phase is failed/stopping phase or not.
     * @param[in] runningPhase - the running phase.
     * @return bool - failed/stopping phase flag.
     */
    static constexpr bool IsFailedOrStoppingPhase(int runningPhase);
};

/**
 * \brief The service drive mode enumeration.
 */
class LLBC_ServiceDriveMode
{
public:
    enum ENUM
    {
        Begin = 0,
        // Self drive mode: Using service internal thread to drive OnSvc() method call.
        SelfDrive = Begin,
        // External drive mode: Using external internal thread to drive OnSvc() method call.
        ExternalDrive,

        End
    };

    /**
     * Check given drive mode is validate or not.
     * @param[in] driveMode - the service drive mode.
     * @return bool - return true if validate, otherwise return false.
     */
    static constexpr bool IsValid(int driveMode);
};

/**
 * \brief The service interface class define.
 */
class LLBC_EXPORT LLBC_Service : protected LLBC_Task
{
public:
    // Import Base::Push/Base::Wait method to service.
    using LLBC_Task::Push;
    using LLBC_Task::Wait;

public:
    ~LLBC_Service() override = default;

public:
    /**
     * Factory method, create service type service type.
     * @param[in] name               - the service name.
     * @param[in] dftProtocolFactory - the service default protocol factory, if null will use library normal protocol factory.
     * @param[in] fullStack          - the full stack option, default is true.
     * @return LLBC_Service * - new service.
     */
    static LLBC_Service *Create(const LLBC_String &name = "",
                                LLBC_IProtocolFactory *dftProtocolFactory = nullptr,
                                bool fullStack = true);

public:
    /**
     * Get the service Id.
     * @return int - the service Id.
     */
    virtual int GetId() const = 0;

    /**
     * Get the service name.
     * @return const LLBC_String & - the service name.
     */
    virtual const LLBC_String &GetName() const = 0;

    /**
     * Get config type.
     * @return int - the config type, see LLBC_AppConfigType enum.
     */
    virtual int GetConfigType() const = 0;

    /**
     * Get service config.
     * @return const LLBC_Variant & - the service config.
     */
    virtual const LLBC_Variant &GetConfig() const = 0;

    /**
     * Get full stack option.
     * @return bool - the full stack option.
     */
    virtual bool IsFullStack() const = 0;

    /**
     * Get the service drive mode.
     * @return LLBC_ServiceDriveMode::ENUM - the service drive mode.
     */
    virtual LLBC_ServiceDriveMode::ENUM GetDriveMode() const = 0;

    /**
     * Set the service drive mode.
     * @param[in] driveMode - the service drive mode.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int SetDriveMode(LLBC_ServiceDriveMode::ENUM driveMode) = 0;

public:
    /**
     * Suppress coder not found warning in protocol-stack.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int SuppressCoderNotFoundWarning() = 0;

public:
    /**
     * Startup service, default will startup one poller to work.
     * @param[in] pollerCount - the poller count.
     * @return int - return 0 if startup successful, otherwise return -1.
     */
    virtual int Start(int pollerCount = 1) = 0;

    /**
     * Check service is started or not.
     * @return bool - return true if running, otherwise return false.
     */
    virtual bool IsStarted() const = 0;

    /**
     * Stop the service.
     * @param[in] destroyComp - destroy service components or not, default is false.
     * @return int - return 0 if success, otherwise return failed.
     */
    virtual int Stop(bool destroyComp = false) = 0;

public:
    /**
     * Get service FPS.
     * @return int - the service FPS.
     */
    virtual int GetFPS() const = 0;

    /**
     * Set service FPS.
     * @return int - return 0 if success, ohterwise return -1.
     */
    virtual int SetFPS(int fps) = 0;

    /**
     * Get service frame interval, in milli-seconds.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int GetFrameInterval() const = 0;

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
    virtual int Listen(const char *ip,
                       uint16 port,
                       LLBC_IProtocolFactory *protoFactory = nullptr,
                       const LLBC_SessionOpts &sessionOpts = LLBC_DftSessionOpts) = 0;

    /**
     * Establisthes a connection to a specified address.
     * @param[in] ip           - the ip address.
     * @param[in] port         - the port number.
     * @param[in] timeout      - the timeout value on connect operation, default use OS setting.
     * @param[in] protoFactory - the protocol factory, default use service protocol factory.
     *                           if use custom protocol factory, when Connect failed, the factory will delete by framework.
     * @param[in] sessionOpts  - the session options.
     * @return int - the new session Id, if return 0, means failed, see LLBC_GetLastError().
     */
    virtual int Connect(const char *ip,
                        uint16 port,
                        double timeout = -1.0,
                        LLBC_IProtocolFactory *protoFactory = nullptr,
                        const LLBC_SessionOpts &sessionOpts = LLBC_DftSessionOpts) = 0;

    /**
     * Asynchronous establishes a connection to a specified address.
     * @param[in] ip           - the ip address.
     * @param[in] port         - the port number.
     * @param[in] timeout      - the timeout value on connect operation, default use OS setting.
     * @param[in] protoFactory - the protocol factory, default use service protocol factory.
     *                           if use custom protocol factory, when AsyncConn failed, the factory will delete by framework.
     * @param[in] sessionOpts  - the session options.
     * @return int - the new session Id(not yet connected), if return 0 means failed, see LLBC_GetLastError().
     */
    virtual int AsyncConn(const char *ip,
                          uint16 port,
                          double timeout = -1.0,
                          LLBC_IProtocolFactory *protoFactory = nullptr,
                          const LLBC_SessionOpts &sessionOpts = LLBC_DftSessionOpts) = 0;

    /**
     * Check given sessionId is validate or not.
     * @param[in] sessionId - the given session Id.
     * @return bool - return true is given session Id validate, otherwise return false.
     */
    virtual bool IsSessionValidate(int sessionId) = 0;

public:
    /**
     * Send packet.
     * Note: 
     *      no matter this method success or not, packet will be managed by this call,
     *      it means no matter this call success or not, delete packet operation will
     *      execute by llbc framework.
     * @param[in] packet - the packet.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Send(LLBC_Packet *packet) = 0;

    /**
     * Send data(these methods will automatics create packet to send).
     * Note: 
     *      no matter this method success or not, coder will be managed by this call,
     *      it means no matter this call success or not, delete coder operation will
     *      execute by llbc framework.
     * @param[in] sessionId - the session Id.
     * @param[in] opcode    - the opcode.
     * @param[in] coder     - the coder.
     * @param[in] status    - the status, default is 0.
     * @param[in] flags     - the flags, default is 0.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Send(int sessionId,
             int opcode,
             LLBC_Coder *coder,
             int status = 0,
             uint32 flags = 0);

    /**
     * Send bytes(these methods will automatics create packet to send).
     * @param[in] sessionId - the session Id.
     * @param[in] opcode    - the opcode.
     * @param[in] bytes     - the bytes data.
     * @param[in] len       - data length.
     * @param[in] status    - the status, default is 0.
     * @param[in] flags     - the flags, default is 0.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Send(int sessionId,
             int opcode,
             const void *bytes,
             size_t len,
             int status = 0,
             uint32 flags = 0);

    /** 
     * Multicast data(these methods will automatics create packet to send).
     * Note: 
     *      no matter this method success or not, coder will be managed by this call,
     *      it means no matter this call success or not, delete coder operation will
     *      execute by llbc framework.
     * @param[in] sessionIds - the session Ids.
     * @param[in] opcode     - the opcode.
     * @param[in] coder      - the coder.
     * @param[in] status     - the status, default is 0.
     * @param[in] flags      - the flags, default is 0.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Multicast(const LLBC_SessionIds &sessionIds,
                  int opcode,
                  LLBC_Coder *coder,
                  int status = 0,
                  uint32 flags = 0);

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
    virtual int Multicast(const LLBC_SessionIds &sessionIds,
                          int opcode,
                          const void *bytes,
                          size_t len,
                          int status = 0,
                          uint32 flags = 0) = 0;

    /** 
     * Broadcast data(these methods will automatics create packet to send).
     * Note: 
     *      no matter this method success or not, coder will be managed by this call,
     *      it means no matter this call success or not, delete coder operation will
     *      execute by llbc framework.
     * @param[in] opcode - the opcode.
     * @param[in] coder  - the coder.
     * @param[in] status - the status, default is 0.
     * @param[in] flags  - the flags, default is 0.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Broadcast(int opcode,
                  LLBC_Coder *coder,
                  int status = 0,
                  uint32 flags = 0);

    /**
     * Broadcast bytes.
     * @param[in] opcode - the opcode.
     * @param[in] bytes  - bytes to multi cast.
     * @param[in] len    - will send bytes len, in bytes.
     * @param[in] status - the status, default is 0.
     * @param[in] flags  - the flags, default is 0.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Broadcast(int opcode,
                          const void *bytes,
                          size_t len,
                          int status = 0,
                          uint32 flags = 0) = 0;

    /**
     * Remove session, always success.
     * @param[in] sessionId - the will close session Id.
     * @param[in] reason    - the close reason string, use to describe session close reason.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int RemoveSession(int sessionId, const char *reason = nullptr) = 0;

    /**
     * Control session protocol stack.
     * @param[in] sessionId - the sessionId.
     * @param[in] ctrlCmd   - the stack control command(user defined).
     * @param[in] ctrlData  - the stack control data(user defined).
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int CtrlProtocolStack(int sessionId,
                                  int ctrlCmd,
                                  const LLBC_Variant &ctrlData) = 0;

public:
    /**
     * Add component by component class or pointer.
     */
    template <typename Comp>
    typename std::enable_if<std::is_base_of<LLBC_Component, Comp>::value, int>::type
    AddComponent();
    virtual int AddComponent(LLBC_Component *comp) = 0;

    /**
     * Add component by component factory class or pointer.
     */
    template <typename CompFactory>
    typename std::enable_if<std::is_base_of<LLBC_ComponentFactory, CompFactory>::value, int>::type
    AddComponent();
    int AddComponent(LLBC_ComponentFactory *compFactory);

    /**
     * Add component by shared library.
     */
    int AddComponent(const LLBC_String &compSharedLibPath,
                     const LLBC_String &compName);
    virtual int AddComponent(const LLBC_String &compSharedLibPath,
                             const LLBC_String &compName,
                             LLBC_Component *&comp) = 0;

    /**
     * Get component.
     */
    template <typename Comp>
    typename std::enable_if<std::is_base_of<LLBC_Component, Comp>::value &&
                            !std::is_same<LLBC_Component, Comp>::value,
                            Comp *>::type
    GetComponent();
    virtual LLBC_Component *GetComponent(const LLBC_CString &compName) = 0;

    template <typename Comp>
    typename std::enable_if<std::is_base_of<LLBC_Component, Comp>::value &&
                            !std::is_same<LLBC_Component, Comp>::value,
                            const Comp *>::type
    GetComponent() const;
    const LLBC_Component *GetComponent(const LLBC_CString &compName) const;

public:
    /**
     * Add coder factory.
     */
    template <typename CoderFactory>
    typename std::enable_if<std::is_base_of<LLBC_CoderFactory, CoderFactory>::value, int>::type
    AddCoderFactory(int opcode);
    virtual int AddCoderFactory(int opcode, LLBC_CoderFactory *coderFactory) = 0;

public:
    /**
     * Subscribe message to specified handler method.
     */
    template <typename ObjType>
    int Subscribe(int opcode, ObjType *obj, void (ObjType::*method)(LLBC_Packet &));

    /**
     * Subscribe message to specified delegate.
     */
    virtual int Subscribe(int opcode, const LLBC_Delegate<void(LLBC_Packet &)> &deleg) = 0;

    /**
     * Previous subscribe message to specified handler method, if method return false, will stop packet process flow.
     */
    int PreSubscribe(int opcode, bool (*func)(LLBC_Packet &));
    template <typename ObjType>
    int PreSubscribe(int opcode, ObjType *obj, bool (ObjType::*method)(LLBC_Packet &));

    /**
     * Unify previous subscribe message to specified delegate, if method return false, will stop packet process flow.
     */
    virtual int PreSubscribe(int opcode, const LLBC_Delegate<bool(LLBC_Packet &)> &deleg) = 0;

    #if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    /**
     * Unify previous subscribe message to specified handler method, if method return false, will stop packet process flow.
     */
    int UnifyPreSubscribe(bool(*func)(LLBC_Packet &));
    template <typename ObjType>
    int UnifyPreSubscribe(ObjType *obj, bool (ObjType::*method)(LLBC_Packet &));

    /**
     * Unify previous subscribe message to specified delegate, if method return false, will stop packet process flow.
     */
    virtual int UnifyPreSubscribe(const LLBC_Delegate<bool(LLBC_Packet &)> &deleg) = 0;
    #endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

    #if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
    /**
     * Subscribe message status to specified method, if subscribed, service will not call default opcode handler.
     */
    int SubscribeStatus(int opcode, int status, void(*func)(LLBC_Packet &));
    template <typename ObjType>
    int SubscribeStatus(int opcode, int status, ObjType *obj, void (ObjType::*method)(LLBC_Packet &));

    /**
     * Subscribe message status to specified delegate, if subscribed, service will not call default opcode handler.
     */
    virtual int SubscribeStatus(int opcode, int status, const LLBC_Delegate<void(LLBC_Packet &)> &deleg) = 0;
    #endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER

public:
    /**
     * Enable/Disable timer scheduler.
     */
    virtual int EnableTimerScheduler() = 0;
    virtual int DisableTimerScheduler() = 0;

public:
    /**
     * Subscribe event to specified handler method.
     */
    template <typename ObjType>
    LLBC_ListenerStub SubscribeEvent(int event, ObjType *obj, void (ObjType::*method)(LLBC_Event &));

    /**
     * Subscribe event to specified delegate.
     */
    virtual LLBC_ListenerStub SubscribeEvent(int event, const LLBC_Delegate<void(LLBC_Event &)> &deleg) = 0;

    /**
     * Subscribe event to specified event listener.
     */
    virtual LLBC_ListenerStub SubscribeEvent(int event, LLBC_EventListener *listener) = 0;

    /**
     * Unsubscribe event.
     * @param[in] event - the event Id, all the event's handler will delete.
     */
    virtual void UnsubscribeEvent(int event) = 0;

    /**
     * Unsubscribe event.
     * @param[in] stub - the event stub.
     */
    virtual void UnsubscribeEvent(const LLBC_ListenerStub &stub) = 0;

    /**
     * Fire event(asynchronous operation).
     * @param[in] ev             - the fill fire event pointer.
     * @param[in] enqueueHandler - the event enqueue handler.
     * @param[in] dequeueHandler - the event dequeue handler.
     */
    virtual void FireEvent(LLBC_Event *ev,
                           const LLBC_Delegate<void(LLBC_Event *)> &enqueueHandler = nullptr,
                           const LLBC_Delegate<void(LLBC_Event *)> &dequeueHandler = nullptr) = 0;
 
    /**
     * Begin fire event(asynchronous operation).
     * @param[in] eventId - the event id.
     * @return LLBC_ServiceEventFirer - the event firer object. 
     */
    virtual LLBC_ServiceEventFirer BeginFireEvent(int eventId) = 0;

    /**
     * Get event manager.
     * @return LLBC_EventMgr & - the event manager.
     */
    virtual LLBC_EventMgr &GetEventManager() = 0;

public:
    /**
     * Add component event into service. Operated in the next service drive loop.
     * @param[in] eventType   - the event type, see LLBC_ComponentEventType enum.
     * @param[in] eventParams - the event params.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int AddComponentEvent(int eventType, const LLBC_Variant &eventParams) = 0;

public:
    /**
     * Post runnable to service.
     * @param[in] obj    - the runnable object.
     * @param[in] method - the runnable method.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename ObjType>
    int Post(ObjType *obj, void (ObjType:: *method)(LLBC_Service *));

    /**
     * Post runnable to service.
     * @param[in] runnable - the runnable obj.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Post(const LLBC_Delegate<void(LLBC_Service *)> &runnable) = 0;

    /**
     * Get service codec protocol stack, only full-stack option disabled available.
     * Warning: This is a danger method, only use in user-defined protocol.
     * @param[in] sessionId - the session Id.
     * @return const LLBC_ProtocolStack * - the protocol stack.
     */
    virtual const LLBC_ProtocolStack *GetCodecProtocolStack(int sessionId) const = 0;

public:
    /**
     * Get service thread-safe object pool.
     * @return LLBC_ObjPool & - the service thread-safe object pool.
     */
    virtual LLBC_ObjPool &GetThreadSafeObjPool() = 0;

    /**
     * Get service thread-unsafe object pool.
     * @return LLBC_ObjPool & - the service thread-unsafe object pool.
     */
    virtual LLBC_ObjPool &GetThreadUnsafeObjPool() = 0;

public:
    /**
     * One time service call routine, if service drive mode is ExternalDrive, you must manual call this method.
     * @param[in] fullFrame - the full frame flag.
     */
    virtual void OnSvc(bool fullFrame = true) = 0;

protected:
    /**
     * Lock service.
     */
    virtual void LockService() = 0;

    /**
     * Unlock service.
     */
    virtual void UnlockService() = 0;

protected:
    /**
     * Declare friend class: LLBC_Session.
     *  Access method list:
     *      CreateFullStack()
     *      CreatePackStack()
     */
    friend class LLBC_Session;

    /**
     * Stack create helper methods(call by service and session class).
     */
    virtual LLBC_ProtocolStack *CreatePackStack(int sessionId,
                                                int acceptSessionId = 0,
                                                LLBC_ProtocolStack *stack = nullptr) = 0;
    virtual LLBC_ProtocolStack *CreateCodecStack(int sessionId,
                                                 int acceptSessionId = 0,
                                                 LLBC_ProtocolStack *stack = nullptr) = 0;
    virtual LLBC_ProtocolStack *CreateFullStack(int sessionId, int acceptSessionId = 0) = 0;


protected:
    /**
     * Declare friend class: LLBC_BasePoller.
     *  Access method list:
     *      AddReadySession()
     */
    friend class LLBC_BasePoller;

    /**
     * Ready session operation methods.
     */
    virtual void AddReadySession(int sessionId,
                                 int acceptSessionId,
                                 bool isListenSession,
                                 bool repeatCheck = false) = 0;

protected:
    /**
     * Declare friend class: LLBC_Component.
     * Access method list:
     *     GetComponentList()
     */
    friend class LLBC_Component;

    /**
     * Get component list.
     */
    virtual const std::vector<LLBC_Component *> &GetComponentList() const = 0;

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
    virtual void AddSessionProtocolFactory(int sessionId, LLBC_IProtocolFactory *protoFactory) = 0;
    virtual LLBC_IProtocolFactory *FindSessionProtocolFactory(int sessionId) = 0;
    virtual void RemoveSessionProtocolFactory(int sessionId) = 0;
};

__LLBC_NS_END

#include "llbc/comm/ServiceInl.h"
