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
#include "llbc/objbase/ObjBase.h"

__LLBC_NS_BEGIN

/**
 * Previous declare some classes.
 */
class LLBC_ICoder;
class LLBC_Packet;
class LLBC_IFacade;
class LLBC_Session;
class LLBC_ICoderFactory;
class LLBC_IFacadeFactory;
class LLBC_IProtocolFactory;
class LLBC_ProtocolStack;
class LLBC_IProtocolFilter;

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
     * @param[in] This * - new service.
     */
    static This *Create(Type type, const LLBC_String &name = "", LLBC_IProtocolFactory *protoFactory = NULL);

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

public:
    /**
     * Create a session and listening.
     * @param[in] ip           - the ip address.
     * @param[in] port         - the port number.
     * @param[in] protoFactory - the protocol factory, default use service protocol factory.
     * @return int - the new session Id, if return 0, means failed, see LLBC_GetLastError().
     */
    virtual int Listen(const char *ip, uint16 port, LLBC_IProtocolFactory *protoFactory = NULL) = 0;

    /**
     * Establisthes a connection to a specified address.
     * @param[in] ip           - the ip address.
     * @param[in] port         - the port number.
     * @param[in] timeout      - the timeout value on connect operation, default use OS setting.
     * @param[in] protoFactory - the protocol factory, default use service protocol factory.
     * @return int - the new session Id, if return 0, means failed, see LBLC_GetLastError().
     */
    virtual int Connect(const char *ip, uint16 port, double timeout = -1, LLBC_IProtocolFactory *protoFactory = NULL) = 0;

    /**
     * Asynchronous establishes a connection to a specified address.
     * @param[in] ip           - the ip address.
     * @param[in] port         - the port number.
     * @param[in] timeout      - the timeout value on connect operation, default use OS setting.
     * @param[in] protoFactory - the protocol factory, default use service protocol factory.
     * @return int - return 0 if success, otherwise return -1.
     *               Note: return 0 is not means the connection was established,
     *                     it only means post async-conn request to poller success.
     */
    virtual int AsyncConn(const char *ip, uint16 port, double timeout = -1, LLBC_IProtocolFactory *protoFactory = NULL) = 0;

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
    virtual int Multicast(const LLBC_SessionIdList &sessionIds);
    virtual int Multicast(const LLBC_SessionIdList &sessionIds, int opcode);
    virtual int Multicast(const LLBC_SessionIdList &sessionIds, LLBC_ICoder *coder);
    virtual int Multicast(const LLBC_SessionIdList &sessionIds, int opcode, LLBC_ICoder *coder);
    virtual int Multicast(const LLBC_SessionIdList &sessionIds, int opcode, LLBC_ICoder *coder, int status);
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
    virtual int Multicast(const LLBC_SessionIdList &sessionIds, const void *bytes, size_t len);
    virtual int Multicast(const LLBC_SessionIdList &sessionIds, int opcode, const void *bytes, size_t len);
    virtual int Multicast(const LLBC_SessionIdList &sessionIds, int opcode, const void *bytes, size_t len, int status);
    virtual int Multicast(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, const void *bytes, size_t len, int status) = 0;

    /** 
     * Broadcast data(these methods will automatics create packet to send).
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
    virtual int Broadcast();
    virtual int Broadcast(int opcode);
    virtual int Broadcast(int opcode, LLBC_ICoder *coder, int status);
    virtual int Broadcast(int svcId, int opcode, LLBC_ICoder *coder, int status) = 0;

    /**
     * Broadcast bytes(these methods will automatics create packet to send).
     * @param[in] svcId      - the service Id.
     * @param[in] sessionIds - the session Ids.
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
    virtual int RemoveSession(int sessionId, const char *reason = NULL) = 0;

public:
    /**
     * Register facade.
     */
    template <typename FacadeFactoryCls>
    LLBC_IFacade *RegisterFacade();
    virtual LLBC_IFacade *RegisterFacade(LLBC_IFacadeFactory *facadeFactory) = 0;
    virtual int RegisterFacade(LLBC_IFacade *facade) = 0;

    /**
     * Get facade/facades.
     */
    template <typename FacadeCls>
    FacadeCls *GetFacade();
    template <typename FacadeCls>
    std::vector<LLBC_IFacade *> GetFacades();
    virtual LLBC_IFacade *GetFacade(const LLBC_String &facadeName) = 0;
    template <typename FacadeCls>
    FacadeCls *GetFacade(const LLBC_String &facadeName);
    virtual std::vector<LLBC_IFacade *> GetFacades(const LLBC_String &facadeName) = 0;

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
    virtual int Subscribe(int opcode, LLBC_IDelegate1<void, LLBC_Packet &> *deleg) = 0;

    /**
     * Previous subscribe message to specified handler method, if method return NULL, will stop packet process flow.
     */
    int PreSubscribe(int opcode, bool (*func)(LLBC_Packet &));
    template <typename ObjType>
    int PreSubscribe(int opcode, ObjType *obj, bool (ObjType::*method)(LLBC_Packet &));

    /**
     * Previous subscribe message to specified delegate, if method return NULL, will stop packet process flow.
     */
    virtual int PreSubscribe(int opcode, LLBC_IDelegate1<bool, LLBC_Packet &> *deleg) = 0;

