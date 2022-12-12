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

template <size_t Len>
void MyTemp(const char (&arr)[Len])
{
    std::cout << arr << std::endl;
}

class MyStr
{
public:
    template <size_t Len>
    MyStr(const char (&arr)[Len])
    {
        std::cout << "Templ ctor called:" << arr << std::endl;
    }

    // template <>
    // MyStr(const char (&arr)[6])
    // {
        // std::cout << "Templ(6) ctor called:" << arr << std::endl;
    // }

    MyStr(const std::string &stlStr)
    {
        std::cout << "Stl String ctor called:" << stlStr << std::endl;
    }

    template <typename Ptr,
              typename = typename std::enable_if<std::is_pointer<Ptr>::value &&
                                                 std::is_same<char, typename std::remove_reference<typename std::remove_const<typename std::remove_pointer<Ptr>::type>::type>::type>::value, Ptr>::type>
    MyStr(Ptr str, size_t len = 0)
    {
        std::cout << "Non-Templ ctor called:" << str << std::endl;
    }
};

int TestCase_Com_DataType::Run(int argc, char *argv[])
{
    LLBC_PrintLine("llbc library test case(datatype test):");

    const wchar *wstr = L"";
    MyStr myStr("hello");
    const char *rawStr = "Hey judy";
    std::string stlStr = "Hey judy(stlstr)";
    LLBC_String llbcStr = "Hey judy(llbcstr)";
    MyStr myStr2(rawStr);

    LLBC_CString emptyStr; // construct by nullptr.
    LLBC_CString cstr("hello"); // construct by string literal.
    LLBC_CString cstr2(rawStr); // construct by string pointer.

    // operator < test
    {
        std::cout << (emptyStr < nullptr) << std::endl; // nullptr
        std::cout << (emptyStr < "hey") << std::endl; // string literal
        std::cout << (emptyStr < rawStr) << std::endl; // string pointer
        std::cout << (emptyStr < stlStr) << std::endl; // stl str
        std::cout << (emptyStr < llbcStr) << std::endl; // llbc str
    }

    // raw types test.
    RawTest();

    // string base test.
    StringBaseTest();

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
    for(int i = 0; i < 1000; ++i)
    {
        testStr.append("hello world!");
    }

    testStr2.append_format("%s", testStr.c_str());
    LLBC_PrintLine("LLBC_String:format large string test: %s", testStr2.c_str());

    // tolower/toupper test.
    testStr = "Hello WoRlD!";
    LLBC_PrintLine("'%s' to lower: '%s'", testStr.c_str(), testStr.tolower().c_str());
    LLBC_PrintLine("'%s' to upper: '%s'", testStr.c_str(), testStr.toupper().c_str());

    // isalpha/isupper/islower.
    LLBC_String str("HELLO");
    LLBC_PrintLine("%s islower?%d, isupper?%d, isalpha?%d", 
        str.c_str(), str.islower(), str.isupper(), str.isalpha());
    str = "hello";
    LLBC_PrintLine("%s islower?%d, isupper?%d, isalpha?%d", 
        str.c_str(), str.islower(), str.isupper(), str.isalpha());
    str = "HeLlO";
    LLBC_PrintLine("%s islower?%d, isupper?%d, isalpha?%d", 
        str.c_str(), str.islower(), str.isupper(), str.isalpha());
    str = "hello123";
    LLBC_PrintLine("%s islower?%d, isupper?%d, isalpha?%d", 
        str.c_str(), str.islower(), str.isupper(), str.isalpha());
    str = "HELLO123";
    LLBC_PrintLine("%s islower?%d, isupper?%d, isalpha?%d", 
        str.c_str(), str.islower(), str.isupper(), str.isalpha());
    str = "Hello123";
    LLBC_PrintLine("%s islower?%d, isupper?%d, isalpha?%d", 
        str.c_str(), str.islower(), str.isupper(), str.isalpha());
    str = "H";
    LLBC_PrintLine("%s islower?%d, isupper?%d, isalpha?%d", 
        str.c_str(), str.islower(), str.isupper(), str.isalpha());
    str = "h";
    LLBC_PrintLine("%s islower?%d, isupper?%d, isalpha?%d", 
        str.c_str(), str.islower(), str.isupper(), str.isalpha());
    str = "3";
    LLBC_PrintLine("%s islower?%d, isupper?%d, isalpha?%d", 
        str.c_str(), str.islower(), str.isupper(), str.isalpha());

    // startswith/endswith
    str = "Hello world!";
    LLBC_PrintLine("%s starts with 'Hello'?: %s", str.c_str(), str.startswith("Hello") ? "true" : "false");
    LLBC_PrintLine("%s starts with 'hello'?: %s", str.c_str(), str.startswith("hello") ? "true" : "false");
    LLBC_PrintLine("%s ends with 'world!'?: %s", str.c_str(), str.endswith("world!") ? "true" : "false");
    LLBC_PrintLine("%s starts with 'world!!'?: %s", str.c_str(), str.endswith("world!!") ? "true" : "false");

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

void TestCase_Com_DataType::StringFindReplaceTest()
{
    LLBC_PrintLine("FindReplace test:");

    LLBC_String str("Hello World, Hello World");
    LLBC_PrintLine("Before findreplace(), str: %s", str.c_str());
    str.findreplace("Hel", "HEL");
    LLBC_PrintLine("After findreplace(), str: %s", str.c_str());

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
    for (size_t i = 0; i < splitted.size(); ++i)
        LLBC_PrintLine("    \"%s\"", splitted[i].c_str());
}
