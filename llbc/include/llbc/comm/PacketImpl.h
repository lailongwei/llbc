/**
 * @file    PacketImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/16
 * @version 1.0
 *
 * @brief   The packet class member function template implementations 
 *          and some very simple non-template function implementations.
 */
#ifdef __LLBC_COMM_PACKET_H__

#include "llbc/comm/headerdesc/PacketHeaderDesc.h"

__LLBC_NS_BEGIN

template <>
inline int LLBC_Packet::SetHeaderPartVal(int serialNo, const sint8 &val)
{
    return this->RawSetNonFloatTypeHeaderPartVal<sint8>(serialNo, val);
}

template <>
inline int LLBC_Packet::SetHeaderPartVal(int serialNo, const uint8 &val)
{
    return this->RawSetNonFloatTypeHeaderPartVal<uint8>(serialNo, val);
}

template <>
inline int LLBC_Packet::SetHeaderPartVal(int serialNo, const sint16 &val)
{
    return this->RawSetNonFloatTypeHeaderPartVal<sint16>(serialNo, val);
}

template <>
inline int LLBC_Packet::SetHeaderPartVal(int serialNo, const uint16 &val)
{
    return this->RawSetNonFloatTypeHeaderPartVal<uint16>(serialNo, val);
}

template <>
inline int LLBC_Packet::SetHeaderPartVal(int serialNo, const sint32 &val)
{
    return this->RawSetNonFloatTypeHeaderPartVal<sint32>(serialNo, val);
}

template <>
inline int LLBC_Packet::SetHeaderPartVal(int serialNo, const uint32 &val)
{
    return this->RawSetNonFloatTypeHeaderPartVal<uint32>(serialNo, val);
}

template <>
inline int LLBC_Packet::SetHeaderPartVal(int serialNo, const sint64 &val)
{
    return this->RawSetNonFloatTypeHeaderPartVal<sint64>(serialNo, val);
}

template <>
inline int LLBC_Packet::SetHeaderPartVal(int serialNo, const uint64 &val)
{
    return this->RawSetNonFloatTypeHeaderPartVal<uint64>(serialNo, val);
}

template <>
inline int LLBC_Packet::SetHeaderPartVal(int serialNo, const float &val)
{
    return this->RawSetFloatTypeHeaderPartVal<float>(serialNo, val);
}

template <>
inline int LLBC_Packet::SetHeaderPartVal(int serialNo, const double &val)
{
    return this->RawSetFloatTypeHeaderPartVal<double>(serialNo, val);
}

template <>
inline int LLBC_Packet::SetHeaderPartVal(int serialNo, const char * const &val)
{
    return this->SetHeaderPartVal(serialNo, val, LLBC_StrLenA(val));
}

template <>
inline int LLBC_Packet::SetHeaderPartVal(int serialNo, char * const &val)
{
    return this->SetHeaderPartVal(serialNo, val, LLBC_StrLenA(val));
}

template <>
inline int LLBC_Packet::SetHeaderPartVal(int serialNo, const std::string &val)
{
    return this->SetHeaderPartVal(serialNo, val.data(), val.size());
}

