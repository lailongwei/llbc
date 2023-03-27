// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), to deal in 
// the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
// the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include "core/file/TestCase_Core_File_File.h"

TestCase_Core_File_File::TestCase_Core_File_File()
: _testFileName("core_file_file__TestFile.test")
{
#if LLBC_TARGET_PLATFORM_IPHONE
    _testFileName = LLBC_Directory::Join(LLBC_Directory::TempDir(), _testFileName);
#endif // iPhone
}

TestCase_Core_File_File::~TestCase_Core_File_File()
{
    LLBC_File::DeleteFile(_testFileName);
}

int TestCase_Core_File_File::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Core/File test:");

    int retCode = LLBC_FAILED;
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

        retCode = LLBC_OK;
    } while (0);

    LLBC_PrintLn("Press any key to continue...");
    getchar();

    return retCode;
}

bool TestCase_Core_File_File::FileModeDescTest()
{
    LLBC_PrintLn("LLBC_FileMode describe test:");
    LLBC_PrintLn("LLBC_FileMode::TextRead(%08x): %s", LLBC_FileMode::TextRead, LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::TextRead).c_str());
    LLBC_PrintLn("LLBC_FileMode::TextWrite(%08x): %s", LLBC_FileMode::TextWrite, LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::TextWrite).c_str());
    LLBC_PrintLn("LLBC_FileMode::TextReadWrite(%08x): %s", LLBC_FileMode::TextReadWrite, 
        LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::TextReadWrite).c_str());
    LLBC_PrintLn("LLBC_FileMode::TextAppendWrite(%08x): %s", LLBC_FileMode::TextAppendWrite, 
        LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::TextAppendWrite).c_str());
    LLBC_PrintLn("LLBC_FileMode::TextAppendReadWrite(%08x): %s", LLBC_FileMode::TextAppendReadWrite, 
        LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::TextAppendReadWrite).c_str());

    LLBC_PrintLn("LLBC_FileMode::BinaryRead(%08x): %s", LLBC_FileMode::BinaryRead, 
        LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::BinaryRead).c_str());
    LLBC_PrintLn("LLBC_FileMode::BinaryWrite(%08x): %s", LLBC_FileMode::BinaryWrite, 
        LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::BinaryWrite).c_str());
    LLBC_PrintLn("LLBC_FileMode::BinaryReadWrite(%08x): %s", LLBC_FileMode::BinaryReadWrite, 
        LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::BinaryReadWrite).c_str());
    LLBC_PrintLn("LLBC_FileMode::BinaryAppendWrite(%08x): %s", LLBC_FileMode::BinaryAppendWrite, 
        LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::BinaryAppendWrite).c_str());
    LLBC_PrintLn("LLBC_FileMode::BinaryAppendReadWrite(%08x): %s", LLBC_FileMode::BinaryAppendReadWrite, 
        LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::BinaryAppendReadWrite).c_str());

    LLBC_Print("\n");

    return true;
}

