/**
 * @file    IProtocol.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/01/10
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/protocol/IProtocol.h"

__LLBC_NS_BEGIN

LLBC_IProtocol::LLBC_IProtocol()
: _stack(NULL)
, _filter(NULL)
{
}

LLBC_IProtocol::~LLBC_IProtocol()
{
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
