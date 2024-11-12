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

__LLBC_NS_BEGIN

class LLBC_HIDDEN LLBC_SelectPoller final : public LLBC_BasePoller
{
public:
    LLBC_SelectPoller();

    ~LLBC_SelectPoller() override;

public:
    /**
     * Startup poller.
     * @return int - return 0 if start success, otherwise return -1.
     */
    int Start() override;

    /**
     * Task startup method.
     */
    void Svc() override;

    /**
     * Task Cleanup handler.
     */
    void Cleanup() override;

protected:
    /**
     * Queued event handlers.
     */
    void HandleEv_AddSock(LLBC_PollerEvent &ev) override;

    void HandleEv_AsyncConn(LLBC_PollerEvent &ev) override;

    void HandleEv_Send(LLBC_PollerEvent &ev) override;

    void HandleEv_Close(LLBC_PollerEvent &ev) override;

    void HandleEv_Monitor(LLBC_PollerEvent &ev) override;

    void HandleEv_TakeOverSession(LLBC_PollerEvent &ev) override;

    /**
     * Add session to poller.
     */
#if LLBC_TARGET_PLATFORM_NON_WIN32
    void AddSession(LLBC_Session *session) override;
#else
    virtual void AddSession(LLBC_Session *session, bool needAddToIocp = true);
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

    /**
     * Remove session from poller.
     */
    void RemoveSession(LLBC_Session *session) override;

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
