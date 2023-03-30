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
 * \brief The session options encapsulation.
 */
class LLBC_EXPORT LLBC_SessionOpts
{
public:
    /**
     * Ctor & Dtor.
     */
    explicit LLBC_SessionOpts(bool noDelay = true,
                              size_t sockSendBufSize = 0,
                              size_t sockRecvBufSize = 0,
                              size_t sessionSendBufSize = LLBC_CFG_COMM_DFT_SESSION_SEND_BUF_SIZE,
                              size_t sessionRecvBufSize = LLBC_CFG_COMM_DFT_SESSION_RECV_BUF_SIZE,
                              size_t maxPacketSize = LLBC_CFG_COMM_DFT_MAX_PACKET_SIZE);
    ~LLBC_SessionOpts();

public:
    /**
     * Get No-Delay option.
     * @return bool - return the option value.
     */
    bool IsNoDelay() const;

    /**
     * Set No-Delay option.
     * @param[in] noDelay - the option value.
     */
    void SetNoDelay(bool noDelay);

public:
    /**
     * Get socket send buffer size.
     * @return size_t - the socket send buffer size.
     */
    size_t GetSockSendBufSize() const;

    /**
     * Set socket send buffer size.
     * Note:
     *  Some OS Platform supported send buffer dynamic adjust, like: windows 7 and laters, windows 2008 server and laters, linux kerner 2.4+.
     * @param[in] sockSendBufSize - the socket send buffer size.
     */
    void SetSockSendBufSize(size_t sockSendBufSize);

    /**
     * Get socket recv buffer size.
     * @return size_t - the socket recv buffer size.
     */
    size_t GetSockRecvBufSize() const;

    /**
     * Set socket recv buffer size.
     * Note:
     *  Some OS Platform supported send buffer dynamic adjust, like: windows 7 and laters, windows 2008 server and laters, linux kerner 2.4+.
     * @param[in] sockRecvBufSize - the socket recv buffer size.
     */
    void SetSockRecvBufSize(size_t sockRecvBufSize);

public:
    /**
     * Get session send buffer size.
     * @return size_t - the session send buffer size.
     */
    size_t GetSessionSendBufSize() const;

    /**
     * Set session send buffer size.
     * @param[in] sessionSendBufSize - the session send buffer size.
     */
    void SetSessionSendBufSize(size_t sessionSendBufSize);

    /**
     * Get session recv buffer size.
     * @return size_t - the session recv buffer size.
     */
    size_t GetSessionRecvBufSize() const;

    /**
     * Set session recv buffer size.
     * @param[in] sessionRecvBufSize - the session recv buffer size.
     */
    void SetSessionRecvBufSize(size_t sessionRecvBufSize);

public:
    /**
     * Get max packet size.
     * @return size_t - the max packet size.
     */
    size_t GetMaxPacketSize() const;

    /**
     * Set max packet size.
     * @param[in] size - packet size, in bytes.
     */
    void SetMaxPacketSize(size_t size);

public:
    /**
     * operator ==
     */
    bool operator ==(const LLBC_SessionOpts &another) const;

private:
    bool _noDelay; // No-delay option, default is true.
    size_t _sockSendBufSize; // socket send buffer size, in bytes, default is 0, it means use os default.
    size_t _sockRecvBufSize; // socket recv buffer size, in bytes, default is 0, it means use os default.
    size_t _sessionSendBufSize; // session send buffer size, in bytes, default is LLBC_CFG_COMM_DFT_SESSION_SEND_BUF_SIZE
    size_t _sessionRecvBufSize; // session recv buffer size(init size), in bytes, default is LLBC_CFG_COMM_DFT_SESSION_RECV_BUF_SIZE.
    size_t _maxPacketSize; // max packet seize in packet protocol
};

__LLBC_NS_END

#include "llbc/comm/SessionOptsInl.h"

__LLBC_NS_BEGIN

/**
 * Default session options define.
 */
LLBC_EXTERN LLBC_EXPORT const LLBC_SessionOpts LLBC_DftSessionOpts;

__LLBC_NS_END
