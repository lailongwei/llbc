/**
 * @file    BasePoller.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/11
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_BASE_POLLER_H__
#define __LLBC_COMM_BASE_POLLER_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

#include "llbc/comm/PollerEvent.h"
#include "llbc/comm/AsyncConnInfo.h"

__LLBC_NS_BEGIN

/**
 * Previous declare some classes.
 */
class LLBC_Socket;
class LLBC_Session;
class LLBC_IService;
class LLBC_PollerMgr;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The base poller class encapsulation.
 */
class LLBC_HIDDEN LLBC_BasePoller : public LLBC_BaseTask
{
    typedef LLBC_BasePoller This;

public:
    // Public Task::Push method, hide other Task methods.
    using LLBC_BaseTask::Push;

public:
    /**
     * Constructor & Destructor.
     */
    LLBC_BasePoller();
    virtual ~LLBC_BasePoller();

public:
    /**
     * Factory method, use to create specified type's poller.
     * @param[in] type - the poller type.
     * @return This * - the poller, if failed, return NULL.
     */
    static This *Create(int type);

public:
    /**
     * Set poller Id.
     * @param[in] id - the poller Id.
     */
    void SetPollerId(int id);

    /**
     * Set the poller brothers count.
     * @param[in] count - the brother count.
     */
    void SetBrothersCount(int count);

    /**
     * Set service.
     * @param[in] svc - the service.
     */
    void SetService(LLBC_IService *svc);

    /**
     * Set poller manager.
     * @param[in] mgr - the poller manager.
     */
    void SetPollerMgr(LLBC_PollerMgr *mgr);

public:
    /**
     * Startup poller to work.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Start();

    /**
     * Stop poller.
     */
    virtual void Stop();

    /**
     * Task cleanup handler.
     */
    virtual void Cleanup();

protected:
    /**
     * Handle queued events.
     */
    virtual void HandleQueuedEvents(int waitTime);
    virtual void HandleEv_AddSock(LLBC_PollerEvent &ev);
    virtual void HandleEv_AsyncConn(LLBC_PollerEvent &ev);
    virtual void HandleEv_Send(LLBC_PollerEvent &ev);
    virtual void HandleEv_Close(LLBC_PollerEvent &ev);
    virtual void HandleEv_Monitor(LLBC_PollerEvent &ev);
    virtual void HandleEv_TakeOverSession(LLBC_PollerEvent &ev);

    /**
     * Create new session from socket.
     */
    LLBC_Session *CreateSession(LLBC_Socket *socket, int sessionId = 0);

protected:
    /**
     * Add session to poller.
     * @param[in] session - the session.
     */
    void AddToPoller(LLBC_Session *session);

    /**
     * Add session to poller.
     */
#if LLBC_TARGET_PLATFORM_NON_WIN32
    virtual void AddSession(LLBC_Session *session);
#else
    virtual void AddSession(LLBC_Session *session, bool needAddToIocp = true);
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

    /**
     * Remove session from poller.
     */
    virtual void RemoveSession(LLBC_Session *session);

protected:
    /**
     * Set connected socket default options.
     * @param[in] sock - the connected socket.
     */
    void SetConnectedSocketDftOpts(LLBC_Socket *sock);

private:
    /**
     * Decleare friend class: LLBC_Session.
     * Access method list:
     *      AddSession(LLBC_Session *)
     *      RemoveSession(LLBC_Session *)
     */
    friend class LLBC_Session;

protected:
    volatile bool _started;
    volatile bool _stopping;

    int _id;
    int _brotherCount;
    LLBC_IService *_svc;
    LLBC_PollerMgr *_pollerMgr;
    
    typedef std::map<LLBC_SocketHandle, LLBC_Session *> _Sockets;
    _Sockets _sockets;
    typedef std::map<int, LLBC_Session *> _Sessions;
    _Sessions _sessions;

    typedef std::map<LLBC_SocketHandle, LLBC_AsyncConnInfo> _Connecting;
    _Connecting _connecting;

protected:
    typedef LLBC_PollerEvent _Ev;
    typedef void (LLBC_BasePoller::*_Handler)(_Ev &);
    static _Handler _handlers[LLBC_PollerEvent::End];
};

__LLBC_NS_END

#endif // !__LLBC_COMM_BASE_POLLER_H__
