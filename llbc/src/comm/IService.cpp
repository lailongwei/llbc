/**
 * @file    IService.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/26
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/headerdesc/PacketHeaderDesc.h"
#include "llbc/comm/PacketHeaderDescAccessor.h"
#include "llbc/comm/Service.h"

namespace
{
    typedef LLBC_NS LLBC_IService This;
}

__LLBC_NS_BEGIN

This *LLBC_IService::Create(Type type)
{
    return LLBC_New1(LLBC_Service, type);
}

int LLBC_IService::SetPacketHeaderDesc(LLBC_PacketHeaderDesc *headerDesc)
{
    typedef LLBC_PacketHeaderDescAccessor _Accessor;

    if (_Accessor::GetHeaderDesc(false))
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    _Accessor::SetPacketDesc(headerDesc);

    return LLBC_OK;
}

int LLBC_IService::SetPacketHeaderDescFactory(LLBC_IPacketHeaderDescFactory *factory)
{
    typedef LLBC_PacketHeaderDescAccessor _Accessor;

    if (!factory)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }
    else if (_Accessor::GetHeaderDesc(false))
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    _Accessor::SetPacketDesc(factory->Create());
    LLBC_Delete(factory);

    return LLBC_OK;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
