/**
 * @file    TestCase_Core_File_Directory.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/09
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_FILE_DIRECTORY_H__
#define __LLBC_TEST_CASE_CORE_FILE_DIRECTORY_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_File_Directory : public LLBC_BaseTestCase
{
public:
    virtual int Run(int argc, char *argv[]);

private:
    int CurDirTest();
    int ExistsTest();
    int CreateRemoveTest();
    int AbsPathTest();
    int JoinTest();
    int SplitExtTest();
    int GetFilesTest();
    int GetDirectoriesTest();
    int ModuleFileTest();
    int DirNameBaseNameTest();
    int MiscTest();

private:
    int JoinTest(const LLBC_Strings &pathParts);
    int GetFilesTest(const LLBC_String &path, bool recursive);
    int GetDirectoriesTest(const LLBC_String &path, bool recursive);
};

#endif // __LLBC_TEST_CASE_CORE_FILE_DIRECTORY_H__
