/**
 * @file    TestCase_Core_File_File2.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/01/23
 * @version 1.0
 *
 * @brief
 */

#include "core/file/TestCase_Core_File_File.h"

TestCase_Core_File_File::TestCase_Core_File_File()
: _testFileName("core_file_file__TestFile.test")
{
}

TestCase_Core_File_File::~TestCase_Core_File_File()
{
    LLBC_File::DeleteFile(_testFileName);
}

int TestCase_Core_File_File::Run(int argc, char *argv[])
{
    LLBC_PrintLine("Core/File test:");

    int retCode = LLBC_RTN_FAILED;
    do
    {
        if (!FileModeDescTest())
            break;

        if (!OpenCloseTest())
            break;

        if (!GetXXXMethodsTest())
            break;

        if (!ReadWriteTest())
            break;

        if (!FileAttributeTest())
            break;

        if (!CopyFileTest())
            break;

        if (!MoveFileTest())
            break;

        retCode = LLBC_RTN_OK;
    } while (0);

    LLBC_PrintLine("Press any key to continue...");
    getchar();

    return LLBC_RTN_OK;
}

bool TestCase_Core_File_File::FileModeDescTest()
{
    LLBC_PrintLine("LLBC_FileMode describe test:");
    LLBC_PrintLine("LLBC_FileMode::TextRead(%08x): %s", LLBC_FileMode::TextRead, LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::TextRead).c_str());
    LLBC_PrintLine("LLBC_FileMode::TextWrite(%08x): %s", LLBC_FileMode::TextWrite, LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::TextWrite).c_str());
    LLBC_PrintLine("LLBC_FileMode::TextReadWrite(%08x): %s", LLBC_FileMode::TextReadWrite, 
        LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::TextReadWrite).c_str());
    LLBC_PrintLine("LLBC_FileMode::TextAppendWrite(%08x): %s", LLBC_FileMode::TextAppendWrite, 
        LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::TextAppendWrite).c_str());
    LLBC_PrintLine("LLBC_FileMode::TextAppendReadWrite(%08x): %s", LLBC_FileMode::TextAppendReadWrite, 
        LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::TextAppendReadWrite).c_str());

    LLBC_PrintLine("LLBC_FileMode::BinaryRead(%08x): %s", LLBC_FileMode::BinaryRead, 
        LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::BinaryRead).c_str());
    LLBC_PrintLine("LLBC_FileMode::BinaryWrite(%08x): %s", LLBC_FileMode::BinaryWrite, 
        LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::BinaryWrite).c_str());
    LLBC_PrintLine("LLBC_FileMode::BinaryReadWrite(%08x): %s", LLBC_FileMode::BinaryReadWrite, 
        LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::BinaryReadWrite).c_str());
    LLBC_PrintLine("LLBC_FileMode::BinaryAppendWrite(%08x): %s", LLBC_FileMode::BinaryAppendWrite, 
        LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::BinaryAppendWrite).c_str());
    LLBC_PrintLine("LLBC_FileMode::BinaryAppendReadWrite(%08x): %s", LLBC_FileMode::BinaryAppendReadWrite, 
        LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::BinaryAppendReadWrite).c_str());

    LLBC_PrintLine("");

    return true;
}

