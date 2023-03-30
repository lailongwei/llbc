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


#include "common/TestCase_Com_DataType.h"

int TestCase_Com_DataType::Run(int argc, char *argv[])
{
    LLBC_PrintLn("llbc library test case(datatype test):");

    // raw types test.
    RawTest();

    // string base test.
    StringBaseTest();

    // string compare test.
    StringCompareTest();

    // string logic-operation test.
    StringLogicOperationTest();

    // utf8 test.
    StringUTF8Test();

    // findreplace t est.
    StringFindReplaceTest();

    // split test.
    StringSplitTest("com.packet.battle.");
    StringSplitTest("sys.io.stdout");
    StringSplitTest(".os.path");
    StringSplitTest(".");
    StringSplitTest("a.b.c.d.");
    StringSplitTest("a.b.c.d.", 3);
    StringSplitTest("X.Y.Z", -1, "/");

    // strip test.
    StringStripTest();

    getchar();

    return 0;
}

void TestCase_Com_DataType::RawTest()
{
    LLBC_PrintLn("Raw types test:");
    LLBC_PrintLn("sizeof(sint8): %lu", sizeof(sint8));
    LLBC_PrintLn("sizeof(sint16): %lu", sizeof(sint16));
    LLBC_PrintLn("sizeof(sint32): %lu", sizeof(sint32));
    LLBC_PrintLn("sizeof(sint64): %lu", sizeof(sint64));
    LLBC_Print("\n");

    LLBC_PrintLn("sizeof(uint8): %lu", sizeof(uint8));
    LLBC_PrintLn("sizeof(uint16): %lu", sizeof(sint16));
    LLBC_PrintLn("sizeof(uint32): %lu", sizeof(sint32));
    LLBC_PrintLn("sizeof(uint64): %lu", sizeof(sint64));
    LLBC_Print("\n");

    LLBC_PrintLn("sizeof(tchar): %lu", sizeof(tchar));

    LLBC_PrintLn("\n");
}

void TestCase_Com_DataType::StringBaseTest()
{
    LLBC_PrintLn("String base functions test:");

    LLBC_String testStr;
    testStr.format("%s", "hello world!");
    testStr.append_format("%s", "hello world!");
    LLBC_PrintLn("LLBC_String::format/append_format test: %s", testStr.c_str());

    LLBC_String testStr2;
    for(int i = 0; i < 1000; ++i)
    {
        testStr.append("hello world!");
    }

    testStr2.append_format("%s", testStr.c_str());
    LLBC_PrintLn("LLBC_String:format large string test: %s", testStr2.c_str());

    // tolower/toupper test.
    testStr = "Hello WoRlD!";
    LLBC_PrintLn("'%s' to lower: '%s'", testStr.c_str(), testStr.tolower().c_str());
    LLBC_PrintLn("'%s' to upper: '%s'", testStr.c_str(), testStr.toupper().c_str());

    // isalpha/isupper/islower.
    LLBC_String str("HELLO");
    LLBC_PrintLn("%s islower?%d, isupper?%d, isalpha?%d", 
        str.c_str(), str.islower(), str.isupper(), str.isalpha());
    str = "hello";
    LLBC_PrintLn("%s islower?%d, isupper?%d, isalpha?%d", 
        str.c_str(), str.islower(), str.isupper(), str.isalpha());
    str = "HeLlO";
    LLBC_PrintLn("%s islower?%d, isupper?%d, isalpha?%d", 
        str.c_str(), str.islower(), str.isupper(), str.isalpha());
    str = "hello123";
    LLBC_PrintLn("%s islower?%d, isupper?%d, isalpha?%d", 
        str.c_str(), str.islower(), str.isupper(), str.isalpha());
    str = "HELLO123";
    LLBC_PrintLn("%s islower?%d, isupper?%d, isalpha?%d", 
        str.c_str(), str.islower(), str.isupper(), str.isalpha());
    str = "Hello123";
    LLBC_PrintLn("%s islower?%d, isupper?%d, isalpha?%d", 
        str.c_str(), str.islower(), str.isupper(), str.isalpha());
    str = "H";
    LLBC_PrintLn("%s islower?%d, isupper?%d, isalpha?%d", 
        str.c_str(), str.islower(), str.isupper(), str.isalpha());
    str = "h";
    LLBC_PrintLn("%s islower?%d, isupper?%d, isalpha?%d", 
        str.c_str(), str.islower(), str.isupper(), str.isalpha());
    str = "3";
    LLBC_PrintLn("%s islower?%d, isupper?%d, isalpha?%d", 
        str.c_str(), str.islower(), str.isupper(), str.isalpha());

    // startswith/endswith
    str = "Hello world!";
    LLBC_PrintLn("%s starts with 'Hello'?: %s", str.c_str(), str.startswith("Hello") ? "true" : "false");
    LLBC_PrintLn("%s starts with 'hello'?: %s", str.c_str(), str.startswith("hello") ? "true" : "false");
    LLBC_PrintLn("%s ends with 'world!'?: %s", str.c_str(), str.endswith("world!") ? "true" : "false");
    LLBC_PrintLn("%s starts with 'world!!'?: %s", str.c_str(), str.endswith("world!!") ? "true" : "false");

    LLBC_PrintLn("\n");
}

