/**
 * @file    TestCase_Core_Entity.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/13
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_ENTITY_H__
#define __LLBC_TEST_CASE_CORE_ENTITY_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_Entity : public LLBC_BaseTestCase
{
public:
    TestCase_Core_Entity();
    virtual ~TestCase_Core_Entity();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_ENTITY_H__
