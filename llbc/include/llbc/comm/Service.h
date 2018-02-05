/**
 * @file    Service.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/16
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_SERVICE_H__
#define __LLBC_COMM_SERVICE_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

#include "llbc/comm/IService.h"
#include "llbc/comm/ServiceEvent.h"
#include "llbc/comm/PollerMgr.h"
#if !LLBC_CFG_COMM_USE_FULL_STACK
#include "llbc/comm/protocol/ProtocolStack.h"
#endif

__LLBC_NS_BEGIN

/**
 * Previous declare some classes.
 */
class LLBC_ServiceMgr;

__LLBC_NS_END

__LLBC_NS_BEGIN

class LLBC_HIDDEN LLBC_Service : public LLBC_IService
{
    typedef LLBC_IService Base;
    typedef LLBC_Service This;

public:
    /**
     * Create specified type service.
     * @param[in] type         - the service type, see LLBC_IService::Type enumeration.
     * @param[in] name         - type service name.
     * @param[in] protoFactory - the protocol factory, when type is Custom, will use this protocol factory to create protocols.
     */
    LLBC_Service(Type type, const LLBC_String &name = "", LLBC_IProtocolFactory *protoFactory = NULL);

    /**
     * Service destructor.
     */
    virtual ~LLBC_Service();

public:
    /**
     * Get the service Id.
     * @return int - the service Id.
     */
    virtual int GetId() const;

    /**
     * Get the service type.
     * @return Type - the service type.
     */
    virtual Type GetType() const;

    /**
     * Get the service name.
     * @return const LLBC_String & - the service name.
     */
    virtual const LLBC_String &GetName() const;

    /**
     * Get the service drive mode.
     * @return DriveMode - the service drive mode.
     */
    virtual DriveMode GetDriveMode() const;

    /**
     * Set the service drive mode.
     * @param[in] mode - the service drive mode.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int SetDriveMode(DriveMode mode);

public:
    /**
     * Suppress coder not found warning in protocol-stack.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int SuppressCoderNotFoundWarning();

public:
    /**
     * Startup service, default will startup one poller to work.
     * @param[in] pollerCount - the poller count.
     * @return int - return 0 if startup successful, otherwise return -1.
     */
    virtual int Start(int pollerCount = 1);

    /**
     * Check service is started or not.
     * @return bool - return true if running, otherwise return false.
     */
    virtual bool IsStarted() const;

    /**
     * Stop the service.
     */
    virtual void Stop();

public:
    /**
     * Get service FPS.
     * @return int - the service FPS.
     */
    virtual int GetFPS() const;

    /**
     * Set service FPS.
     * @return int - return 0 if success, ohterwise return -1.
     */
    virtual int SetFPS(int fps);

    /**
     * Get service frame interval, in milli-seconds.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int GetFrameInterval() const;

public:
    /**
     * Create a session and listening.
     * Note:
     *      If service not start when call this method, connection operation will 
     *      create a pending-operation and recorded in service, your maybe could not get error.
     * @param[in] ip           - the ip address.
     * @param[in] port         - the port number.
     * @param[in] protoFactory - the protocol factory, default use service protocol factory.
     * @return int - the new session Id, if return 0, means failed, see LLBC_GetLastError().
     */
    virtual int Listen(const char *ip, uint16 port, LLBC_IProtocolFactory *protoFactory = NULL);

    /**
     * Establishes a connection to a specified address.
     * Note:
     *      If service not start when call this method, connection operation will 
     *      create a pending-operation and recorded in service, your maybe could not get error.
     * @param[in] ip           - the ip address.
     * @param[in] port         - the port number.
     * @param[in] timeout      - the timeout value on connect operation, default use OS setting.
     * @param[in] protoFactory - the protocol factory, default use service protocol factory.
     * @return int - the new session Id, if return 0, means failed, see LBLC_GetLastError().
     */
    virtual int Connect(const char *ip, uint16 port, double timeout = -1, LLBC_IProtocolFactory *protoFactory = NULL);

