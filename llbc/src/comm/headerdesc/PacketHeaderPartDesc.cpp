/**
 * @file    PacketHeaderPartDesc.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/08/28
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/headerdesc/PacketHeaderPartDesc.h"
#include "llbc/comm/headerdesc/PacketHeaderDesc.h"

namespace
{
    typedef LLBC_NS LLBC_PacketHeaderPartDesc This;
}

__LLBC_NS_BEGIN

LLBC_PacketHeaderPartDesc::LLBC_PacketHeaderPartDesc(LLBC_PacketHeaderDesc *headerDesc)
: _headerDesc(headerDesc)

, _hasBeenSet(false)

, _serialNo(0)

, _lenPart(0)
, _lenIncludedSelf(true)

, _opcodePart(false)
, _statusPart(false)
, _serviceIdPart(false)
, _flagsPart(false)

, _partLen(0)

, _fixedValue(NULL)
{
}

LLBC_PacketHeaderPartDesc::LLBC_PacketHeaderPartDesc(const This &another)
{
    _headerDesc = another._headerDesc;

    _hasBeenSet = another._hasBeenSet;

    _serialNo = another._serialNo;

    _lenPart = another._lenPart;
    _lenIncludedSelf = another._lenIncludedSelf;

    _opcodePart = another._opcodePart;
    _statusPart = another._statusPart;
    _serviceIdPart = another._serviceIdPart;
    _flagsPart = another._flagsPart;

    _partLen = another._partLen;

    if (another._fixedValue)
    {
        _fixedValue = LLBC_Malloc(void, _partLen);
        LLBC_MemCpy(_fixedValue, another._fixedValue, _partLen);
    }
    else
    {
        _fixedValue = NULL;
    }
}

LLBC_PacketHeaderPartDesc::~LLBC_PacketHeaderPartDesc()
{
    ClearFixedValue();
}

bool LLBC_PacketHeaderPartDesc::IsValid() const
{
    if (!_hasBeenSet)
        return false;
    else if (_partLen <= 0 || _partLen >= This::MaxPartLen)
        return false;

    if (_lenPart)
    {
        if (_partLen > sizeof(LLBC_NS sint32))
            return false;
        else if (IsHasFixedValue())
            return false;
    }
    else if (_opcodePart && _partLen > sizeof(LLBC_NS sint32))
        return false;
    else if (_statusPart && _partLen > sizeof(LLBC_NS sint32))
        return false;
    else if (_serviceIdPart && _partLen > sizeof(LLBC_NS sint32))
        return false;
    else if (_flagsPart && _partLen > sizeof(LLBC_NS sint32))
        return false;

    int specialFlagsCnt = 0;
    if (_lenPart)
        specialFlagsCnt += 1;
    if (_opcodePart)
        specialFlagsCnt += 1;
    if (_statusPart)
        specialFlagsCnt += 1;
    if (_serviceIdPart)
        specialFlagsCnt += 1;
    if (_flagsPart)
        specialFlagsCnt += 1;

    if (specialFlagsCnt > 1)
        return false;

    return true;
}

int LLBC_PacketHeaderPartDesc::Done()
{
    if (!_headerDesc)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    return _headerDesc->AddPartDesc(*this);
}

LLBC_String LLBC_PacketHeaderPartDesc::ToString() const
{
    LLBC_String repr;
    repr.append_format("hasBeenSet: %s, ", _hasBeenSet ? "true" : "false");
    repr.append_format("serial: %d, ", _serialNo);
    repr.append_format("len: %lu, ", _partLen);
    repr.append_format("headerInclSelf: %s, ", _lenIncludedSelf ? "true" : "false");
    
    repr.append("flags: {");
    repr.append_format("len:%s, ", _lenPart ? "true" : "false");
    repr.append_format("opcode:%s, ", _opcodePart ? "true" : "false");
    repr.append_format("status:%s, ", _statusPart ? "true" : "false");
    repr.append_format("serviceId:%s, ", _serviceIdPart ? "true" : "false");
    repr.append_format("flags:%s", _flagsPart ? "true" : "false");
    repr.append("}");

    return repr;
}

This &LLBC_PacketHeaderPartDesc::operator =(const This &another)
{
    if (UNLIKELY(&another == this))
        return *this;

    _headerDesc = another._headerDesc;

    _hasBeenSet = another._hasBeenSet;

    _serialNo = another._serialNo;

    _lenPart = another._lenPart;
    _lenIncludedSelf = another._lenIncludedSelf;

    _opcodePart = another._opcodePart;
    _statusPart = another._statusPart;
    _serviceIdPart = another._serviceIdPart;
    _flagsPart = another._flagsPart;

    _partLen = another._partLen;

    ClearFixedValue();
    if (another._fixedValue)
    {
        _fixedValue = LLBC_Malloc(void, _partLen);
        LLBC_MemCpy(_fixedValue, another._fixedValue, _partLen);
    }

    return *this;
}

bool LLBC_PacketHeaderPartDesc::operator <(const This &another) const
{
    return _serialNo < another._serialNo;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