bool TestCase_Core_File_File::OpenCloseTest()
{
    LLBC_PrintLn("Open/Close file test:");

    // Open/Close use BinaryRead mode.
    LLBC_String fileName = _testFileName;
    LLBC_PrintLn("Open file use constructor method(BinaryRead): %s", fileName.c_str());
    LLBC_File file(fileName, LLBC_FileMode::BinaryRead);
    if (file.IsOpened())
    {
        LLBC_PrintLn("File opened, test failed!!!");
        return false;
    }

    LLBC_PrintLn("File open failed, error: %s", LLBC_FormatLastError());

    // Open/Close use BinaryWrite mode.
    LLBC_PrintLn("Open file use constructor method(BinaryWrite): %s", fileName.c_str());
    LLBC_File file2(fileName, LLBC_FileMode::BinaryWrite);
    if (!file2.IsOpened())
    {
        LLBC_PrintLn("File not open, error: %s, test failed!!!", LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLn("File open success, close it");
    file2.Close();

    // Open/Close file use Open method.
    LLBC_PrintLn("Open file use Open() method(BinaryRead): %s", fileName.c_str());
    LLBC_File file3;
    if (file3.Open(fileName, LLBC_FileMode::BinaryRead) != 0)
    {
        LLBC_PrintLn("File open failed, error: %s, test failed", LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLn("Open successful, reopen it(LastestMode):");
    if (file3.ReOpen(LLBC_FileMode::LastestMode) != 0)
    {
        LLBC_PrintLn("Reopen file failed, error: %s", LLBC_FormatLastError());
        return false;
    }
    else
    {
        LLBC_PrintLn("Reopen success, mode: %s", LLBC_FileMode::GetFileModeDesc(file3.GetFileMode()).c_str());
    }

    LLBC_PrintLn("Reopen successful, reopen again(BinaryWrite):");
    if (file3.ReOpen(LLBC_FileMode::BinaryWrite) != 0)
    {
        LLBC_PrintLn("Reopen file failed, error: %s", LLBC_FormatLastError());
        return false;
    }

    file3.Close();

    // Final, delete test file.
    LLBC_PrintLn("Delete test file");
    if (LLBC_File::DeleteFile(fileName) != LLBC_OK)
    {
        LLBC_PrintLn("Delete file failed, error: %s", LLBC_FormatLastError());
        return false;
    }

    LLBC_Print("\n");

    return true;
}

bool TestCase_Core_File_File::GetXXXMethodsTest()
{
    LLBC_PrintLn("GetXXXMethods test:");

    // Open file as BinaryWrite mode for test.
    LLBC_File file(_testFileName, LLBC_FileMode::BinaryWrite);
    if (!file.IsOpened())
    {
        LLBC_PrintLn("Open file to test failed, error: %s", LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLn("File %s opened", _testFileName.c_str());

    // GetFileNo():
    LLBC_PrintLn("LLBC_File::GetFileNo(): %d", file.GetFileNo());
    // GetFileHandle():
    LLBC_PrintLn("LLBC_File::GetFileHandle()(FILE *): %p", file.GetFileHandle());
    // GetFileMode():
    LLBC_PrintLn("LLBC_File::GetFileMode(): %s", LLBC_FileMode::GetFileModeDesc(file.GetFileMode()).c_str());

    LLBC_Print("\n");

    return true;
}

bool TestCase_Core_File_File::ReadWriteTest()
{
    LLBC_PrintLn("Read/Write test:");
    // Open file as BinaryWrite mode for test.
    LLBC_File file(_testFileName, LLBC_FileMode::BinaryReadWrite);
    if (!file.IsOpened())
    {
        LLBC_PrintLn("Open file to test failed, error: %s", LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLn("File %s opened", _testFileName.c_str());

    // Write raw data:
    LLBC_PrintLn("Write (bool)true, (bool)false, (sint8)-8, (uint8)8, (sint16)-16, (uint16)16, "
        "(sint32)-32, (uint32)32, (sint64)-64, (uint64)64, (float)3.14, (double)1.618");
    file.Write(true), file.Write(false);
    file.Write(static_cast<sint8>(-8)); file.Write(static_cast<uint8>(8));
    file.Write(static_cast<sint16>(-16)); file.Write(static_cast<uint16>(16));
    file.Write(static_cast<sint32>(-32)); file.Write(static_cast<uint32>(32));
    file.Write(static_cast<sint64>(-64)); file.Write(static_cast<uint64>(64));
    file.Write(static_cast<float>(3.14)); file.Write(static_cast<double>(1.618));

    // Seek file pointer to file begin.
    LLBC_PrintLn("Seek file pointer to begin.");
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
    LLBC_PrintLn("Read it: %s, %s, %d, %u, %d, %u, %d, %u, %lld, %llu, %f, %f",
        trueVal ? "true" : "false", falseVal ? "true" : "false",
        sint8Val, uint8Val, sint16Val, uint16Val, sint32Val, uint32Val,
        sint64Val, uint64Val, floatVal, doubleVal);

    LLBC_PrintLn("Seek(Current, -sizeof(double)), and read again the double value:");
    file.Seek(LLBC_FileSeekOrigin::Current, -8);
    doubleVal = 0.0; file.Read(doubleVal);
    LLBC_PrintLn("The doubleVal: %f", doubleVal);

    LLBC_PrintLn("Seek(End, -sizeof(double)), and read again the double value:");
    file.Seek(LLBC_FileSeekOrigin::End, -8);
    doubleVal = 0.0; file.Read(doubleVal);
    LLBC_PrintLn("The doubleVal: %f", doubleVal);

    LLBC_PrintLn("Now file size: %ld, file pos: %ld", file.GetFileSize(), file.GetFilePosition());
    file.Seek(LLBC_FileSeekOrigin::Current, -16);
    LLBC_PrintLn("After Seek(Current, -16), file size: %ld, file pos: %ld", file.GetFileSize(), file.GetFilePosition());

    // Test ReadLine/WriteLine/ReadToEnd methods:
    LLBC_PrintLn("ReOpen file for test ReadLine/WriteLine/ReadToEnd methods:");
    file.ReOpen(LLBC_FileMode::BinaryReadWrite);

    LLBC_PrintLn("WriteLine(\"Hello World!\"):");
    file.WriteLine("Hello World!");
    LLBC_PrintLn("WriteLine(\"Hey, Judy!\"):");
    file.WriteLine("Hey, Judy!");

    file.SetFilePosition(0);
    LLBC_PrintLn("Read lines:");
    LLBC_PrintLn("First line: %s", file.ReadLine().c_str());
    LLBC_PrintLn("Second line: %s", file.ReadLine().c_str());
    const LLBC_String notExistLine = file.ReadLine();
    LLBC_PrintLn("Not exist line: %s, error: %s", notExistLine.c_str(), LLBC_FormatLastError());

    LLBC_Print("\n");

    return true;
}

bool TestCase_Core_File_File::FileAttributeTest()
{
    LLBC_PrintLn("file attribute about test:");
    LLBC_File file(_testFileName, LLBC_FileMode::BinaryRead);
    if (!file.IsOpened())
    {
        LLBC_PrintLn("Open file to test failed, error: %s", LLBC_FormatLastError());
        return false;
    }

    LLBC_FileAttributes fileAttrs;
    if (file.GetFileAttributes(fileAttrs) != LLBC_OK)
    {
        LLBC_PrintLn("Failed to get file attributes, error: %s", LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLn("File %s attributes got, print it: ", _testFileName.c_str());
    PrintFileAttributes(fileAttrs);

    // Test directory attributes:
    if (LLBC_File::GetFileAttributes(".", fileAttrs) != LLBC_OK)
    {
        LLBC_PrintLn("Failed to get file attributes, error: %s", LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLn("File %s attributes got, print it: ", ".");
    PrintFileAttributes(fileAttrs);

    file.Close();

    LLBC_PrintLn("Touch file test, file: %s", _testFileName.c_str());
    LLBC_PrintLn("Touch all times to now:");
    if (LLBC_File::TouchFile(_testFileName, true, nullptr, true, nullptr) != LLBC_OK)
    {
        LLBC_PrintLn("Failed to touch file: %s, error: %s", _testFileName.c_str(), LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLn("Touched, attributes:");
    LLBC_File::GetFileAttributes(_testFileName, fileAttrs);
    PrintFileAttributes(fileAttrs);

    LLBC_PrintLn("Sleep 2 seconds...");
    LLBC_Sleep(2000);
    LLBC_PrintLn("Touch last access time to now");
    LLBC_File::TouchFile(_testFileName, true, nullptr, false, nullptr);
    LLBC_PrintLn("Touched, attributes:");
    LLBC_File::GetFileAttributes(_testFileName, fileAttrs);
    PrintFileAttributes(fileAttrs);

    LLBC_PrintLn("Sleep 2 seconds...");
    LLBC_Sleep(2000);
    LLBC_PrintLn("Touch last modify time to now");
    LLBC_File::TouchFile(_testFileName, false, nullptr, true, nullptr);
    LLBC_PrintLn("Touched, attributes:");
    LLBC_File::GetFileAttributes(_testFileName, fileAttrs);
    PrintFileAttributes(fileAttrs);

    LLBC_Print("\n");

    return true;
}

bool TestCase_Core_File_File::CopyFileTest()
{
    LLBC_PrintLn("Copy file test:");

    const LLBC_String copyFileName = _testFileName + ".copy";
    LLBC_File file(_testFileName, LLBC_FileMode::BinaryReadWrite);
    if (!file.IsOpened())
    {
        LLBC_PrintLn("Open test file[%s] failed, error: %s", _testFileName.c_str(), LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLn("test file[name: %s, will copy] opened, write line string: Hello World!", _testFileName.c_str());
    file.WriteLine("Hello World");

    LLBC_PrintLn("Begin copy(overlapped): %s ---> %s", _testFileName.c_str(), copyFileName.c_str());
    if (file.CopyFile(copyFileName, true) != LLBC_OK)
    {
        LLBC_PrintLn("Copy file failed, error: %s", LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLn("Open the copy file:");
    LLBC_File copyFile(copyFileName, LLBC_FileMode::BinaryRead);
    if (!copyFile.IsOpened())
    {
        LLBC_PrintLn("Failed to open copy file, error: %s", LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLn("Copy file opened, content: %s", copyFile.ReadToEnd().c_str());
    copyFile.Close();

    LLBC_PrintLn("Copy again(don't overlapped): %s ---> %s", _testFileName.c_str(), copyFileName.c_str());
    if (file.CopyFile(copyFileName, false) == LLBC_OK)
    {
        LLBC_PrintLn("Copy success, failed. check your code!");
        LLBC_File::DeleteFile(copyFileName);

        return false;
    }
    else
    {
        LLBC_PrintLn("Copy failed, error: %s, right!", LLBC_FormatLastError());
    }

    LLBC_PrintLn("Delete copy file");
    LLBC_File::DeleteFile(copyFileName);

    LLBC_Print("\n");

    return true;
}

bool TestCase_Core_File_File::MoveFileTest()
{
    LLBC_PrintLn("Move file test:");

    const LLBC_String moveFileName = _testFileName + ".move";
    LLBC_File file(_testFileName, LLBC_FileMode::BinaryReadWrite);
    if (!file.IsOpened())
    {
        LLBC_PrintLn("Open test file[%s] failed, error: %s", _testFileName.c_str(), LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLn("test file[name: %s, will move] opened, write line string: Hello World!", _testFileName.c_str());
    file.WriteLine("Hello World");

    LLBC_PrintLn("Begin move(overlapped): %s ---> %s", _testFileName.c_str(), moveFileName.c_str());
    if (file.MoveFile(moveFileName, true) != LLBC_OK)
    {
        LLBC_PrintLn("Move file failed, error: %s", LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLn("Open the move file:");
    LLBC_File moveFile(moveFileName, LLBC_FileMode::BinaryRead);
    if (!moveFile.IsOpened())
    {
        LLBC_PrintLn("Failed to open move file, error: %s", LLBC_FormatLastError());
        return false;
    }

    LLBC_PrintLn("Move file opened, content: %s", moveFile.ReadToEnd().c_str());
    moveFile.Close();

    const LLBC_String copyFileName = _testFileName + ".copy";
    LLBC_File::CopyFile(moveFileName, copyFileName, true);
    LLBC_PrintLn("Copy move file and move again(don't overlapped): %s ---> %s", copyFileName.c_str(), copyFileName.c_str());
    if (LLBC_File::MoveFile(copyFileName, moveFileName, false) == LLBC_OK)
    {
        LLBC_PrintLn("Move success, failed. check your code!");
        LLBC_File::DeleteFile(copyFileName);
        LLBC_File::DeleteFile(moveFileName);

        return false;
    }
    else
    {
        LLBC_PrintLn("Move failed, error: %s, right!", LLBC_FormatLastError());
    }

    LLBC_PrintLn("Delete copy file and move file");
    LLBC_File::DeleteFile(copyFileName);
    LLBC_File::DeleteFile(moveFileName);

    LLBC_Print("\n");

    return true;
}

void TestCase_Core_File_File::PrintFileAttributes(const LLBC_FileAttributes &fileAttrs)
{
    LLBC_PrintLn("    Readable: %s", fileAttrs.readable ? "true" : "false");
    LLBC_PrintLn("    Writable: %s", fileAttrs.writable ? "true" : "false");
    LLBC_PrintLn("    Execable: %s", fileAttrs.execable ? "true" : "false");

    LLBC_PrintLn("    Is directory: %s", fileAttrs.isDirectory ? "true" : "false");

#if LLBC_TARGET_PLATFORM_WIN32
    const LLBC_Time createTime = LLBC_Time::FromTimeSpec(fileAttrs.createTime);
    LLBC_PrintLn("    create time: %s", createTime.Format().c_str());
#else
    LLBC_PrintLn("    last change status time: %s",  LLBC_Time::FromTimeSpec(fileAttrs.lastChangeStatusTime).Format().c_str());
#endif
    LLBC_PrintLn("    last modify time: %s", LLBC_Time::FromTimeSpec(fileAttrs.lastModifyTime).Format().c_str());
    LLBC_PrintLn("    last access time: %s", LLBC_Time::FromTimeSpec(fileAttrs.lastAccessTime).Format().c_str());
}
