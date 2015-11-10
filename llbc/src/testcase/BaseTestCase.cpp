/**
 * @file    BaseTestCase.cpp
 * @author  Longwei Lai
 * @date    2013/03/31
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/testcase/BaseTestCase.h"

__LLBC_NS_BEGIN

LLBC_BaseTestCase::LLBC_BaseTestCase()
{
    _testcaseName = "llbc testcase(unnamed)";
}

LLBC_BaseTestCase::~LLBC_BaseTestCase()
{
}

const LLBC_String &LLBC_BaseTestCase::GetName() const
{
    return _testcaseName;
}

void LLBC_BaseTestCase::SetName(const LLBC_String &name)
{
    _testcaseName.clear();
    _testcaseName.append(name);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
