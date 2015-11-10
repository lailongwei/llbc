/**
 * @file    IService.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/16
 * @version 1.0
 *
 * @brief
 */
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
class LLBC_ProtocolStack;
class LLBC_IProtocolFilter;
class LLBC_PacketHeaderDesc;
class LLBC_IPacketHeaderDescFactory;
class LLBC_PacketHeaderParts;

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
     */
    static This *Create(Type type);

    /**
     * Set the packet header describe.
     * @param[in] headerDesc - the packet header describe.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int SetPacketHeaderDesc(LLBC_PacketHeaderDesc *headerDesc);

    /**
     * Set the packet header desc factory, this method is thread unsafty, call in progress start.
     * @param[in] factory - the factory.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int SetPacketHeaderDescFactory(LLBC_IPacketHeaderDescFactory *factory);

public:
    /**
     * Get the service Id.
     * @return int - the service Id.
     */
    virtual int GetId() const = 0;

    /**
     * Set the service Id.
     * @param[in] id - the service Id.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int SetId(int id) = 0;

    /**
     * Get the service type.
     * @return Type - the service type.
     */
    virtual Type GetType() const = 0;

    /**
     * Set the service drive mode.
     * @param[in] mode - the service drive mode.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int SetDriveMode(DriveMode mode) = 0;

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
     * @param[in] ip   - the ip address.
     * @param[in] port - the port number.
     * @return int - the new session Id, if return 0, means failed, see LLBC_GetLastError().
     */
    virtual int Listen(const char *ip, uint16 port) = 0;

    /**
     * Establisthes a connection to a specified address.
     * @param[in] ip   - the ip address.
     * @param[in] port - the port number.
     * @return int - the new session Id, if return 0, means failed, see LBLC_GetLastError().
     */
    virtual int Connect(const char *ip, uint16 port) = 0;

    /**
     * Asynchronous establishes a connection to a specified address.
     * @param[in] ip   - the ip address.
     * @param[in] port - the port number.
     * @return int - return  if success, otherwise return -1.
     *               Note: return 0 is not means the connection was established,
     *                     it only means post async-conn request to poller success.
     */
    virtual int AsyncConn(const char *ip, uint16 port) = 0;

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
     * @param[in] parts     - the packet header parts values, default is NULL.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Send(int sessionId, int opcode, LLBC_ICoder *coder, int status) = 0;
    virtual int Send(int svcId, int sessionId, int opcode, LLBC_ICoder *coder, int status) = 0;
    virtual int Send2(int sessionId, int opcode, LLBC_ICoder *coder, int status, LLBC_PacketHeaderParts *parts) = 0;
    virtual int Send2(int svcId, int sessionId, int opcode, LLBC_ICoder *coder, int status, LLBC_PacketHeaderParts *parts) = 0;

    /**
     * Send bytes(these methods will automatics create packet to send).
     * @param[in] svcId     - the service Id.
     * @param[in] sessionId - the session Id.
     * @param[in] opcode    - the opcode.
     * @param[in] bytes     - the bytes data.
     * @param[in] len       - data length.
     * @param[in] status    - the status, default is 0.
     * @param[in] parts     - the packet header parts values, default is NULL.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Send(int sessionId, int opcode, const void *bytes, size_t len, int status) = 0;
    virtual int Send(int svcId, int sessionId, int opcode, const void *bytes, size_t len, int status) = 0;
    virtual int Send2(int sessionId, int opcode, const void *bytes, size_t len, int status, LLBC_PacketHeaderParts *parts) = 0;
    virtual int Send2(int svcId, int sessionId, int opcode, const void *bytes, size_t len, int status, LLBC_PacketHeaderParts *parts) = 0;

    /**
     * Send data(the template methods will automatics create packet to send).
     * @param[in] sessionId - the session Id.
     * @param[in] opcode    - the opcode.
     * @param[in] data      - the data.
     * @param[in] status    - the status, default is 0.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename T>
    int Send(int sessionId, int opcode, const T &data, int status);
    template <typename T>
    int Send(int svcId, int sessionId, int opcode, const T &data, int status);
    template <typename T>
    int Send2(int sessionId, int opcode, const T &data, int status, LLBC_PacketHeaderParts *parts);
    template <typename T>
    int Send2(int svcId, int sessionId, int opcode, const T &data, int status, LLBC_PacketHeaderParts *parts);

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
     * @param[in] parts     - the packet header parts values, default is NULL.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Multicast(const LLBC_SessionIdList &sessionIds, int opcode, LLBC_ICoder *coder, int status) = 0;
    virtual int Multicast(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, LLBC_ICoder *coder, int status) = 0;
    virtual int Multicast2(const LLBC_SessionIdList &sessionIds, int opcode, LLBC_ICoder *coder, int status, LLBC_PacketHeaderParts *parts) = 0;
    virtual int Multicast2(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, LLBC_ICoder *coder, int status, LLBC_PacketHeaderParts *parts) = 0;

    /**
     * Multicast bytes(these methods will automatics create packet to send).
     * @param[in] svcId      - the service Id.
     * @param[in] sessionIds - the session Ids.
     * @param[in] opcode     - the opcode.
     * @param[in] bytes      - bytes to multi cast.
     * @param[in] len   `    - will send bytes len, in bytes.
     * @param[in] status     - the status, default is 0.
     * @param[in] parts     - the packet header parts values, default is NULL.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Multicast(const LLBC_SessionIdList &sessionIds, int opcode, const void *bytes, size_t len, int status) = 0;
    virtual int Multicast(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, const void *bytes, size_t len, int status) = 0;
    virtual int Multicast2(const LLBC_SessionIdList &sessionIds, int opcode, const void *bytes, size_t len, int status, LLBC_PacketHeaderParts *parts) = 0;
    virtual int Multicast2(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, const void *bytes, size_t len, int status, LLBC_PacketHeaderParts *parts) = 0;
    /**
     * Multicast data(these template methods will automatics create packet to send).
     * @param[in] sessionIds - the session Ids.
     * @param[in] opcode     - the opcode.
     * @param[in] data       - the data.
     * @param[in] status     - the status, default is 0.
     * @param[in] parts     - the packet header parts values, default is NULL.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename T>
    int Multicast(const LLBC_SessionIdList &sessionIds, int opcode, const T &data, int status);
    template <typename T>
    int Multicast(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, const T &data, int status);
    template <typename T>
    int Multicast2(const LLBC_SessionIdList &sessionIds, int opcode, const T &data, int status, LLBC_PacketHeaderParts *parts);
    template <typename T>
    int Multicast2(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, const T &data, int status, LLBC_PacketHeaderParts *parts);
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
     * @param[in] parts     - the packet header parts values, default is NULL.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Broadcast(int opcode, LLBC_ICoder *coder, int status) = 0;
    virtual int Broadcast(int svcId, int opcode, LLBC_ICoder *coder, int status) = 0;
    virtual int Broadcast2(int opcode, LLBC_ICoder *coder, int status, LLBC_PacketHeaderParts *parts) = 0;
    virtual int Broadcast2(int svcId, int opcode, LLBC_ICoder *coder, int status, LLBC_PacketHeaderParts *parts) = 0;

    /**
     * Broadcast bytes(these methods will automatics create packet to send).
     * @param[in] svcId      - the service Id.
     * @param[in] sessionIds - the session Ids.
     * @param[in] opcode     - the opcode.
     * @param[in] bytes      - bytes to multi cast.
     * @param[in] len   `    - will send bytes len, in bytes.
     * @param[in] status     - the status, default is 0.
     * @param[in] parts     - the packet header parts values, default is NULL.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Broadcast(int opcode, const void *bytes, size_t len, int status) = 0;
    virtual int Broadcast(int svcId, int opcode, const void *bytes, size_t len, int status) = 0;
    virtual int Broadcast2(int opcode, const void *bytes, size_t len, int status, LLBC_PacketHeaderParts *parts) = 0;
    virtual int Broadcast2(int svcId, int opcode, const void *bytes, size_t len, int status, LLBC_PacketHeaderParts *parts) = 0;

    /**
     * Broadcast data(these template methods will automatics create packet to send).
     * @param[in] sessionIds - the session Ids.
     * @param[in] opcode     - the opcode.
     * @param[in] data       - the data.
     * @param[in] status     - the status, default is 0.
     * @param[in] parts     - the packet header parts values, default is NULL.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename T>
    int Broadcast(int opcode, const T &data, int status);
    template <typename T>
    int Broadcast(int svcId, int opcode, const T &data, int status);
    template <typename T>
    int Broadcast2(int opcode, const T &data, int status, LLBC_PacketHeaderParts *parts);
    template <typename T>
    int Broadcast2(int svcId, int opcode, const T &data, int status, LLBC_PacketHeaderParts *parts);

    /**
     * Remove session, always success.
     * @param[in] sessionId - the will close session Id.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int RemoveSession(int sessionId) = 0;

public:
    /**
     * Register facade.
     */
    virtual int RegisterFacade(LLBC_IFacade *facade) = 0;

    /**
     * Register coder.
     */
    virtual int RegisterCoder(int opcode, LLBC_ICoderFactory *coder) = 0;

