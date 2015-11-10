/**
 * @file    TestCase_Com_DataType.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/03/31
 * @version 1.0
 *
 * @brief
 */

#include "common/TestCase_Com_DataType.h"

int TestCase_Com_DataType::Run(int argc, char *argv[])
{
    LLBC_PrintLine("llbc library test case(datatype test):");

    // raw types test.
    this->RawTest();

    // string base test.
    this->StringBaseTest();

    // utf8 test.
    this->StringUTF8Test();

    // split test.
    this->StringSplitTest("com.packet.battle.");
    this->StringSplitTest("sys.io.stdout");
    this->StringSplitTest(".os.path");
    this->StringSplitTest(".");
    this->StringSplitTest("a.b.c.d.");
    this->StringSplitTest("a.b.c.d.", 3);
    this->StringSplitTest("X.Y.Z", -1, "/");

    // strip test.
    this->StringStripTest();

    getchar();

    return 0;
}

void TestCase_Com_DataType::RawTest()
{
    LLBC_PrintLine("Raw types test:");
    LLBC_PrintLine("sizeof(sint8): %lu", sizeof(sint8));
    LLBC_PrintLine("sizeof(sint16): %lu", sizeof(sint16));
    LLBC_PrintLine("sizeof(sint32): %lu", sizeof(sint32));
    LLBC_PrintLine("sizeof(sint64): %lu", sizeof(sint64));
    LLBC_PrintLine("");

    LLBC_PrintLine("sizeof(uint8): %lu", sizeof(uint8));
    LLBC_PrintLine("sizeof(uint16): %lu", sizeof(sint16));
    LLBC_PrintLine("sizeof(uint32): %lu", sizeof(sint32));
    LLBC_PrintLine("sizeof(uint64): %lu", sizeof(sint64));
    LLBC_PrintLine("");

    LLBC_PrintLine("sizeof(tchar): %lu", sizeof(tchar));

    LLBC_PrintLine("\n");
}

void TestCase_Com_DataType::StringBaseTest()
{
    LLBC_PrintLine("String base functions test:");

    LLBC_String testStr;
    testStr.format("%s", "hello world!");
    testStr.append_format("%s", "hello world!");
    LLBC_PrintLine("LLBC_String::format/append_format test: %s", testStr.c_str());

    LLBC_String testStr2;
    for(int i = 0; i < 1000; i++)
    {
        testStr.append("hello world!");
    }

    testStr2.append_format("%s", testStr.c_str());
    LLBC_PrintLine("LLBC_String:format large string test: %s", testStr2.c_str());

    // tolower/toupper test.
    testStr = "Hello WoRlD!";
    LLBC_PrintLine("'%s' to lower: '%s'", testStr.c_str(), testStr.tolower().c_str());
    LLBC_PrintLine("'%s' to upper: '%s'", testStr.c_str(), testStr.toupper().c_str());

    LLBC_PrintLine("\n");
}

void TestCase_Com_DataType::StringUTF8Test()
{
    LLBC_PrintLine("UTF-8 test:");

    LLBC_String utf8Str;
    utf8Str.append("\xef\xbb\xbf"); // Add bomb.
    LLBC_PrintLine("utf8Str exist bomb? %s", utf8Str.has_utf8_bomb() ? "true" : "false");

    // 0000000: e4bd a0e5 a5bd e4b8 96e7 958c 4865 6c6c  ............Hell
    // 0000010: 6f20 576f 726c 640a                      o World.
    utf8Str.append(1, (char)0xe4).append(1, (char)0xbd).append(1, (char)0xa0);
    utf8Str.append(1, (char)0xe5).append(1, (char)0xa5).append(1, (char)0xbd);
    utf8Str.append(1, (char)0xe4).append(1, (char)0xb8).append(1, (char)0x96);
    utf8Str.append(1, (char)0xe7).append(1, (char)0x95).append(1, (char)0x8c);
    utf8Str.append("Hello World");
    LLBC_PrintLine("When add string, len: %ld, utf8 len: %ld", utf8Str.length(), utf8Str.length_with_utf8());

    // split_utf8_string().
    std::vector<LLBC_String> utf8Vec;
    utf8Str.split_utf8_string(1, utf8Vec);
    LLBC_PrintLine("Split utf8 string done, first str size: %ld, second str size: %ld", utf8Vec[0].length(), utf8Vec[1].length());
    utf8Str.split_utf8_string(-1, utf8Vec);
    LLBC_PrintLine("Reverse split utf8 string done, first str size: %ld, second str size: %ld", utf8Vec[0].length(), utf8Vec[1].length());

    // trim/add bomb.
    utf8Str.trim_utf8_bomb();
    utf8Str.add_utf8_bomb();

    // scatter_utf8_string().
    utf8Str.scatter_utf8_string(utf8Vec, 0);
    utf8Str.scatter_utf8_string(utf8Vec, 1);
    utf8Str.scatter_utf8_string(utf8Vec, 2);
    utf8Str.scatter_utf8_string(utf8Vec, 3);
    LLBC_PrintLine("After scatter utf8 string, vector size: %ld", utf8Vec.size());

    LLBC_PrintLine("\n");
}

void TestCase_Com_DataType::StringStripTest()
{
    LLBC_PrintLine("Strip test:");
    LLBC_String str = "\t \t Hello World! \t \t";
    LLBC_PrintLine("Before strip, str: %s, len: %ld", str.c_str(), str.length());

    str.strip();
    LLBC_PrintLine("After strip, str: %s, len: %ld", str.c_str(), str.length());
}

void TestCase_Com_DataType::StringSplitTest(const LLBC_String &str, size_t maxSplit, const LLBC_String &sep)
{
    LLBC_PrintLine("Will split string(maxSplit: %ld, sep: %s): %s", maxSplit, sep.c_str(), str.c_str());
    std::vector<LLBC_String> splitted = str.split(sep, maxSplit);
    LLBC_PrintLine("Split use separator:., size: %ld", splitted.size());
    for (size_t i = 0; i < splitted.size(); i++)
        LLBC_PrintLine("    \"%s\"", splitted[i].c_str());
}
