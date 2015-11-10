/**
 * @file    ObjectFactory.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/02/05
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/objbase/ObjectFactory.h"

__LLBC_NS_BEGIN

LLBC_ObjectFactory::LLBC_ObjectFactory()
{
}

LLBC_ObjectFactory::~LLBC_ObjectFactory()
{
}

LLBC_Object *LLBC_ObjectFactory::CreateObject() const
{
    return (new LLBC_Object);
}

LLBC_Object *LLBC_ObjectFactory::Clone() const
{
    return new LLBC_ObjectFactory;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
