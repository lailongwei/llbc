/**
 * @file    Event.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/28
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/event/Event.h"

__LLBC_NS_BEGIN

LLBC_Event::LLBC_Event(int id)
: _id(id)
{
}

LLBC_Event::~LLBC_Event()
{
}

int LLBC_Event::GetId() const
{
    return _id;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
