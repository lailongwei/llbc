/**
 * @file    TestCase_Comm_SvcFps.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2018/02/08
 * @version 1.0
 * 
 * @brief
 */
#ifndef __LLBC_TEST_CASE_COMM_SVC_FPS_H__
#define __LLBC_TEST_CASE_COMM_SVC_FPS_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Comm_SvcFps : public LLBC_BaseTestCase
{
public:
    TestCase_Comm_SvcFps();
    virtual ~TestCase_Comm_SvcFps();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_COMM_SVC_FPS_H__
