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

#ifndef __LLBC_COMM_SERVICE_H__
#define __LLBC_COMM_SERVICE_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"

#include "llbc/comm/ComponentEvents.h"
#include "llbc/comm/IService.h"
#include "llbc/comm/ServiceEvent.h"
#include "llbc/comm/PollerMgr.h"
#include "llbc/comm/protocol/ProtocolStack.h"

__LLBC_NS_BEGIN

/**
 * Previous declare some classes.
 */
class LLBC_ServiceMgr;

__LLBC_NS_END

__LLBC_NS_BEGIN

class LLBC_HIDDEN LLBC_Service final : public LLBC_IService
{
    typedef LLBC_IService Base;
    typedef LLBC_Service This;

public:
    /**
     * Create specified type service.
     * @param[in] type         - the service type, see LLBC_IService::Type enumeration.
     * @param[in] name         - type service name.
     * @param[in] protoFactory - the protocol factory, when type is Custom, will use this protocol factory to create protocols.
     * @param[in] fullStack    - the full stack option, default is true.
     */
    LLBC_Service(Type type,
                 const LLBC_String &name = "",
                 LLBC_IProtocolFactory *protoFactory = nullptr,
                 bool fullStack = true);

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
     * Get full stack option.
     * @return bool - the full stack option.
     */
    virtual bool IsFullStack() const;

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
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int SetFPS(int fps);

    /**
     * Get service frame interval, in milli-seconds.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int GetFrameInterval() const;

    /**
     * Set service frame max time out.
     * @param[in] frameMaxTimeout - in nano-seconds.
     */
    virtual void SetFrameMaxTimeout(uint64 frameMaxTimeout);

    /**
     * Get service frame max time out.
     * @return LLBC_TimeSpan
     */
    virtual LLBC_TimeSpan GetFrameMaxTimeout() const;

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
                       const LLBC_SessionOpts &sessionOpts = LLBC_DftSessionOpts);

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
    virtual int Connect(const char *ip,
                        uint16 port,
                        double timeout = -1.0,
                        LLBC_IProtocolFactory *protoFactory = nullptr,
                        const LLBC_SessionOpts &sessionOpts = LLBC_DftSessionOpts);

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
     * @return int - return 0 if success, otherwise return -1.
     * @return int - the new session Id(not yet connected), if return 0 means failed, see LLBC_GetLastError().
     */
    virtual int AsyncConn(const char *ip,
                          uint16 port,
                          double timeout = -1.0,
                          LLBC_IProtocolFactory *protoFactory = nullptr,
                          const LLBC_SessionOpts &sessionOpts = LLBC_DftSessionOpts);

    /**
     * Check given sessionId is legal or not.
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
    virtual int Multicast(int svcId, const LLBC_SessionIdSet &sessionIds, int opcode, LLBC_ICoder *coder, int status);
    virtual int Multicast(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, LLBC_ICoder *coder, int status);
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
    virtual int Multicast(int svcId, const LLBC_SessionIdSet &sessionIds, int opcode, const void *bytes, size_t len, int status);
    virtual int Multicast(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, const void *bytes, size_t len, int status);

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
    virtual int Broadcast(int svcId, int opcode, LLBC_ICoder *coder, int status);
    /**
     * Broadcast bytes(these methods will automatics create packet to send).
     * @param[in] svcId      - the service Id.
     * @param[in] opcode     - the opcode.
     * @param[in] bytes      - bytes to multi cast.
     * @param[in] len   `    - will send bytes len, in bytes.
     * @param[in] status     - the status, default is 0.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Broadcast(int svcId, int opcode, const void *bytes, size_t len, int status);

    /**
     * Remove session, always success.
     * @param[in] sessionId - the will close session Id.
	 * @param[in] reason    - the close reason string, use to describe session close reason.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int RemoveSession(int sessionId, const char *reason = nullptr);

    /**
     * Control session protocol stack.
     * @param[in] sessionId - the sessionId.
     * @param[in] ctrlCmd   - the stack control command(user defined).
     * @param[in] ctrlData  - the stack control data(user defined).
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int CtrlProtocolStack(int sessionId,
                                  int ctrlCmd,
                                  const LLBC_Variant &ctrlData);

public:
    /**
     * Register component.
     */
    virtual int RegisterComponent(LLBC_IComponentFactory *compFactory);
    virtual int RegisterComponent(LLBC_IComponent *comp);
    virtual int RegisterComponent(const LLBC_String &libPath, const LLBC_String &compName, LLBC_IComponent *&comp);

    /**
     * Get component/componemts.
     */
    virtual LLBC_IComponent *GetComponent(const char *compName);
    virtual LLBC_IComponent *GetComponent(const LLBC_String &compName);
    virtual const std::vector<LLBC_IComponent *> &GetComponents(const LLBC_String &compName);

    /**
     * Register coder.
     */
    virtual int RegisterCoder(int opcode, LLBC_ICoderFactory *coderFactory);