#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
    /**
     * Register status code describe.
     */
    virtual int RegisterStatusDesc(int status, const LLBC_String &desc) = 0;
#endif // LLBC_CFG_COMM_ENABLE_STATUS_DESC

    /**
     * Subscribe message to specified delegate.
     */
    virtual int Subscribe(int opcode, LLBC_IDelegate1<LLBC_Packet &> *deleg) = 0;

    /**
     * Subscribe message to specified handler method.
     */
    template <typename ObjType>
    int Subscribe(int opcode, ObjType *obj, void (ObjType::*method)(LLBC_Packet &));

    /**
     * Previous subscribe message to specified delegate, if method return NULL, will direct close specified session.
     */
    virtual int PreSubscribe(int opcode, LLBC_IDelegateEx<LLBC_Packet &> *deleg) = 0;

    /**
     * Previous subscribe message to specified handler method, if method return NULL, will direct close specified session.
     */
    template <typename ObjType>
    int PreSubscribe(int opcode, ObjType *obj, void *(ObjType::*method)(LLBC_Packet &));

#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    /**
     * Unify previous subscribe message to specified delegate, if method return NULL, will direct close specified session.
     */
    virtual int UnifyPreSubscribe(LLBC_IDelegateEx<LLBC_Packet &> *deleg) = 0;

    /**
     * Unify previous subscribe message to specified handler method, if method return NULL, will direct close specified session.
     */
    template <typename ObjType>
    int UnifyPreSubscribe(ObjType *obj, void *(ObjType::*method)(LLBC_Packet &));
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

