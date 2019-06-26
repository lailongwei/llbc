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

#ifndef __LLBC_COMM_POLLER_EVENT_H__
#define __LLBC_COMM_POLLER_EVENT_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

__LLBC_NS_BEGIN

/**
 * Previous declare some classes.
 */
class LLBC_Packet;
class LLBC_Socket;
class LLBC_Session;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The poller event structure encapsulation.
 */
struct LLBC_HIDDEN LLBC_PollerEvent
{
    enum Type
    {
        // Add socket(listen type or connected type socket, generate by Service layer.
        AddSock,
        // Asynchronous connect request, generate by Service layer.
        AsyncConn,
        // Send packet request, generate by Service layer.
        Send,
        // Close session request, generate by Service layer.
        Close,
        // Monitor event, only Iocp/Epoll poller available, generate by PollerMonitor thread.
        Monitor,
        // Take over session request, once poller found it can't process the new session,
        // poller will create this event and post to appropriate brother.
        TakeOverSession,
        // Control protocol stack, generate by Service layer.
        CtrlProtocolStack,

        // Sentinel.
        End
    };

    Type type;
    int sessionId;
    LLBC_SockAddr_IN peerAddr;
    union
    {
        LLBC_Socket *socket;
        LLBC_Packet *packet;
        LLBC_Session *session;
        char *monitorEv;
        char *closeReason;
        struct
        {
            int ctrlType;
            void *ctrlData;
            size_t ctrlDataLen;
        } protocolStackCtrlInfo;
    } un;
};

/**
 * \brief The poller event util class encapsulation.
 *        It's a utils class, use to simply create poller event.
 */
class LLBC_HIDDEN LLBC_PollerEvUtil
{
public:
    /**
     * Build AddSock event.
     */
    static LLBC_MessageBlock *BuildAddSockEv(int sessionId, LLBC_Socket *sock);
    
    /**
     * Build Async-Conn event.
     */
    static LLBC_MessageBlock *BuildAsyncConnEv(int sessionId, 
                                               const LLBC_SockAddr_IN &peerAddr);

    /**
     * Build Send event.
     */
    static LLBC_MessageBlock *BuildSendEv(LLBC_Packet *packet);

    /**
     * Build close event.
     */
    static LLBC_MessageBlock *BuildCloseEv(int sessionId, const char *reason);

    /**
     * Build Iocp monitor event.
     */
#if LLBC_TARGET_PLATFORM_WIN32
    static LLBC_MessageBlock *BuildIocpMonitorEv(int ret, LLBC_POverlapped ol, int errNo, int subErrNo);
#endif
    /**
     * Build Epoll monitor event.
     */
#if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID
    static LLBC_MessageBlock *BuildEpollMonitorEv(const LLBC_EpollEvent *evs, int count);
#endif

    /**
     * Build take over session event.
     */
    static LLBC_MessageBlock *BuildTakeOverSessionEv(LLBC_Session *session);

    /**
     * Build take over socket event(only available in WIN32 platform).
     */
#if LLBC_TARGET_PLATFORM_WIN32
    static LLBC_MessageBlock *BuildTakeOverSocketEv(int sessionId, LLBC_Socket *sock);
#endif

    /**
     * Build control protocol stack event.
     */
    static LLBC_MessageBlock *BuildCtrlProtocolStackEv(int sessionId, int ctrlType, const LLBC_Variant &ctrlData);

public:
    /**
     * Destroy poller event.
     */
    static void DestroyEv(LLBC_PollerEvent &ev);
    static void DestroyEv(LLBC_MessageBlock *block);
};

__LLBC_NS_END

#endif // !__LLBC_COMM_POLLER_EVENT_H__

