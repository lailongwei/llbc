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

#include "llbc/core/Core.h"

__LLBC_NS_BEGIN

/**
 * Previous declare some classes.
 */
class LLBC_Session;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The socket class encapsulation.
 */
class LLBC_EXPORT LLBC_Socket
{
public:
    /**
     * Parameter constructor, construct socket object.
     * @param[in] handle - socket handle, if not specific, auto create new socket handler in internal.
     */
    explicit LLBC_Socket(LLBC_SocketHandle handle = LLBC_INVALID_SOCKET_HANDLE);

    /**
     * Destructor.
     */
    ~LLBC_Socket();

public:
    /**
     * Set the session.
     * @param[in] session - the session.
     */
    void SetSession(LLBC_Session *session);

    #if LLBC_CFG_COMM_SESSION_RECV_BUF_USE_OBJ_POOL
    /**
     * Set message block pool instance.
     * @param[in] msgBlockPoolInst - the message block pool instance.
     */
    void SetMsgBlockPoolInst(LLBC_ObjectPoolInst<LLBC_MessageBlock> *msgBlockPoolInst);
    #endif // LLBC_CFG_COMM_SESSION_RECV_BUF_USE_OBJ_POOL

    /**
     * Get the poller type.
     * @return int - the poller type.
     */
    int GetPollerType() const;

    /**
     * Set the poller type.
     * @param[in] type - the poller type.
     */
    void SetPollerType(int type);

public:
    /**
     * Get socket handle.
     * @return LLBC_SocketHandle - socket handle.
     */
    LLBC_SocketHandle Handle();

    /**
     * Shutdown socket input.
     * @return int - return 0 if success, otherwise return -1.
     */
    int ShutdownInput();
    /**
     * Shutdown socket output.
     * @return int - return 0 if success, otherwise reutrn -1.
     */
    int ShutdownOutput();
    /**
     * Shutdown socket input & output.
     * @return int - return 0 if success, otherwise return -1.
     */
    int ShutdownInputOutput();

    /**
     * Close socket.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Close();

    /**
     * If the socket was closed, return true, otherwise return false.
     * @return bool - closed flag.
     */
    bool IsClosed() const;

    /**
     * Implement bool operator.
     */
    operator bool () const;

    /**
     * Implement ! operator.
     */
    bool operator ! () const;

public:
    /**
     * Enable address reusble option.
     * @return int - return 0 if success, otherwise return -1.
     */
    int EnableAddressReusable();

    /**
     * Disable address reusable option.
     * @return int - return 0 if success, otherwise return -1.
     */
    int DisableAddressReusable();

    /**
     * Check the socket blocking flag.
     * @return bool - return true if is non-blocking, 
     *                return false and LLBC_GetLastError() == 0if the socket is blocking,
     *                otherwise error occurred(LLBC_GetLastError() != 0)
     */
    bool IsNonBlocking() const;

    /**
     * Check socket no-delay option.
     * @return bool - return true if enabled no-delay option, otherwise return false(included error occurred).
     */
    bool IsNoDelay() const;

    /**
     * Set socket no-delay option.
     * @param[in] noDelay - no-delay option.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetNoDelay(bool noDelay);

    /**
     * Set socket to non-blocking.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetNonBlocking();

    /**
     * Get send buffer size.
     * @return size_t - the send buffer size.
     */
    size_t GetSendBufSize() const;

    /**
     * Set send buffer size.
     * @param[in] size - buffer size, in bytes.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetSendBufSize(size_t size);

    /**
     * Get recv buffer size.
     * @return size_t - the recv buffer size.
     */
    size_t GetRecvBufSize() const;

    /**
     * Set recv buffer size.
     * @param[in] size - buffer size, in bytes.
     * @return int - return 0 if succerss, otherwise return -1.
     */
    int SetRecvBufSize(size_t size);

    /**
     * Get socket option.
     * @param[in] level      - socket level, eg: SOL_TCP, SOL_SOCKET.
     * @param[in] optname    - the socket option name, eg: SO_KEEPALIVE.
     * @param[in] optval     - the buffer, option value will store in this buffer.
     * @param[in/out] optlen - pointer to the size of the optval buffer.
     * @return int - return 0 if success, otherwise return -1.
     */
    int GetOption(int level, int optname, void *optval, LLBC_SocketLen *optlen) const;

    /**
     * Set socket option.
     * @param[in] level   - socket level, eg: SOL_TCP, IPPROTO_TCP, IPPROTO_IP, ....
     * @param[in] optname - the socket option name, eg: SO_KEEPALIVE.
     * @param[in] optval  - the option value buffer.
     * @param[in] optlen  - the option value length.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetOption(int level, int optname, const void *optval, LLBC_SocketLen optlen);

     /**
     * Get max packet size.
     * @return size_t - the max packet size.
     */
    size_t GetMaxPacketSize() const;

