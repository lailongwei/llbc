/**
 * @file    TestCase_Core_Transcoder.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/09/16
 * @version 1.0
 *
 * @brief
 */

#include "core/transcoder/TestCase_Core_Transcoder.h"

TestCase_Core_Transcoder::TestCase_Core_Transcoder()
{
}

TestCase_Core_Transcoder::~TestCase_Core_Transcoder()
{
}

int TestCase_Core_Transcoder::Run(int argc, char *argv[])
{
    LLBC_PrintLine("core/transfer test:");
	LLBC_PrintLine("sizeof(wchar): %lu", sizeof(wchar));
    LLBC_PrintLine("All test files string encoding was nobomb format!!");

#if LLBC_TARGET_PLATFORM_IPHONE
    LLBC_PrintLine("In Iphone platform, not support this test case!");
    return -1;
#endif

    // Simple convert test.
    if(this->SimpleConvertTest() != 0)
    {
        getchar();
        return -1;
    }

    // UTF8 format string convert test.
    const LLBC_String utf8FileName = "core/transcoder/Transcoder_Test_UTF8.txt";
    if(this->UTF8ConvertTest(utf8FileName) != 0)
    {
        getchar();
        return -1;
    }

    // GBK format string convert test.
    const LLBC_String gbkFileName = "core/transcoder/Transcoder_Test_GBK.txt";
    if(this->GBKConvertTest(gbkFileName) != 0)
    {
        getchar();
        return -1;
    }

    // UTF16 format string convert test.
    const LLBC_String utf16FileName = "core/transcoder/Transcoder_Test_UTF16.txt";
    if(this->UTF16ConvertTest(utf16FileName) != 0)
    {
        getchar();
        return -1;
    }

    LLBC_PrintLine("Press any key to continue ... ...");
    getchar();

    return 0;
}

int TestCase_Core_Transcoder::SimpleConvertTest()
{
    LLBC_PrintLine("Simple convert test");

    LLBC_Transcoder *transcoder = LLBC_TranscoderSingleton;

    // Test coding string(gbk coding): ÄãºÃ.
    // GBK binary view:      c4 e3 ba c3
    const LLBC_String gbkBinaryData = "c4 e3 ba c3";
    // UTF8 binary view:     e4 bd a0 e5 a5 bd
    const LLBC_String utf8BinaryData = "e4 bd a0 e5 a5 bd";
    // UTF16-LE binary view: 60 4f 7d 59
    const LLBC_String utf16BinaryData = "60 4f 7d 59";

    LLBC_PrintLine("Test string GBK binary view: %s", gbkBinaryData.c_str());
    LLBC_PrintLine("Test string UTF8 binary view: %s", utf8BinaryData.c_str());
    LLBC_PrintLine("Test string UTF16-LE binary view: %s", utf16BinaryData.c_str());

    // Test GBK->UTF8.
    LLBC_String utf8Str;
    LLBC_String gbkStr = "ÄãºÃ";
    if(transcoder->MultiByteToMultiByte(
        "GBK", gbkStr, "UTF-8", utf8Str) != LLBC_OK)
    {
        LLBC_PrintLine("GBK->UTF8 convert failed, reason: %s", LLBC_FormatLastError());
        return -1;
    }
    LLBC_PrintLine("GBK->UTF8 consvert successed, utf8 binary view: %s", 
        LLBC_Byte2Hex(utf8Str.c_str(), utf8Str.length()).c_str() );
    // Test GBK->UTF16.
    LLBC_WString utf16Str;
    if(transcoder->MultiByteToWideChar("GBK", gbkStr, utf16Str) != LLBC_OK)
    {
        LLBC_PrintLine("GBK->UTF16 convert failed, reason: %s", LLBC_FormatLastError());
        return -1;
    }
    LLBC_PrintLine("GBK->UTF16 convert successed, utf16 binary view: %s",
        LLBC_Byte2Hex(utf16Str.data(), utf16Str.size() * sizeof(wchar)).c_str() );

    // Test UTF8->GBK.
    if(transcoder->MultiByteToMultiByte("UTF-8", utf8Str, "GBK", gbkStr) != LLBC_OK)
    {
        LLBC_PrintLine("UTF8->GBK convert failed, reason: %s", LLBC_FormatLastError());
        return -1;
    }
    LLBC_PrintLine("UTF8->GBK convert successed, gbk binary view: %s",
        LLBC_Byte2Hex(gbkStr.c_str(), gbkStr.length()).c_str() );
    // Test UTF8->UTF16.
    if(transcoder->MultiByteToWideChar("UTF8", utf8Str, utf16Str) != LLBC_OK)
    {
        LLBC_PrintLine("UTF8->UTF16-LE convert failed, reason: %s", LLBC_FormatLastError());
        return -1;
    }
    LLBC_PrintLine("UTF8->UTF16-LE convert successed, utf16 binary view: %s",
        LLBC_Byte2Hex(utf16Str.c_str(), utf16Str.length() * sizeof(wchar)).c_str() );

    return 0;
}

