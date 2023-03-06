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

LLBC_FORCE_INLINE size_t LLBC_Packet::GetLength() const
{
    return _length;
}

LLBC_FORCE_INLINE void LLBC_Packet::SetLength(size_t length)
{
    _length = length;
}

LLBC_FORCE_INLINE int LLBC_Packet::GetSenderServiceId() const
{
    return _senderSvcId;
}

LLBC_FORCE_INLINE void LLBC_Packet::SetSenderServiceId(int senderServiceId)
{
    _senderSvcId = senderServiceId;
}

LLBC_FORCE_INLINE int LLBC_Packet::GetRecverServiceId() const
{
    return _recverSvcId;
}

LLBC_FORCE_INLINE void LLBC_Packet::SetRecverServiceId(int recverServiceId)
{
    _recverSvcId = recverServiceId;
}

LLBC_FORCE_INLINE int LLBC_Packet::GetSessionId() const
{
    return _sessionId;
}

LLBC_FORCE_INLINE int LLBC_Packet::GetAcceptSessionId() const
{
    return _acceptSessionId;
}

LLBC_FORCE_INLINE void LLBC_Packet::SetAcceptSessionId(int sessionId)
{
    _acceptSessionId = sessionId;
}

LLBC_FORCE_INLINE void LLBC_Packet::SetSessionId(int sessionId)
{
    _sessionId = sessionId;
}

LLBC_FORCE_INLINE const LLBC_SockAddr_IN &LLBC_Packet::GetLocalAddr() const
{
    return _localAddr;
}

LLBC_FORCE_INLINE void LLBC_Packet::SetLocalAddr(const LLBC_SockAddr_IN &addr)
{
    _localAddr = addr;
}

LLBC_FORCE_INLINE const LLBC_SockAddr_IN &LLBC_Packet::GetPeerAddr() const
{
    return _peerAddr;
}

LLBC_FORCE_INLINE void LLBC_Packet::SetPeerAddr(const LLBC_SockAddr_IN &addr)
{
    _peerAddr = addr;
}

LLBC_FORCE_INLINE int LLBC_Packet::GetOpcode() const
{
    return _opcode;
}

LLBC_FORCE_INLINE void LLBC_Packet::SetOpcode(int opcode)
{
    _opcode = opcode;
}

LLBC_FORCE_INLINE int LLBC_Packet::GetStatus() const
{
    return _status;
}

LLBC_FORCE_INLINE void LLBC_Packet::SetStatus(int status)
{
    _status = status;
}

LLBC_FORCE_INLINE int LLBC_Packet::GetFlags() const
{
    return _flags;
}

LLBC_FORCE_INLINE void LLBC_Packet::SetFlags(int flags)
{
    _flags = flags;
}

LLBC_FORCE_INLINE bool LLBC_Packet::HasFlags(int flags) const
{
    return (_flags & flags) == flags;
}

LLBC_FORCE_INLINE void LLBC_Packet::AddFlags(int flags)
{
    SetFlags(_flags | flags);
}

LLBC_FORCE_INLINE void LLBC_Packet::RemoveFlags(int flags)
{
    SetFlags(_flags & (~flags));
}

LLBC_FORCE_INLINE const sint64 &LLBC_Packet::GetExtData1() const
{
    return _extData1;
}

LLBC_FORCE_INLINE void LLBC_Packet::SetExtData1(const sint64 &extData1)
{
    _extData1 = extData1;
}

LLBC_FORCE_INLINE const sint64 &LLBC_Packet::GetExtData2() const
{
    return _extData2;
}

LLBC_FORCE_INLINE void LLBC_Packet::SetExtData2(const sint64 &extData2)
{
    _extData2 = extData2;
}

LLBC_FORCE_INLINE const sint64 &LLBC_Packet::GetExtData3() const
{
    return _extData3;
}

LLBC_FORCE_INLINE void LLBC_Packet::SetExtData3(const sint64 &extData3)
{
    _extData3 = extData3;
}

