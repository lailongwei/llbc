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
     * Stop poller.
     */
    virtual void Stop();

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
    virtual void HandleEv_CtrlProtocolStack(LLBC_PollerEvent &ev);

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