int TestCase_Core_Transcoder::UTF8ConvertTest(const LLBC_String &inputFile)
{
    LLBC_File input;
    if(input.Open(inputFile, LLBC_FileMode::BinaryRead) != LLBC_OK)
    {
        LLBC_PrintLine("UTF8 input file open failed, reason: %s.", LLBC_FormatLastError());
        return -1;
    }

    LLBC_String utf8Str = input.ReadToEnd();

    // Test UTF8->UTF16
    LLBC_File output;
    const LLBC_String utf16FileName = LLBC_BaseName(inputFile, false) + "_out_utf8_to_utf16." + LLBC_ExtensionName(inputFile);
    if(output.Open(utf16FileName, LLBC_FileMode::BinaryWrite))
    {
        LLBC_PrintLine("UTF16 output file open failed, reason: %s", LLBC_FormatLastError());
        return -1;
    }

    LLBC_WString utf16Str;
    if(LLBC_TranscoderSingleton->MultiByteToWideChar("UTF8", utf8Str, utf16Str) != LLBC_OK)
    {
        LLBC_PrintLine("UTF8->UTF16 convert failed, reason: %s", LLBC_FormatLastError());
        return -1;
    }

    output.Write(utf16Str.data(), utf16Str.size() * sizeof(wchar));
    output.Flush();
    output.Close();

    LLBC_PrintLine("UTF8 file[%s] convert to UTF16 successfully, output file: %s", inputFile.c_str(), utf16FileName.c_str());

    // Test UTF8->GBK.
    const LLBC_String gbkFileName = LLBC_BaseName(inputFile, false) + "_out_utf8_to_gbk." + LLBC_ExtensionName(inputFile);
    if(output.Open(gbkFileName, LLBC_FileMode::BinaryWrite) != LLBC_OK)
    {
        LLBC_PrintLine("UTF8 output file open failed, reason: %s", LLBC_FormatLastError());
        return -1;
    }

    LLBC_String gbkStr;
    if(LLBC_TranscoderSingleton->MultiByteToMultiByte("UTF8", utf8Str, "GBK", gbkStr) != LLBC_OK)
    {
        LLBC_PrintLine("UTF8->GBK convert failed, reason: %s", LLBC_FormatLastError());
        return -1;
    }

    output.Write(gbkStr.c_str(), gbkStr.length());
    output.Flush();
    output.Close();

    LLBC_PrintLine("UTF8 file[%s] convert to GBK successfully, output file: %s", inputFile.c_str(), gbkFileName.c_str());

    std::vector<LLBC_String> willDeleteFiles;
    willDeleteFiles.push_back(utf16FileName);
    willDeleteFiles.push_back(gbkFileName);
    this->DeleteOutputFiles(willDeleteFiles);

    return 0;
}

