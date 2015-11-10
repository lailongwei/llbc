/**
 * @file    PacketHeaderAssembler.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/08/30
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/PacketHeaderAssembler.h"
#include "llbc/comm/headerdesc/PacketHeaderDesc.h"

__LLBC_NS_BEGIN

LLBC_PacketHeaderAssembler::LLBC_PacketHeaderAssembler(size_t headerLen)
: _header(LLBC_Malloc(char, headerLen))
, _headerLen(headerLen)

, _curRecved(0)
{
}

LLBC_PacketHeaderAssembler::~LLBC_PacketHeaderAssembler()
{
    LLBC_Free(_header);
}

bool LLBC_PacketHeaderAssembler::Assemble(const void *data, size_t len ,size_t &used)
{
    used = 0;
    if (_curRecved == _headerLen)
        return true;

    size_t needSize = _headerLen - _curRecved;
    if (len >= needSize)
    {
        ::memcpy(_header + _curRecved, data, needSize);
        _curRecved = _headerLen;

        used = needSize;

        return true;
    }
    else
    {
        ::memcpy(_header + _curRecved, data, len);
        _curRecved += len;

        used = len;

        return false;
    }
}

void LLBC_PacketHeaderAssembler::Reset()
{
    _curRecved = 0;
}

const void *LLBC_PacketHeaderAssembler::GetHeader() const
{
    return _header;
}

size_t LLBC_PacketHeaderAssembler::GetHeaderLen() const
{
    return _headerLen;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