template <>
inline int LLBC_Packet::SetHeaderPartVal(int serialNo, const LLBC_String &val)
{
    return this->SetHeaderPartVal(serialNo, val.data(), val.size());
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

template <typename _Ty>
inline int LLBC_Packet::Read(std::vector<_Ty> &val)
{
    uint32 len = 0;
    if (this->Read(len) != LLBC_RTN_OK)
    {
        return LLBC_RTN_FAILED;
    }

    for (register uint32 i = 0; i < len; i++)
    {
        _Ty elem;
        if (this->Read(elem) != LLBC_RTN_OK)
        {
            return LLBC_RTN_FAILED;
        }

        val.push_back(elem);
    }

    return LLBC_RTN_OK;
}

template <typename _Ty>
inline int LLBC_Packet::Read(std::list<_Ty> &val)
{
    uint32 len = 0; 
    if (this->Read(len) != LLBC_RTN_OK)
    {
        return LLBC_RTN_FAILED;
    }

    for (register uint32 i = 0; i < len; i++)
    {
        _Ty elem;
        if (this->Read(elem) != LLBC_RTN_OK)
        {
            return LLBC_RTN_FAILED;
        }

        val.push_back(elem);
    }

    return LLBC_RTN_OK;
}

template <typename _Ty>
inline int LLBC_Packet::Read(std::deque<_Ty> &val)
{
    uint32 len = 0;
    if (this->Read(len) != LLBC_RTN_OK)
    {
        return LLBC_RTN_FAILED;
    }

    for (register uint32 i = 0; i < len; i++)
    {
        _Ty elem;
        if (this->Read(elem) != LLBC_RTN_OK)
        {
            return LLBC_RTN_FAILED;
        }

        val.push_back(elem);
    }

    return LLBC_RTN_OK;
}

template <typename _Kty>
inline int LLBC_Packet::Read(std::set<_Kty> &val)
{
    uint32 len = 0;
    if (this->Read(len) != LLBC_RTN_OK)
    {
        return LLBC_RTN_FAILED;
    }

    for (register uint32 i = 0; i < len; i++)
    {
        _Kty key;
        if (this->Read(key) != LLBC_RTN_OK)
        {
            return LLBC_RTN_FAILED;
        }

        val.insert(key);
    }

    return LLBC_RTN_OK;
}

template <typename _Kty, typename _Ty>
inline int LLBC_Packet::Read(std::map<_Kty, _Ty> &val)
{
    uint32 len = 0;
    if (this->Read(len) != LLBC_RTN_OK)
    {
        return LLBC_RTN_FAILED;
    }

    for (register uint32 i = 0; i < len; i++)
    {
        _Kty key;
        if (this->Read(key) != LLBC_RTN_OK)
        {
            return LLBC_RTN_FAILED;
        }

        _Ty elem;
        if (this->Read(elem) != LLBC_RTN_OK)
        {
            return LLBC_RTN_FAILED;
        }

        val.insert(std::make_pair(key, elem));
    }

    return LLBC_RTN_OK;
}

template <typename _Ty>
inline int LLBC_Packet::Read(_Ty &val)
{
    LLBC_Stream s;
    s.Attach(_block->
        GetDataStartWithReadPos(), _block->GetReadableSize());
    if (!s.Read(val))
    {
        LLBC_SetLastError(LLBC_ERROR_LIMIT);
        return LLBC_RTN_FAILED;
    }

    _block->ShiftReadPos(static_cast<long>(s.GetPos()));

    return LLBC_RTN_OK;
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

    return LLBC_RTN_OK;
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

    return LLBC_RTN_OK;
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

    return LLBC_RTN_OK;
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

    return LLBC_RTN_OK;
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

    return LLBC_RTN_OK;
}

template <typename _Ty>
inline int LLBC_Packet::Write(const _Ty &obj)
{
    LLBC_Stream s;
    s.Write(obj);

    return this->Write(s.GetBuf(), s.GetPos());
}

inline void *LLBC_Packet::GetPreHandleResult() const
{
    return _preHandleResult;
}

template <typename Obj>
inline void LLBC_Packet::SetPreHandleResult(void *result, Obj *obj, void (Obj::*clearMethod)(void *))
{
    this->CleanupPreHandleResult();
    if ((_preHandleResult = result))
        if (obj && clearMethod)
            _resultClearDeleg = new LLBC_Delegate1<Obj, void *>(obj, clearMethod);
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

template <typename _RawTy>
inline void LLBC_Packet::RawGetFloatTypeHeaderPartVal(int serialNo, _RawTy &val) const
{
    // Get part describe.
    const LLBC_PacketHeaderPartDesc *
        partDesc = _headerDesc->GetPart(serialNo);
    if (!partDesc)
    {
        val = _RawTy();
        return;
    }

    // Get part begin buffer, buffer length.
    const char *partBeg = reinterpret_cast<const char *>(
        _block->GetData()) + _headerDesc->GetPartOffset(serialNo);
    const size_t partLen = partDesc->GetPartLen();

    // Real get part value.
    this->RawGetFloatTypeHeaderPartVal<_RawTy>(partBeg, partLen, val);
}

template <typename _RawTy>
inline void LLBC_Packet::RawGetNonFloatTypeHeaderPartVal(int serialNo, _RawTy &val) const
{
    // Get part describe.
    const LLBC_PacketHeaderPartDesc *
        partDesc = _headerDesc->GetPart(serialNo);
    if (!partDesc)
    {
        val = _RawTy();
        return;
    }

    // Get part begin buffer, buffer length.
    const char *partBeg = reinterpret_cast<const char *>(
        _block->GetData()) + _headerDesc->GetPartOffset(serialNo);
    const size_t partLen = partDesc->GetPartLen();

    // Real get part value.
    this->RawGetNonFloatTypeHeaderPartVal<_RawTy>(partBeg, partLen, val);
}

template <typename _RawTy>
inline void LLBC_Packet::RawGetFloatTypeHeaderPartVal(const char *buf, size_t bufLen, _RawTy &val) const
{
    // Try test 'bufLen' variable order: 8->4->others(1,2,3,5,6,7)
    if (bufLen == 8)
    {
        double dblVal =
            *reinterpret_cast<const double *>(buf);
#if LLBC_CFG_COMM_ORDER_IS_NET_ORDER
        LLBC_Net2Host(dblVal);
#endif // LLBC_CFG_COMM_ORDER_IS_NET_ORDER

        val = static_cast<_RawTy>(dblVal);
    }
    else if (bufLen == 4)
    {
        float floatVal =
            *reinterpret_cast<const float *>(buf);
#if LLBC_CFG_COMM_ORDER_IS_NET_ORDER
        LLBC_Net2Host(floatVal);
#endif // LLBC_CFG_COMM_ORDER_IS_NET_ORDER

        val = static_cast<_RawTy>(floatVal);
    }
    else
    {
        val = _RawTy();
        const size_t copyLen =
            sizeof(_RawTy) > bufLen ? bufLen : sizeof(_RawTy);
        ::memcpy(&val, buf, copyLen);
    }
}

template <typename _RawTy>
inline void LLBC_Packet::RawGetNonFloatTypeHeaderPartVal(const char *buf, size_t bufLen, _RawTy &val) const
{
    // Try test 'bufLen' variable order: 2->4->8->1->others(3,5,6,7)
    if (bufLen == 2)
    {
        sint16 sint16Val =
            *reinterpret_cast<const sint16 *>(buf);
#if LLBC_CFG_COMM_ORDER_IS_NET_ORDER
        LLBC_Net2Host(sint16Val);
#endif // LLBC_CFG_COMM_ORDER_IS_NET_ORDER

        val = static_cast<_RawTy>(sint16Val);
    }
    else if (bufLen == 4)
    {
        sint32 sint32Val =
            *reinterpret_cast<const sint32 *>(buf);
#if LLBC_CFG_COMM_ORDER_IS_NET_ORDER
        LLBC_Net2Host(sint32Val);
#endif // LLBC_CFG_COMM_ORDER_IS_NET_ORDER

        val = static_cast<_RawTy>(sint32Val);
    }
    else if (bufLen == 8)
    {
        sint64 sint64Val =
            *reinterpret_cast<const sint64 *>(buf);
#if LLBC_CFG_COMM_ORDER_IS_NET_ORDER
        LLBC_Net2Host(sint64Val);
#endif // LLBC_CFG_COMM_ORDER_IS_NET_ORDER

        val = static_cast<_RawTy>(sint64Val);
    }
    else if (bufLen == 1)
    {
        val = static_cast<_RawTy>(*buf);
    }
    else
    {
        val = _RawTy();
        const size_t copyLen =
            sizeof(_RawTy) > bufLen ? bufLen : sizeof(_RawTy);
        ::memcpy(&val, buf, copyLen);
    }
}

template <typename _RawTy>
inline int LLBC_Packet::RawSetFloatTypeHeaderPartVal(int serialNo, const _RawTy &val)
{
    // Get part describe.
    const LLBC_PacketHeaderPartDesc *
        partDesc = _headerDesc->GetPart(serialNo);
    if (!partDesc)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_RTN_FAILED;
    }

    // Get part length and part begin buffer.
    const size_t partLen = partDesc->GetPartLen();
    char *partBeg = const_cast<char *>(reinterpret_cast<
        const char *>(_block->GetData())) + _headerDesc->GetPartOffset(serialNo);

    // Set it.
    this->RawSetFloatTypeHeaderPartVal<_RawTy>(partBeg, partLen, val);
 
    return LLBC_RTN_OK;
}

template <typename _RawTy>
inline int LLBC_Packet::RawSetNonFloatTypeHeaderPartVal(int serialNo, const _RawTy &val)
{
    // Get part describe.
    const LLBC_PacketHeaderPartDesc *
        partDesc = _headerDesc->GetPart(serialNo);
    if (!partDesc)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_RTN_FAILED;
    }

    // Get part length and part begin buffer.
    const size_t partLen = partDesc->GetPartLen();
    char *partBeg = const_cast<char *>(reinterpret_cast<
        const char *>(_block->GetData())) + _headerDesc->GetPartOffset(serialNo);

    // Set it.
    this->RawSetNonFloatTypeHeaderPartVal<_RawTy>(partBeg, partLen, val);
 
    return LLBC_RTN_OK;
}

