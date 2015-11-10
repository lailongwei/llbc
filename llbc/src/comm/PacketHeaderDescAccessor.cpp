/**
 * @file    PacketHeaderDescAccessor.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/08/30
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/IService.h"

#include "llbc/comm/headerdesc/PacketHeaderDesc.h"

#include "llbc/comm/LibPacketHeaderDescFactory.h"
#include "llbc/comm/PacketHeaderDescAccessor.h"

__LLBC_NS_BEGIN

LLBC_PacketHeaderDesc *LLBC_PacketHeaderDescAccessor::_headerDesc = NULL;

const LLBC_PacketHeaderDesc *LLBC_PacketHeaderDescAccessor::GetHeaderDesc(bool tryCreate)
{
    if (UNLIKELY(!_headerDesc))
        if (tryCreate)
            _headerDesc = LLBC_LibPacketHeaderDescFactory().Create();

    return _headerDesc;
}

int LLBC_PacketHeaderDescAccessor::SetPacketDesc(LLBC_PacketHeaderDesc *headerDesc)
{
    if (_headerDesc)
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_RTN_FAILED;
    }

    _headerDesc = headerDesc;

    return LLBC_RTN_OK;
}

void LLBC_PacketHeaderDescAccessor::CleanupHeaderDesc()
{
    LLBC_XDelete(_headerDesc);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
