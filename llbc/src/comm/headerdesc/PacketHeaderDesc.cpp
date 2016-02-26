/**
 * @file    PacketHeaderDesc.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/08/29
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/headerdesc/PacketHeaderDesc.h"

namespace
{
    typedef LLBC_NS LLBC_PacketHeaderDesc This;
    typedef LLBC_NS LLBC_PacketHeaderPartDesc _PartDesc;
}

__LLBC_NS_BEGIN

LLBC_PacketHeaderDesc::LLBC_PacketHeaderDesc()
: _parts()

, _headerLen(0)
, _lenPartIncludedLen(0)

, _lenPartOffset(0)
, _lenPart(NULL)

, _opcodePartOffset(0)
, _opcodePart(NULL)

, _statusPartOffset(0)
, _statusPart(NULL)

, _serviceIdPartOffset(0)
, _serviceIdPart(NULL)

, _flagsPartOffset(0)
, _flagsPart(NULL)
{
}

LLBC_PacketHeaderDesc::LLBC_PacketHeaderDesc(const This &another)
{
    for (_Parts::const_iterator it = another._parts.begin();
         it != another._parts.end();
         it++)
        _parts.insert(std::make_pair(it->first, it->second));

    UpdateExtraBookKeeping();
}

LLBC_PacketHeaderDesc::~LLBC_PacketHeaderDesc()
{
}

int LLBC_PacketHeaderDesc::AddPartDesc(const LLBC_PacketHeaderPartDesc &partDesc)
{
    if (!partDesc.IsValid())
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }
    else if (IsCanAdd(partDesc) != LLBC_OK)
        return LLBC_FAILED;

    _parts.insert(std::make_pair(partDesc.GetSerialNo(), partDesc));

    UpdateExtraBookKeeping();

    return LLBC_OK;
}

int LLBC_PacketHeaderDesc::RemovePartDesc(int serialNo)
{
    This::_Parts::iterator it = _parts.find(serialNo);
    if (it == _parts.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    _parts.erase(it);
    UpdateExtraBookKeeping();

    return LLBC_OK;
}

const _PartDesc *LLBC_PacketHeaderDesc::GetPart(int serialNo) const
{
    This::_Parts::const_iterator it = _parts.find(serialNo);
    if (it == _parts.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return NULL;
    }

    return &it->second;
}

size_t LLBC_PacketHeaderDesc::GetPartOffset(int serialNo) const
{
    This::_PartOffsets::const_iterator it = _partOffsets.find(serialNo);
    if (it == _partOffsets.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return _PartDesc::MaxPartLen;
    }

    return it->second;
}

size_t LLBC_PacketHeaderDesc::GetPartLen(int serialNo) const
{
    This::_Parts::const_iterator it = _parts.find(serialNo);
    if (it == _parts.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return _PartDesc::MaxPartLen;
    }

    return it->second.GetPartLen();
}

LLBC_String LLBC_PacketHeaderDesc::ToString() const
{
    LLBC_String repr;
    repr.append_format("headerLen: %lu, ", _headerLen);

    repr.append("parts: [\n");
    for (_Parts::const_iterator it = _parts.begin();
         it != _parts.end();
         it++)
        repr.append_format("%s,\n", it->second.ToString().c_str());

    repr.append("]");

    return repr;
}

This &LLBC_PacketHeaderDesc::operator =(const This &another)
{
    if (UNLIKELY(&another == this))
        return *this;

    _parts.clear();
    for (_Parts::const_iterator it = another._parts.begin();
         it != another._parts.end();
         it++)
         _parts.insert(std::make_pair(it->first, it->second));

    UpdateExtraBookKeeping();

    return *this;
}

int LLBC_PacketHeaderDesc::IsCanAdd(const LLBC_PacketHeaderPartDesc &partDesc) const
{
    // Conflict check.
    for (_Parts::const_iterator it = _parts.begin();
         it != _parts.end();
         it++)
    {
        bool conflicted = false;

        const _PartDesc &curPartDesc = it->second;
        if (curPartDesc.GetSerialNo() == partDesc.GetSerialNo())
            conflicted = true;
        else if (curPartDesc.IsLenPart() && partDesc.IsLenPart())
            conflicted = true;
        else if (curPartDesc.IsOpcodePart() && partDesc.IsOpcodePart())
            conflicted = true;
        else if (curPartDesc.IsStatusPart() && partDesc.IsStatusPart())
            conflicted = true;
        else if (curPartDesc.IsServiceIdPart() && partDesc.IsServiceIdPart())
            conflicted = true;
        else if (curPartDesc.IsFlagsPart() && partDesc.IsFlagsPart())
            conflicted = true;

        if (conflicted)
        {
            LLBC_SetLastError(LLBC_ERROR_REPEAT);
            return LLBC_FAILED;
        }
    }

    // Check new header length is >= MaxHeaderLen or not.
    const size_t newHeaderLen = _headerLen + partDesc.GetPartLen();
    if (newHeaderLen >= This::MaxHeaderLen)
    {
        LLBC_SetLastError(LLBC_ERROR_LIMIT);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

void LLBC_PacketHeaderDesc::UpdateExtraBookKeeping()
{
    _partOffsets.clear();

    _headerLen = 0;
    _lenPartIncludedLen = 0;

    _lenPart = NULL;
    _lenPartOffset = 0;

    _opcodePart = NULL;
    _opcodePartOffset = 0;

    _statusPart = NULL;
    _statusPartOffset = 0;

    _serviceIdPart = NULL;
    _serviceIdPartOffset = 0;

    _flagsPart = NULL;
    _flagsPartOffset = 0;

    size_t curOffset = 0;
    for (_Parts::const_iterator it = _parts.begin();
         it != _parts.end();
         it++)
    {
        const _PartDesc &part = it->second;

        _headerLen += part.GetPartLen();
        _partOffsets.insert(std::make_pair(it->first, curOffset));

        if (part.IsLenIncludedSelf())
            _lenPartIncludedLen += part.GetPartLen();

        if (part.IsLenPart())
        {
            _lenPart = &part;
            _lenPartOffset = curOffset;
        }
        else if (part.IsOpcodePart())
        {
            _opcodePart = &part;
            _opcodePartOffset = curOffset;
        }
        else if (part.IsStatusPart())
        {
            _statusPart = &part;
            _statusPartOffset = curOffset;
        }
        else if (part.IsServiceIdPart())
        {
            _serviceIdPart = &part;
            _serviceIdPartOffset = curOffset;
        }
        else if (part.IsFlagsPart())
        {
            _flagsPart = &part;
            _flagsPartOffset = curOffset;
        }

        curOffset += part.GetPartLen();
    }
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