#if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
    /**
     * Subscribe message status to specified delegate, if subscribed, service will not call default opcode handler.
     */
    virtual int SubscribeStatus(int opcode, int status, LLBC_IDelegate1<LLBC_Packet &> *deleg) = 0;

    /**
     * Subscribe message status to specified method, if subscribed, service will not call default opcode handler.
     */
    template <typename ObjType>
    int SubscribeStatus(int opcode ,int status, ObjType *obj, void (ObjType::*method)(LLBC_Packet &));
#endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER

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
    template <typename ObjType>
    LLBC_ListenerStub SubscribeEvent(int event, ObjType *obj, void (ObjType::*method)(LLBC_Event *));

    /**
     * Subscribe event to specified delegate.
     */
    virtual LLBC_ListenerStub SubscribeEvent(int event, LLBC_IDelegate1<LLBC_Event *> *deleg) = 0;

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
     * @param[in] delegate - the task delegate.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Post(LLBC_IDelegate1<This *> *deleg) = 0;

    /**
     * Post lazy task to service.
     * @param[in] obj    - the task object.
     * @param[in] method - the task method.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename ObjType>
    int Post(ObjType *obj, void (ObjType::*method)(This *));

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
     *      CreateRawStack()
     */
    friend class LLBC_Session;

    /**
     * Stack create helper method(call by service and session class).
     */
    virtual LLBC_ProtocolStack *CreateRawStack(LLBC_ProtocolStack *stack = NULL) = 0;
    virtual LLBC_ProtocolStack *CreateCodecStack(LLBC_ProtocolStack *stack = NULL) = 0;
    virtual LLBC_ProtocolStack *CreateFullStack() = 0;
};

__LLBC_NS_END

#include "llbc/comm/IServiceImpl.h"

#endif // !__LLBC_COMM_ISERVICE_H__