#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
    /**
     * Register status code describe.
     */
    virtual int RegisterStatusDesc(int status, const LLBC_String &desc);
#endif // LLBC_CFG_COMM_ENABLE_STATUS_DESC 

    /**
     * Subscribe message to specified delegate.
     */
    virtual int Subscribe(int opcode, const LLBC_Delegate<void(LLBC_Packet &)> &deleg);

    /**
     * Previous subscribe message to specified delegate, if method return nullptr, will stop packet process flow.
     */
    virtual int PreSubscribe(int opcode, const LLBC_Delegate<bool(LLBC_Packet &)> &deleg);

#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    /**
     * Unify previous subscribe message to specified delegate, if method return false, will stop packet process flow.
     */
    virtual int UnifyPreSubscribe(const LLBC_Delegate<bool(LLBC_Packet &)> &deleg);
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

#if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
    /**
     * Subscribe message status to specified delegate, if subscribed, service will not call default opcode handler.
     */
    virtual int SubscribeStatus(int opcode, int status, const LLBC_Delegate<void(LLBC_Packet &)> &deleg);
#endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER

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
    virtual LLBC_ListenerStub SubscribeEvent(int event, const LLBC_Delegate<void(LLBC_Event &)> &deleg);

    /**
     * Subscribe event to specified event listener.
     */
    virtual LLBC_ListenerStub SubscribeEvent(int event, LLBC_EventListener *listener);

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
     * @param[in] ev             - the will fire event pointer.
     * @param[in] enqueueHandler - the event enqueue handler.
     * @param[in] dequeueHandler - the event dequeue handler.
     */
    virtual void FireEvent(LLBC_Event *ev,
                           const LLBC_Delegate<void(LLBC_Event *)> &enqueueHandler = nullptr,
                           const LLBC_Delegate<void(LLBC_Event *)> &dequeueHandler = nullptr);


    /**
     * Get event manager.
     * @return LLBC_EventManager & - the event manager.
     */
    virtual LLBC_EventManager &GetEventManager();

public:
    /**
     * Post lazy task to service.
     * @param[in] runnable - the runnable obj.
     * @param[in] data     - the runnable data, can be null.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Post(const LLBC_Delegate<void(Base *, const LLBC_Variant &)> &runnable, const LLBC_Variant &data = LLBC_Variant::nil);

    /**
     * Get service protocol stack, only full-stack option disabled available.
     * Warning: This is a danger method, only use in user-defined protocol.
     * @param[in] sessionId - the session Id.
     * @return const LLBC_ProtocolStack * - the protocol stack.
     */
    virtual const LLBC_ProtocolStack *GetCodecProtocolStack(int sessionId) const;

