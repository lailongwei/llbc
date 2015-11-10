/**
 * @file    SelectPoller.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/13
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_SELECT_POLLER_H__
#define __LLBC_COMM_SELECT_POLLER_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

#include "llbc/comm/BasePoller.h"

__LLBC_NS_BEGIN

class LLBC_HIDDEN LLBC_SelectPoller : public LLBC_BasePoller
{
public:
    LLBC_SelectPoller();
    virtual ~LLBC_SelectPoller();

public:
    /**
     * Startup poller.
     * @return int - return 0 if start success, otherwise return -1.
     */
    virtual int Start();

    /**
     * Task startup method.
     */
    virtual void Svc();

    /**
     * Task Cleanup handler.
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
#if LLBC_TARGET_PLATFORM_NON_WIN32
    virtual void AddSession(LLBC_Session *session);
#else
    virtual void AddSession(LLBC_Session *session, bool needAddToIocp = true);
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

    /**
     * Remove session from poller.
     */
    virtual void RemoveSession(LLBC_Session *session);

private:
    /**
     * Update the max fd.
     */
    void UpdateMaxFd();

    /**
     * Accept new connection.
     */
    void Accept(LLBC_Session *session);

    /**
     * Handle connecting socket.
     */
    int HandleConnecting(LLBC_FdSet &writes, LLBC_FdSet &excepts);

private:
    LLBC_SocketHandle _maxFd;

    LLBC_FdSet _reads;
    LLBC_FdSet _writes;
    LLBC_FdSet _excepts;
};

__LLBC_NS_END

#endif // !__LLBC_COMM_SELECT_POLLER_H__
