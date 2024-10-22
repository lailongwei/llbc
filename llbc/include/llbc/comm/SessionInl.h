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

inline bool LLBC_SessionCloseInfo::IsFromService() const
{
    return _fromSvc;
}

inline const LLBC_String &LLBC_SessionCloseInfo::GetReason() const
{
    return _reason;
}

inline int LLBC_SessionCloseInfo::GetErrno() const
{
    return _errNo;
}

inline int LLBC_SessionCloseInfo::GetSubErrno() const
{
    return _subErrNo;
}

inline int LLBC_Session::GetId() const
{
    return _id;
}

inline void LLBC_Session::SetId(int id)
{
    _id = id;
}

inline int LLBC_Session::GetAcceptId() const
{
    return _acceptId;
}

inline void LLBC_Session::SetAcceptId(int acceptId)
{
    _acceptId = acceptId;
}

inline const LLBC_SessionOpts & LLBC_Session::GetSessionOpts() const
{
    return _sessionOpts;
}

inline LLBC_SocketHandle LLBC_Session::GetSocketHandle() const
{
    return _sockHandle;
}

inline LLBC_Socket *LLBC_Session::GetSocket()
{
    return _socket;
}

inline const LLBC_Socket *LLBC_Session::GetSocket() const
{
    return _socket;
}

inline LLBC_Service *LLBC_Session::GetService()
{
    return _svc;
}

inline LLBC_ProtocolStack *LLBC_Session::GetProtocolStack()
{
    return this->_protoStack;
}

inline LLBC_BasePoller *LLBC_Session::GetPoller()
{
    return _poller;
}

inline void LLBC_Session::SetPoller(LLBC_BasePoller *poller)
{
    _poller = poller;
}

__LLBC_NS_END