template <typename _RawTy>
inline void LLBC_Packet::RawSetFloatTypeHeaderPartVal(char *buf, size_t bufLen, const _RawTy &val)
{
    // Begin assign.
    // Try test 'bufLen' variable order: 8->4->others(1,2,3,5,6,7)
    if (bufLen == 8)
    {
        double convertedVal = static_cast<double>(val);
#if LLBC_CFG_COMM_ORDER_IS_NET_ORDER
        LLBC_Host2Net(convertedVal);
#endif // LLBC_CFG_COMM_ORDER_IS_NET_ORDER
        *reinterpret_cast<double *>(buf) = convertedVal;
    }
    else if (bufLen == 4)
    {
        float convertedVal = static_cast<float>(val);
#if LLBC_CFG_COMM_ORDER_IS_NET_ORDER
        LLBC_Host2Net(convertedVal);
#endif // LLBC_CFG_COMM_ORDER_IS_NET_ORDER
        *reinterpret_cast<float *>(buf) = convertedVal;
    }
    else
    {
        if (bufLen > sizeof(_RawTy))
        {
            ::memset(buf + bufLen, 0, bufLen - sizeof(_RawTy));
            ::memcpy(buf, &val, sizeof(_RawTy));
        }
        else
        {
            ::memcpy(buf, &val, bufLen);
        }
    }
}