public:
    /**
     * Get service safety object pool.
     * @return LLBC_SafetyObjectPool & - the thread safety object pool reference.
     */
    virtual LLBC_SafetyObjectPool &GetSafetyObjectPool();

    /**
     * Get service unsafety object pool.
     * @return LLBC_UnsafetyObjectPool & - the thread unsafety object pool reference.
     */
    virtual LLBC_UnsafetyObjectPool &GetUnsafetyObjectPool();

    /**
     * Get service packet object pool(thread safety).
     * @return LLBC_ObjectPoolInst<LLBC_Packet, LLBC_SpinLock> & - the packet object pool.
     */
    virtual LLBC_ObjectPoolInst<LLBC_Packet> &GetPacketObjectPool();

    /**
     * Get message block object pool(thread safety).
     * @return LLBC_ObjectPoolInst<LLBC_MessageBlock, LLBC_SpinLock> & - the message block object pool.
     */
    virtual LLBC_ObjectPoolInst<LLBC_MessageBlock> &GetMsgBlockObjectPool();

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
    virtual LLBC_ProtocolStack *CreatePackStack(int sessionId, int acceptSessionId = 0, LLBC_ProtocolStack *stack = nullptr);
    virtual LLBC_ProtocolStack *CreateCodecStack(int sessionId, int acceptSessionId = 0, LLBC_ProtocolStack *stack = nullptr);
    virtual LLBC_ProtocolStack *CreateFullStack(int sessionId, int acceptSessionId = 0);

protected:
    /**
     * Application ini/property config reload event notifiy method.
     */
    virtual void NtyApplicationConfigReloaded(bool iniReloaded, bool propReloaded);

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
    virtual void AddSessionProtocolFactory(int sessionId, LLBC_IProtocolFactory *protoFactory);
    virtual LLBC_IProtocolFactory *FindSessionProtocolFactory(int sessionId);
    virtual void RemoveSessionProtocolFactory(int sessionId);

protected:
    /**
     * Ready session operation methods.
     */
    void AddReadySession(int sessionId, int acceptSessionId, bool isListenSession, bool repeatCheck = false);
    void RemoveReadySession(int sessionId);
    void RemoveAllReadySessions();

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
    typedef std::vector<std::pair<LLBC_Delegate<void(Base *, const LLBC_Variant &)>, LLBC_Variant> > _FrameTasks;
    void HandleFrameTasks();
    void DestroyFrameTasks();

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
    void HandleEv_AppCfgReloaded(LLBC_ServiceEvent &ev);

    /**
     * Component operation methods.
     */
    int InitComps();
    int StartComps();
    void UpdateComps();
    void StopComps();
    void DestroyComps();
    void DestroyWillRegComps();
    void CloseAllCompLibraries();
    void AddComp(LLBC_IComponent *comp);
    void AddCompToCaredEventsArray(LLBC_IComponent *comp);
    LLBC_Library *OpenCompLibrary(const LLBC_String &libPath, bool &existingLib);
    void CloseCompLibrary(const LLBC_String &libPath);
    void ClearCompsWhenInitCompFailed();

    /**
     * Auto-Release pool operation methods.
     */
    void InitAutoReleasePool();
    void UpdateAutoReleasePool();
    void ClearAutoReleasePool();

    /**
    * Object pool operation methods.
    */
    void InitObjectPools();
    void UpdateObjectPools();
    void ClearHoldedObjectPools();

    /**
     * Timer-Scheduler operation methods.
     */
    void InitTimerScheduler();
    void UpdateTimers();
    void ClearHoldedTimerScheduler();

    /**
     * Idle process method.
     */
    void ProcessIdle(bool fullFrame);

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
                     bool lock = true,
                     bool validCheck = true);

    template <typename SessionIds>
    int MulticastSendCoder(int svcId,
                           const SessionIds &sessionIds,
                           int opcode,
                           LLBC_ICoder *coder,
                           int status,
                           bool validCheck = true);

private:
    int _id;
    static int _maxId;

    Type _type;
    bool _fullStack;
    LLBC_String _name;
    LLBC_IProtocolFactory *_protoFactory;
    std::map<int, LLBC_IProtocolFactory *> _sessionProtoFactory;
    DriveMode _driveMode;
    bool _suppressedCoderNotFoundWarning;

    volatile bool _started;
    volatile bool _stopping;
    volatile bool _initingComp;

    LLBC_RecursiveLock _lock;
    LLBC_SpinLock _protoLock;

    int _fps;
    int _frameInterval;
    uint64 _relaxTimes;
    sint64 _begHeartbeatTime;
    uint64 _frameMaxTimeout;

    volatile bool _sinkIntoLoop;
    volatile bool _afterStop;

