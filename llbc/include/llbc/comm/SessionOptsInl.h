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

__LLBC_NS_BEGIN

inline LLBC_SessionOpts::LLBC_SessionOpts(bool noDelay,
                                          size_t sockSendBufSize,
                                          size_t sockRecvBufSize,
                                          size_t sessionSendBufSize,
                                          size_t sessionRecvBufSize,
                                          size_t maxPacketSize)
: _noDelay(noDelay)
, _sockSendBufSize(sockSendBufSize)
, _sockRecvBufSize(sockRecvBufSize)
, _sessionSendBufSize(sessionSendBufSize)
, _sessionRecvBufSize(sessionRecvBufSize)
, _maxPacketSize(maxPacketSize)
{
}

inline LLBC_SessionOpts::~LLBC_SessionOpts()
{
}

inline bool LLBC_SessionOpts::IsNoDelay() const
{
    return _noDelay;
}

inline void LLBC_SessionOpts::SetNoDelay(bool noDelay)
{
    _noDelay = noDelay;
}

inline size_t LLBC_SessionOpts::GetSockSendBufSize() const
{
    return _sockSendBufSize;
}

inline void LLBC_SessionOpts::SetSockSendBufSize(size_t sockSendBufSize)
{
    _sockSendBufSize = sockSendBufSize;
}

inline size_t LLBC_SessionOpts::GetSockRecvBufSize() const
{
    return _sockRecvBufSize;
}

inline void LLBC_SessionOpts::SetSockRecvBufSize(size_t sockRecvBufSize)
{
    _sockRecvBufSize = sockRecvBufSize;
}

inline size_t LLBC_SessionOpts::GetSessionSendBufSize() const
{
    return _sessionSendBufSize;
}

inline void LLBC_SessionOpts::SetSessionSendBufSize(size_t sessionSendBufSize)
{
    _sessionSendBufSize = sessionSendBufSize;
}

inline size_t LLBC_SessionOpts::GetSessionRecvBufSize() const
{
    return _sessionRecvBufSize;
}

inline void LLBC_SessionOpts::SetSessionRecvBufSize(size_t sessionRecvBufSize)
{
    _sessionRecvBufSize = sessionRecvBufSize;
}

inline bool LLBC_SessionOpts::operator==(const LLBC_SessionOpts &another) const
{
    return memcmp(this, &another, sizeof(LLBC_SessionOpts)) == 0;
}

inline size_t LLBC_SessionOpts::GetMaxPacketSize() const
{
    return _maxPacketSize;
}

inline void LLBC_SessionOpts::SetMaxPacketSize(size_t size)
{
    _maxPacketSize = size;
}

__LLBC_NS_END
