/**
 * @file    TestCase_Com_DataType.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/03/31
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_COM_DATA_TYPE_H__
#define __LLBC_TEST_CASE_COM_DATA_TYPE_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Com_DataType : public LLBC_BaseTestCase
{
public:
    virtual int Run(int argc, char *argv[]);

private:
    void RawTest();
    void StringBaseTest();
    void StringUTF8Test();
    void StringFindReplaceTest();
    void StringStripTest();
    void StringSplitTest(const LLBC_String &str, size_t maxSplit = -1, const LLBC_String &sep = ".");
};

#endif // !__LLBC_TEST_CASE_COM_DATA_TYPE_H__