private:
    LLBC_PollerMgr _pollerMgr;

    class _ReadySessionInfo
    {
    public:
        int sessionId;
        int acceptSessionId;
        bool isListenSession;
        LLBC_ProtocolStack *codecStack;

    public:
        _ReadySessionInfo(int sessionId, int acceptSessionId, bool isListenSession, LLBC_ProtocolStack *codecStack = nullptr);
        ~_ReadySessionInfo();
    };
    typedef std::map<int, _ReadySessionInfo *> _ReadySessionInfos;
    typedef _ReadySessionInfos::iterator _ReadySessionInfosIter;
    typedef _ReadySessionInfos::const_iterator _ReadySessionInfosCIter;
    _ReadySessionInfos _readySessionInfos;
    LLBC_SpinLock _readySessionInfosLock;

    class _WillRegComp
    {
    public:
        LLBC_IComponent *comp;
        LLBC_IComponentFactory *compFactory;

        _WillRegComp(LLBC_IComponent *comp);
        _WillRegComp(LLBC_IComponentFactory *compFactory);
    };
    typedef std::vector<_WillRegComp> _WillRegComps;
    _WillRegComps _willRegComps;

    volatile int _compsInitFinished;
    volatile int _compsInitRet;
    volatile int _compsStartFinished;
    volatile int _compsStartRet;

    LLBC_String _compNameKey;
    typedef std::vector<LLBC_IComponent *> _Comps;
    _Comps _comps;
    typedef std::map<LLBC_String, _Comps> _Comps2;
    _Comps2 _comps2;
    _Comps *_caredEventComps[LLBC_ComponentEventsOffset::End];
    typedef std::map<LLBC_String, LLBC_Library *> _CompLibraries;
    _CompLibraries _compLibraries;
    typedef std::map<int, LLBC_ICoderFactory *> _Coders;
    _Coders _coders;
    typedef std::map<int, LLBC_Delegate<void(LLBC_Packet &)> > _Handlers;
    _Handlers _handlers;
    typedef std::map<int, LLBC_Delegate<bool(LLBC_Packet &)> > _PreHandlers;
    _PreHandlers _preHandlers;
#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    LLBC_Delegate<bool(LLBC_Packet &)> _unifyPreHandler;
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
#if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
    typedef std::map<int, LLBC_Delegate<void(LLBC_Packet &)> > _StatusHandlers;
    typedef std::map<int, _StatusHandlers *> _OpStatusHandlers;
    _OpStatusHandlers _statusHandlers;
#endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
    typedef std::map<int, LLBC_String> _StatusDescs;
    _StatusDescs _statusDescs;
#endif // LLBC_CFG_COMM_ENABLE_STATUS_DESC

private:
    uint32 _frameTaskIdx;
    _FrameTasks _frameTasks[2];

private:
    LLBC_AutoReleasePoolStack *_releasePoolStack;

private:
    LLBC_SafetyObjectPool _safetyObjectPool;
    LLBC_UnsafetyObjectPool _unsafetyObjectPool;
    LLBC_ObjectPoolInst<LLBC_Packet> &_packetObjectPool;
    LLBC_ObjectPoolInst<LLBC_MessageBlock> &_msgBlockObjectPool;

private:
    LLBC_TimerScheduler *_timerScheduler;

private:
    LLBC_EventManager _evManager;
    LLBC_ListenerStub _evManagerMaxListenerStub;

private:
    LLBC_ServiceMgr &_svcMgr;

private:
    std::vector<LLBC_Packet *> _multicastOtherPackets;

private:
    typedef void (LLBC_Service::*_EvHandler)(LLBC_ServiceEvent &);
    static _EvHandler _evHandlers[LLBC_SvcEvType::End];
};

__LLBC_NS_END

#include "llbc/comm/ServiceImpl.h"

#endif // !__LLBC_COMM_SERVICE_H__