    /**
     * Set max packet size.
     * @param[in] size - packet size, in bytes.
     * @return int - return 0 if succerss, otherwise return -1.
     */
    int SetMaxPacketSize(size_t size);

public:
    /**
     * Bind current socket to specific ip address and port.
     * @param[in] ip   - ip address.
     * @param[in] port - port number.
     * @param[in] addr - the socket address.
     * @return int - return 0 if success, otherwise return -1.
     */
    int BindTo(const char *ip, uint16 port);
    int BindTo(const LLBC_SockAddr_IN &addr);

    /**
     * places the socket a state where it is listening for an incoming connection.
     * @param[in] backlog - maximum length of the queue of pending connections.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Listen(int backlog = LLBC_CFG_OS_DFT_BACKLOG_SIZE);

    /**
     * Determine this socket is listen socket or not.
     * @return bool - return true if is listen type socket, otherwise return false.
     */
    bool IsListen() const;

    /**
     * Permits a incoming connection attempt on the socket.
     * @return LLBC_Socket * - the new socket, if error occurred, return nullptr.
     */
    LLBC_Socket *Accept();

#if LLBC_TARGET_PLATFORM_WIN32
    /**
     * WIN32 specific socket method, accept a new connection(asynchronous).
     */
    int AcceptEx(LLBC_SocketHandle listenSock,
                 LLBC_SocketHandle acceptSock,
                 LLBC_POverlapped ol);
#endif // LLBC_TARGET_PLATFORM_WIN32

    /**
     * Establishes a connection to specified peer address.
     * @param[in] addr - peer address.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Connect(const LLBC_SockAddr_IN &addr);

#if LLBC_TARGET_PLATFORM_WIN32
    /**
     * WIN32 specific socket method, connect to peer(asynchronous).
     * @param[in] addr - peer address.
     * @param[in] ol   - overlapped.
     * @return int - return 0 if success, otherwise return -1.
     */
    int ConnectEx(const LLBC_SockAddr_IN &addr,
                  LLBC_POverlapped ol);
#endif // LLBC_TARGET_PLATFORM_WIN32

#if LLBC_TARGET_PLATFORM_WIN32
    /**
     * Associate the socket of an input/output completion port.
     * @param[in] iocp - the input/output completion port.
     * @return int - return 0 if success, otherwise return -1.
     */
    int AttachToIocp(LLBC_IocpHandle iocp);
#endif // LLBC_TARGET_PLATFORM_WIN32

public:
    /**
     * Send data on a connected socket.
     * @param[in] buf - data buffer.
     * @param[in] len - data length.
     * @return int - return the total number of bytes sent, if error occurred, return -1.
     */
    int Send(const char *buf, int len);

    /**
     * Asynchronous send data, data will append the socket's send queue.
     * @param[in] buf - data buffer.
     * @param[in] len - length of buf.
     * @return int - return 0 return success, otherwise return -1.
     */
    int AsyncSend(const char *buf, int len);

    /**
     * Asynchronous send data, data will append the socket's send queue.
     * Note: 
     *       No matter method call success or not, method will steal <block> the parameter.
     * @param[in] block - data block.
     * @return int - return 0 if success, otherwise return -1.
     */
    int AsyncSend(LLBC_MessageBlock *block);

    /**
     * Check the socket exist no send data or not.
     * @return bool - return true it means exist data not send.
     */
    bool IsExistNoSendData() const;

    /**
     * Get will send message buffer.
     * @return const LLBC_MessageBuffer & - the will send message buffer.
     */
    const LLBC_MessageBuffer &GetWillSendBuffer() const;

    /**
     * Receive data from a connected socket.
     * @param[in] buf - buffer for the incoming data.
     * @param[in] len - length of buf.
     * @return int - returns the number of bytes received, if error occurred, return -1.
     */
    int Recv(char *buf, int len);

public:
    /**
     * Update the socket's local address.
     * @return int - return 0 if success, otherwise return -1.
     */
    int UpdateLocalAddress();

    /**
     * Get current socket local address.
     * @return const LLBC_SockAddr_IN & - the local address, always success.
     */
    const LLBC_SockAddr_IN &GetLocalAddress() const;

    /**
     * Get local hostent name.
     * @return LLBC_String - host name.
     */
    LLBC_String GetLocalHostname() const;

    /**
     * Get local port.
     * @return uint16 - port number.
     */
    uint16 GetLocalPort() const;

