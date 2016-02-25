/**
 * @file    TestCase_Core_File_File.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/01/23
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
    TestCase_Core_File_File();
    virtual ~TestCase_Core_File_File();

public:
    virtual int Run(int argc, char *argv[]);

private:
    bool FileModeDescTest();
    bool OpenCloseTest();
    bool GetXXXMethodsTest();
    bool ReadWriteTest();
    bool FileAttributeTest();
    bool CopyFileTest();
    bool MoveFileTest();

private:
    void PrintFileAttributes(const LLBC_FileAttributes &fileAttrs);

private:
    LLBC_String _testFileName;
};

#endif // !__LLBC_TEST_CASE_CORE_FILE_FILE_H__