LLBC_FORCE_INLINE void LLBC_Packet::SetHeader(int sessionId, int opcode, int status)
{
    SetSessionId(sessionId);
    SetOpcode(opcode);
    SetStatus(status);
}

LLBC_FORCE_INLINE void LLBC_Packet::SetHeader(int svcId, int sessionId, int opcode, int status)
{
    SetRecverServiceId(svcId);

    SetSessionId(sessionId);
    SetOpcode(opcode);
    SetStatus(status);
}

LLBC_FORCE_INLINE void LLBC_Packet::SetHeader(const LLBC_Packet &packet, int opcode, int status)
{
    SetSessionId(packet._sessionId);

    SetOpcode(opcode);
    SetStatus(status);
}

LLBC_FORCE_INLINE const void *LLBC_Packet::GetPayload() const
{
    if (!_payload)
        return nullptr;

    return _payload->GetDataStartWithReadPos();
}

LLBC_FORCE_INLINE size_t LLBC_Packet::GetPayloadLength() const
{
    if (!_payload)
        return 0;

    return _payload->GetReadableSize();
}

LLBC_FORCE_INLINE LLBC_MessageBlock *LLBC_Packet::GetMutablePayload()
{
    if (!_payload && _msgBlockPoolInst)
        _payload = reinterpret_cast<LLBC_MessageBlock *>(_msgBlockPoolInst->Get());

    return _payload;
}

LLBC_FORCE_INLINE LLBC_MessageBlock * LLBC_Packet::DetachPayload()
{
    LLBC_MessageBlock *payload = _payload;
    _payload = nullptr;

    return payload;
}

LLBC_FORCE_INLINE void LLBC_Packet::SetPayload(LLBC_MessageBlock *payload)
{
    if (UNLIKELY(payload == _payload))
        return;

    CleanupPayload();

    _payload = payload;
}

LLBC_FORCE_INLINE void LLBC_Packet::ResetPayload()
{
    if (_payload)
    {
        _payload->SetReadPos(0);
        _payload->SetWritePos(0);
    }
}

LLBC_FORCE_INLINE LLBC_IObjectPoolInst *LLBC_Packet::GetPoolInst()
{
    return _selfPoolInst;
}

template<typename RetType>
LLBC_FORCE_INLINE RetType *LLBC_Packet::GetPreHandleResult() const
{
    return reinterpret_cast<RetType *>(_preHandleResult);
}

