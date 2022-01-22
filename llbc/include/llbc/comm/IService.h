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

#ifndef __LLBC_COMM_ISERVICE_H__
#define __LLBC_COMM_ISERVICE_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"

#include "llbc/comm/SessionOpts.h"

__LLBC_NS_BEGIN

/**
 * Previous declare some classes.
 */
class LLBC_ICoder;
class LLBC_Packet;
class LLBC_IComponent;
class LLBC_Session;
class LLBC_PollerMgr;
class LLBC_ICoderFactory;
class LLBC_IComponentFactory;
class LLBC_IProtocolFactory;
class LLBC_ProtocolStack;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The service interface class define.
 */
class LLBC_EXPORT LLBC_IService : protected LLBC_BaseTask
{
    typedef LLBC_IService This;

public:
    /**
     * The service type enumeration.
     */
    enum Type
    {
        Raw,
        Normal,
        Custom,
    };

    /**
     * The service drive mode enumeration.
     */
    enum DriveMode
    {
        SelfDrive,
        ExternalDrive,
    };

public:
    // Import Base::Push/Base::Wait method to service.
    using LLBC_BaseTask::Push;
    using LLBC_BaseTask::Wait;

public:
    virtual ~LLBC_IService() {  }

public:
    /**
     * Factory method, create service type service type.
     * @param[in] type         - the service type.
     * @param[in] name         - the service name.
     * @param[in] protoFactory - the protocol factory, when type is Custom, will use this protocol factory to create protocols.
     * @param[in] fullStack    - the full stack option, default is true.
     * @return This * - new service.
     */
    static This *Create(Type type,
                        const LLBC_String &name = "",
                        LLBC_IProtocolFactory *protoFactory = nullptr,
                        bool fullStack = true);

public:
    /**
     * Get the service Id.
     * @return int - the service Id.
     */
    virtual int GetId() const = 0;

    /**
     * Get the service type.
     * @return Type - the service type.
     */
    virtual Type GetType() const = 0;

    /**
     * Get the service name.
     * @return const LLBC_String & - the service name.
     */
    virtual const LLBC_String &GetName() const = 0;

    /**
     * Get full stack option.
     * @return bool - the full stack option.
     */
    virtual bool IsFullStack() const = 0;

    /**
     * Get the service drive mode.
     * @return DriveMode - the service drive mode.
     */
    virtual DriveMode GetDriveMode() const = 0;

    /**
     * Set the service drive mode.
     * @param[in] mode - the service drive mode.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int SetDriveMode(DriveMode mode) = 0;

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
     */
    virtual void Stop() = 0;

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

    /**
     * Set service frame max time out.
     * @param[in] frameMaxTimeout - in nano-seconds.
     */
    virtual void SetFrameMaxTimeout(uint64 frameMaxTimeout) = 0;

    /**
     * Get service frame max time out.
     * @return LLBC_TimeSpan
     */
    virtual LLBC_TimeSpan GetFrameMaxTimeout() const = 0;

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
     * @param[in] svcId     - the service Id.
     * @param[in] sessionId - the session Id.
     * @param[in] opcode    - the opcode.
     * @param[in] coder     - the coder.
     * @param[in] status    - the status, default is 0.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Send(int sessionId);
    virtual int Send(int sessionId, int opcode);
    virtual int Send(int sessionId, LLBC_ICoder *coder);
    virtual int Send(int sessionId, int opcode, LLBC_ICoder *coder);
    virtual int Send(int sessionId, int opcode, LLBC_ICoder *coder, int status);
    virtual int Send(int svcId, int sessionId, int opcode, LLBC_ICoder *coder, int status);

