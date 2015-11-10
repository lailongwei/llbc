/**
 * @file    ITestCase.h
 * @author  Longwei Lai
 * @date    2013/03/31
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_ITESTCASE_TESTCASE_H__
#define __LLBC_CORE_ITESTCASE_TESTCASE_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief Test case interface class encapsulation.
 */
class LLBC_EXPORT LLBC_ITestCase
{
public:
    /**
     * Destructor.
     */
    virtual ~LLBC_ITestCase() {  }

public:
    /**
     * Get testcase name.
     * @return const LLBC_String & - the testcase name.
     */
    virtual const LLBC_String &GetName() const = 0;

    /**
     * Set testcase name.
     * @param[in] name - testcase name.
     */
    virtual void SetName(const LLBC_String &name) = 0;

    /**
     * Run function.
     * @param[in] argc - argunemts count.
     * @param[in] argv - arguments array.
     *
     * @return int - testcase return status code.
     */
    virtual int Run(int argc, char *argv[]) = 0;
};

__LLBC_NS_END

#endif // __LLBC_CORE_ITESTCASE_TESTCASE_H__
