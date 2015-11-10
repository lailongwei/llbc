/**
 * @file    BaseTestCase.h
 * @author  Longwei Lai
 * @date    2013/03/31
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_TESTCASE_BASE_TEST_CASE_H__
#define __LLBC_CORE_TESTCASE_BASE_TEST_CASE_H__

#include "llbc/common/Common.h"

#include "llbc/testcase/ITestCase.h"

__LLBC_NS_BEGIN

/**
 * Base testcase class encapsulation.
 */
class LLBC_EXPORT LLBC_BaseTestCase : public LLBC_ITestCase
{
public:
    /**
     * Default constructor.
     */
    LLBC_BaseTestCase();
    /**
     * Destructor.
     */
    virtual ~LLBC_BaseTestCase();

public:
    /**
     * Get testcase name.
     * @return const LLBC_String & - testcase name.
     */
    virtual const LLBC_String &GetName() const;

    /**
     * Set testcase name.
     * @param[in] name - testcase name.
     */
    void SetName(const LLBC_String &name);

private:
    LLBC_String _testcaseName;
};

__LLBC_NS_END

#endif // __LLBC_CORE_TESTCASE_BASE_TEST_CASE_H__