LLBC_FORCE_INLINE int LLBC_Packet::Read(bool &val)
{
    return ReadRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Read(sint8 &val)
{
    return ReadRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Read(uint8 &val)
{
    return ReadRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Read(sint16 &val)
{
    return ReadRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Read(uint16 &val)
{
    return ReadRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Read(sint32 &val)
{
    return ReadRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Read(uint32 &val)
{
    return ReadRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Read(long &val)
{
    return ReadRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Read(ulong &val)
{
    return ReadRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Read(sint64 &val)
{
    return ReadRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Read(uint64 &val)
{
    return ReadRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Read(float &val)
{
    return ReadRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Read(double &val)
{
    return ReadRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Read(void *buf, size_t len)
{
    if (!_payload)
    {
        LLBC_SetLastError(LLBC_ERROR_LIMIT);
        return LLBC_FAILED;
    }

    return _payload->Read(buf, len);
}

template <typename _Ty>
LLBC_FORCE_INLINE int LLBC_Packet::Read(std::vector<_Ty> &val)
{
    uint32 len = 0;
    if (this->Read(len) != LLBC_OK)
        return LLBC_FAILED;

    for (uint32 i = 0; i < len; ++i)
    {
        _Ty elem;
        if (this->Read(elem) != LLBC_OK)
            return LLBC_FAILED;

        val.push_back(elem);
    }

    return LLBC_OK;
}

template <typename _Ty>
LLBC_FORCE_INLINE int LLBC_Packet::Read(std::list<_Ty> &val)
{
    uint32 len = 0;
    if (this->Read(len) != LLBC_OK)
        return LLBC_FAILED;

    for (uint32 i = 0; i < len; ++i)
    {
        _Ty elem;
        if (this->Read(elem) != LLBC_OK)
            return LLBC_FAILED;

        val.push_back(elem);
    }

    return LLBC_OK;
}

template <typename _Ty>
LLBC_FORCE_INLINE int LLBC_Packet::Read(std::deque<_Ty> &val)
{
    uint32 len = 0;
    if (this->Read(len) != LLBC_OK)
        return LLBC_FAILED;

    for (uint32 i = 0; i < len; ++i)
    {
        _Ty elem;
        if (this->Read(elem) != LLBC_OK)
            return LLBC_FAILED;

        val.push_back(elem);
    }

    return LLBC_OK;
}

template <typename _Kty>
LLBC_FORCE_INLINE int LLBC_Packet::Read(std::set<_Kty> &val)
{
    uint32 len = 0;
    if (this->Read(len) != LLBC_OK)
        return LLBC_FAILED;

    for (uint32 i = 0; i < len; ++i)
    {
        _Kty key;
        if (this->Read(key) != LLBC_OK)
            return LLBC_FAILED;

        val.insert(key);
    }

    return LLBC_OK;
}

template <typename _Kty, typename _Ty>
LLBC_FORCE_INLINE int LLBC_Packet::Read(std::map<_Kty, _Ty> &val)
{
    uint32 len = 0;
    if (this->Read(len) != LLBC_OK)
        return LLBC_FAILED;

    for (uint32 i = 0; i < len; ++i)
    {
        _Kty key;
        if (this->Read(key) != LLBC_OK)
            return LLBC_FAILED;

        _Ty elem;
        if (this->Read(elem) != LLBC_OK)
            return LLBC_FAILED;

        val.insert(std::make_pair(key, elem));
    }

    return LLBC_OK;
}

template <typename _Ty>
LLBC_FORCE_INLINE int LLBC_Packet::Read(_Ty &val)
{
    if (_payload == nullptr)
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

LLBC_FORCE_INLINE int LLBC_Packet::Write(bool val)
{
    return WriteRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Write(sint8 val)
{
    return WriteRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Write(uint8 val)
{
    return WriteRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Write(sint16 val)
{
    return WriteRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Write(uint16 val)
{
    return WriteRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Write(sint32 val)
{
    return WriteRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Write(uint32 val)
{
    return WriteRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Write(long val)
{
    return WriteRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Write(ulong val)
{
    return WriteRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Write(sint64 val)
{
    return WriteRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Write(uint64 val)
{
    return WriteRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Write(float val)
{
    return WriteRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Write(double val)
{
    return WriteRawType<>(val);
}

LLBC_FORCE_INLINE int LLBC_Packet::Write(const char *val)
{
    if (LIKELY(val))
        return Write(val, strlen(val) + 1);
    else
        return LLBC_OK;
}

LLBC_FORCE_INLINE int LLBC_Packet::Write(const void *buf, size_t len)
{
    return CheckAndCreatePayload(len)->Write(buf, len);
}

LLBC_FORCE_INLINE int LLBC_Packet::Write(const LLBC_Stream &stream)
{
    return CheckAndCreatePayload(stream.GetPos())->Write(stream.GetBuf(), stream.GetPos());
}

template <typename _Ty>
LLBC_FORCE_INLINE int LLBC_Packet::Write(const std::vector<_Ty> &val)
{
    this->Write(static_cast<uint32>(val.size()));

    const size_t size = val.size();
    for (size_t i = 0; i < size; ++i)
    {
        this->Write(val[i]);
    }

    return LLBC_OK;
}

template <typename _Ty>
LLBC_FORCE_INLINE int LLBC_Packet::Write(const std::list<_Ty> &val)
{
    this->Write(static_cast<uint32>(val.size()));

    typedef typename std::list<_Ty>::const_iterator _Iter;

    _Iter it = val.begin(), endIt = val.end();
    for (; it != endIt; ++it)
        this->Write(*it);

    return LLBC_OK;
}

template <typename _Ty>
LLBC_FORCE_INLINE int LLBC_Packet::Write(const std::deque<_Ty> &val)
{
    this->Write(static_cast<uint32>(val.size()));

    const size_t size = val.size();
    for (size_t i = 0; i < size; ++i)
        this->Write(val[i]);

    return LLBC_OK;
}

template <typename _Kty>
LLBC_FORCE_INLINE int LLBC_Packet::Write(const std::set<_Kty> &val)
{
    this->Write(static_cast<uint32>(val.size()));

    typedef typename std::set<_Kty>::const_iterator _Iter;

    _Iter it = val.begin(), endIt = val.end();
    for (; it != endIt; ++it)
        this->Write(*it);

    return LLBC_OK;
}

template <typename _Kty, typename _Ty>
LLBC_FORCE_INLINE int LLBC_Packet::Write(const std::map<_Kty, _Ty> &val)
{
    this->Write(static_cast<uint32>(val.size()));

    typedef typename std::map<_Kty, _Ty>::const_iterator _Iter;

    _Iter it = val.begin(), endIt = val.end();
    for (; it != endIt; ++it)
    {
        this->Write(it->first);
        this->Write(it->second);
    }

    return LLBC_OK;
}

template <typename _Ty>
LLBC_FORCE_INLINE int LLBC_Packet::Write(const _Ty &obj)
{
    LLBC_Stream s;
    s.Write(obj);

    return this->Write(s.GetBuf(), s.GetPos());
}

template <typename _Ty>
LLBC_FORCE_INLINE LLBC_Packet &LLBC_Packet::operator <<(const _Ty &val)
{
    this->Write(val);
    return *this;
}

template <typename _Ty>
LLBC_FORCE_INLINE LLBC_Packet &LLBC_Packet::operator >>(_Ty &val)
{
    this->Read(val);
    return *this;
}

LLBC_FORCE_INLINE void LLBC_Packet::SetPreHandleResult(void *result, void(*clearFunc)(void *))
{
    SetPreHandleResult(result, LLBC_Delegate<void(void *)>(clearFunc));
}

template <typename ObjType>
LLBC_FORCE_INLINE void LLBC_Packet::SetPreHandleResult(void *result, ObjType *obj, void(ObjType::*clearMethod)(void *))
{
    SetPreHandleResult(result, LLBC_Delegate<void(void *)>(obj, clearMethod));
}

template <typename _RawTy>
LLBC_FORCE_INLINE int LLBC_Packet::ReadRawType(_RawTy &val)
{
    if (this->Read(&val, sizeof(val)) != LLBC_OK)
    {
        val = _RawTy();
        return LLBC_FAILED;
    }

#if LLBC_CFG_COMM_ORDER_IS_NET_ORDER
    LLBC_Net2Host(val);
#endif // LLBC_CFG_COMM_ORDER_IS_NET_ORDER

    return LLBC_OK;
}

template <typename _RawTy>
LLBC_FORCE_INLINE int LLBC_Packet::WriteRawType(_RawTy val)
{
#if LLBC_CFG_COMM_ORDER_IS_NET_ORDER
    LLBC_Host2Net(val);
#endif // LLBC_CFG_COMM_ORDER_IS_NET_ORDER

    return CheckAndCreatePayload(sizeof(val))->Write(&val, sizeof(val));
}

LLBC_FORCE_INLINE LLBC_MessageBlock *&LLBC_Packet::CheckAndCreatePayload(size_t initSize)
{
    if (!_payload)
    {
        if (_msgBlockPoolInst)
            _payload = reinterpret_cast<LLBC_MessageBlock *>(_msgBlockPoolInst->Get());
        else
            _payload = new LLBC_MessageBlock(initSize);
    }

    return _payload;
}

template <typename CoderType>
LLBC_FORCE_INLINE CoderType *LLBC_Packet::GetDecoder() const
{
    return static_cast<CoderType *>(_decoder);
}

template <typename CoderType>
LLBC_FORCE_INLINE CoderType *LLBC_Packet::GetEncoder() const
{
    return static_cast<CoderType *>(_encoder);
}

__LLBC_NS_END

inline std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_Packet &packet)
{
    return (o << packet.ToString());
}


