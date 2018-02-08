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

#include "llbc/comm/Service.h"

namespace
{
    typedef LLBC_NS LLBC_IService This;
}

__LLBC_NS_BEGIN

This *LLBC_IService::Create(Type type, const LLBC_String &name, LLBC_IProtocolFactory *protoFactory)
{
    return LLBC_New3(LLBC_Service, type, name, protoFactory);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