template <typename _RawTy>
inline void LLBC_Packet::RawSetNonFloatTypeHeaderPartVal(char *buf, size_t bufLen, const _RawTy &val)
{
    // Begin assign.
    // Try test 'bufLen' variable order: 2->4->8->1->others(3,5,6,7)
    if (bufLen == 2)
    {
        sint16 convertedVal = static_cast<sint16>(val);
#if LLBC_CFG_COMM_ORDER_IS_NET_ORDER
        LLBC_Host2Net(convertedVal);
#endif // LLBC_CFG_COMM_ORDER_IS_NET_ORDER
        *reinterpret_cast<sint16 *>(buf) = convertedVal;
    }
    else if (bufLen == 4)
    {
        sint32 convertedVal = static_cast<sint32>(val);
#if LLBC_CFG_COMM_ORDER_IS_NET_ORDER
        LLBC_Host2Net(convertedVal);
#endif // LLBC_CFG_COMM_ORDER_IS_NET_ORDER
        *reinterpret_cast<sint32 *>(buf) = convertedVal;
    }
    else if (bufLen == 8)
    {
        sint64 convertedVal = static_cast<sint64>(val);
#if LLBC_CFG_COMM_ORDER_IS_NET_ORDER
        LLBC_Host2Net(convertedVal);
#endif // LLBC_CFG_COMM_ORDER_IS_NET_ORDER
        *reinterpret_cast<sint64 *>(buf) = convertedVal;
    }
    else if (bufLen == 1)
    {
        *buf = static_cast<sint8>(val);
    }
    else
    {
        if (bufLen > sizeof(_RawTy))
        {
            ::memset(buf + bufLen, 0, bufLen - sizeof(_RawTy));
            ::memcpy(buf, &val, sizeof(_RawTy));
        }
        else
        {
            ::memcpy(buf, &val, bufLen);
        }
    }
}

template <typename _RawTy>
inline int LLBC_Packet::ReadRawType(_RawTy &val)
{
    if (this->Read(&val, sizeof(val)) != LLBC_RTN_OK)
    {
        val = _RawTy();
        return LLBC_RTN_FAILED;
    }
#if LLBC_CFG_COMM_ORDER_IS_NET_ORDER
    else
    {
        LLBC_Net2Host(val);
    }
#endif // LLBC_CFG_COMM_ORDER_IS_NET_ORDER

    return LLBC_RTN_OK;
}

template <typename _RawTy>
inline int LLBC_Packet::WriteRawType(_RawTy val)
{
#if LLBC_CFG_COMM_ORDER_IS_NET_ORDER
    LLBC_Host2Net(val);
#endif // LLBC_CFG_COMM_ORDER_IS_NET_ORDER

    return _block->Write(&val, sizeof(val));
}

__LLBC_NS_END

#endif // __LLBC_COMM_PACKET_H__