int TestCase_Core_Transcoder::GBKConvertTest(const LLBC_String &inputFile)
{
    LLBC_File input;
    if(input.Open(inputFile, LLBC_FileMode::BinaryRead) != LLBC_OK)
    {
        LLBC_PrintLine("Open file [%s] failed, reason: %s", inputFile.c_str(), LLBC_FormatLastError());
        return -1;
    }

    LLBC_String gbkStr = input.ReadToEnd();

    // GBK->UTF16.
    LLBC_File output;
    const LLBC_String utf16FileName = LLBC_BaseName(inputFile) + "_out_gbk_to_utf16." + LLBC_ExtensionName(inputFile);
    if(output.Open(utf16FileName, LLBC_FileMode::BinaryWrite) != LLBC_OK)
    {
        LLBC_PrintLine("UTF16 output file [%s] open failed, reason: %s", utf16FileName.c_str(), LLBC_FormatLastError());
        return -1;
    }

    LLBC_WString utf16Str;
    if(LLBC_TranscoderSingleton->MultiByteToWideChar("GBK", gbkStr, utf16Str) != LLBC_OK)
    {
        LLBC_PrintLine("GBK->UTF16 convert failed, reason: %s", LLBC_FormatLastError());
        return -1;
    }

    output.Write(utf16Str.c_str(), utf16Str.length() * sizeof(wchar));
    output.Close();

    LLBC_PrintLine("GBK file[%s] convert to UTF16 successfully, output file: %s", inputFile.c_str(), utf16FileName.c_str());

    // GBK->UTF8.
    const LLBC_String utf8FileName = LLBC_BaseName(inputFile) + "_out_gbk_to_utf8." + LLBC_ExtensionName(inputFile);
    if(output.Open(utf8FileName, LLBC_FileMode::BinaryWrite) != LLBC_OK)
    {
        LLBC_PrintLine("GBK->UTF8 output file [%s] open failed, reason: %s", utf8FileName.c_str(), LLBC_FormatLastError());
        return -1;
    }

    LLBC_String utf8Str;
    if(LLBC_TranscoderSingleton->MultiByteToMultiByte("GBK", gbkStr, "UTF8", utf8Str) != LLBC_OK)
    {
        LLBC_PrintLine("GBK->UTF8 convert failed, reason: %s", LLBC_FormatLastError());
        return -1;
    }

    output.Write(utf8Str.c_str(), utf8Str.length());
    output.Close();

    LLBC_PrintLine("GBK file[%s] convert to UTF8 successfully, output file: %s", inputFile.c_str(), utf8FileName.c_str());

    std::vector<LLBC_String> willDeleteFiles;
    willDeleteFiles.push_back(utf16FileName);
    willDeleteFiles.push_back(utf8FileName);
    this->DeleteOutputFiles(willDeleteFiles);

    return 0;
}

int TestCase_Core_Transcoder::UTF16ConvertTest(const LLBC_String &inputFile)
{
    // Test UTF16->UTF8.
    const LLBC_String utf8FileName = LLBC_BaseName(inputFile) + "_out_utf16_to_utf8." + LLBC_ExtensionName(inputFile);
    if(LLBC_TranscoderSingleton->WideCharFileToMultiByteFile("UTF8", inputFile, utf8FileName) != LLBC_OK)
    {
        LLBC_PrintLine("UTF16->UTF8 convert failed, reason: %s", LLBC_FormatLastError());
        return -1;
    }

    LLBC_PrintLine("UTF16 file[%s] convert to UTF8 successfully, output file: %s", inputFile.c_str(), utf8FileName.c_str());

    // Test UTF16->GBK.
    const LLBC_String gbkFileName = LLBC_BaseName(inputFile) + "_out_utf16_to_gbk." + LLBC_ExtensionName(inputFile);
    if(LLBC_TranscoderSingleton->WideCharFileToMultiByteFile("GBK", inputFile, gbkFileName) != LLBC_OK)
    {
        LLBC_PrintLine("UTF16->GBK convert failed, reason: %s", LLBC_FormatLastError());
        return -1;
    }

    LLBC_PrintLine("UTF16 file[%s] convert to GBK successfully, output file: %s", inputFile.c_str(), gbkFileName.c_str());

    std::vector<LLBC_String> willDeleteFiles;
    willDeleteFiles.push_back(utf8FileName);
    willDeleteFiles.push_back(gbkFileName);
    this->DeleteOutputFiles(willDeleteFiles);

    return 0;
}

void TestCase_Core_Transcoder::DeleteOutputFiles(const std::vector<LLBC_String> &files)
{
    LLBC_PrintLine("Press any key to delete these output files ...");
    getchar();

    const size_t filesCnt = files.size();
    for(size_t i = 0; i < filesCnt; i ++)
    {
        LLBC_PrintLine("    Delete file: %s ......", files[i].c_str());
        if(LLBC_File::DeleteFile(files[i]) != LLBC_OK)
        {
            LLBC_PrintLine("[FAILED]");
        }
        else
        {
            LLBC_PrintLine("[OK]");
        }
    }
}
