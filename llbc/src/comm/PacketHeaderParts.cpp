/**
 * @file    PacketHeaderParts.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/09/30
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/Packet.h"
#include "llbc/comm/PacketHeaderParts.h"

__LLBC_INTERNAL_NS_BEGIN

const LLBC_NS sint8 __LLBC_PacketHeaderPartTypes::UnknownType = 0;

const LLBC_NS sint8 __LLBC_PacketHeaderPartTypes::SInt8Type = 1;
const LLBC_NS sint8 __LLBC_PacketHeaderPartTypes::UInt8Type = 2;
const LLBC_NS sint8 __LLBC_PacketHeaderPartTypes::SInt16Type = 3;
const LLBC_NS sint8 __LLBC_PacketHeaderPartTypes::UInt16Type = 4;
const LLBC_NS sint8 __LLBC_PacketHeaderPartTypes::SInt32Type = 5;
const LLBC_NS sint8 __LLBC_PacketHeaderPartTypes::UInt32Type = 6;
const LLBC_NS sint8 __LLBC_PacketHeaderPartTypes::SInt64Type = 7;
const LLBC_NS sint8 __LLBC_PacketHeaderPartTypes::UInt64Type = 8;

const LLBC_NS sint8 __LLBC_PacketHeaderPartTypes::FloatType = 9;
const LLBC_NS sint8 __LLBC_PacketHeaderPartTypes::DoubleType = 10;

const LLBC_NS sint8 __LLBC_PacketHeaderPartTypes::StrType = 11;

const LLBC_NS sint8 __LLBC_PacketHeaderPartTypes::BytesType = 12;

__LLBC_INTERNAL_NS_END

namespace
{
    typedef LLBC_INL_NS __LLBC_PacketHeaderPartTypes _PartTypes;
    typedef LLBC_NS LLBC_PacketHeaderParts This;
}

__LLBC_NS_BEGIN

LLBC_PacketHeaderParts::LLBC_PacketHeaderParts()
{
}

LLBC_PacketHeaderParts::~LLBC_PacketHeaderParts()
{
    for (_Parts::iterator it = _parts.begin();
         it != _parts.end();
         it++)
         This::CleanupPart(it->second);
}

bool LLBC_PacketHeaderParts::IsHasPart(int serialNo) const
{
    return (_parts.find(serialNo) != _parts.end() ? true : false);
}

int LLBC_PacketHeaderParts::SetPart(int serialNo, const void *val, size_t valSize)
{
    This::_Part part;
    part.type = _PartTypes::BytesType;
    part.value.bytesVal = LLBC_New1(LLBC_MessageBlock, valSize);
    if (part.value.bytesVal->Write(val, valSize) != LLBC_RTN_OK)
    {
        This::CleanupPart(part);
        return LLBC_RTN_FAILED;
    }

    this->SetConstructedPart(serialNo, part);

    return LLBC_RTN_OK;
}

void LLBC_PacketHeaderParts::RemovePart(int serialNo)
{
    _Parts::iterator it = _parts.find(serialNo);
    if (it != _parts.end())
    {
        This::CleanupPart(it->second);
        _parts.erase(it);
    }
}

void LLBC_PacketHeaderParts::SetToPacket(LLBC_Packet &packet) const
{
    for (_Parts::const_iterator it = _parts.begin();
         it != _parts.end();
         it++)
         This::SetPartToPacket(it->first, it->second, packet);
}

void LLBC_PacketHeaderParts::SetPartToPacket(int serialNo, const _Part &part, LLBC_Packet &packet)
{
    switch (part.type)
    {
    case _PartTypes::SInt32Type:
        packet.SetHeaderPartVal(serialNo, static_cast<sint32>(part.value.i64Val));
        break;
    case _PartTypes::UInt32Type:
        packet.SetHeaderPartVal(serialNo, static_cast<uint32>(part.value.i64Val));
        break;

    case _PartTypes::SInt16Type:
        packet.SetHeaderPartVal(serialNo, static_cast<sint16>(part.value.i64Val));
        break;
    case _PartTypes::UInt16Type:
        packet.SetHeaderPartVal(serialNo, static_cast<uint16>(part.value.i64Val));
        break;

    case _PartTypes::SInt8Type:
        packet.SetHeaderPartVal(serialNo, static_cast<sint8>(part.value.i64Val));
        break;
    case _PartTypes::UInt8Type:
        packet.SetHeaderPartVal(serialNo, static_cast<uint8>(part.value.i64Val));
        break;

    case _PartTypes::SInt64Type:
        packet.SetHeaderPartVal(serialNo, part.value.i64Val);
        break;
    case _PartTypes::UInt64Type:
        packet.SetHeaderPartVal(serialNo, static_cast<uint64>(part.value.i64Val));
        break;

    case _PartTypes::FloatType:
        packet.SetHeaderPartVal(serialNo, *reinterpret_cast<const float *>(&part.value.i64Val));
        break;
    case _PartTypes::DoubleType:
        packet.SetHeaderPartVal(serialNo, *reinterpret_cast<const double *>(&part.value.i64Val));
        break;

    case _PartTypes::StrType:
        packet.SetHeaderPartVal(serialNo, *part.value.strVal);
        break;

    case _PartTypes::BytesType:
        packet.SetHeaderPartVal(serialNo,
                                part.value.bytesVal->GetDataStartWithReadPos(),
                                part.value.bytesVal->GetReadableSize());
        break;

    default:
        ASSERT(false && "llbc library internal error, Unknown packet header part type: %d");
        break;
    }
}

void LLBC_PacketHeaderParts::SetConstructedPart(int serialNo, const This::_Part &part)
{
    this->RemovePart(serialNo);
    _parts.insert(std::make_pair(serialNo, part));
}

void LLBC_PacketHeaderParts::CleanupPart(_Part &part)
{
    if (part.type == _PartTypes::StrType)
        LLBC_Delete(part.value.strVal);
    else if (part.type == _PartTypes::BytesType)
        LLBC_Delete(part.value.bytesVal);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
