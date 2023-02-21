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

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Startup network library.
 * @return int - return 0 if successed, otherwise return -1.
 */
LLBC_EXPORT int LLBC_StartupNetLibrary();

/**
 * Cleanup network library.
 */
LLBC_EXPORT int LLBC_CleanupNetLibrary();

/**
 * Create TCP socket.
 * @return LLBC_SocketHandle - socket handle, if failed, return LLBC_INVALID_SOCKET_HANDLE.
 */
LLBC_EXPORT LLBC_SocketHandle LLBC_CreateTcpSocket();

/**
 * Create overlapped TCP socket. WIN32 specific, If in any non-win32 platform 
 * call this API, will like LLBC_CreateTcpSocket().
 * @return LLBC_SocketHandle - socket handle.
 */
LLBC_EXPORT LLBC_SocketHandle LLBC_CreateTcpSocketEx();

/**
 * Shutdown socket input.
 * @param[in] handle - socket handle.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_ShutdownSocketInput(LLBC_SocketHandle handle);

/**
 * Shutdown socket output.
 * @param[in] handle - socket handle.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_ShutdownSocketOutput(LLBC_SocketHandle handle);

/**
 * Shutdown socket input/output.
 * @param[in] handle - socket handle.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_ShutdownSocketInputOutput(LLBC_SocketHandle handle);

/**
 * Sends data on a connected socket.
 * @param[in] handle - socket handle.
 * @param[in] buf    - buffer containing the data to be transmitted.
 * @param[in] len    - length of data in buf.
 * @param[in] flag   - flags.
 * @return int       - if no error occurs, return the total number bytes sent, otherwise return -1.
 */
LLBC_EXPORT int LLBC_Send(LLBC_SocketHandle handle, const void *buf, int len, int flags);

/**
 * Send data on a connected socket(WIN32 specific).
 * @param[in]  handle         - socket handle.
 * @param[in]  buffers        - pointer to array of LLBC_SockBuf structures.
 * @param[in]  bufferCount    - number of LLBC_SockBuf structures in the buffers.
 * @param[out] numOfBytesSent - pointer to the number of bytes sent by this call if the I/O 
 *                              complete immediately.
 * @param[in]  flags          - flags.
 * @param[in]  ol             - overlapped structure(WIN32 available).
 * @return int - if no error occurs and the send operation has completed immediately, this fun returns 0.
 *               otherwise return -1. see LLBC_GetLastError().
 */
LLBC_EXPORT int LLBC_SendEx(LLBC_SocketHandle handle,
                            LLBC_SockBuf *buffers,
                            ulong bufferCount,
                            ulong_ptr numOfBytesSent,
                            ulong flags,
                            LLBC_POverlapped ol);

/**
 * Receive data from a connected socket.
 * @param[in] handle - socket handle.
 * @param[in] buf    - buffer for incoming data.
 * @param[in] len    - length of buf.
 * @param[in] flags  - flags.
 * @return int - if no error occurs, returns the number of bytes received, if the connection has been closed,
 *               the return value is zero, otherwise return -1, it means error occurred.
 */
LLBC_EXPORT int LLBC_Recv(LLBC_SocketHandle handle, void *buf, int len, int flags);

/**
 * Receives data from a connected socket.
 * @param[in]     handle          - socket handle.
 * @param[in]     buffers         - pointer to array of LLBC_SockBuf structures.
 * @param[in]     bufferCount     - number of LLBC_SockBuf structures in the buffers.
 * @param[out]    numOfBytesRecvd - pointer to the number of bytes received by this call, if the
 *                                 receive operation completes immediately.
 * @param[in/out] flags           - point to flags.
 * @param[in]     ol - overlapped structure.
 */
LLBC_EXPORT int LLBC_RecvEx(LLBC_SocketHandle handle,
                            LLBC_SockBuf *buffers,
                            ulong bufferCount,
                            ulong_ptr numOfBytesRecvd,
                            ulong_ptr flags,
                            LLBC_POverlapped ol);

/**
 * Close socket.
 * @param[in] handle - socket handle.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_CloseSocket(LLBC_SocketHandle handle);

/**
 * Determine given socket is in non-blocking mode or not.
 * Note: 
 *      !!!Only avaliable in Non-WIN32 platform, in WIN3 platform, always return false 
 *      and LLBC_GetLastError() always equal LLBC_ERROR_NOTIMPL.
 * @return bool - if return true, it means this socket working in non-blocking mode,
 *                if return false and LLBC_GetLastError() == 0, it means this socket working in blocking mode.
 *                otherwise LLBC_GetLastError() != 0, means error occurred.
 */
LLBC_EXPORT bool LLBC_IsNonBlocking(LLBC_SocketHandle handle);

/**
 * Set socket to non-blocking.
 * @param[in] handle - socket handle.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_SetNonBlocking(LLBC_SocketHandle handle);

/**
 * Enable socket reusable.
 * @param[in] handle - socket handle.
 * @return int - socket handle.
 */
LLBC_EXPORT int LLBC_EnableAddressReusable(LLBC_SocketHandle handle);

/**
 * Disable socket reusable.
 * @param[in] handle - socket handle.
 * @return int - socket handle.
 */
LLBC_EXPORT int LLBC_DisableAddressReusable(LLBC_SocketHandle handle);

