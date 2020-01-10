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
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/PollerType.h"
#include "llbc/comm/Socket.h"
#include "llbc/comm/Session.h"

namespace
{
    typedef LLBC_NS LLBC_PollerType _PollerType;
    typedef LLBC_NS LLBC_OverlappedOpcode _Opcode;

}

__LLBC_INTERNAL_NS_BEGIN

void __OnOverlappedDelHook(void *data)
{
    if (data)
        LLBC_Recycle(reinterpret_cast<
            LLBC_NS LLBC_MessageBlock *>(data));
}

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

#if LLBC_TARGET_PLATFORM_WIN32

char LLBC_Socket::_acceptExBuf[(sizeof(LLBC_SockAddr_IN) + 16) * 2] = {0};
#endif // LLBC_TARGET_PLATFORM_WIN32

LLBC_Socket::LLBC_Socket(LLBC_SocketHandle handle)
: _handle(handle)

, _session(NULL)
, _pollerType(_PollerType::End)

, _listenSocket(false)
, _peerAddr()
, _localAddr()

, _willSend()
#if LLBC_TARGET_PLATFORM_WIN32
, _nonBlocking(false)
, _olGroup()
#endif // LLBC_TARGET_PLATFORM_WIN32
{
    if (_handle == LLBC_INVALID_SOCKET_HANDLE)
        _handle = LLBC_CreateTcpSocket();

#if LLBC_TARGET_PLATFORM_WIN32
    _olGroup.SetDeleteDataProc(&LLBC_INL_NS __OnOverlappedDelHook);
#endif
}

LLBC_Socket::~LLBC_Socket()
{
    Close();
}

void LLBC_Socket::SetSession(LLBC_Session *session)
{
    _session = session;
}

int LLBC_Socket::GetPollerType() const
{
    return _pollerType;
}

void LLBC_Socket::SetPollerType(int type)
{
    _pollerType = type;
}

LLBC_SocketHandle LLBC_Socket::Handle()
{
    return _handle;
}

int LLBC_Socket::ShutdownInput()
{
    return LLBC_ShutdownSocketInput(_handle);
}

int LLBC_Socket::ShutdownOutput()
{
    return LLBC_ShutdownSocketOutput(_handle);
}

int LLBC_Socket::ShutdownInputOutput()
{
    return LLBC_ShutdownSocketInputOutput(_handle);
}

