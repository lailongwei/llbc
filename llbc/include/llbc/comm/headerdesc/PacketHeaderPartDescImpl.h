/**
 * @file    PacketHeaderPartDescImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/08/30
 * @version 1.0
 *
 * @brief
 */
#ifdef __LLBC_COMM_PACKET_HEADER_PART_DESC_H__

__LLBC_NS_BEGIN

inline int LLBC_PacketHeaderPartDesc::GetSerialNo() const
{
    return _serialNo;
}

inline LLBC_PacketHeaderPartDesc &LLBC_PacketHeaderPartDesc::SetSerialNo(int serialNo)
{
    _serialNo = serialNo;
    _hasBeenSet = true;

    return *this;
}

inline bool LLBC_PacketHeaderPartDesc::IsLenPart() const
{
    return _lenPart;
}

inline LLBC_PacketHeaderPartDesc &LLBC_PacketHeaderPartDesc::SetIsLenPart(bool lenPart)
{
    _lenPart = lenPart;
    _hasBeenSet = true;

    return *this;
}

inline bool LLBC_PacketHeaderPartDesc::IsLenIncludedSelf() const
{
    return _lenIncludedSelf;
}

inline LLBC_PacketHeaderPartDesc &LLBC_PacketHeaderPartDesc::SetIsLenIncludedSelf(bool includedSelf)
{
    _lenIncludedSelf = includedSelf;
    _hasBeenSet = true;

    return *this;
}

inline size_t LLBC_PacketHeaderPartDesc::GetPartLen() const
{
    return _partLen;
}

inline LLBC_PacketHeaderPartDesc &LLBC_PacketHeaderPartDesc::SetPartLen(size_t len)
{
    _partLen = len;
    _hasBeenSet = true;

    return *this;
}

inline bool LLBC_PacketHeaderPartDesc::IsOpcodePart() const
{
    return _opcodePart;
}

inline LLBC_PacketHeaderPartDesc &LLBC_PacketHeaderPartDesc::SetIsOpcodePart(bool opcodePart)
{
    _opcodePart = opcodePart;
    _hasBeenSet = true;

    return *this;
}

inline bool LLBC_PacketHeaderPartDesc::IsStatusPart() const
{
    return _statusPart;
}

inline LLBC_PacketHeaderPartDesc &LLBC_PacketHeaderPartDesc::SetIsStatusPart(bool statusPart)
{
    _statusPart = statusPart;
    _hasBeenSet = true;

    return *this;
}

inline bool LLBC_PacketHeaderPartDesc::IsServiceIdPart() const
{
    return _serviceIdPart;
}

inline LLBC_PacketHeaderPartDesc &LLBC_PacketHeaderPartDesc::SetIsServiceIdPart(bool serviceIdPart)
{
    _serviceIdPart = serviceIdPart;
    _hasBeenSet = true;

    return *this;
}

inline bool LLBC_PacketHeaderPartDesc::IsFlagsPart() const
{
    return _flagsPart;
}

inline LLBC_PacketHeaderPartDesc &LLBC_PacketHeaderPartDesc::SetIsFlagsPart(bool flagsPart)
{
    _flagsPart = flagsPart;
    _hasBeenSet = true;

    return *this;
}

inline bool LLBC_PacketHeaderPartDesc::IsHasFixedValue() const
{
    return !!_fixedValue;
}

inline void *LLBC_PacketHeaderPartDesc::GetFixedValue()
{
    return _fixedValue;
}

inline const void *LLBC_PacketHeaderPartDesc::GetFixedValue() const
{
    return _fixedValue;
}

inline LLBC_PacketHeaderPartDesc &LLBC_PacketHeaderPartDesc::SetFixedValue(const void *value)
{
    if (UNLIKELY(!value))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return *this;
    }

    this->ClearFixedValue();

    _fixedValue = LLBC_Malloc(void, _partLen);
    LLBC_MemCpy(_fixedValue, value, _partLen);

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    return *this;
}

inline LLBC_PacketHeaderPartDesc &LLBC_PacketHeaderPartDesc::ClearFixedValue()
{
    LLBC_XFree(_fixedValue);
    return *this;
}

__LLBC_NS_END

#endif // __LLBC_COMM_PACKET_HEADER_PART_DESC_H__