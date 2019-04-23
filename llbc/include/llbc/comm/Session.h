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

#ifndef __LLBC_COMM_SESSION_H__
#define __LLBC_COMM_SESSION_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

__LLBC_NS_BEGIN

/**
 * Previous declare some classes.
 */
class LLBC_Packet;
class LLBC_Socket;
class LLBC_IService;
class LLBC_BasePoller;
class LLBC_ProtocolStack;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The session close params class encapsulation.
 */
class LLBC_EXPORT LLBC_SessionCloseInfo
{
public:
    /**
     * Construct a fromSvc=False, errInfo auto create LLBC_SessionCloseParams object.
     */
    LLBC_SessionCloseInfo();

    /**
     * Construct a fromSvc=True LLBC_SessionCloseParams object.
     */
    LLBC_SessionCloseInfo(char *reason);

    /**
     * Construct a fromSvc=False, errInfo specified LLBC_SessionCloseParams object.
     */
    LLBC_SessionCloseInfo(int errNo, int subErrNo);

    ~LLBC_SessionCloseInfo();

public:
    /**
     * Get from service flag.
     * @return bool - the from service flag.
     */
    bool IsFromService() const;

    /**
     * Get close reason.
     * @return const LLBC_String & - the close reason.
     */
    const LLBC_String &GetReason() const;

    /**
     * Get error no.
     * @return int - the errno.
     */
    int GetErrno() const;

    /**
     * Get sub errno.
     * @return int - the sub errno.
     */
    int GetSubErrno() const;

private:
    bool _fromSvc;
    LLBC_String _reason;

    int _errNo;
    int _subErrNo;
};

/**
 * \brief The session class encapsulation.
 */
class LLBC_HIDDEN LLBC_Session
{
public:
    /**
     * Constructor & Destructor.
     */
    LLBC_Session();
    ~LLBC_Session();

public:
    /**
     * Get the session Id.
     * @return int - the session Id.
     */
    int GetId() const;

    /**
     * Set the session Id.
     * @param[in] id - the session Id.
     */
    void SetId(int id);

    /**
     * Get accept session Id.
     * @return int - the accept session Id.
     */
    int GetAcceptId() const;

    /**
     * Set accept session Id.
     * @param[in] acceptId - the accept session Id.
     */
    void SetAcceptId(int acceptId);

    /**
     * Get the socket handle.
     * @return LLBC_SocketHanle - the socket handle.
     */
    LLBC_SocketHandle GetSocketHandle() const;

    /**
     * Get socket.
     * @return (const) LLBC_Socket * - socket.
     */
    LLBC_Socket *GetSocket();
    const LLBC_Socket *GetSocket() const;

    /**
     * Check this session is listen session or not.
     * @return bool - return true if is listen session, otherwise return false.
     */
    bool IsListen() const;

    /**
     * Set socket.
     * Note: Once the socket setting into the session, the session will own the socket.
     * @param[in] socket - socket.
     */
    void SetSocket(LLBC_Socket *socket);

    /**
     * Get service.
     * @return LLBC_IService * - service.
     */
    LLBC_IService *GetService();

    /**
     * Set service.
     * @param[in] svc - service.
     */
    void SetService(LLBC_IService *svc);

    /**
     * Get protocol stack.
     * @return LLBC_ProtocolStack * - protocol stack.
     */
    LLBC_ProtocolStack *GetProtocolStack();

    /**
     * Set protocol stack.
     * @param[in] protoStack - the protocol stack.
     */
    void SetProtocolStack(LLBC_ProtocolStack *protoStack);

    /**
     * Get the poller.
     * @return LLBC_BasePoller * - poller.
     */
    LLBC_BasePoller *GetPoller();

    /**
     * Set the poller.
     * @param[in] poller - poller.
     */
    void SetPoller(LLBC_BasePoller *poller);

public:
    /**
     * @Send packet.
     * @param[in] packet - the packet.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Send(LLBC_Packet *packet);

    /**
     * Send message block.
     * Note: 
     *       No matter method call success or not, method will steal <block> the parameter.
     * @param[in] block - the message block.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Send(LLBC_MessageBlock *block);

public:
    /**
     * Send event handler method, call by poller.
     * @param[in] ol - overlapped structure(WIN32 specified).
     */
#if LLBC_TARGET_PLATFORM_WIN32
    void OnSend(LLBC_POverlapped ol = NULL);
#else
    void OnSend();
#endif // LLBC_TARGET_PLATFORM_WIN32

    /**
     * Receive event handler method, call by poller.
     * @param[in] ol - overlapped structure(WIN3 specified).
     */
#if LLBC_TARGET_PLATFORM_WIN32
    void OnRecv(LLBC_POverlapped ol = NULL);
#else
    void OnRecv();
#endif // LLBC_TARGET_PLATFORM_WIN32

    /**
     * Close event handler method, call by poller or socket.
     * @param[in] ol        - overlapped structure(WIN32 specified).
     * @param[in] closeInfo - session close info, default is NULL, method will auto create close info.
     */
#if LLBC_TARGET_PLATFORM_WIN32
    void OnClose(LLBC_POverlapped ol = NULL, LLBC_SessionCloseInfo *closeInfo = NULL);
#else
    void OnClose(LLBC_SessionCloseInfo *info = NULL);
#endif // LLBC_TARGET_PLATFORM_WIN32

public:
    /**
     * Sent event handler method, call by socket, when has data sent, will call this method.
     * @param[in] len - data length, in bytes.
     */
    void OnSent(size_t len);

    /**
     * Received event handler method, call by socket, when data received, will call this metho.
     * @param[in] block - the data block.
     * @return bool - return false if success, otherwise return false(if failed, this method will perform OnClose() op).
     */
    bool OnRecved(LLBC_MessageBlock *block);

private:
    int _id;
    int _acceptId;

    LLBC_Socket *_socket;
    LLBC_SocketHandle _sockHandle;

    LLBC_IService *_svc;
    LLBC_BasePoller *_poller;

    LLBC_ProtocolStack *_protoStack;
    std::vector<LLBC_Packet *> _recvedPackets;

    int _pollerType;
};

__LLBC_NS_END

#endif // !__LLBC_COMM_SESSION_H__