int LLBC_Socket::Close()
{
    if (_handle == LLBC_INVALID_SOCKET_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_FAILED;
    }
    else if (LLBC_CloseSocket(_handle) != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    _handle = LLBC_INVALID_SOCKET_HANDLE;
    return LLBC_OK;
}

bool LLBC_Socket::IsClosed() const
{
    return _handle == LLBC_INVALID_SOCKET_HANDLE;
}

LLBC_Socket::operator bool () const
{
    return !IsClosed();
}

bool LLBC_Socket::operator ! () const
{
    return IsClosed();
}

int LLBC_Socket::EnableAddressReusable()
{
    return LLBC_EnableAddressReusable(_handle);
}

int LLBC_Socket::DisableAddressReusable()
{
    return LLBC_DisableAddressReusable(_handle);
}

bool LLBC_Socket::IsNonBlocking() const
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    return LLBC_IsNonBlocking(_handle);
#else // LLBC_TARGET_PLATFORM_WIN32
    return _nonBlocking;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_Socket::SetNonBlocking()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    return LLBC_SetNonBlocking(_handle);
#else // LLBC_TARGET_PLATFORM_WIN32
    if (_nonBlocking)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }
    else if (LLBC_SetNonBlocking(_handle) != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    _nonBlocking = true;
    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_Socket::SetSendBufSize(size_t size)
{
    return LLBC_SetSendBufSize(_handle, size);
}

int LLBC_Socket::SetRecvBufSize(size_t size)
{
    return LLBC_SetRecvBufSize(_handle, size);
}

int LLBC_Socket::GetOption(int level, int optname, void *optval, LLBC_SocketLen *optlen)
{
    return LLBC_GetSocketOption(_handle, level, optname, optval, optlen);
}

int LLBC_Socket::SetOption(int level, int optname, const void *optval, LLBC_SocketLen optlen)
{
    return LLBC_SetSocketOption(_handle, level, optname, optval, optlen);
}

int LLBC_Socket::BindTo(const char *ip, uint16 port)
{
    return BindTo(LLBC_SockAddr_IN(ip, port));
}

int LLBC_Socket::BindTo(const LLBC_SockAddr_IN &addr)
{
    if (LLBC_BindToAddress(_handle, addr) != LLBC_OK)
        return LLBC_FAILED;

    _localAddr = addr;
    return LLBC_OK;
}

int LLBC_Socket::Listen(int backlog)
{
    if (LLBC_ListenForConnection(_handle, backlog) != LLBC_OK)
        return LLBC_FAILED;

    _listenSocket = true;
    return LLBC_OK;
}

bool LLBC_Socket::IsListen() const
{
    return _listenSocket;
}

LLBC_Socket *LLBC_Socket::Accept()
{
    LLBC_SocketHandle newHandle = LLBC_AcceptClient(_handle, &_peerAddr);
    if (newHandle == LLBC_INVALID_SOCKET_HANDLE)
        return NULL;

    LLBC_Socket *newSocket = LLBC_New1(LLBC_Socket, newHandle);
    newSocket->_pollerType = _pollerType;

    return newSocket;
}

#if LLBC_TARGET_PLATFORM_WIN32
int LLBC_Socket::AcceptEx(LLBC_SocketHandle listenSock,
                          LLBC_SocketHandle acceptSock,
                          LLBC_POverlapped ol)
{
    static char _acceptExBuf[(sizeof(LLBC_SockAddr_IN) + 16) * 2];
    return LLBC_AcceptClientEx(listenSock,
                               acceptSock,
                               _acceptExBuf,
                               sizeof(_acceptExBuf) - ((sizeof(LLBC_SockAddr_IN) + 16) * 2),
                               sizeof(LLBC_SockAddr_IN) + 16,
                               sizeof(LLBC_SockAddr_IN) + 16,
                               ol);

}
#endif // LLBC_TARGET_PLATFORM_WIN32

int LLBC_Socket::Connect(const LLBC_SockAddr_IN &addr)
{
    if (LLBC_ConnectToPeer(_handle, addr) != LLBC_OK)
        return LLBC_FAILED;

    if (UpdateLocalAddress() != LLBC_OK ||
            UpdatePeerAddress() != LLBC_OK)
        return LLBC_FAILED;

    return LLBC_OK;
}

#if LLBC_TARGET_PLATFORM_WIN32
int LLBC_Socket::ConnectEx(const LLBC_SockAddr_IN &addr, LLBC_POverlapped ol)
{
    return LLBC_ConnectToPeerEx(_handle,    // in
                                addr,       // in
                                NULL,       // in_opt
                                0,          // in
                                NULL,       // out(if send buffer is null, ignored) 
                                ol);        // out
}
#endif // LLBC_TARGET_PLATFORM_WIN32

#if LLBC_TARGET_PLATFORM_WIN32
int LLBC_Socket::AttachToIocp(LLBC_IocpHandle iocp)
{
    return LLBC_AddSocketToIocp(iocp, _handle, &_olGroup);
}
#endif // LLBC_TARGET_PLATFORM_WIN32

int LLBC_Socket::Send(const char *buf, int len)
{
    return LLBC_Send(_handle, buf, len, 0);
}

int LLBC_Socket::AsyncSend(const char *buf, int len)
{
    LLBC_MessageBlock *block = LLBC_New1(LLBC_MessageBlock, len);
    block->Write(buf, len);

    return AsyncSend(block);
}

int LLBC_Socket::AsyncSend(LLBC_MessageBlock *block)
{
    if (_willSend.Append(block) != LLBC_OK)
    {
        LLBC_XDelete(block);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_WIN32
    if (_pollerType != _PollerType::IocpPoller)
        return LLBC_OK;

    LLBC_MessageBlock *mergedBlock = _willSend.MergeBuffersAndDetach();

    LLBC_POverlapped ol = LLBC_New(LLBC_Overlapped);
    ol->sock = _handle;
    ol->data = mergedBlock;
    ol->opcode = _Opcode::Send;

    LLBC_SockBuf buf;
    buf.len = static_cast<ULONG>(mergedBlock->GetReadableSize());
    buf.buf = reinterpret_cast<char *>(mergedBlock->GetDataStartWithReadPos());

    int ret = 0;
    ulong flags = 0;
    ulong bytesSent = 0;
    if ((ret = LLBC_SendEx(_handle, &buf, 1, &bytesSent, flags, ol)) != LLBC_OK)
    {
        if (LLBC_GetLastError() == LLBC_ERROR_NETAPI && LLBC_GetSubErrorNo() == WSAENOBUFS)
        {
            ::memset(ol, 0, sizeof(OVERLAPPED));
            ol->data = NULL;
            buf.buf = NULL;
            buf.len = 0;

            // Failed and error is WSAENOBUFS, rebuff the mergedBlock, wait next time to send.
            _willSend.Append(mergedBlock);
            ret = LLBC_SendEx(_handle, &buf, 1, &bytesSent, 0, ol);
        }
    }

    _olGroup.InsertOverlapped(ol);
    if (ret != LLBC_OK && LLBC_GetLastError() != LLBC_ERROR_PENDING)
    {
        _olGroup.DeleteOverlapped(ol);
        return LLBC_FAILED;
    }
#endif // LLBC_TARGET_PLATFORM_WIN32
    return LLBC_OK;
}

bool LLBC_Socket::IsExistNoSendData() const
{
    return !!_willSend.FirstBlock();
}

const LLBC_MessageBuffer &LLBC_Socket::GetWillSendBuffer() const
{
    return _willSend;
}

int LLBC_Socket::Recv(char *buf, int len)
{
    return LLBC_Recv(_handle, buf, len, 0);
}

int LLBC_Socket::UpdateLocalAddress()
{
    return LLBC_GetSocketName(_handle, _localAddr);
}

const LLBC_SockAddr_IN &LLBC_Socket::GetLocalAddress() const
{
    return _localAddr;
}

LLBC_String LLBC_Socket::GetLocalHostname() const
{
    return _localAddr.GetIpAsString();
}

uint16 LLBC_Socket::GetLocalPort() const
{
    return _localAddr.GetPort();
}

int LLBC_Socket::UpdatePeerAddress()
{
    return LLBC_GetPeerSocketName(_handle, _peerAddr);
}

const LLBC_SockAddr_IN &LLBC_Socket::GetPeerAddress() const
{
    return _peerAddr;
}

LLBC_String LLBC_Socket::GetPeerHostname() const
{
    return _peerAddr.GetIpAsString();
}

uint16 LLBC_Socket::GetPeerPort() const
{
    return _peerAddr.GetPort();
}

int LLBC_Socket::GetPendingError(int &pendingError)
{
    LLBC_SocketLen soLen = sizeof(int);
    return GetOption(SOL_SOCKET, SO_ERROR, &pendingError, &soLen);
}

#if LLBC_TARGET_PLATFORM_WIN32
void LLBC_Socket::OnSend(LLBC_POverlapped ol)
#else
void LLBC_Socket::OnSend()
#endif // LLBC_TARGET_PLATFORM_WIN32
{
    // If is WIN32 platform & Iocp poller model, process overlapped.
#if LLBC_TARGET_PLATFORM_WIN32
    if (_pollerType == _PollerType::IocpPoller)
    {
        // NonZero-WSASend overlapped, delete the block and overlapped, and then, direct return.
        LLBC_MessageBlock *block = reinterpret_cast<LLBC_MessageBlock *>(ol->data);
        if (LIKELY(block))
        {
            size_t sent = block->GetReadableSize();
            _olGroup.DeleteOverlapped(ol);

            _session->OnSent(sent);

            return;
        }

        _olGroup.DeleteOverlapped(ol);
    }
#endif // LLBC_TARGET_PLATFORM_WIN32

    int len = 0, totalLen = 0;
    LLBC_MessageBlock *block = _willSend.FirstBlock();
    while (block)
    {
        if ((len = LLBC_Send(_handle, 
                             block->GetDataStartWithReadPos(), 
                             static_cast<int>(block->GetReadableSize()), 0)) < 0)
            break;

        totalLen += len;
        _willSend.Remove(len);
        block = _willSend.FirstBlock();
    }

    if (len < 0 && LLBC_GetLastError() != LLBC_ERROR_WBLOCK
#if LLBC_TARGET_PLATFORM_NON_WIN32
        // In Non-WIN32 platform, send() API return error maybe EAGAIN or EWOULDBLOCK.
         && LLBC_GetLastError() != LLBC_ERROR_AGAIN
#endif
         )
    {
         _session->OnClose();
         return;
    }

    if (totalLen > 0)
        _session->OnSent(totalLen);

#if LLBC_TARGET_PLATFORM_WIN32
    if (_pollerType != _PollerType::IocpPoller)
        return;

    block = _willSend.MergeBuffersAndDetach();
    if (!block)
        return;

    ol = LLBC_New(LLBC_Overlapped);
    ol->opcode = _Opcode::Send;
    ol->sessionId = _session->GetId();
    ol->sock = _handle;
    ol->data = block;

    LLBC_SockBuf buf;
    buf.buf = reinterpret_cast<char *>(block->GetDataStartWithReadPos());
    buf.len = static_cast<ULONG>(block->GetReadPos());

    ulong flags = 0;
    ulong bytesSent = 0;
    int ret = LLBC_SendEx(_handle, &buf, 1, &bytesSent, flags, ol);
    if (ret != LLBC_OK)
    {
        // Would block, post Zero-WSASend overlapped.
        if (LLBC_GetLastError() == LLBC_ERROR_NETAPI &&
            LLBC_GetSubErrorNo() == WSAENOBUFS)
        {
            ::memset(ol, 0, sizeof(OVERLAPPED));
            ol->data = NULL;
            buf.buf = NULL;
            buf.len = 0;

            _willSend.Append(block);
            ret = LLBC_SendEx(_handle, &buf, 1, &bytesSent, flags, ol);
        }
    }

    if (ret != LLBC_OK && LLBC_GetLastError() != LLBC_ERROR_PENDING)
    {
        trace("LLBC_Socket::OnSend() call LLBC_SendEx() failed, reason: %s\n", LLBC_FormatLastError());
        LLBC_Delete(reinterpret_cast<LLBC_MessageBlock *>(ol->data));

        LLBC_Delete(ol);
        _session->OnClose();
        return;
    }

    _olGroup.InsertOverlapped(ol);
#endif // LLBC_TARGET_PLATFORM_WIN32
}

#if LLBC_TARGET_PLATFORM_WIN32
void LLBC_Socket::OnRecv(LLBC_POverlapped ol)
#else
void LLBC_Socket::OnRecv()
#endif // LLBC_TARGET_PLATFORM_WIN32
{
#if LLBC_TARGET_PLATFORM_WIN32
    if (_pollerType == _PollerType::IocpPoller)
    {
        _olGroup.DeleteOverlapped(ol);
    }
#endif // LLBC_TARGET_PLATFORM_WIN32

    int len = 0;
    bool recvFlag = false;
    
    LLBC_MessageBlock *block = LLBC_New(LLBC_MessageBlock);
    while ((len = LLBC_Recv(_handle,
                            block->GetDataStartWithWritePos(),
                            static_cast<int>(block->GetWritableSize()),
                            0)) > 0)
    {
        block->ShiftWritePos(len);
        if (block->GetWritableSize() == 0)
            block->Allocate();

        recvFlag = true;
    }

    // If recv failed, firstly get last error.
    int errNo = LLBC_ERROR_SUCCESS;
    int subErrNo = LLBC_ERROR_SUCCESS;
    if (len < 0)
    {
        errNo = LLBC_Errno;
        if (LLBC_ERROR_TYPE_IS_LIBRARY(errNo))
            subErrNo = 0;
        else
            subErrNo = LLBC_SubErrno;
    }

    // Try process already received data, whether the errors occurred or not.
    if (recvFlag)
    {
        bool sessionRemoved;
        if (!_session->OnRecved(block, sessionRemoved))
        {
            #if LLBC_TARGET_PLATFORM_WIN32
            if (sessionRemoved)
                return;

            // In WIN32 platform & poller model is IOCP model, we post a Zero-WSASend overlapped.
            if (_pollerType == _PollerType::IocpPoller)
            {
                if (UNLIKELY(PostZeroWSARecv() != LLBC_OK))
                    _session->OnClose();
            }
            #endif // LLBC_TARGET_PLATFORM_WIN32

            return;
        }
    }
    else
    {
        LLBC_Delete(block);
    }

    // Process errors.
    if (len < 0)
    {
        if (errNo != LLBC_ERROR_WBLOCK
#if LLBC_TARGET_PLATFORM_NON_WIN32
            // In Non-WIN32 platform, recv() API return errnor maybe EAGAIN or EWOULDBLOCK.
            && errNo != LLBC_ERROR_AGAIN
#endif
           )
        {
#if LLBC_TARGET_PLATFORM_NON_WIN32
            _session->OnClose(LLBC_New2(LLBC_SessionCloseInfo, errNo, subErrNo));
#else
            _session->OnClose(NULL, LLBC_New2(LLBC_SessionCloseInfo, errNo, subErrNo));
#endif
            return;
        }
    }
    else if (len == 0) // Connection gracefully close by peer, explicit set a OS dependent's error number.
                       // In Non-Win32 platform: Set to ECONNRESET(104)
                       // In Win32 platform    : set to WSAECONNRESET(10054)
    {
#if LLBC_TARGET_PLATFORM_NON_WIN32
        LLBC_SessionCloseInfo *closeInfo = 
            LLBC_New2(LLBC_SessionCloseInfo, LLBC_ERROR_CLIB, ECONNRESET);
        _session->OnClose(closeInfo);
#else
        LLBC_SessionCloseInfo *closeInfo =
            LLBC_New2(LLBC_SessionCloseInfo, LLBC_ERROR_NETAPI, WSAECONNRESET);
        _session->OnClose(NULL, closeInfo);
#endif
        return;
    }

    // In WIN32 platform & poller model is IOCP model, we post a Zero-WSASend overlapped.
#if LLBC_TARGET_PLATFORM_WIN32
    if (_pollerType == _PollerType::IocpPoller)
        if (PostZeroWSARecv() != LLBC_OK)
            _session->OnClose();
#endif // LLBC_TARGET_PLATFORM_WIN32
}

#if LLBC_TARGET_PLATFORM_WIN32
void LLBC_Socket::OnClose(LLBC_POverlapped ol)
#else
void LLBC_Socket::OnClose()
#endif // LLBC_TARGET_PLATFORM_WIN32
{
#if LLBC_TARGET_PLATFORM_WIN32
    if (ol != NULL)
        DeleteOverlapped(ol);
#endif

    Close();
}

#if LLBC_TARGET_PLATFORM_WIN32
LLBC_OverlappedGroup &LLBC_Socket::GetOverlappedGroup()
{
    return _olGroup;
}

const LLBC_OverlappedGroup &LLBC_Socket::GetOverlappedGroup() const
{
    return _olGroup;
}

void LLBC_Socket::InsertOverlapped(LLBC_POverlapped ol)
{
    _olGroup.InsertOverlapped(ol);
}

void LLBC_Socket::RemoveOverlapped(LLBC_POverlapped ol)
{
    _olGroup.RemoveOverlapped(ol);
}

void LLBC_Socket::DeleteOverlapped(LLBC_POverlapped ol)
{
    _olGroup.DeleteOverlapped(ol);
}

void LLBC_Socket::DeleteAllOverlappeds()
{
    _olGroup.DeleteAllOverlappeds();
}
#endif // LLBC_TARGET_PLATFORM_WIN32

#if LLBC_TARGET_PLATFORM_WIN32
int LLBC_Socket::PostZeroWSARecv()
{
    LLBC_POverlapped ol = LLBC_New(LLBC_Overlapped);
    ol->opcode = _Opcode::Receive;
    ol->sessionId = _session->GetId();
    ol->sock = _handle;

    LLBC_SockBuf buf = {0};

    ulong flags = 0;
    ulong bytesRecv = 0;
    const int ret = LLBC_RecvEx(_handle,
                                &buf,
                                1,
                                &bytesRecv,
                                &flags,
                                ol);
    if (ret != LLBC_OK && 
        LLBC_GetLastError() != LLBC_ERROR_PENDING)
    {
        trace("LLBC_Socket::PostWSARecv() call LLBC_RecvEx() failed, "
            "reason: %s\n", LLBC_FormatLastError());

        LLBC_Delete(ol);
        return LLBC_FAILED;
    }

    _olGroup.InsertOverlapped(ol);
    return LLBC_OK;
}

int LLBC_Socket::PostAsyncAccept()
{
    LLBC_POverlapped ol = LLBC_New(LLBC_Overlapped);
    ol->opcode = LLBC_OverlappedOpcode::Accept;
    ol->sessionId = _session->GetId();
    ol->sock = _handle;
    if (UNLIKELY((ol->acceptSock = 
            LLBC_CreateTcpSocketEx()) == 
                    LLBC_INVALID_SOCKET_HANDLE))
    {
        LLBC_Delete(ol);
        return LLBC_FAILED;
    }

    const int ret = LLBC_AcceptClientEx(ol->sock,
                                        ol->acceptSock,
                                        _acceptExBuf,
                                        sizeof(_acceptExBuf) - ((sizeof(LLBC_SockAddr_IN) + 16) * 2),
                                        sizeof(LLBC_SockAddr_IN) + 16,
                                        sizeof(LLBC_SockAddr_IN) + 16,
                                        ol);
    if (UNLIKELY(ret != LLBC_OK &&
        LLBC_GetLastError() != LLBC_ERROR_PENDING))
    {
        LLBC_CloseSocket(ol->acceptSock);
        LLBC_Delete(ol);

        return LLBC_FAILED;
    }

    InsertOverlapped(ol);
    return LLBC_OK;
}

#endif // LLBC_TARGET_PLATFORM_WIN32

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