void TestCase_Com_DataType::StringCompareTest()
{
    LLBC_PrintLn("String compare test:");
    {
        LLBC_PrintLn("- LLBC_String compare LLBC_String:");
        const LLBC_String str1("Hello world");
        const LLBC_String str2("Hello world!");
        const LLBC_String str3("Hello World!");
        const LLBC_String str4("Hey!");

        LLBC_PrintLn("  - %s == %s ? %d", str1.c_str(), str2.c_str(), str1 == str2);
        LLBC_PrintLn("  - %s != %s ? %d", str1.c_str(), str2.c_str(), str1 != str2);
        LLBC_PrintLn("  - %s < %s ? %d", str1.c_str(), str2.c_str(), str1 < str2);
        LLBC_PrintLn("  - %s <= %s ? %d", str1.c_str(), str2.c_str(), str1 <= str2);
        LLBC_PrintLn("  - %s > %s ? %d", str1.c_str(), str2.c_str(), str1 > str2);
        LLBC_PrintLn("  - %s >= %s ? %d", str1.c_str(), str2.c_str(), str1 >= str2);

        LLBC_PrintLn("  - %s == %s ? %d", str2.c_str(), str3.c_str(), str2 == str3);
        LLBC_PrintLn("  - %s != %s ? %d", str2.c_str(), str3.c_str(), str2 != str3);
        LLBC_PrintLn("  - %s < %s ? %d", str2.c_str(), str3.c_str(), str2 < str3);
        LLBC_PrintLn("  - %s <= %s ? %d", str2.c_str(), str3.c_str(), str2 <= str3);
        LLBC_PrintLn("  - %s > %s ? %d", str2.c_str(), str3.c_str(), str2 > str3);
        LLBC_PrintLn("  - %s >= %s ? %d", str2.c_str(), str3.c_str(), str2 >= str3);

        LLBC_PrintLn("  - %s == %s ? %d", str3.c_str(), str4.c_str(), str3 == str4);
        LLBC_PrintLn("  - %s != %s ? %d", str3.c_str(), str4.c_str(), str3 != str4);
        LLBC_PrintLn("  - %s < %s ? %d", str3.c_str(), str4.c_str(), str3 < str4);
        LLBC_PrintLn("  - %s <= %s ? %d", str3.c_str(), str4.c_str(), str3 <= str4);
        LLBC_PrintLn("  - %s > %s ? %d", str3.c_str(), str4.c_str(), str3 > str4);
        LLBC_PrintLn("  - %s >= %s ? %d", str3.c_str(), str4.c_str(), str3 >= str4);
    }

    {
        LLBC_PrintLn("- LLBC_String compare LLBC_CString:");
        LLBC_String str1("Hello world");
        LLBC_String str2("Hey Judy");
        LLBC_CString cstr1("Hello world");
        LLBC_CString cstr2("Hey Judy");

        LLBC_PrintLn("  - %s == %s ? %d", str1.c_str(), cstr1.c_str(), str1 == cstr1);
        LLBC_PrintLn("  - %s != %s ? %d", str1.c_str(), cstr1.c_str(), str1 != cstr1);
        LLBC_PrintLn("  - %s < %s ? %d", str1.c_str(), cstr1.c_str(), str1 < cstr1);
        LLBC_PrintLn("  - %s <= %s ? %d", str1.c_str(), cstr1.c_str(), str1 <= cstr1);
        LLBC_PrintLn("  - %s > %s ? %d", str1.c_str(), cstr1.c_str(), str1 > cstr1);
        LLBC_PrintLn("  - %s >= %s ? %d", str1.c_str(), cstr1.c_str(), str1 >= cstr1);

        LLBC_PrintLn("  - %s == %s ? %d", str2.c_str(), cstr2.c_str(), str2 == cstr2);
        LLBC_PrintLn("  - %s != %s ? %d", str2.c_str(), cstr2.c_str(), str2 != cstr2);
        LLBC_PrintLn("  - %s < %s ? %d", str2.c_str(), cstr2.c_str(), str2 < cstr2);
        LLBC_PrintLn("  - %s <= %s ? %d", str2.c_str(), cstr2.c_str(), str2 <= cstr2);
        LLBC_PrintLn("  - %s > %s ? %d", str2.c_str(), cstr2.c_str(), str2 > cstr2);
        LLBC_PrintLn("  - %s >= %s ? %d", str2.c_str(), cstr2.c_str(), str2 >= cstr2);

        LLBC_PrintLn("  - %s == %s ? %d", str2.c_str(), cstr1.c_str(), str2 == cstr1);
        LLBC_PrintLn("  - %s != %s ? %d", str2.c_str(), cstr1.c_str(), str2 != cstr1);
        LLBC_PrintLn("  - %s < %s ? %d", str2.c_str(), cstr1.c_str(), str2 < cstr1);
        LLBC_PrintLn("  - %s <= %s ? %d", str2.c_str(), cstr1.c_str(), str2 <= cstr1);
        LLBC_PrintLn("  - %s > %s ? %d", str2.c_str(), cstr1.c_str(), str2 > cstr1);
        LLBC_PrintLn("  - %s >= %s ? %d", str2.c_str(), cstr1.c_str(), str2 >= cstr1);
    }

    {
        LLBC_PrintLn("- LLBC_CString compare LLBC_String:");
        LLBC_String str1("Hello world");
        LLBC_String str2("Hey Judy");
        LLBC_CString cstr1("Hello world");
        LLBC_CString cstr2("Hey Judy");

        LLBC_PrintLn("  - %s == %s ? %d", cstr1.c_str(), str1.c_str(), cstr1 == str1);
        LLBC_PrintLn("  - %s != %s ? %d", cstr1.c_str(), str1.c_str(), cstr1 != str1);
        LLBC_PrintLn("  - %s < %s ? %d", cstr1.c_str(), str1.c_str(), cstr1 < str1);
        LLBC_PrintLn("  - %s <= %s ? %d", cstr1.c_str(), str1.c_str(), cstr1 <= str1);
        LLBC_PrintLn("  - %s > %s ? %d", cstr1.c_str(), str1.c_str(), cstr1 > str1);
        LLBC_PrintLn("  - %s >= %s ? %d", cstr1.c_str(), str1.c_str(), cstr1 >= str1);

        LLBC_PrintLn("  - %s == %s ? %d", cstr2.c_str(), str2.c_str(), str2 == str2);
        LLBC_PrintLn("  - %s != %s ? %d", cstr2.c_str(), str2.c_str(), str2 != str2);
        LLBC_PrintLn("  - %s < %s ? %d", cstr2.c_str(), str2.c_str(), str2 < str2);
        LLBC_PrintLn("  - %s <= %s ? %d", cstr2.c_str(), str2.c_str(), str2 <= str2);
        LLBC_PrintLn("  - %s > %s ? %d", cstr2.c_str(), str2.c_str(), str2 > str2);
        LLBC_PrintLn("  - %s >= %s ? %d", cstr2.c_str(), str2.c_str(), (str2 >= str2));

        LLBC_PrintLn("  - %s == %s ? %d", cstr2.c_str(), str1.c_str(), str2 == str1);
        LLBC_PrintLn("  - %s != %s ? %d", cstr2.c_str(), str1.c_str(), str2 != str1);
        LLBC_PrintLn("  - %s < %s ? %d", cstr2.c_str(), str1.c_str(), str2 < str1);
        LLBC_PrintLn("  - %s <= %s ? %d", cstr2.c_str(), str1.c_str(), str2 <= str1);
        LLBC_PrintLn("  - %s > %s ? %d", cstr2.c_str(), str1.c_str(), str2 > str1);
        LLBC_PrintLn("  - %s >= %s ? %d", cstr2.c_str(), str1.c_str(), str2 >= str1);
    }

    {
        LLBC_PrintLn("- LLBC_String compare const char *:");
        LLBC_String str("Hello world");
        auto cstr("Hello world");
        LLBC_PrintLn(" - %s == %s ? %d", str.c_str(), cstr, str == cstr);
    }
}

