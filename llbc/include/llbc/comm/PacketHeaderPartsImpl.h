/**
 * @file    PacketHeaderPartsImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @version 1.0
 * @date    2015/09/30
 *
 * @brief
 */
#ifdef __LLBC_COMM_PACKET_HEADER_PARTS_H__

__LLBC_INTERNAL_NS_BEGIN

/**
 * \brief The internal packet header part types enumeration.
 */
class LLBC_EXPORT __LLBC_PacketHeaderPartTypes
{
public:
    static const LLBC_NS sint8 UnknownType;

    static const LLBC_NS sint8 SInt8Type;
    static const LLBC_NS sint8 UInt8Type;
    static const LLBC_NS sint8 SInt16Type;
    static const LLBC_NS sint8 UInt16Type;
    static const LLBC_NS sint8 SInt32Type;
    static const LLBC_NS sint8 UInt32Type;
    static const LLBC_NS sint8 SInt64Type;
    static const LLBC_NS sint8 UInt64Type;

    static const LLBC_NS sint8 FloatType;
    static const LLBC_NS sint8 DoubleType;

    static const LLBC_NS sint8 StrType;

    static const LLBC_NS sint8 BytesType;
};

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

template <>
inline int LLBC_PacketHeaderParts::SetPart(int serialNo, const sint8 &val)
{
    _Part part;
    part.value.i64Val = val;
    part.type = LLBC_INL_NS __LLBC_PacketHeaderPartTypes::SInt8Type;

    this->SetConstructedPart(serialNo, part);

    return LLBC_OK;
}

template <>
inline int LLBC_PacketHeaderParts::SetPart(int serialNo, const uint8 &val)
{
    _Part part;
    part.value.i64Val = val;
    part.type = LLBC_INL_NS __LLBC_PacketHeaderPartTypes::UInt8Type;

    this->SetConstructedPart(serialNo, part);

    return LLBC_OK;
}

template <>
inline int LLBC_PacketHeaderParts::SetPart(int serialNo, const sint16 &val)
{
    _Part part;
    part.value.i64Val = val;
    part.type = LLBC_INL_NS __LLBC_PacketHeaderPartTypes::SInt16Type;

    this->SetConstructedPart(serialNo, part);

    return LLBC_OK;
}

template <>
inline int LLBC_PacketHeaderParts::SetPart(int serialNo, const uint16 &val)
{
    _Part part;
    part.value.i64Val = val;
    part.type = LLBC_INL_NS __LLBC_PacketHeaderPartTypes::UInt16Type;

    this->SetConstructedPart(serialNo, part);

    return LLBC_OK;
}

template <>
inline int LLBC_PacketHeaderParts::SetPart(int serialNo, const sint32 &val)
{
    _Part part;
    part.value.i64Val = val;
    part.type = LLBC_INL_NS __LLBC_PacketHeaderPartTypes::SInt32Type;

    this->SetConstructedPart(serialNo, part);

    return LLBC_OK;
}

template <>
inline int LLBC_PacketHeaderParts::SetPart(int serialNo, const uint32 &val)
{
    _Part part;
    part.value.i64Val = val;
    part.type = LLBC_INL_NS __LLBC_PacketHeaderPartTypes::UInt32Type;

    this->SetConstructedPart(serialNo, part);

    return LLBC_OK;
}

template <>
inline int LLBC_PacketHeaderParts::SetPart(int serialNo, const long &val)
{
    _Part part;
    part.value.i64Val = val;
    part.type = LLBC_INL_NS __LLBC_PacketHeaderPartTypes::SInt64Type;

    this->SetConstructedPart(serialNo, part);

    return LLBC_OK;
}

template <>
inline int LLBC_PacketHeaderParts::SetPart(int serialNo, const LLBC_NS ulong &val)
{
    _Part part;
    part.value.i64Val = val;
    part.type = LLBC_INL_NS __LLBC_PacketHeaderPartTypes::UInt64Type;

    this->SetConstructedPart(serialNo, part);

    return LLBC_OK;
}

template <>
inline int LLBC_PacketHeaderParts::SetPart(int serialNo, const sint64 &val)
{
    _Part part;
    part.value.i64Val = val;
    part.type = LLBC_INL_NS __LLBC_PacketHeaderPartTypes::SInt64Type;

    this->SetConstructedPart(serialNo, part);

    return LLBC_OK;
}

template <>
inline int LLBC_PacketHeaderParts::SetPart(int serialNo, const uint64 &val)
{
    _Part part;
    part.value.i64Val = *reinterpret_cast<const sint64 *>(&val);
    part.type = LLBC_INL_NS __LLBC_PacketHeaderPartTypes::UInt64Type;

    this->SetConstructedPart(serialNo, part);

    return LLBC_OK;
}


template <>
inline int LLBC_PacketHeaderParts::SetPart(int serialNo, const float &val)
{
    _Part part;
    part.value.i64Val = 0;
    ::memcpy(&part.value.i64Val, &val, sizeof(float));
    part.type = LLBC_INL_NS __LLBC_PacketHeaderPartTypes::FloatType;

    this->SetConstructedPart(serialNo, part);

    return LLBC_OK;
}

template <>
inline int LLBC_PacketHeaderParts::SetPart(int serialNo, const double &val)
{
    _Part part;
    part.value.i64Val = *reinterpret_cast<const sint64 *>(&val);
    part.type = LLBC_INL_NS __LLBC_PacketHeaderPartTypes::DoubleType;

    this->SetConstructedPart(serialNo, part);

    return LLBC_OK;
}

template <>
inline int LLBC_PacketHeaderParts::SetPart(int serialNo, char * const &val)
{
    _Part part;
    part.value.strVal = LLBC_New1(LLBC_String, val);
    part.type = LLBC_INL_NS __LLBC_PacketHeaderPartTypes::StrType;

    this->SetConstructedPart(serialNo, part);

    return LLBC_OK;
}

template <>
inline int LLBC_PacketHeaderParts::SetPart(int serialNo, const char * const &val)
{
    _Part part;
    part.value.strVal = LLBC_New1(LLBC_String, val);
    part.type = LLBC_INL_NS __LLBC_PacketHeaderPartTypes::StrType;

    this->SetConstructedPart(serialNo, part);

    return LLBC_OK;
}

template <>
inline int LLBC_PacketHeaderParts::SetPart(int serialNo, const std::string &val)
{
    _Part part;
    part.value.strVal = LLBC_New1(LLBC_String, val);
    part.type = LLBC_INL_NS __LLBC_PacketHeaderPartTypes::StrType;

    this->SetConstructedPart(serialNo, part);

    return LLBC_OK;
}

template <>
inline int LLBC_PacketHeaderParts::SetPart(int serialNo, const LLBC_String &val)
{
    _Part part;
    part.value.strVal = LLBC_New1(LLBC_String, val);
    part.type = LLBC_INL_NS __LLBC_PacketHeaderPartTypes::StrType;

    this->SetConstructedPart(serialNo, part);

    return LLBC_OK;
}

__LLBC_NS_END

#endif // __LLBC_COMM_PACKET_HEADER_PARTS_H__
