/**
 * @file    PacketHeaderDescImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/08/30
 * @version 1.0
 *
 * @brief
 */
#ifdef __LLBC_COMM_PACKET_HEADER_DESC_H__

__LLBC_NS_BEGIN

inline LLBC_PacketHeaderPartDesc LLBC_PacketHeaderDesc::AddPartDesc()
{
    return LLBC_PacketHeaderPartDesc(this);
}

inline size_t LLBC_PacketHeaderDesc::GetHeaderLen() const
{
    return _headerLen;
}

inline size_t LLBC_PacketHeaderDesc::GetLenPartIncludedLen() const
{
    return _lenPartIncludedLen;
}

inline size_t LLBC_PacketHeaderDesc::GetLenPartNotIncludedLen() const
{
    return _headerLen - _lenPartIncludedLen;
}

inline const LLBC_PacketHeaderPartDesc *LLBC_PacketHeaderDesc::GetLenPart() const
{
    return _lenPart;
}

inline size_t LLBC_PacketHeaderDesc::GetLenPartOffset() const
{
    if (UNLIKELY(!_lenPart))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return This::MaxHeaderLen;
    }
    
    return _lenPartOffset;
}

inline size_t LLBC_PacketHeaderDesc::GetLenPartLen() const
{
    if (UNLIKELY(!_lenPart))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_PacketHeaderPartDesc::MaxPartLen;
    }

    return _lenPart->GetPartLen();
}

inline bool LLBC_PacketHeaderDesc::IsHasOpcodePart() const
{
    return !!_opcodePart;
}

inline size_t LLBC_PacketHeaderDesc::GetOpcodePartOffset() const
{
    if (UNLIKELY(!_opcodePart))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return This::MaxHeaderLen;
    }

    return _opcodePartOffset;
}

inline size_t LLBC_PacketHeaderDesc::GetOpcodePartLen() const
{
    if (UNLIKELY(!_opcodePart))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_PacketHeaderPartDesc::MaxPartLen;
    }

    return _opcodePart->GetPartLen();
}

inline bool LLBC_PacketHeaderDesc::IsHasStatusPart() const
{
    return !!_statusPart;
}

inline size_t LLBC_PacketHeaderDesc::GetStatusPartOffset() const
{
    if (UNLIKELY(!_statusPart))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return This::MaxHeaderLen;
    }

    return _statusPartOffset;
}

inline size_t LLBC_PacketHeaderDesc::GetStatusPartLen() const
{
    if (UNLIKELY(!_statusPart))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_PacketHeaderPartDesc::MaxPartLen;
    }

    return _statusPart->GetPartLen();
}

inline bool LLBC_PacketHeaderDesc::IsHasServiceIdPart() const
{
    return !!_serviceIdPart;
}

inline size_t LLBC_PacketHeaderDesc::GetServiceIdPartOffset() const
{
    if (UNLIKELY(!_serviceIdPart))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return This::MaxHeaderLen;
    }

    return _serviceIdPartOffset;
}

inline size_t LLBC_PacketHeaderDesc::GetServiceIdPartLen() const
{
    if (UNLIKELY(!_serviceIdPart))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_PacketHeaderPartDesc::MaxPartLen;
    }

    return _serviceIdPart->GetPartLen();
}

inline bool LLBC_PacketHeaderDesc::IsHasFlagsPart() const
{
    return !!_flagsPart;
}

inline size_t LLBC_PacketHeaderDesc::GetFlagsPartOffset() const
{
    if (UNLIKELY(!_flagsPart))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return This::MaxHeaderLen;
    }

    return _flagsPartOffset;
}

inline size_t LLBC_PacketHeaderDesc::GetFlagsPartLen() const
{
    if (UNLIKELY(!_flagsPart))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_PacketHeaderPartDesc::MaxPartLen;
    }

    return _flagsPart->GetPartLen();
}

__LLBC_NS_END

#endif // __LLBC_COMM_PACKET_HEADER_DESC_H__