void TestCase_Com_DataType::StringLogicOperationTest()
{
    LLBC_PrintLn("String logic-operation test:");

    {
        LLBC_PrintLn("- LLBC_String += LLBC_String test:");
        LLBC_String str1("Hello");
        LLBC_String str2(" World");

        const LLBC_String str3 = str1 + str2;
        LLBC_PrintLn("%s + %s = %s", str1.c_str(), str2.c_str(), str3.c_str());
        LLBC_PrintLn("%s += %s = %s", str1.c_str(), str2.c_str(), (str1 += str2).c_str());
    }

    {
        LLBC_PrintLn("- LLBC_String + literal string test:");
        LLBC_String str("Hey");
        auto cstr = " Judy";
        
        const LLBC_String str2 = str + cstr;
        LLBC_PrintLn("%s + %s = %s", str.c_str(), cstr, str2.c_str());
        LLBC_PrintLn("%s += %s = %s", str.c_str(), cstr, (str + cstr).c_str());
    }

    {
        LLBC_PrintLn("- LLBC_String + LLBC_CString test:");
        LLBC_String str("Hey");
        LLBC_CString cstr(" Judy");
        
        const LLBC_String str2 = str + cstr;
        LLBC_PrintLn("%s + %s = %s", str.c_str(), cstr.c_str(), str2.c_str());
        LLBC_PrintLn("%s += %s = %s", str.c_str(), cstr.c_str(), (str + cstr).c_str());
    }

    {
        LLBC_PrintLn("- LLBC_String * int test:");
        LLBC_String str1("Hello ");

        LLBC_PrintLn("- %s * 0 = %s", str1.c_str(), (str1 * 0).c_str());
        LLBC_PrintLn("- %s * 1 = %s", str1.c_str(), (str1 * 1).c_str());
        LLBC_PrintLn("- %s * 3 = %s", str1.c_str(), (str1 * 3).c_str());
    }
}

