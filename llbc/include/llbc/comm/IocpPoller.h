/**
 * @file    IocpPoller.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/14
 * @version 1.0
 * 
 * @brief
 */
#ifndef __LLBC_COMM_IOCP_POLLER_H__
#define __LLBC_COMM_IOCP_POLLER_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

#include "llbc/comm/BasePoller.h"

#if LLBC_TARGET_PLATFORM_WIN32

__LLBC_NS_BEGIN

/**
 * Previous declare some classes.
 */
class LLBC_PollerMonitor;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The Iocp poller class encapsulation.
 */
class LLBC_HIDDEN LLBC_IocpPoller : public LLBC_BasePoller
{
public:
    LLBC_IocpPoller();
    virtual ~LLBC_IocpPoller();

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
    virtual void AddSession(LLBC_Session *session, bool needAddToIocp = true);

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
     * The connecting sockets handler.
     */
    bool HandleConnecting(int waitRet, LLBC_POverlapped ol, int errNo, int subErrNo);

    /**
     * Accept new session.
     */
    void Accept(LLBC_Session *session, LLBC_POverlapped ol);

private:
    LLBC_IocpHandle _iocp;
    LLBC_PollerMonitor *_monitor;
};

__LLBC_NS_END

#endif // LLBC_TARGET_PLATFORM_WIN32

#endif // !__LLBC_COMM_IOCP_POLLER_H__

