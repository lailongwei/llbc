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

#ifdef __LLBC_COMM_PACKET_H__

__LLBC_NS_BEGIN

inline size_t LLBC_Packet::GetLength() const
{
    return _length;
}

inline void LLBC_Packet::SetLength(size_t length)
{
    _length = length;
}

inline int LLBC_Packet::GetSenderServiceId() const
{
    return _senderSvcId;
}

inline void LLBC_Packet::SetSenderServiceId(int senderServiceId)
{
    _senderSvcId = senderServiceId;
}

inline int LLBC_Packet::GetRecverServiceId() const
{
    return _recverSvcId;
}

inline void LLBC_Packet::SetRecverServiceId(int recverServiceId)
{
    _recverSvcId = recverServiceId;
}

inline int LLBC_Packet::GetSessionId() const
{
    return _sessionId;
}

inline void LLBC_Packet::SetSessionId(int sessionId)
{
    _sessionId = sessionId;
}

inline const LLBC_SockAddr_IN &LLBC_Packet::GetLocalAddr() const
{
    return _localAddr;
}

inline void LLBC_Packet::SetLocalAddr(const LLBC_SockAddr_IN &addr)
{
    _localAddr = addr;
}

inline const LLBC_SockAddr_IN &LLBC_Packet::GetPeerAddr() const
{
    return _peerAddr;
}

inline void LLBC_Packet::SetPeerAddr(const LLBC_SockAddr_IN &addr)
{
    _peerAddr = addr;
}

inline int LLBC_Packet::GetOpcode() const
{
    return _opcode;
}

inline void LLBC_Packet::SetOpcode(int opcode)
{
    _opcode = opcode;
}

inline int LLBC_Packet::GetStatus() const
{
    return _status;
}

inline void LLBC_Packet::SetStatus(int status)
{
    _status = status;
}

inline int LLBC_Packet::GetFlags() const
{
    return _flags;
}

inline void LLBC_Packet::SetFlags(int flags)
{
    _flags = flags;
}

inline bool LLBC_Packet::HasFlags(int flags) const
{
    return (_flags & flags) == flags;
}

inline void LLBC_Packet::AddFlags(int flags)
{
    SetFlags(_flags | flags);
}

inline void LLBC_Packet::RemoveFlags(int flags)
{
    SetFlags(_flags & (~flags));
}

inline const sint64 &LLBC_Packet::GetExtData1() const
{
    return _extData1;
}

inline void LLBC_Packet::SetExtData1(const sint64 &extData1)
{
    _extData1 = extData1;
}

inline const sint64 &LLBC_Packet::GetExtData2() const
{
    return _extData2;
}

inline void LLBC_Packet::SetExtData2(const sint64 &extData2)
{
    _extData2 = extData2;
}

inline const sint64 &LLBC_Packet::GetExtData3() const
{
    return _extData3;
}

inline void LLBC_Packet::SetExtData3(const sint64 &extData3)
{
    _extData3 = extData3;
}

inline void LLBC_Packet::SetHeader(int sessionId, int opcode, int status)
{
    SetSessionId(sessionId);
    SetOpcode(opcode);
    SetStatus(status);
}

inline void LLBC_Packet::SetHeader(int svcId, int sessionId, int opcode, int status)
{
    SetRecverServiceId(svcId);

    SetSessionId(sessionId);
    SetOpcode(opcode);
    SetStatus(status);
}

inline void LLBC_Packet::SetHeader(const LLBC_Packet &packet, int opcode, int status)
{
    SetSessionId(packet._sessionId);

    SetOpcode(opcode);
    SetStatus(status);
}