void TestCase_Com_DataType::StringUTF8Test()
{
    LLBC_PrintLn("UTF-8 test:");

    LLBC_String utf8Str;
    utf8Str.append("\xef\xbb\xbf"); // Add bomb.
    LLBC_PrintLn("utf8Str exist bomb? %s", utf8Str.has_utf8_bomb() ? "true" : "false");

    // 0000000: e4bd a0e5 a5bd e4b8 96e7 958c 4865 6c6c  ............Hell
    // 0000010: 6f20 576f 726c 640a                      o World.
    utf8Str.append(1, (char)0xe4).append(1, (char)0xbd).append(1, (char)0xa0);
    utf8Str.append(1, (char)0xe5).append(1, (char)0xa5).append(1, (char)0xbd);
    utf8Str.append(1, (char)0xe4).append(1, (char)0xb8).append(1, (char)0x96);
    utf8Str.append(1, (char)0xe7).append(1, (char)0x95).append(1, (char)0x8c);
    utf8Str.append("Hello World");
    LLBC_PrintLn("When add string, len: %ld, utf8 len: %ld", utf8Str.length(), utf8Str.length_with_utf8());

    // split_utf8_string().
    std::vector<LLBC_String> utf8Vec;
    utf8Str.split_utf8_string(1, utf8Vec);
    LLBC_PrintLn("Split utf8 string done, first str size: %ld, second str size: %ld", utf8Vec[0].length(), utf8Vec[1].length());
    utf8Str.split_utf8_string(-1, utf8Vec);
    LLBC_PrintLn("Reverse split utf8 string done, first str size: %ld, second str size: %ld", utf8Vec[0].length(), utf8Vec[1].length());

    // trim/add bomb.
    utf8Str.trim_utf8_bomb();
    utf8Str.add_utf8_bomb();

    // scatter_utf8_string().
    utf8Str.scatter_utf8_string(utf8Vec, 0);
    utf8Str.scatter_utf8_string(utf8Vec, 1);
    utf8Str.scatter_utf8_string(utf8Vec, 2);
    utf8Str.scatter_utf8_string(utf8Vec, 3);
    LLBC_PrintLn("After scatter utf8 string, vector size: %ld", utf8Vec.size());

    LLBC_PrintLn("\n");
}

void TestCase_Com_DataType::StringFindReplaceTest()
{
    LLBC_PrintLn("FindReplace test:");

    LLBC_String str("Hello World, Hello World");
    LLBC_PrintLn("Before findreplace(), str: %s", str.c_str());
    str.findreplace("Hel", "HEL");
    LLBC_PrintLn("After findreplace(), str: %s", str.c_str());

    LLBC_PrintLn("\n");
}

void TestCase_Com_DataType::StringStripTest()
{
    LLBC_PrintLn("Strip test:");
    LLBC_String str = "\t \t Hello World! \t \t";
    LLBC_PrintLn("Before strip, str: %s, len: %ld", str.c_str(), str.length());

    str.strip();
    LLBC_PrintLn("After strip, str: %s, len: %ld", str.c_str(), str.length());
}

void TestCase_Com_DataType::StringSplitTest(const LLBC_String &str, size_t maxSplit, const LLBC_String &sep)
{
    LLBC_PrintLn("Will split string(maxSplit: %ld, sep: %s): %s", maxSplit, sep.c_str(), str.c_str());
    std::vector<LLBC_String> splitted = str.split(sep, maxSplit);
    LLBC_PrintLn("Split use separator:., size: %ld", splitted.size());
    for (size_t i = 0; i < splitted.size(); ++i)
        LLBC_PrintLn("    \"%s\"", splitted[i].c_str());
}
