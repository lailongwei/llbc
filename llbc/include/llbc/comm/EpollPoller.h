/**
 * @file    EpollPoller.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/15
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_EPOLL_POLLER_H__
#define __LLBC_COMM_EPOLL_POLLER_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

#include "llbc/comm/BasePoller.h"

#if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID

__LLBC_NS_BEGIN

/**
 * Previous declare some classes.
 */
class LLBC_PollerMonitor;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The Epoll poller class encapsulation.
 */
class LLBC_HIDDEN LLBC_EpollPoller : public LLBC_BasePoller
{
public:
    LLBC_EpollPoller();
    virtual ~LLBC_EpollPoller();

public:
    /**
     * Startup poller.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Start();

    /**
     * Task startup method.
     */
    virtual void Svc();

    /**
     * Task cleanup method.
     */
    virtual void Cleanup();

protected:
    /**
     * Queued event handlers.
     */
    virtual void HandleEv_AddSock(LLBC_PollerEvent &ev);
    virtual void HandleEv_AsyncConn(LLBC_PollerEvent &ev);
    virtual void HandleEv_Send(LLBC_PollerEvent &ev);
    virtual void HandleEv_Close(LLBC_PollerEvent &ev);
    virtual void HandleEv_Monitor(LLBC_PollerEvent &ev);
    virtual void HandleEv_TakeOverSession(LLBC_PollerEvent &ev);

    /**
     * Add session to poller.
     */
    virtual void AddSession(LLBC_Session *session);

    /**
     * Remove session from poller.
     */
    virtual void RemoveSession(LLBC_Session *session);

private:
    /**
     * Startup monitor.
     */
    int StartupMonitor();

    /**
     * Stop monitor.
     */
    void StopMonitor();

    /**
     * The monitor thread service method.
     */
    void MonitorSvc();

    /**
     * Handle connecting sockets.
     */
    bool HandleConnecting(LLBC_SocketHandle handle, int events);

    /**
     * Accept new session.
     */
    void Accept(LLBC_Session *session);

private:
    LLBC_Handle _epoll;
    LLBC_PollerMonitor *_monitor;

    LLBC_EpollEvent _events[LLBC_CFG_COMM_MAX_EVENT_COUNT];
};

__LLBC_NS_END

#endif // LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID

#endif // !__LLBC_COMM_EPOLL_POLLER_H__