    /**
     * Asynchronous establishes a connection to a specified address.
     * Note:
     *      If service not start when call this method, connection operation will 
     *      create a pending-operation and recorded in service, your maybe could not get error.
     * @param[in] ip           - the ip address.
     * @param[in] port         - the port number.
     * @param[in] timeout      - the timeout value on connect operation, default use OS setting.
     * @param[in] protoFactory - the protocol factory, default use service protocol factory.
     * @return int - return 0 if success, otherwise return -1.
     *               Note: return 0 is not means the connection was established,
     *                     it only means post async-conn request to poller success.
     */
    virtual int AsyncConn(const char *ip, uint16 port, double timeout = -1, LLBC_IProtocolFactory *protoFactory = NULL);

    /**
     * Check given sessionId is lgeal or not.
     * @param[in] sessionId - the given session Id.
     * @return bool - return true is given session Id validate, otherwise return false.
     */
    virtual bool IsSessionValidate(int sessionId);

    /**
     * Send packet.
     * Note: 
     *      no matter this method success or not, packet will be managed by this call,
     *      it means no matter this call success or not, delete packet operation will
     *      execute by llbc framework.
     * @param[in] packet - the packet.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Send(LLBC_Packet *packet);

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
    virtual int Send(int sessionId, int opcode, LLBC_ICoder *coder, int status);
    virtual int Send(int svcId, int sessionId, int opcode, LLBC_ICoder *coder, int status);
    virtual int Send2(int sessionId, int opcode, LLBC_ICoder *coder, int status, LLBC_PacketHeaderParts *parts);
    virtual int Send2(int svcId, int sessionId, int opcode, LLBC_ICoder *coder, int status, LLBC_PacketHeaderParts *parts);

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
    virtual int Send(int sessionId, int opcode, const void *bytes, size_t len, int status);
    virtual int Send(int svcId, int sessionId, int opcode, const void *bytes, size_t len, int status);
    virtual int Send2(int sessionId, int opcode, const void *bytes, size_t len, int status, LLBC_PacketHeaderParts *parts);
    virtual int Send2(int svcId, int sessionId, int opcode, const void *bytes, size_t len, int status, LLBC_PacketHeaderParts *parts);

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
    virtual int Multicast(const LLBC_SessionIdList &sessionIds, int opcode, LLBC_ICoder *coder, int status);
    virtual int Multicast(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, LLBC_ICoder *coder, int status);
    virtual int Multicast2(const LLBC_SessionIdList &sessionIds, int opcode, LLBC_ICoder *coder, int status, LLBC_PacketHeaderParts *parts);
    virtual int Multicast2(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, LLBC_ICoder *coder, int status, LLBC_PacketHeaderParts *parts);

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
    virtual int Multicast(const LLBC_SessionIdList &sessionIds, int opcode, const void *bytes, size_t len, int status);
    virtual int Multicast(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, const void *bytes, size_t len, int status);
    virtual int Multicast2(const LLBC_SessionIdList &sessionIds, int opcode, const void *bytes, size_t len, int status, LLBC_PacketHeaderParts *parts);
    virtual int Multicast2(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, const void *bytes, size_t len, int status, LLBC_PacketHeaderParts *parts);

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
    virtual int Broadcast(int opcode, LLBC_ICoder *coder, int status);
    virtual int Broadcast(int svcId, int opcode, LLBC_ICoder *coder, int status);
    virtual int Broadcast2(int opcode, LLBC_ICoder *coder, int status, LLBC_PacketHeaderParts *parts);
    virtual int Broadcast2(int svcId, int opcode, LLBC_ICoder *coder, int status, LLBC_PacketHeaderParts *parts);
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
    virtual int Broadcast(int opcode, const void *bytes, size_t len, int status);
    virtual int Broadcast(int svcId, int opcode, const void *bytes, size_t len, int status);
    virtual int Broadcast2(int opcode, const void *bytes, size_t len, int status, LLBC_PacketHeaderParts *parts);
    virtual int Broadcast2(int svcId, int opcode, const void *bytes, size_t len, int status, LLBC_PacketHeaderParts *parts);

    /**
     * Remove session, always success.
     * @param[in] sessionId - the will close session Id.
	 * @param[in] reason    - the close reason string, use to describe session close reason.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int RemoveSession(int sessionId, const char *reason = NULL);

public:
    /**
     * Register facade.
     */
    virtual int RegisterFacade(LLBC_IFacade *facade);