/**
 * Set socket send buffer size, in bytes.
 * @param[in] handle - socket.
 * @param[in] size   - send buffer size.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_SetSendBufSize(LLBC_SocketHandle handle, size_t size);

/**
 * Set socket recv buffer size, in bytes.
 * @param[in] handle - socket.
 * @param[in] size   - recv buffer size.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_SetRecvBufSize(LLBC_SocketHandle handle, size_t size);

/**
 * Get socket name.
 * @param[in] handle - socket handle.
 * @param[out] addr  - storage location for socket address.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_GetSocketName(LLBC_SocketHandle handle, LLBC_SockAddr_IN &addr);

/**
 * Get peer socket name.
 * @param[in] handle - socket handle.
 * @param[out] addr  - storage location for socket address.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_GetPeerSocketName(LLBC_SocketHandle handle, LLBC_SockAddr_IN &addr);

/**
 * Bind socket to specify socket address.
 * @param[in] handle - socket handle.
 * @param[in] addr   - socket address.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_BindToAddress(LLBC_SocketHandle handle, const LLBC_SockAddr_IN &addr);

/**
 * Bind socket to specify ip, port.
 * @param[in] handle - socket handle.
 * @param[in] ip     - ip address.
 * @param[in] port   - port number.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_BindToAddress(LLBC_SocketHandle handle, const char *ip, uint16 port);

/**
 * Listen fo wait client connection.
 * @param[in] handle  - socket handle.
 * @param[in] backlog - maximum length of the queue of pending connections.
 * @rerturn int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_ListenForConnection(LLBC_SocketHandle handle, int backlog);

/**
 * Permits an incoming connection attempt on a socket.
 * @param[in] handle - socket handle.
 * @param[out] addr  - storage location for socket address, can set to nullptr.
 * @return LLBC_SocketHandle - socket handle, if failed, return LLBC_INVALID_SOCKET_HANDLE.
 */
LLBC_EXPORT LLBC_SocketHandle LLBC_AcceptClient(LLBC_SocketHandle handle, LLBC_SockAddr_IN *addr = nullptr);

/**
 * Accepts a new connection, returns the local and remote address, and receives the first block of data sent by the client application.
 * @param[in]  listenSock   - listen socket handle.
 * @param[in]  clientSock   - client socket handle.
 * @param[out] outBuf       - output buffer.
 * @param[in]  outBufLen    - output buffer length.
 * @param[in] localAddrLen  - local address length.
 * @param[in] remoteAddrLen - remote address length.
 * @param[in] ol            - overlapped.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_AcceptClientEx(LLBC_SocketHandle listenSock,
                                    LLBC_SocketHandle clientSock,
                                    void *outBuf,
                                    size_t outBufLen,
                                    size_t localAddrLen,
                                    size_t remoteAddrLen,
                                    LLBC_POverlapped ol);

/**
 * Parse the data obtained from a call to the LLBC_AcceptClientEx() function 
 * and passes the local and remote address to LLBC_SockAddr_IN structure.
 * @param[in]  outBuf        - data buffer.
 * @param[in]  outBufLen     - data buffer len.
 * @param[out] localAddr     - local address.
 * @param[out] remoteAddr    - remote address.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_GetAcceptExSocketAddrs(const void *outBuf,
                                            size_t outBufLen,
                                            LLBC_SockAddr_IN &localAddr,
                                            LLBC_SockAddr_IN &remoteAddr);

/**
 * Establish a connection to a specified socket.
 * @param[in] handle - socket handle.
 * @param[in] addr   - socket address.
 * @return int - if no error occurs, returns zero, return -1 and a specific error set to LLBC_ErrNo.
 */
LLBC_EXPORT int LLBC_ConnectToPeer(LLBC_SocketHandle handle, const LLBC_SockAddr_IN &addr);

/**
 * Establishes a connection to a specified socket, and optionally sends data once the connection is established.
 * @param[in]  handle     - socket handle.
 * @param[in]  addr       - socket address.
 * @param[in]  sendBuf    - a pointer to buffer to be transferred after a connection is established, optional.
 * @param[in]  sendBufLen - the length, in bytes, of data pointed to by the sendBuf parameter.
 * @param[out] bytesSent  - on successful return. this incidates the number of bytes that were sent after the 
 *                          connection was established.
 * @param[in]  ol         - overlapped.
 * @return int - return 0 if successful, otherwise return -1.
 */
LLBC_EXPORT int LLBC_ConnectToPeerEx(LLBC_SocketHandle handle,
                                     const LLBC_SockAddr_IN &addr,
                                     const void *sendBuf,
                                     size_t sendBufLen,
                                     size_t *bytesSent,
                                     LLBC_POverlapped ol);

/**
 * The function receives a socket option.
 * @param[in] handle  - socket handle.
 * @param[in] level   - socket level, eg: SOL_TCP, SOL_SOCKET.
 * @param[in] optname - socket option name, eg: SOL_KEEPALIVE.
 * @param[in] optval  - pointer to the buffer in which the value for the requested option is to be returned.
 * @param[in/out] len - pointer the size of the optval buffer.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_GetSocketOption(LLBC_SocketHandle handle,
                                     int level,
                                     int optname,
                                     void *optval,
                                     LLBC_SocketLen *len);

/**
 * Set socket option.
 * @param[in] handle  - socket handle.
 * @param[in] level   - socket level, eg: SOL_TCP, SOL_SOCKET.
 * @param[in] optname - socket option name, eg: SOL_KEEPALIVE.
 * @param[in] optval  - pointer to the buffer in which the value for the requested option is supplied.
 * @param[in] len     - size of the optval buffer.
 * @return int - if no error occurs, return zero, otherwise return -1.
 */
LLBC_EXPORT int LLBC_SetSocketOption(LLBC_SocketHandle handle,
                                     int level,
                                     int optname,
                                     const void *optval,
                                     LLBC_SocketLen len);

__LLBC_NS_END