    /**
     * Send bytes(these methods will automatics create packet to send).
     * @param[in] svcId     - the service Id.
     * @param[in] sessionId - the session Id.
     * @param[in] opcode    - the opcode.
     * @param[in] bytes     - the bytes data.
     * @param[in] len       - data length.
     * @param[in] status    - the status, default is 0.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Send(int sessionId, const void *bytes, size_t len);
    virtual int Send(int sessionId, int opcode, const void *bytes, size_t len);
    virtual int Send(int sessionId, int opcode, const void *bytes, size_t len, int status);
    virtual int Send(int svcId, int sessionId, int opcode, const void *bytes, size_t len, int status);

public:
    /** 
     * Multicast data(these methods will automatics create packet to send).
     * Note: 
     *      no matter this method success or not, coder will be managed by this call,
     *      it means no matter this call success or not, delete coder operation will
     *      execute by llbc framework.
     * @param[in] svcId      - the service Id.
     * @param[in] sessionIds - the session Ids.
     * @param[in] opcode    - the opcode.
     * @param[in] coder     - the coder.
     * @param[in] status    - the status, default is 0.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename SessionIds>
    int Multicast(const SessionIds &sessionIds);
    template <typename SessionIds>
    int Multicast(const SessionIds &sessionIds, int opcode);
    template <typename SessionIds>
    int Multicast(const SessionIds &sessionIds, LLBC_ICoder *coder);
    template <typename SessionIds>
    int Multicast(const SessionIds &sessionIds, int opcode, LLBC_ICoder *coder);
    template <typename SessionIds>
    int Multicast(const SessionIds &sessionIds, int opcode, LLBC_ICoder *coder, int status);
    virtual int Multicast(int svcId, const LLBC_SessionIdSet &sessionIds, int opcode, LLBC_ICoder *coder, int status) = 0;
    virtual int Multicast(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, LLBC_ICoder *coder, int status) = 0;

    /**
     * Multicast bytes(these methods will automatics create packet to send).
     * @param[in] svcId      - the service Id.
     * @param[in] sessionIds - the session Ids.
     * @param[in] opcode     - the opcode.
     * @param[in] bytes      - bytes to multi cast.
     * @param[in] len   `    - will send bytes len, in bytes.
     * @param[in] status     - the status, default is 0.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename SessionIds>
    int Multicast(const SessionIds &sessionIds, const void *bytes, size_t len);
    template <typename SessionIds>
    int Multicast(const SessionIds &sessionIds, int opcode, const void *bytes, size_t len);
    template <typename SessionIds>
    int Multicast(const SessionIds &sessionIds, int opcode, const void *bytes, size_t len, int status);
    virtual int Multicast(int svcId, const LLBC_SessionIdSet &sessionIds, int opcode, const void *bytes, size_t len, int status) = 0;
    virtual int Multicast(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, const void *bytes, size_t len, int status) = 0;

    /** 
     * Broadcast data(these methods will automatics create packet to send).
     * Note: 
     *      no matter this method success or not, coder will be managed by this call,
     *      it means no matter this call success or not, delete coder operation will
     *      execute by llbc framework.
     * @param[in] svcId      - the service Id.
     * @param[in] opcode    - the opcode.
     * @param[in] coder     - the coder.
     * @param[in] status    - the status, default is 0.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Broadcast();
    virtual int Broadcast(int opcode);
    virtual int Broadcast(int opcode, LLBC_ICoder *coder, int status);
    virtual int Broadcast(int svcId, int opcode, LLBC_ICoder *coder, int status) = 0;

    /**
     * Broadcast bytes(these methods will automatics create packet to send).
     * @param[in] svcId      - the service Id.
     * @param[in] opcode     - the opcode.
     * @param[in] bytes      - bytes to multi cast.
     * @param[in] len   `    - will send bytes len, in bytes.
     * @param[in] status     - the status, default is 0.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Broadcast(int opcode, const void *bytes, size_t len);
    virtual int Broadcast(int opcode, const void *bytes, size_t len, int status);
    virtual int Broadcast(int svcId, int opcode, const void *bytes, size_t len, int status) = 0;

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
     * Register component.
     */
    template <typename ComponentFactoryCls>
    int RegisterComponent();
    virtual int RegisterComponent(LLBC_IComponentFactory *compFactory) = 0;
    virtual int RegisterComponent(LLBC_IComponent *comp) = 0;
    virtual int RegisterComponent(const LLBC_String &libPath, const LLBC_String &compName);
    virtual int RegisterComponent(const LLBC_String &libPath, const LLBC_String &compName, LLBC_IComponent *&comp) = 0;

