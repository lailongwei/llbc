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

#include "llbc/comm/ComponentEvents.h"
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
    typedef LLBC_Service Base;
    typedef LLBC_ServiceImpl This;

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
    virtual ~LLBC_ServiceImpl();

public:
    /**
     * Get the service Id.
     * @return int - the service Id.
     */
    virtual int GetId() const;

    /**
     * Get the service name.
     * @return const LLBC_String & - the service name.
     */
    virtual const LLBC_String &GetName() const;

    /**
     * Get config type.
     * @return int - the config type, see LLBC_AppConfigType enum.
     */
    virtual int GetConfigType() const;

    /**
     * Get non-property type config.
     * @return const LLBC_Variant & - the non-property application config.
     */
    virtual const LLBC_Variant &GetConfig() const;

    /**
     * Get property type config.
     * @return const LLBC_Property & - the property config.
     */
    virtual const LLBC_Property &GetPropertyConfig() const;

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
    virtual int Multicast(int svcId, const LLBC_SessionIdSet &sessionIds, int opcode, LLBC_Coder *coder, int status);
    virtual int Multicast(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, LLBC_Coder *coder, int status);
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
    virtual int Broadcast(int svcId, int opcode, LLBC_Coder *coder, int status);
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
    virtual int AddComponent(LLBC_Component *comp);
    virtual int AddComponent(const LLBC_String &compSharedLibPath,
                             const LLBC_String &compName,
                             LLBC_Component *&comp);

    /**
     * Get component.
     */
    virtual LLBC_Component *GetComponent(const LLBC_CString &compName);

 
public:
    /**
     * Add coder factory.
     */
    virtual int AddCoderFactory(int opcode, LLBC_CoderFactory *coderFactory);

public:
    #if LLBC_CFG_COMM_ENABLE_STATUS_DESC
    /**
     * Add status code describe.
     */
    virtual int AddStatusDesc(int status, const LLBC_String &desc);
    #endif // LLBC_CFG_COMM_ENABLE_STATUS_DESC 

public:
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
     * Begin fire event(asynchronous operation).
     * @param[in] eventId - the event id.
     * @return LLBC_Event & - the event firer object. 
     */
    virtual LLBC_ServiceEventFirer &BeginFireEvent(int eventId);

    /**
     * Get event manager.
     * @return LLBC_EventMgr & - the event manager.
     */
    virtual LLBC_EventMgr &GetEventManager();

public:
    /**
     * Post lazy task to service.
     * @param[in] runnable - the runnable obj.
     * @param[in] data     - the runnable data, can be null.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Post(const LLBC_Delegate<void(Base *, const LLBC_Variant &)> &runnable,
                     const LLBC_Variant &data = LLBC_Variant::nil);

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
    virtual LLBC_ProtocolStack *CreatePackStack(int sessionId,
                                                int acceptSessionId = 0,
                                                LLBC_ProtocolStack *stack = nullptr);
    virtual LLBC_ProtocolStack *CreateCodecStack(int sessionId,
                                                 int acceptSessionId = 0,
                                                 LLBC_ProtocolStack *stack = nullptr);
    virtual LLBC_ProtocolStack *CreateFullStack(int sessionId, int acceptSessionId = 0);

protected:
    /**
     * Get component list.
     */
    virtual const std::vector<LLBC_Component *> &GetComponentList() const;

protected:
    /**
     * Process application config reload event.
     */
    virtual void ProcessAppConfigReload();

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
    virtual void AddReadySession(int sessionId,
                                 int acceptSessionId,
                                 bool isListenSession,
                                 bool repeatCheck = false);
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
     * Service config operation methods.
     */
    void UpdateServiceCfg(LLBC_SvcEv_AppCfgReloadedEv *ev = nullptr);

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
    void HandleEv_AppPhaseEv(LLBC_ServiceEvent &ev);
    void HandleEv_AppCfgReload(LLBC_ServiceEvent &ev);

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
    void AddComp(LLBC_Component *comp);
    void AddCompToCaredEventsArray(LLBC_Component *comp);
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
    void UpdateTimerScheduler();
    void ClearHoldedTimerScheduler();

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
                     bool lock = true,
                     bool validCheck = true);

    template <typename SessionIds>
    int MulticastSendCoder(int svcId,
                           const SessionIds &sessionIds,
                           int opcode,
                           LLBC_Coder *coder,
                           int status,
                           bool validCheck = true);

private:
    static int _maxId;

    int _id;
    LLBC_String _name;
    LLBC_ThreadId _svcThreadId;

    int _cfgType;
    LLBC_Property _propCfg;
    LLBC_Variant _nonPropCfg;

    bool _fullStack;
    LLBC_IProtocolFactory *_dftProtocolFactory;
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
        _ReadySessionInfo(int sessionId,
                          int acceptSessionId,
                          bool isListenSession,
                          LLBC_ProtocolStack *codecStack = nullptr);
        ~_ReadySessionInfo();
    };
    std::map<int, _ReadySessionInfo *> _readySessionInfos;
    LLBC_SpinLock _readySessionInfosLock;

    std::list<LLBC_Component *> _willRegComps;
    volatile bool _compsInitFinished;
    volatile int _compsInitRet;
    volatile bool _compsStartFinished;
    volatile int _compsStartRet;

    std::vector<LLBC_Component *> _compList;
    std::map<LLBC_CString, LLBC_Component *> _name2Comps;
    std::vector<LLBC_Component *> _caredEventComps[LLBC_ComponentEventIndex::End];
    std::map<LLBC_String, LLBC_Library *> _compLibraries;
    std::map<int, LLBC_CoderFactory *> _coders;
    std::map<int, LLBC_Delegate<void(LLBC_Packet &)> > _handlers;
    std::map<int, LLBC_Delegate<bool(LLBC_Packet &)> > _preHandlers;
    #if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    LLBC_Delegate<bool(LLBC_Packet &)> _unifyPreHandler;
    #endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    #if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
    std::map<int, std::map<int, LLBC_Delegate<void(LLBC_Packet &)> > > _statusHandlers;
    #endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
    #if LLBC_CFG_COMM_ENABLE_STATUS_DESC
    std::map<int, LLBC_String> _statusDescs;
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
    LLBC_ObjectPoolInst<LLBC_ServiceEventFirer> &_eventFirerPool;

private:
    LLBC_TimerScheduler *_timerScheduler;

private:
    LLBC_EventMgr _evManager;
    LLBC_ListenerStub _evManagerMaxListenerStub;

private:
    LLBC_ServiceMgr &_svcMgr;

private:
    std::vector<LLBC_Packet *> _multicastOtherPackets;

private:
    typedef void (LLBC_ServiceImpl::*_EvHandler)(LLBC_ServiceEvent &);
    static _EvHandler _evHandlers[LLBC_ServiceEventType::End];
};

__LLBC_NS_END

#include "llbc/comm/ServiceImplInl.h"