    /**
     * Update the socket's peer address.
     * @return int - return 0 if success, otherwise return -1.
     */
    int UpdatePeerAddress();

    /**
     * Get peer address.
     * @return const LLBC_SockAddr_IN - the peer address, always success.
     */
    const LLBC_SockAddr_IN &GetPeerAddress() const;

    /**
     * Get peer hostent name.
     * @return LLBC_String - host name.
     */
    LLBC_String GetPeerHostname() const;

    /**
     * Get peer port.
     * @return uint16 - port number.
     */
    uint16 GetPeerPort() const;

public:
    /**
     * Get socket pending error.
     * @param[out] pendingError - the pending error value.
     * @return int - return 0 if success, otherwise return -1.
     *               
     */
    int GetPendingError(int &pendingError);

public:
    /**
     * Event handle function, if socket in event trigger mode, must call this function to send data.
     * @param[in] ol - overlapped, available in WIN32 platform and IOCP poller model.
     */
#if LLBC_TARGET_PLATFORM_WIN32
    void OnSend(LLBC_POverlapped ol);
#else
    void OnSend();
#endif

    /**
     * Event handle function, if socket in event trigger mode, must call this function to recv data.
     * @param[in] ol        - overlapped, available in WIN32 platform and IOCP poller model. 
     * @param[in] bytesRecv - number of bytes received, available in WIN32 platform and IOCP poller model.
     */
#if LLBC_TARGET_PLATFORM_WIN32
    void OnRecv(LLBC_POverlapped ol);
#else
    void OnRecv();
#endif // LLBC_TARGET_PLATFORM_WIN32

    /**
     * Event handle function, if socket in event trigger mode, must call this function close socket.
     * @param[in] ol - overlapped, available in WIN32 platform and IOCP poller model.
     */
#if LLBC_TARGET_PLATFORM_WIN32
    void OnClose(LLBC_POverlapped ol = nullptr);
#else
    void OnClose();
#endif // LLBC_TARGET_PLATFORM_WIN32

public:
#if LLBC_TARGET_PLATFORM_WIN32
    /**
     * Get the overlapped group.
     * @return LLBC_POverlappedGroup - overlapped group.
     */
    LLBC_OverlappedGroup &GetOverlappedGroup();
    const LLBC_OverlappedGroup &GetOverlappedGroup() const;

    /**
     * About all overlapped operate methods.
     */
    void InsertOverlapped(LLBC_POverlapped ol);
    void RemoveOverlapped(LLBC_POverlapped ol);
    void DeleteOverlapped(LLBC_POverlapped ol);
    void DeleteAllOverlappeds();

    /**
     * Get Iocp sending data size, only-available in <IocpPoller> poller mode.
     * @return size_t - the sending data size.
     */
    size_t GetIocpSendingDataSize() const;
#endif // LLBC_TARGET_PLATFORM_WIN32

private:
#if LLBC_TARGET_PLATFORM_WIN32
    /**
     * WIN32 specific friend class: IocpPoller.
     *  Access method list:
     *      PostAsyncAccept().
     *      PostZeroWSARecv().
     */
    friend class LLBC_IocpPoller;

    /**
     * WIN32 platform specified method, post asynchronous AcceptEx() call.
     */
    int PostAsyncAccept();

    /**
     * WIN32 platform specified method, post Zero-WSARecv request on the socket.
     */
    int PostZeroWSARecv();
#endif // LLBC_TARGET_PLATFORM_WIN32

private:
    LLBC_SocketHandle _handle;

    LLBC_Session *_session;
    int _pollerType;

    bool _listenSocket;
    LLBC_SockAddr_IN _peerAddr;
    LLBC_SockAddr_IN _localAddr;

    LLBC_MessageBuffer _willSend;
    size_t _maxPacketSize;

#if LLBC_TARGET_PLATFORM_WIN32
    bool _nonBlocking;
    LLBC_OverlappedGroup _olGroup;

    size_t _iocpSendingDataSize;
#endif // LLBC_TARGET_PLATFORM_WIN32

#if LLBC_CFG_COMM_SESSION_RECV_BUF_USE_OBJ_POOL
    LLBC_ObjectPoolInst<LLBC_MessageBlock> *_msgBlockPoolInst;
#endif // LLBC_CFG_COMM_SESSION_RECV_BUF_USE_OBJ_POOL

private:
#if LLBC_TARGET_PLATFORM_WIN32
    static char _acceptExBuf[(sizeof(LLBC_SockAddr_IN) + 16) * 2];
#endif // LLBC_TARGET_PLATFORM_WIN32
};

__LLBC_NS_END