    /**
     * Get component/components.
     */
    template <typename ComponentCls>
    ComponentCls *GetComponent();
    template <typename ComponentCls>
    ComponentCls *GetComponent(const char *compName);
    template <typename ComponentCls>
    ComponentCls *GetComponent(const LLBC_String &compName);
    virtual LLBC_IComponent *GetComponent(const char *compName) = 0;
    virtual LLBC_IComponent *GetComponent(const LLBC_String &compName) = 0;
    template <typename ComponentCls>
    std::vector<LLBC_IComponent *> GetComponents();
    virtual const std::vector<LLBC_IComponent *> &GetComponents(const LLBC_String &compName) = 0;

public:
    /**
     * Register coder.
     */
    template <typename CoderFactoryCls>
    int RegisterCoder(int opcode);
    virtual int RegisterCoder(int opcode, LLBC_ICoderFactory *coder) = 0;

#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
public:
    /**
     * Register status code describe.
     */
    virtual int RegisterStatusDesc(int status, const LLBC_String &desc) = 0;
#endif // LLBC_CFG_COMM_ENABLE_STATUS_DESC

public:
    /**
     * Subscribe message to specified handler method.
     */
    int Subscribe(int opcode, void(*func)(LLBC_Packet &));
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
    LLBC_ListenerStub SubscribeEvent(int event, void(*func)(LLBC_Event &));
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
     * Get event manager.
     * @return LLBC_EventManager & - the event manager.
     */
    virtual LLBC_EventManager &GetEventManager() = 0;

public:
    /**
     * Post runnable to service.
     * @param[in] func - the runnable function.
     * @param[in] data - the runnable data, can be null.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Post(void(*func)(This *, const LLBC_Variant &data), const LLBC_Variant &data = LLBC_Variant::nil);
    /**
     * Post runnable to service.
     * @param[in] obj    - the runnable object.
     * @param[in] method - the runnable method.
     * @param[in] data   - the runnable data, can be null.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename ObjType>
    int Post(ObjType *obj, void (ObjType::*method)(This *, const LLBC_Variant &data), const LLBC_Variant &data = LLBC_Variant::nil);

    /**
     * Post runnable to service.
     * @param[in] runnable - the runnable obj.
     * @param[in] data     - the runnable data, can be null.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Post(const LLBC_Delegate<void(This *, const LLBC_Variant &)> &runnable, const LLBC_Variant &data = LLBC_Variant::nil) = 0;

    /**
     * Get service codec protocol stack, only full-stack option disabled available.
     * Warning: This is a danger method, only use in user-defined protocol.
     * @param[in] sessionId - the session Id.
     * @return const LLBC_ProtocolStack * - the protocol stack.
     */
    virtual const LLBC_ProtocolStack *GetCodecProtocolStack(int sessionId) const = 0;

public:
    /**
     * Get service safety object pool.
     * @return LLBC_SafetyObjectPool & - the thread safety object pool reference.
     */
    virtual LLBC_SafetyObjectPool &GetSafetyObjectPool() = 0;

    /**
     * Get service unsafety object pool.
     * @return LLBC_UnsafetyObjectPool & - the thread unsafety object pool reference.
     */
    virtual LLBC_UnsafetyObjectPool &GetUnsafetyObjectPool() = 0;

    /**
     * Get service packet object pool(thread safety).
     * @return LLBC_ObjectPoolInst<LLBC_Packet, LLBC_SpinLock> & - the packet object pool.
     */
    virtual LLBC_ObjectPoolInst<LLBC_Packet> &GetPacketObjectPool() = 0;

    /**
     * Get message block object pool(thread safety).
     * @return LLBC_ObjectPoolInst<LLBC_MessageBlock, LLBC_SpinLock> & - the message block object pool.
     */
    virtual LLBC_ObjectPoolInst<LLBC_MessageBlock> &GetMsgBlockObjectPool() = 0;

public:
    /**
     * One time service call routine, if service drive mode is ExternalDrive, you must manual call this method.
     * @param[in] fullFrame - the full frame flag.
     */
    virtual void OnSvc(bool fullFrame = true) = 0;

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
    virtual LLBC_ProtocolStack *CreatePackStack(int sessionId, int acceptSessionId = 0, LLBC_ProtocolStack *stack = nullptr) = 0;
    virtual LLBC_ProtocolStack *CreateCodecStack(int sessionId, int acceptSessionId = 0, LLBC_ProtocolStack *stack = nullptr) = 0;
    virtual LLBC_ProtocolStack *CreateFullStack(int sessionId, int acceptSessionId = 0) = 0;

protected:
    /**
     * Declare friend class: LLBC_IApplication.
     *  Access method list:
     *      NtyApplicationIniConfigReload()
     *      NtyApplicationPropertyConfigReload()
     */
    friend class LLBC_IApplication;

    /**
     * Application ini/property config reload event notifiy method.
     */
    virtual void NtyApplicationConfigReloaded(bool iniReloaded, bool propReloaded) = 0;

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

#include "llbc/comm/IServiceImpl.h"

#endif // !__LLBC_COMM_ISERVICE_H__