    /**
     * Register coder.
     */
    virtual int RegisterCoder(int opcode, LLBC_ICoderFactory *coder);

#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
    /**
     * Register status code describe.
     */
    virtual int RegisterStatusDesc(int status, const LLBC_String &desc);
#endif // LLBC_CFG_COMM_ENABLE_STATUS_DESC 

    /**
     * Subscribe message to specified delegate.
     */
    virtual int Subscribe(int opcode, LLBC_IDelegate1<void, LLBC_Packet &> *deleg);

    /**
     * Previous subscribe message to specified delegate, if method return NULL, will stop packet process flow.
     */
    virtual int PreSubscribe(int opcode, LLBC_IDelegate1<bool, LLBC_Packet &> *deleg);

#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    /**
     * Unify previous subscribe message to specified delegate, if method return NULL, will stop packet process flow.
     */
    virtual int UnifyPreSubscribe(LLBC_IDelegate1<bool, LLBC_Packet &> *deleg);
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

#if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
    /**
     * Subscribe message status to specified delegate, if subscribed, service will not call default opcode handler.
     */
    virtual int SubscribeStatus(int opcode, int status, LLBC_IDelegate1<void, LLBC_Packet &> *deleg);
#endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER

    /**
     * Set protocol filter to service's specified protocol layer.
     * @param[in] filter  - the protocol filter.
     * @param[in] toLayer - which layer will add to.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int SetProtocolFilter(LLBC_IProtocolFilter *filter, int toLayer);

public:
    /**
     * Enable/Disable timer scheduler, only use external-drive type service.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int EnableTimerScheduler();
    virtual int DisableTimerScheduler();

public:
    /**
     * Subscribe event to specified delegate.
     */
    virtual LLBC_ListenerStub SubscribeEvent(int event, LLBC_IDelegate1<void, LLBC_Event *> *deleg);

    /**
     * Unsubscribe event.
     * @param[in] event - the event Id, all the event's handler will delegate.
     */
    virtual void UnsubscribeEvent(int event);

    /**
     * Unsubscribe event.
     * @param[in] stub - the event stub.
     */
    virtual void UnsubscribeEvent(const LLBC_ListenerStub &stub);

    /**
     * Fire event(asynchronous operation).
     * @param[in] ev - the will fire event pointer.
     */
    virtual void FireEvent(LLBC_Event *ev);

public:
    /**
     * Post lazy task to service.
     * @param[in] delegate - the task delegate.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Post(LLBC_IDelegate1<void, Base *> *deleg);

public:
    /**
     * One time service call routine, if service drive mode is ExternalDrive, you must manual call this method.
     * @param[in] fullFrame - the full frame flag.
     */
    virtual void OnSvc(bool fullFrame = true);

protected:
    /**
     * Stack create helper method(call by service and session class).
     */
    virtual LLBC_ProtocolStack *CreatePackStack(int sessionId, int acceptSessionId = 0, LLBC_ProtocolStack *stack = NULL);
    virtual LLBC_ProtocolStack *CreateCodecStack(int sessionId, int acceptSessionId = 0, LLBC_ProtocolStack *stack = NULL);
    virtual LLBC_ProtocolStack *CreateFullStack(int sessionId, int acceptSessionId = 0);

protected:
    /**
     * Task entry method.
     */
    virtual void Svc();

    /**
     * Task cleanup method.
     */
    virtual void Cleanup();

private:
    /**
     * Service TLS operation methods.
     */
    void AddServiceToTls();
    void RemoveServiceFromTls();
    bool IsCanContinueDriveService();

    /**
     * Frame tasks operation methods.
     */
    typedef std::set<LLBC_IDelegate1<void, Base *> *> _FrameTasks;
    void HandleFrameTasks(_FrameTasks &tasks, bool &usingFlag);
    void DestroyFrameTasks(_FrameTasks &tasks, bool &usingFlag);

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

    /**
     * Facade operation methods.
     */
    void InitFacades();
    void StartFacades();
    void UpdateFacades();
    void StopFacades();
    void DestroyFacades();