bool TestCase_Core_File_File::OpenCloseTest()
{
    LLBC_PrintLine("Open/Close file test:");

    // Open/Close use BinaryRead mode.
    LLBC_String fileName = _testFileName;
    LLBC_PrintLine("Open file use constructor method(BinaryRead): %s", fileName.c_str());
    LLBC_File file(fileName, LLBC_FileMode::BinaryRead);
    if (file.IsOpened())
    {
        LLBC_PrintLine("File opened, test failed!!!");
        return false;
    }

    LLBC_PrintLine("File open failed, error: %s", LLBC_FormatLastError());

    // Open/Close use BinaryWrite mode.
    LLBC_PrintLine("Open file use constructor method(BinaryWrite): %s", fileName.c_str());
    LLBC_File file2(fileName, LLBC_FileMode::BinaryWrite);
    if (!file2.IsOpened())
    {
        LLBC_PrintLine("File not open, error: %s, test failed!!!", LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLine("File open success, close it");
    file2.Close();

    // Open/Close file use Open method.
    LLBC_PrintLine("Open file use Open() method(BinaryRead): %s", fileName.c_str());
    LLBC_File file3;
    if (file3.Open(fileName, LLBC_FileMode::BinaryRead) != 0)
    {
        LLBC_PrintLine("File open failed, error: %s, test failed", LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLine("Open successful, reopen it(LastestMode):");
    if (file3.ReOpen(LLBC_FileMode::LastestMode) != 0)
    {
        LLBC_PrintLine("Reopen file failed, error: %s", LLBC_FormatLastError());
        return false;
    }
    else
    {
        LLBC_PrintLine("Reopen success, mode: %s", LLBC_FileMode::GetFileModeDesc(file3.GetFileMode()).c_str());
    }

    LLBC_PrintLine("Reopen successful, reopen again(BinaryWrite):");
    if (file3.ReOpen(LLBC_FileMode::BinaryWrite) != 0)
    {
        LLBC_PrintLine("Reopen file failed, error: %s", LLBC_FormatLastError());
        return false;
    }

    file3.Close();

    // Final, delete test file.
    LLBC_PrintLine("Delete test file");
    if (LLBC_File::DeleteFile(fileName) != LLBC_RTN_OK)
    {
        LLBC_PrintLine("Delete file failed, error: %s", LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLine("");

    return true;
}

bool TestCase_Core_File_File::GetXXXMethodsTest()
{
    LLBC_PrintLine("GetXXXMethods test:");

    // Open file as BinaryWrite mode for test.
    LLBC_File file(_testFileName, LLBC_FileMode::BinaryWrite);
    if (!file.IsOpened())
    {
        LLBC_PrintLine("Open file to test failed, error: %s", LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLine("File %s opened", _testFileName.c_str());

    // GetFileNo():
    LLBC_PrintLine("LLBC_File::GetFileNo(): %d", file.GetFileNo());
    // GetFileHandle():
    LLBC_PrintLine("LLBC_File::GetFileHandle()(FILE *): %p", file.GetFileHandle());
    // GetFileMode():
    LLBC_PrintLine("LLBC_File::GetFileMode(): %s", LLBC_FileMode::GetFileModeDesc(file.GetFileMode()).c_str());

    LLBC_PrintLine("");

    return true;
}

bool TestCase_Core_File_File::ReadWriteTest()
{
    LLBC_PrintLine("Read/Write test:");
    // Open file as BinaryWrite mode for test.
    LLBC_File file(_testFileName, LLBC_FileMode::BinaryReadWrite);
    if (!file.IsOpened())
    {
        LLBC_PrintLine("Open file to test failed, error: %s", LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLine("File %s opened", _testFileName.c_str());

    // Write raw data:
    LLBC_PrintLine("Write (bool)true, (bool)false, (sint8)-8, (uint8)8, (sint16)-16, (uint16)16, "
        "(sint32)-32, (uint32)32, (sint64)-64, (uint64)64, (float)3.14, (double)1.618");
    file.Write(true), file.Write(false);
    file.Write(static_cast<sint8>(-8)); file.Write(static_cast<uint8>(8));
    file.Write(static_cast<sint16>(-16)); file.Write(static_cast<uint16>(16));
    file.Write(static_cast<sint32>(-32)); file.Write(static_cast<uint32>(32));
    file.Write(static_cast<sint64>(-64)); file.Write(static_cast<uint64>(64));
    file.Write(static_cast<float>(3.14)); file.Write(static_cast<double>(1.618));

    // Seek file pointer to file begin.
    LLBC_PrintLine("Seek file pointer to begin.");
    file.Seek(LLBC_FileSeekOrigin::Begin, 0);

    // Read it
    bool trueVal, falseVal;
    file.Read(trueVal); file.Read(falseVal);
    sint8 sint8Val; uint8 uint8Val;
    file.Read(sint8Val); file.Read(uint8Val);
    sint16 sint16Val; uint16 uint16Val;
    file.Read(sint16Val); file.Read(uint16Val);
    sint32 sint32Val; uint32 uint32Val;
    file.Read(sint32Val); file.Read(uint32Val);
    sint64 sint64Val; uint64 uint64Val;
    file.Read(sint64Val); file.Read(uint64Val);
    float floatVal; double doubleVal;
    file.Read(floatVal); file.Read(doubleVal);
    LLBC_PrintLine("Read it: %s, %s, %d, %u, %d, %u, %d, %u, %lld, %llu, %f, %f",
        trueVal ? "true" : "false", falseVal ? "true" : "false",
        sint8Val, uint8Val, sint16Val, uint16Val, sint32Val, uint32Val,
        sint64Val, uint64Val, floatVal, doubleVal);

    LLBC_PrintLine("Seek(Current, -sizeof(double)), and read again the double value:");
    file.Seek(LLBC_FileSeekOrigin::Current, -8);
    doubleVal = 0.0; file.Read(doubleVal);
    LLBC_PrintLine("The doubleVal: %f", doubleVal);

    LLBC_PrintLine("Seek(End, -sizeof(double)), and read again the double value:");
    file.Seek(LLBC_FileSeekOrigin::End, -8);
    doubleVal = 0.0; file.Read(doubleVal);
    LLBC_PrintLine("The doubleVal: %f", doubleVal);

    LLBC_PrintLine("Now file size: %ld, file pos: %ld", file.GetFileSize(), file.GetFilePosition());
    file.Seek(LLBC_FileSeekOrigin::Current, -16);
    LLBC_PrintLine("After Seek(Current, -16), file size: %ld, file pos: %ld", file.GetFileSize(), file.GetFilePosition());

    // Test ReadLine/WriteLine/ReadToEnd methods:
    LLBC_PrintLine("ReOpen file for test ReadLine/WriteLine/ReadToEnd methods:");
    file.ReOpen(LLBC_FileMode::BinaryReadWrite);

    LLBC_PrintLine("WriteLine(\"Hello World!\"):");
    file.WriteLine("Hello World!");
    LLBC_PrintLine("WriteLine(\"Hey, Judy!\"):");
    file.WriteLine("Hey, Judy!");

    file.SetFilePosition(0);
    LLBC_PrintLine("Read lines:");
    LLBC_PrintLine("First line: %s", file.ReadLine().c_str());
    LLBC_PrintLine("Second line: %s", file.ReadLine().c_str());
    const LLBC_String notExistLine = file.ReadLine();
    LLBC_PrintLine("Not exist line: %s, error: %s", notExistLine.c_str(), LLBC_FormatLastError());

    LLBC_PrintLine("");

    return true;
}

bool TestCase_Core_File_File::FileAttributeTest()
{
    LLBC_PrintLine("file attribute about test:");
    LLBC_File file(_testFileName, LLBC_FileMode::BinaryRead);
    if (!file.IsOpened())
    {
        LLBC_PrintLine("Open file to test failed, error: %s", LLBC_FormatLastError());
        return false;
    }

    LLBC_FileAttributes fileAttrs;
    if (file.GetFileAttributes(fileAttrs) != LLBC_RTN_OK)
    {
        LLBC_PrintLine("Failed to get file attributes, error: %s", LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLine("File %s attributes got, print it: ", _testFileName.c_str());
    PrintFileAttributes(fileAttrs);

    // Test directory attributes:
    if (LLBC_File::GetFileAttributes(".", fileAttrs) != LLBC_RTN_OK)
    {
        LLBC_PrintLine("Failed to get file attributes, error: %s", LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLine("File %s attributes got, print it: ", ".");
    PrintFileAttributes(fileAttrs);

    file.Close();

    LLBC_PrintLine("Touch file test, file: %s", _testFileName.c_str());
    LLBC_PrintLine("Touch all times to now:");
    if (LLBC_File::TouchFile(_testFileName, true, NULL, true, NULL) != LLBC_RTN_OK)
    {
        LLBC_PrintLine("Failed to touch file: %s, error: %s", _testFileName.c_str(), LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLine("Touched, attributes:");
    LLBC_File::GetFileAttributes(_testFileName, fileAttrs);
    PrintFileAttributes(fileAttrs);

    LLBC_PrintLine("Sleep 2 seconds...");
    LLBC_Sleep(2000);
    LLBC_PrintLine("Touch last access time to now");
    LLBC_File::TouchFile(_testFileName, true, NULL, false, NULL);
    LLBC_PrintLine("Touched, attributes:");
    LLBC_File::GetFileAttributes(_testFileName, fileAttrs);
    PrintFileAttributes(fileAttrs);

    LLBC_PrintLine("Sleep 2 seconds...");
    LLBC_Sleep(2000);
    LLBC_PrintLine("Touch last modify time to now");
    LLBC_File::TouchFile(_testFileName, false, NULL, true, NULL);
    LLBC_PrintLine("Touched, attributes:");
    LLBC_File::GetFileAttributes(_testFileName, fileAttrs);
    PrintFileAttributes(fileAttrs);

    LLBC_PrintLine("");

    return true;
}

bool TestCase_Core_File_File::CopyFileTest()
{
    LLBC_PrintLine("Copy file test:");

    const LLBC_String copyFileName = _testFileName + ".copy";
    LLBC_File file(_testFileName, LLBC_FileMode::BinaryReadWrite);
    if (!file.IsOpened())
    {
        LLBC_PrintLine("Open test file[%s] failed, error: %s", _testFileName.c_str(), LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLine("test file[name: %s, will copy] opened, write line string: Hello World!", _testFileName.c_str());
    file.WriteLine("Hello World");

    LLBC_PrintLine("Begin copy(overlapped): %s ---> %s", _testFileName.c_str(), copyFileName.c_str());
    if (file.CopyFile(copyFileName, true) != LLBC_RTN_OK)
    {
        LLBC_PrintLine("Copy file failed, error: %s", LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLine("Open the copy file:");
    LLBC_File copyFile(copyFileName, LLBC_FileMode::BinaryRead);
    if (!copyFile.IsOpened())
    {
        LLBC_PrintLine("Failed to open copy file, error: %s", LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLine("Copy file opened, content: %s", copyFile.ReadToEnd().c_str());
    copyFile.Close();

    LLBC_PrintLine("Copy again(don't overlapped): %s ---> %s", _testFileName.c_str(), copyFileName.c_str());
    if (file.CopyFile(copyFileName, false) == LLBC_RTN_OK)
    {
        LLBC_PrintLine("Copy success, failed. check your code!");
        LLBC_File::DeleteFile(copyFileName);

        return false;
    }
    else
    {
        LLBC_PrintLine("Copy failed, error: %s, right!", LLBC_FormatLastError());
    }

    LLBC_PrintLine("Delete copy file");
    LLBC_File::DeleteFile(copyFileName);

    LLBC_PrintLine("");

    return true;
}

bool TestCase_Core_File_File::MoveFileTest()
{
    LLBC_PrintLine("Move file test:");

    const LLBC_String moveFileName = _testFileName + ".move";
    LLBC_File file(_testFileName, LLBC_FileMode::BinaryReadWrite);
    if (!file.IsOpened())
    {
        LLBC_PrintLine("Open test file[%s] failed, error: %s", _testFileName.c_str(), LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLine("test file[name: %s, will move] opened, write line string: Hello World!", _testFileName.c_str());
    file.WriteLine("Hello World");

    LLBC_PrintLine("Begin move(overlapped): %s ---> %s", _testFileName.c_str(), moveFileName.c_str());
    if (file.MoveFile(moveFileName, true) != LLBC_RTN_OK)
    {
        LLBC_PrintLine("Move file failed, error: %s", LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLine("Open the move file:");
    LLBC_File moveFile(moveFileName, LLBC_FileMode::BinaryRead);
    if (!moveFile.IsOpened())
    {
        LLBC_PrintLine("Failed to open move file, error: %s", LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLine("Move file opened, content: %s", moveFile.ReadToEnd().c_str());
    moveFile.Close();

    const LLBC_String copyFileName = _testFileName + ".copy";
    LLBC_File::CopyFile(moveFileName, copyFileName, true);
    LLBC_PrintLine("Copy move file and move again(don't overlapped): %s ---> %s", copyFileName.c_str(), copyFileName.c_str());
    if (LLBC_File::MoveFile(copyFileName, moveFileName, false) == LLBC_RTN_OK)
    {
        LLBC_PrintLine("Move success, failed. check your code!");
        LLBC_File::DeleteFile(copyFileName);
        LLBC_File::DeleteFile(moveFileName);

        return false;
    }
    else
    {
        LLBC_PrintLine("Move failed, error: %s, right!", LLBC_FormatLastError());
    }

    LLBC_PrintLine("Delete copy file and move file");
    LLBC_File::DeleteFile(copyFileName);
    LLBC_File::DeleteFile(moveFileName);

    LLBC_PrintLine("");

    return true;
}

void TestCase_Core_File_File::PrintFileAttributes(const LLBC_FileAttributes &fileAttrs)
{
    LLBC_PrintLine("    Readable: %s", fileAttrs.readable ? "true" : "false");
    LLBC_PrintLine("    Writable: %s", fileAttrs.writable ? "true" : "false");
    LLBC_PrintLine("    Execable: %s", fileAttrs.execable ? "true" : "false");

    LLBC_PrintLine("    Is directory: %s", fileAttrs.isDirectory ? "true" : "false");

#if LLBC_TARGET_PLATFORM_WIN32
    const LLBC_Time createTime(fileAttrs.createTime);
    LLBC_PrintLine("    create time: %s", createTime.Format().c_str());
#else
    LLBC_PrintLine("    last change status time: %s",  LLBC_Time(fileAttrs.lastChangeStatusTime).Format().c_str());
#endif
    LLBC_PrintLine("    last modify time: %s", LLBC_Time(fileAttrs.lastModifyTime).Format().c_str());
    LLBC_PrintLine("    last access time: %s", LLBC_Time(fileAttrs.lastAccessTime).Format().c_str());
}