#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    /**
     * Unify previous subscribe message to specified handler method, if method return NULL, will stop packet process flow.
     */
    int UnifyPreSubscribe(bool(*func)(LLBC_Packet &));
    template <typename ObjType>
    int UnifyPreSubscribe(ObjType *obj, bool (ObjType::*method)(LLBC_Packet &));

    /**
     * Unify previous subscribe message to specified delegate, if method return NULL, will stop packet process flow.
     */
    virtual int UnifyPreSubscribe(LLBC_IDelegate1<bool, LLBC_Packet &> *deleg) = 0;
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
    virtual int SubscribeStatus(int opcode, int status, LLBC_IDelegate1<void, LLBC_Packet &> *deleg) = 0;
#endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER

public:
    /**
     * Set protocol filter to service's specified protocol layer.
     * @param[in] filter  - the protocol filter.
     * @param[in] toLayer - which layer will add to.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int SetProtocolFilter(LLBC_IProtocolFilter *filter, int toLayer) = 0;

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
    LLBC_ListenerStub SubscribeEvent(int event, void(*func)(LLBC_Event *));
    template <typename ObjType>
    LLBC_ListenerStub SubscribeEvent(int event, ObjType *obj, void (ObjType::*method)(LLBC_Event *));

    /**
     * Subscribe event to specified delegate.
     */
    virtual LLBC_ListenerStub SubscribeEvent(int event, LLBC_IDelegate1<void, LLBC_Event *> *deleg) = 0;

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
     * @param[in] ev - the fill fire event pointer.
     */
    virtual void FireEvent(LLBC_Event *ev) = 0;

public:
    /**
     * Post lazy task to service.
     * @param[in] func - the task function.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Post(void(*func)(This *));
    /**
     * Post lazy task to service.
     * @param[in] obj    - the task object.
     * @param[in] method - the task method.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename ObjType>
    int Post(ObjType *obj, void (ObjType::*method)(This *));

    /**
     * Post lazy task to service.
     * @param[in] delegate - the task delegate.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Post(LLBC_IDelegate1<void, This *> *deleg) = 0;

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
     * Stack create helper method(call by service and session class).
     */
    virtual LLBC_ProtocolStack *CreatePackStack(int sessionId, int acceptSessionId = 0, LLBC_ProtocolStack *stack = NULL) = 0;
    virtual LLBC_ProtocolStack *CreateCodecStack(int sessionId, int acceptSessionId = 0, LLBC_ProtocolStack *stack = NULL) = 0;
    virtual LLBC_ProtocolStack *CreateFullStack(int sessionId, int acceptSessionId = 0) = 0;
};

__LLBC_NS_END

#include "llbc/comm/IServiceImpl.h"

#endif // !__LLBC_COMM_ISERVICE_H__

