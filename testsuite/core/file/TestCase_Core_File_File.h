/**
 * @file    TestCase_Core_File_File.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/07
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_FILE_FILE_H__
#define __LLBC_TEST_CASE_CORE_FILE_FILE_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_File_File : public LLBC_BaseTestCase
{
public:
    virtual int Run(int argc, char *argv[]);

private:
    void OpenCloseTest(const LLBC_NAMESPACE LLBC_String &file);

    void SetBufferModeTest(const LLBC_NAMESPACE LLBC_String &file);

    void RWTest(const LLBC_NAMESPACE LLBC_String &file);

    void DeleteTest(const LLBC_NAMESPACE LLBC_String &file);

    void MoveTest(const LLBC_NAMESPACE LLBC_String &file1,
                  const LLBC_NAMESPACE LLBC_String &file2);

    void CopyTest(const LLBC_NAMESPACE LLBC_String &file1,
                  const LLBC_NAMESPACE LLBC_String &file2);

private:
    LLBC_NAMESPACE LLBC_File m_file1;
    LLBC_NAMESPACE LLBC_File m_file2;
};

#endif // !__LLBC_TEST_CASE_CORE_FILE_FILE_H__
