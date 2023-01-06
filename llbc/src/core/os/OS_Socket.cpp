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


#include "llbc/common/Export.h"

#if LLBC_TARGET_PLATFORM_NON_WIN32
 #include <fcntl.h>
#endif // Non-Win32

#include "llbc/core/os/OS_Socket.h"

// Disable some warnings(on windows platform)
#if LLBC_TARGET_PLATFORM_WIN32
 #pragma warning(push)
 #pragma warning(disable: 4996)
#endif // Win32

__LLBC_INTERNAL_NS_BEGIN

#if LLBC_TARGET_PLATFORM_WIN32
static LPFN_ACCEPTEX __g_AcceptExProc = nullptr;
static LPFN_CONNECTEX __g_ConnectExProc = nullptr;
static LPFN_GETACCEPTEXSOCKADDRS __g_GetAcceptExSockAddrs = nullptr;
#endif // LLBC_TARGET_PLATFORM_WIN32

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

int LLBC_StartupNetLibrary()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    return LLBC_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    WSADATA wsaData;
    WORD version = MAKEWORD(2, 2);
    int wsaInitRet = ::WSAStartup(version, &wsaData);
    if (wsaInitRet != 0)
    {
        switch (wsaInitRet)
        {
        case WSASYSNOTREADY:
            LLBC_SetLastError(LLBC_ERROR_WSA_SYSNOTREADY);
            break;

        case WSAVERNOTSUPPORTED:
            LLBC_SetLastError(LLBC_ERROR_WSA_VERNOTSUPPORTED);
            break;

        case WSAEINPROGRESS:
            LLBC_SetLastError(LLBC_ERROR_WSA_EINPROGRESS);
            break;

        case WSAEPROCLIM:
            LLBC_SetLastError(LLBC_ERROR_WSA_EPROCLIM);
            break;

        default:
            LLBC_SetLastError(LLBC_ERROR_UNKNOWN);
            break;
        }
        return LLBC_FAILED;
    }

    DWORD bytesReturn = 0;
    GUID acceptExGuid = WSAID_ACCEPTEX;
    LLBC_SocketHandle sock = LLBC_CreateTcpSocket();
    int ctlRet = ::WSAIoctl(sock,
                            SIO_GET_EXTENSION_FUNCTION_POINTER,
                            &acceptExGuid,
                            sizeof(acceptExGuid),
                            &(LLBC_INTERNAL_NS __g_AcceptExProc),
                            sizeof(LPFN_ACCEPTEX),
                            &bytesReturn,
                            nullptr,
                            nullptr);
    if (ctlRet != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_NETAPI);

        LLBC_CloseSocket(sock);
        LLBC_CleanupNetLibrary();

        return LLBC_FAILED;
    }

    GUID connectExGuid = WSAID_CONNECTEX;
    ctlRet = ::WSAIoctl(sock,
                        SIO_GET_EXTENSION_FUNCTION_POINTER,
                        &connectExGuid,
                        sizeof(connectExGuid),
                        &(LLBC_INTERNAL_NS __g_ConnectExProc),
                        sizeof(LPFN_CONNECTEX),
                        &bytesReturn,
                        nullptr,
                        nullptr);
    if (ctlRet != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_NETAPI);

        LLBC_CloseSocket(sock);
        LLBC_CleanupNetLibrary();

        return LLBC_FAILED;
    }

    GUID getAcceptExSockAddrsGuid = WSAID_GETACCEPTEXSOCKADDRS;
    ctlRet = ::WSAIoctl(sock,
                        SIO_GET_EXTENSION_FUNCTION_POINTER,
                        &getAcceptExSockAddrsGuid,
                        sizeof(getAcceptExSockAddrsGuid),
                        &(LLBC_INTERNAL_NS __g_GetAcceptExSockAddrs),
                        sizeof(LPFN_GETACCEPTEXSOCKADDRS),
                        &bytesReturn,
                        nullptr,
                        nullptr);
    if (ctlRet != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_NETAPI);

        LLBC_CloseSocket(sock);
        LLBC_CleanupNetLibrary();

        return LLBC_FAILED;
    }

    LLBC_CloseSocket(sock);

    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_CleanupNetLibrary()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    return LLBC_OK;