template <typename _Ty>
inline int LLBC_Packet::Read(std::vector<_Ty> &val)
{
    uint32 len = 0;
    if (this->Read(len) != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    for (register uint32 i = 0; i < len; i++)
    {
        _Ty elem;
        if (this->Read(elem) != LLBC_OK)
        {
            return LLBC_FAILED;
        }

        val.push_back(elem);
    }

    return LLBC_OK;
}

template <typename _Ty>
inline int LLBC_Packet::Read(std::list<_Ty> &val)
{
    uint32 len = 0; 
    if (this->Read(len) != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    for (register uint32 i = 0; i < len; i++)
    {
        _Ty elem;
        if (this->Read(elem) != LLBC_OK)
        {
            return LLBC_FAILED;
        }

        val.push_back(elem);
    }

    return LLBC_OK;
}

template <typename _Ty>
inline int LLBC_Packet::Read(std::deque<_Ty> &val)
{
    uint32 len = 0;
    if (this->Read(len) != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    for (register uint32 i = 0; i < len; i++)
    {
        _Ty elem;
        if (this->Read(elem) != LLBC_OK)
        {
            return LLBC_FAILED;
        }

        val.push_back(elem);
    }

    return LLBC_OK;
}

template <typename _Kty>
inline int LLBC_Packet::Read(std::set<_Kty> &val)
{
    uint32 len = 0;
    if (this->Read(len) != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    for (register uint32 i = 0; i < len; i++)
    {
        _Kty key;
        if (this->Read(key) != LLBC_OK)
        {
            return LLBC_FAILED;
        }

        val.insert(key);
    }

    return LLBC_OK;
}

template <typename _Kty, typename _Ty>
inline int LLBC_Packet::Read(std::map<_Kty, _Ty> &val)
{
    uint32 len = 0;
    if (this->Read(len) != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    for (register uint32 i = 0; i < len; i++)
    {
        _Kty key;
        if (this->Read(key) != LLBC_OK)
        {
            return LLBC_FAILED;
        }

        _Ty elem;
        if (this->Read(elem) != LLBC_OK)
        {
            return LLBC_FAILED;
        }

        val.insert(std::make_pair(key, elem));
    }

    return LLBC_OK;
}

template <typename _Ty>
inline int LLBC_Packet::Read(_Ty &val)
{
    if (_payload == NULL)
    {
        LLBC_SetLastError(LLBC_ERROR_LIMIT);
        return LLBC_FAILED;
    }

    LLBC_Stream s;
    s.Attach(_payload->
        GetDataStartWithReadPos(), _payload->GetReadableSize());
    if (!s.Read(val))
    {
        LLBC_SetLastError(LLBC_ERROR_LIMIT);
        return LLBC_FAILED;
    }

    _payload->ShiftReadPos(static_cast<long>(s.GetPos()));

    return LLBC_OK;
}

template <typename _Ty>
inline int LLBC_Packet::Write(const std::vector<_Ty> &val)
{
    this->Write(static_cast<uint32>(val.size()));

    const size_t size = val.size();
    for (register size_t i = 0; i < size; i++)
    {
        this->Write(val[i]);
    }

    return LLBC_OK;
}

template <typename _Ty>
inline int LLBC_Packet::Write(const std::list<_Ty> &val)
{
    this->Write(static_cast<uint32>(val.size()));

    typedef typename std::list<_Ty>::const_iterator _Iter;

    _Iter it = val.begin(), endIt = val.end();
    for (; it != endIt; it++)
    {
        this->Write(*it);
    }

    return LLBC_OK;
}

template <typename _Ty>
inline int LLBC_Packet::Write(const std::deque<_Ty> &val)
{
    this->Write(static_cast<uint32>(val.size()));

    const size_t size = val.size();
    for (register size_t i = 0; i < size; i++)
    {
        this->Write(val[i]);
    }

    return LLBC_OK;
}

template <typename _Kty>
inline int LLBC_Packet::Write(const std::set<_Kty> &val)
{
    this->Write(static_cast<uint32>(val.size()));

    typedef typename std::set<_Kty>::const_iterator _Iter;

    _Iter it = val.begin(), endIt = val.end();
    for (; it != endIt; it++)
    {
        this->Write(*it);
    }

    return LLBC_OK;
}

template <typename _Kty, typename _Ty>
inline int LLBC_Packet::Write(const std::map<_Kty, _Ty> &val)
{
    this->Write(static_cast<uint32>(val.size()));

    typedef typename std::map<_Kty, _Ty>::const_iterator _Iter;

    _Iter it = val.begin(), endIt = val.end();
    for (; it != endIt; it++)
    {
        this->Write(it->first);
        this->Write(it->second);
    }

    return LLBC_OK;
}

template <typename _Ty>
inline int LLBC_Packet::Write(const _Ty &obj)
{
    LLBC_Stream s;
    s.Write(obj);

    return this->Write(s.GetBuf(), s.GetPos());
}

template <typename _Ty>
LLBC_Packet &LLBC_Packet::operator <<(const _Ty &val)
{
    this->Write(val);
    return *this;
}

template <typename _Ty>
LLBC_Packet &LLBC_Packet::operator >>(_Ty &val)
{
    this->Read(val);
    return *this;
}

inline void LLBC_Packet::SetPreHandleResult(void *result, void(*clearFunc)(void *))
{
    LLBC_IDelegate1<void, void *> *clearDeleg =
        new LLBC_Func1<void, void *>(clearFunc);
    SetPreHandleResult(result, clearDeleg);
}

template <typename ObjType>
inline void LLBC_Packet::SetPreHandleResult(void *result, ObjType *obj, void(ObjType::*clearMethod)(void *))
{
    LLBC_IDelegate1<void, void *> *clearDeleg =
        new LLBC_Delegate1<void, ObjType, void *>(obj, clearMethod);
    SetPreHandleResult(result, clearDeleg);
}

template <typename _RawTy>
inline int LLBC_Packet::ReadRawType(_RawTy &val)
{
    if (this->Read(&val, sizeof(val)) != LLBC_OK)
    {
        val = _RawTy();
        return LLBC_FAILED;
    }
#if LLBC_CFG_COMM_ORDER_IS_NET_ORDER
    else
    {
        LLBC_Net2Host(val);
    }
#endif // LLBC_CFG_COMM_ORDER_IS_NET_ORDER

    return LLBC_OK;
}

template <typename _RawTy>
inline int LLBC_Packet::WriteRawType(_RawTy val)
{
#if LLBC_CFG_COMM_ORDER_IS_NET_ORDER
    LLBC_Host2Net(val);
#endif // LLBC_CFG_COMM_ORDER_IS_NET_ORDER

    if (!_payload)
        _payload = new LLBC_MessageBlock(sizeof(val));

    return _payload->Write(&val, sizeof(val));
}

__LLBC_NS_END

#endif // __LLBC_COMM_PACKET_H__
