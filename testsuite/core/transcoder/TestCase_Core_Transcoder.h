/**
 * @file    TestCase_Core_Transcoder.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/09/16
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_TRANSCODER_H__
#define __LLBC_TEST_CASE_CORE_TRANSCODER_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_Transcoder : public LLBC_BaseTestCase
{
public:
    TestCase_Core_Transcoder();
    virtual ~TestCase_Core_Transcoder();

public:
    virtual int Run(int argc, char *argv[]);

private:
    virtual int SimpleConvertTest();

    virtual int UTF8ConvertTest(const LLBC_String &inputFile);

    virtual int GBKConvertTest(const LLBC_String &inputFile);

    virtual int UTF16ConvertTest(const LLBC_String &inputFile);

private:
    virtual void DeleteOutputFiles(const std::vector<LLBC_String> &files);
};

#endif // !__LLBC_TEST_CASE_CORE_TRANSCODER_H__