    /**
     * Session protocol factory operation methods.
     */
    void AddSessionProtocolFactory(int sessionId, LLBC_IProtocolFactory *protoFactory);
    LLBC_IProtocolFactory *FindSessionProtocolFactory(int sessionId);
    void RemoveSessionProtocolFactory(int sessionId);

#if LLBC_CFG_OBJBASE_ENABLED
    /**
     * Auto-Release pool operation methods.
     */
    void CreateAutoReleasePool();
    void UpdateAutoReleasePool();
    void DestroyAutoReleasePool();
#endif // LLBC_CFG_OBJBASE_ENABLED

    /**
     * Timer-Scheduler operation methods.
     */
    void GetTimerScheduler();
    void UpdateTimers();

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
                     bool validCheck = true);
    int LockableSend(int svcId,
                     int sessionId,
                     int opcode,
                     const void *bytes,
                     size_t len,
                     int status,
                     const LLBC_PacketHeaderParts *parts = NULL,
                     bool lock = true,
                     bool validCheck = true);

    template <typename SessionIds>
    int MulticastSendCoder(int svcId,
                           const SessionIds &sessionIds,
                           int opcode,
                           LLBC_ICoder *coder,
                           int status,
                           const LLBC_PacketHeaderParts *parts = NULL,
                           bool validCheck = true);

private:
    int _id;
    static int _maxId;

    Type _type;
    LLBC_String _name;
    LLBC_IProtocolFactory *_protoFactory;
    std::map<int, LLBC_IProtocolFactory *> _sessionProtoFactory;
    DriveMode _driveMode;
    bool _suppressedCoderNotFoundWarning;

    volatile bool _started;
    volatile bool _stopping;

    LLBC_RecursiveLock _lock;
    LLBC_SpinLock _protoLock;

    int _fps;
    int _frameInterval;
    sint64 _begHeartbeatTime;

    volatile bool _sinkIntoLoop;
    volatile bool _afterStop;

private:
    LLBC_PollerMgr _pollerMgr;
    
    LLBC_SessionIdSet _connectedSessionIds;
    LLBC_SpinLock _connectedSessionIdsLock;

#if !LLBC_CFG_COMM_USE_FULL_STACK
    LLBC_ProtocolStack _stack;
#endif

    typedef std::vector<LLBC_IFacade *> _Facades;
    _Facades _facades;
    typedef std::map<int, LLBC_ICoderFactory *> _Coders;
    _Coders _coders;
    typedef std::map<int, LLBC_IDelegate1<void, LLBC_Packet &> *> _Handlers;
    _Handlers _handlers;
    typedef std::map<int, LLBC_IDelegate1<bool, LLBC_Packet &> *> _PreHandlers;
    _PreHandlers _preHandlers;
#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    LLBC_IDelegate1<bool, LLBC_Packet &> *_unifyPreHandler;
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
#if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
    typedef std::map<int, LLBC_IDelegate1<void, LLBC_Packet &> *> _StatusHandlers;
    typedef std::map<int, _StatusHandlers *> _OpStatusHandlers;
    _OpStatusHandlers _statusHandlers;
#endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
    typedef std::map<int, LLBC_String> _StatusDescs;
    _StatusDescs _statusDescs;
#endif // LLBC_CFG_COMM_ENABLE_STATUS_DESC

    LLBC_IProtocolFilter *_filters[LLBC_ProtocolLayer::End];

private:
    _FrameTasks _beforeFrameTasks;
    _FrameTasks _afterFrameTasks;

    bool _handlingBeforeFrameTasks;
    bool _handledBeforeFrameTasks;
    bool _handlingAfterFrameTasks;

private:
#if LLBC_CFG_OBJBASE_ENABLED
    LLBC_AutoReleasePoolStack *_releasePoolStack;
#endif // LLBC_CFG_OBJBASE_ENABLED

private:
    LLBC_TimerScheduler *_timerScheduler;

private:
    LLBC_EventManager _evManager;

private:
    LLBC_ServiceMgr &_svcMgr;

private:
    typedef void (LLBC_Service::*_EvHandler)(LLBC_ServiceEvent &);
    static _EvHandler _evHandlers[LLBC_SvcEvType::End];
};

__LLBC_NS_END

#include "llbc/comm/ServiceImpl.h"

#endif // !__LLBC_COMM_SERVICE_H__