#else
    LLBC_INTERNAL_NS __g_AcceptExProc = nullptr;
    LLBC_INTERNAL_NS __g_ConnectExProc = nullptr;
    LLBC_INTERNAL_NS __g_GetAcceptExSockAddrs = nullptr;

    if (::WSACleanup() == SOCKET_ERROR)
    {
        LLBC_SetLastError(LLBC_ERROR_NETAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif
}

LLBC_SocketHandle LLBC_CreateTcpSocket()
{
    LLBC_SocketHandle handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (handle == -1)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
    }

    return handle;
#else // LLBC_TARGET_PLATFORM_WIN32
    if (handle == INVALID_SOCKET)
    {
        LLBC_SetLastError(LLBC_ERROR_NETAPI);
    }

    return handle;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

LLBC_SocketHandle LLBC_CreateTcpSocketEx()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    return LLBC_CreateTcpSocket();
#else // LLBC_TARGET_PLATFORM_NON_WIN32
    LLBC_SocketHandle handle = ::WSASocketA(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
    if (handle == LLBC_INVALID_SOCKET_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_NETAPI);
    }

    return handle;
#endif // LLBC_TARGET_PLATFORM_WIN32
}

int LLBC_ShutdownSocketInput(LLBC_SocketHandle handle)
{
    if (UNLIKELY(handle == LLBC_INVALID_SOCKET_HANDLE))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (shutdown(handle, SHUT_RD) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    if (shutdown(handle, SD_RECEIVE) == SOCKET_ERROR)
    {
        LLBC_SetLastError(LLBC_ERROR_NETAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_ShutdownSocketOutput(LLBC_SocketHandle handle)
{
    if (UNLIKELY(handle == LLBC_INVALID_SOCKET_HANDLE))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (shutdown(handle, SHUT_WR) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    if (shutdown(handle, SD_SEND) == SOCKET_ERROR)
    {
        LLBC_SetLastError(LLBC_ERROR_NETAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_ShutdownSocketInputOutput(LLBC_SocketHandle handle)
{
    if (UNLIKELY(handle == LLBC_INVALID_SOCKET_HANDLE))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (shutdown(handle, SHUT_RDWR) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }
    
    return LLBC_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    if (shutdown(handle, SD_BOTH) == SOCKET_ERROR)
    {
        LLBC_SetLastError(LLBC_ERROR_NETAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_Send(LLBC_SocketHandle handle, const void *buf, int len, int flags)
{
#if LLBC_TARGET_PLATFORM_NON_IPHONE
    int ret = 0;
#else // iPhone
    ssize_t ret = 0;
#endif
    while ((ret = send(handle,
                       reinterpret_cast<const char *>(buf), 
                       len, 
                       flags)) < 0 && errno == EINTR);
#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (ret == -1)
    {
        if (errno == EWOULDBLOCK)
        {
            LLBC_SetLastError(LLBC_ERROR_WBLOCK);
            return LLBC_FAILED;
        }
        else if (errno == EAGAIN)
        {
            LLBC_SetLastError(LLBC_ERROR_AGAIN);
            return LLBC_FAILED;
        }

        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_IPHONE
    return ret;
#else // iPhone
    return static_cast<int>(ret);
#endif // LLBC_TARGET_PLATFORM_NON_IPHONE
#else // LLBC_TARGET_PLATFORM_WIN32
    if (ret == SOCKET_ERROR)
    {
        if (::WSAGetLastError() == WSAEWOULDBLOCK)
        {
            LLBC_SetLastError(LLBC_ERROR_WBLOCK);
            return LLBC_FAILED;
        }

        LLBC_SetLastError(LLBC_ERROR_NETAPI);
        return LLBC_FAILED;
    }

    return ret;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_SendEx(LLBC_SocketHandle handle,
                LLBC_SockBuf *buffers,
                ulong bufferCount,
                ulong_ptr numOfBytesSent,
                ulong flags,
                LLBC_POverlapped ol)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
#else // LLBC_TARGET_PLATFORM_WIN32
    int ret = ::WSASend(handle, buffers, bufferCount, numOfBytesSent, flags, ol, nullptr);
    if (ret == SOCKET_ERROR)
    {
        int netLastError = ::WSAGetLastError();
        if (netLastError == WSA_IO_PENDING)
        {
            LLBC_SetLastError(LLBC_ERROR_PENDING);
        }
        else if (netLastError == WSAEWOULDBLOCK)
        {
            LLBC_SetLastError(LLBC_ERROR_WBLOCK);
        }
        else
        {
            LLBC_SetLastError(LLBC_ERROR_NETAPI);
        }

        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_Recv(LLBC_SocketHandle handle, void *buf, int len, int flags)
{
#if LLBC_TARGET_PLATFORM_NON_IPHONE
    int ret = 0;
#else // iPHone
    ssize_t ret = 0;
#endif // LLBC_TARGET_PLATFORM_NON_IPHONE
    while ((ret = recv(handle,
                       reinterpret_cast<char *>(buf), 
                       len, 
                       flags)) < 0 && errno == EINTR);
#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (ret == -1)
    {
        if (errno == EWOULDBLOCK)
        {
            LLBC_SetLastError(LLBC_ERROR_WBLOCK);
            return LLBC_FAILED;
        }
        else if (errno == EAGAIN)
        {
            LLBC_SetLastError(LLBC_ERROR_AGAIN);
            return LLBC_FAILED;
        }

        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_IPHONE
    return ret;
#else // iPHone
    return static_cast<int>(ret);
#endif // LLBC_TARGET_PLATFORM_NON_IPHONE
#else // LLBC_TARGET_PLATFORM_WIN32
    if (ret == SOCKET_ERROR)
    {
        if (::WSAGetLastError() == WSAEWOULDBLOCK)
        {
            LLBC_SetLastError(LLBC_ERROR_WBLOCK);
            return LLBC_FAILED;
        }

        LLBC_SetLastError(LLBC_ERROR_NETAPI);
        return LLBC_FAILED;
    }

    return ret;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_RecvEx(LLBC_SocketHandle handle,
                LLBC_SockBuf *buffers,
                ulong bufferCount,
                ulong_ptr numOfBytesRecvd,
                ulong_ptr flags,
                LLBC_POverlapped ol)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
#else // LLBC_TARGET_PLATFORM_WIN32
    int ret = ::WSARecv(handle, buffers, bufferCount, numOfBytesRecvd, flags, ol, nullptr);
    if (ret == SOCKET_ERROR)
    {
        int netLastError = ::WSAGetLastError();
        if (netLastError == WSA_IO_PENDING)
            LLBC_SetLastError(LLBC_ERROR_PENDING);
        else if (netLastError == WSAEWOULDBLOCK)
            LLBC_SetLastError(LLBC_ERROR_WBLOCK);
        else
            LLBC_SetLastError(LLBC_ERROR_NETAPI);

        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_CloseSocket(LLBC_SocketHandle handle)
{
    if (UNLIKELY(handle == LLBC_INVALID_SOCKET_HANDLE))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (close(handle) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    if (closesocket(handle) == SOCKET_ERROR)
    {
        LLBC_SetLastError(LLBC_ERROR_NETAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

bool LLBC_IsNonBlocking(LLBC_SocketHandle handle)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    int opts = 0;
    if ((opts = fcntl(handle, F_GETFL)) == -1)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return false;
    }

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    return !!(opts & O_NONBLOCK);
#else // LLBC_TARGET_PLATFORM_WIN32
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return false;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_SetNonBlocking(LLBC_SocketHandle handle)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    int opts = 0;
    if ((opts = fcntl(handle, F_GETFL)) == -1)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    opts |= O_NONBLOCK;
    if (fcntl(handle, F_SETFL, opts) == -1)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    unsigned long arg = 1;
    if (ioctlsocket(handle, FIONBIO, &arg) == SOCKET_ERROR)
    {
        LLBC_SetLastError(LLBC_ERROR_NETAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_EnableAddressReusable(LLBC_SocketHandle handle)
{
    int reuse = 1;
#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (setsockopt(handle, SOL_SOCKET, 
        SO_REUSEADDR, reinterpret_cast<const char *>(&reuse), sizeof(int))!= 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    if (setsockopt(handle, SOL_SOCKET, 
        SO_REUSEADDR, reinterpret_cast<const char *>(&reuse), sizeof(int)) == SOCKET_ERROR)
    {
        LLBC_SetLastError(LLBC_ERROR_NETAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_DisableAddressReusable(LLBC_SocketHandle handle)
{
    int reuse = 0;
#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (::setsockopt(handle, SOL_SOCKET, 
        SO_REUSEADDR, reinterpret_cast<const char *>(&reuse), sizeof(int))!= 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    if (setsockopt(handle, SOL_SOCKET, 
        SO_REUSEADDR, reinterpret_cast<const char *>(&reuse), sizeof(int)) == SOCKET_ERROR)
    {
        LLBC_SetLastError(LLBC_ERROR_NETAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_SetSendBufSize(LLBC_SocketHandle handle, size_t size)
{
    if (size <= 0)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (setsockopt(handle, SOL_SOCKET, SO_SNDBUF, 
        reinterpret_cast<const char *>(&size), sizeof(int)) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    if (setsockopt(handle, SOL_SOCKET, SO_SNDBUF, 
        reinterpret_cast<const char *>(&size), sizeof(int)) == SOCKET_ERROR)
    {
        LLBC_SetLastError(LLBC_ERROR_NETAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_SetRecvBufSize(LLBC_SocketHandle handle, size_t size)
{
    if (size <= 0)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (::setsockopt(handle, SOL_SOCKET, 
        SO_RCVBUF, reinterpret_cast<char *>(&size), sizeof(int)) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    if (setsockopt(handle, SOL_SOCKET, 
        SO_RCVBUF, reinterpret_cast<char *>(&size), sizeof(int)) == SOCKET_ERROR)
    {
        LLBC_SetLastError(LLBC_ERROR_NETAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif //LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_GetSocketName(LLBC_SocketHandle handle, LLBC_SockAddr_IN &addr)
{
    struct sockaddr_in inAddr;
    LLBC_SocketLen len = sizeof(struct sockaddr_in);

#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (getsockname(handle, reinterpret_cast<struct sockaddr *>(&inAddr), &len) == -1)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }
#else // LLBC_TARGET_PLATFORM_WIN32
    if (getsockname(handle, reinterpret_cast<struct sockaddr *>(&inAddr), &len) == SOCKET_ERROR)
    {
        LLBC_SetLastError(LLBC_ERROR_NETAPI);
        return LLBC_FAILED;
    }
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

    return addr.FromOSDataType(&inAddr);
}

int LLBC_GetPeerSocketName(LLBC_SocketHandle handle, LLBC_SockAddr_IN &addr)
{
    struct sockaddr_in inAddr;
    LLBC_SocketLen len = sizeof(struct sockaddr_in);

#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (getpeername(handle, reinterpret_cast<struct sockaddr *>(&inAddr), &len) == -1)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }
#else // LLBC_TARGET_PLATFORM_WIN32
    if (getpeername(handle, reinterpret_cast<struct sockaddr *>(&inAddr), &len) == SOCKET_ERROR)
    {
        LLBC_SetLastError(LLBC_ERROR_NETAPI);
        return LLBC_FAILED;
    }
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

    return addr.FromOSDataType(&inAddr);
}

int LLBC_BindToAddress(LLBC_SocketHandle handle, const LLBC_SockAddr_IN &addr)
{
    return LLBC_BindToAddress(handle, addr.GetIpAsString().c_str(), addr.GetPort());
}

int LLBC_BindToAddress(LLBC_SocketHandle handle, const char *ip, uint16 port)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (ip)
    {
        addr.sin_addr.s_addr = inet_addr(ip);
    }
    else
    {
        addr.sin_addr.s_addr = INADDR_ANY;
    }

    if (bind(handle, reinterpret_cast<struct sockaddr *>(&addr), sizeof(struct sockaddr_in)) == -1)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    if (ip)
    {
        addr.sin_addr.S_un.S_addr = inet_addr(ip);
    }
    else
    {
        addr.sin_addr.S_un.S_addr = INADDR_ANY;
    }

    if (bind(handle, reinterpret_cast<struct sockaddr *>(&addr), sizeof(struct sockaddr_in)) == SOCKET_ERROR)
    {
        LLBC_SetLastError(LLBC_ERROR_NETAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_ListenForConnection(LLBC_SocketHandle handle, int backlog)
{
    if (backlog <= 0)
    {
        backlog = LLBC_CFG_OS_DFT_BACKLOG_SIZE;
    }
#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (listen(handle, backlog) == -1)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    if (listen(handle, backlog) == SOCKET_ERROR)
    {
        LLBC_SetLastError(LLBC_ERROR_NETAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

LLBC_SocketHandle LLBC_AcceptClient(LLBC_SocketHandle handle, LLBC_SockAddr_IN *addr)
{
    struct sockaddr_in inAddr;
    LLBC_SocketLen len = sizeof(struct sockaddr_in);

    LLBC_SocketHandle clientHandle = LLBC_INVALID_SOCKET_HANDLE;
    if ((clientHandle = accept(
        handle, reinterpret_cast<struct sockaddr *>(&inAddr), &len)) == LLBC_INVALID_SOCKET_HANDLE)
    {
#if LLBC_TARGET_PLATFORM_NON_WIN32
        LLBC_SetLastError(LLBC_ERROR_CLIB);
#else // LLBC_TARGET_PLATFORM_WIN32
        LLBC_SetLastError(LLBC_ERROR_NETAPI);
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

        return clientHandle;
    }

    if (addr)
    {
        addr->FromOSDataType(&inAddr);
    }

    return clientHandle;
}

int LLBC_AcceptClientEx(LLBC_SocketHandle listenSock,
                        LLBC_SocketHandle clientSock,
                        void * outBuf,
                        size_t outBufLen,
                        size_t localAddrLen,
                        size_t remoteAddrLen,
                        LLBC_POverlapped ol)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
#else // LLBC_TARGET_PLATFORM_WIN32
    DWORD bytesRecv = 0;
    BOOL ret = (*LLBC_INTERNAL_NS __g_AcceptExProc)(listenSock,
                                                    clientSock,
                                                    outBuf,
                                                    static_cast<DWORD>(outBufLen),
                                                    static_cast<DWORD>(localAddrLen),
                                                    static_cast<DWORD>(remoteAddrLen),
                                                    &bytesRecv,
                                                    ol);
    if (ret == FALSE)
    {
        if (::WSAGetLastError() == WSA_IO_PENDING)
        {
            LLBC_SetLastError(LLBC_ERROR_PENDING);
        }
        else
        {
            LLBC_SetLastError(LLBC_ERROR_NETAPI);
        }

        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_GetAcceptExSocketAddrs(const void *outBuf,
                                size_t outBufLen,
                                LLBC_SockAddr_IN &localAddr,
                                LLBC_SockAddr_IN &remoteAddr)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
#else // LLBC_TARGET_PLATFORM_WIN32
    if (!outBuf || outBufLen < sizeof(struct sockaddr_in) * 2)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    struct sockaddr *sysLocalAddr = nullptr;
    struct sockaddr *sysRemoteAddr = nullptr;
    LLBC_SocketLen localSockAddrLen = sizeof(struct sockaddr);
    LLBC_SocketLen remoteSockAddrLen = sizeof(struct sockaddr);

    (*LLBC_INTERNAL_NS __g_GetAcceptExSockAddrs)(const_cast<PVOID>(outBuf),
                                                 static_cast<DWORD>(outBufLen),
                                                 localSockAddrLen,
                                                 remoteSockAddrLen,
                                                 &sysLocalAddr,
                                                 &localSockAddrLen,
                                                 &sysRemoteAddr,
                                                 &remoteSockAddrLen);

    localAddr.FromOSDataType(sysLocalAddr, localSockAddrLen);
    remoteAddr.FromOSDataType(sysRemoteAddr, remoteSockAddrLen);

    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_ConnectToPeer(LLBC_SocketHandle handle, const LLBC_SockAddr_IN &addr)
{
    struct sockaddr_in inAddr = addr.ToOSDataType();
    LLBC_SocketLen len = sizeof(struct sockaddr_in);

#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (connect(handle, reinterpret_cast<const struct sockaddr *>(&inAddr), len) == -1)
    {
        if (errno == EINPROGRESS)
            LLBC_SetLastError(LLBC_ERROR_WBLOCK);
        else
            LLBC_SetLastError(LLBC_ERROR_CLIB);

        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    if (connect(handle, reinterpret_cast<const struct sockaddr *>(&inAddr), len) == SOCKET_ERROR)
    {
        if (::WSAGetLastError() == WSAEWOULDBLOCK)
            LLBC_SetLastError(LLBC_ERROR_WBLOCK);
        else
            LLBC_SetLastError(LLBC_ERROR_NETAPI);

        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_ConnectToPeerEx(LLBC_SocketHandle handle,
                         const LLBC_SockAddr_IN &addr,
                         const void *sendBuf,
                         size_t sendBufLen,
                         size_t *bytesSent,
                         LLBC_POverlapped ol)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    return LLBC_ConnectToPeer(handle, addr);
#else // LLBC_TARGET_PLATFORM_WIN32
    if (LLBC_BindToAddress(handle, "0.0.0.0", 0) != LLBC_OK)
        return LLBC_FAILED;

    struct sockaddr_in sa = addr.ToOSDataType();
    BOOL ret = (*(LLBC_INTERNAL_NS __g_ConnectExProc))(handle,
                                                       (struct sockaddr *)&sa,
                                                       sizeof(struct sockaddr_in),
                                                       const_cast<void *>(sendBuf),
                                                       static_cast<DWORD>(sendBufLen),
                                                       reinterpret_cast<LPDWORD>(bytesSent),
                                                       ol);
    if (ret == FALSE)
    {
        if (::WSAGetLastError() == WSA_IO_PENDING)
            LLBC_SetLastError(LLBC_ERROR_PENDING);
        else
            LLBC_SetLastError(LLBC_ERROR_NETAPI);

        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_GetSocketOption(LLBC_SocketHandle handle,
                         int level,
                         int optname,
                         void *optval,
                         LLBC_SocketLen *len)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (getsockopt(handle, level, optname, 
        reinterpret_cast<char *>(optval), len) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    if (getsockopt(handle, level, optname, 
        reinterpret_cast<char *>(optval), len) == SOCKET_ERROR)
    {
        LLBC_SetLastError(LLBC_ERROR_NETAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_SetSocketOption(LLBC_SocketHandle handle,
                         int level,
                         int optname,
                         const void *optval,
                         LLBC_SocketLen len)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (setsockopt(handle, level, optname, 
        reinterpret_cast<const char *>(optval), len) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    if (setsockopt(handle, level, optname, 
        reinterpret_cast<const char *>(optval), len) == SOCKET_ERROR)
    {
        LLBC_SetLastError(LLBC_ERROR_NETAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

__LLBC_NS_END

// Recover warnings setting(on windows platform)
#if LLBC_TARGET_PLATFORM_WIN32
 #pragma warning(pop)
#endif // Win32
