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


#include "core/utils/TestCase_Core_Utils_Text.h"

int TestCase_Core_Utils_Text::Run(int argc, char *argv[])
{
    LLBC_PrintLine("core/utils/Util_Text test:");

    // Split string test.
    LLBC_String str = "hello world hello world";
    LLBC_PrintLine("split string: %s", str.c_str());
    std::vector<LLBC_String> strs;
    LLBC_SplitString(str, " ", strs);
    LLBC_PrintLine("result(sub string count: %lu):", strs.size());
    for(size_t i = 0; i < strs.size(); ++i)
    {
        LLBC_PrintLine("\t%s", strs[i].c_str());
    }
    LLBC_PrintLine("");

    // Filter-out test.
    LLBC_PrintLine("filter out string: %s", str.c_str());
    LLBC_PrintLine("result: %s", LLBC_FilterOutString(str, " ").c_str());
    LLBC_PrintLine("");
    
    // Upper<->Lower test.
    str = "AaBbCcDd eEfFgG";
    LLBC_PrintLine("string [%s] to Upper: %s", str.c_str(), LLBC_ToUpper(str.c_str()).c_str());
    LLBC_PrintLine("string [%s] to Lower: %s", str.c_str(), LLBC_ToLower(str.c_str()).c_str());
    LLBC_PrintLine("");

    // String -> Number test.
    str = "-30";
    LLBC_PrintLine("string [%s] to number(Str2Int32): %d", 
        str.c_str(), LLBC_Str2Int32(str.c_str()));
    LLBC_PrintLine("string [%s] to number(Str2Long): %ld", 
        str.c_str(), LLBC_Str2Long(str.c_str()));
    LLBC_PrintLine("string [%s] to number(Str2Int64): %lld", 
        str.c_str(), LLBC_Str2Int64(str.c_str()));
    LLBC_PrintLine("string [%s] to number(Str2UInt32): %u", 
        str.c_str(), LLBC_Str2UInt32(str.c_str()));
    LLBC_PrintLine("string [%s] to number(Str2ULong): %lu", 
        str.c_str(), LLBC_Str2ULong(str.c_str()));
    LLBC_PrintLine("string [%s] to pointer(Str2Ptr): %p",
        "30", LLBC_Str2Ptr("30"));
    LLBC_PrintLine("string [%s] to pointer(Str2Ptr): %p",
        "0xcdcdcdcd", LLBC_Str2Ptr("0xcdcdcdcd"));
    LLBC_PrintLine("string [%s] to number(Str2UInt64): %llu", 
        str.c_str(), LLBC_Str2UInt64(str.c_str()));
    LLBC_PrintLine("string [%s] to number(Str2Double): %f", 
        str.c_str(), LLBC_Str2Double(str.c_str()));
    LLBC_PrintLine("");

    // Number -> String test.
    {
        sint8 sint8Val = 'a';
        LLBC_PrintLine("LLBC_Num2Str<sint8>()[%c] -> string(base:10): %s",
            sint8Val, LLBC_Num2Str(sint8Val).c_str());
        uint8 uint8Val = 97;
        LLBC_PrintLine("LLBC_Num2Str<uint8>()[%d] -> string(base:10): %s",
            uint8Val, LLBC_Num2Str(uint8Val).c_str());

        sint16 sint16Val = -16;
        LLBC_PrintLine("LLBC_Num2Str<sint16>()[%d] -> string(base:16): %s",
            sint16Val, LLBC_Num2Str(sint16Val, 16).c_str());
        uint16 uint16Val = 16;
        LLBC_PrintLine("LLBC_Num2Str<uint16>()[%u] -> string(base:8): %s",
            uint16Val, LLBC_Num2Str(uint16Val, 8).c_str());

        sint32 sint32Val = -32;
        LLBC_PrintLine("LLBC_Num2Str<sint32>()[%d] -> string(base:10): %s",
            sint32Val, LLBC_Num2Str(sint32Val).c_str());
        uint32 uint32Val = 32;
        LLBC_PrintLine("LLBC_Num2Str<uint32>()[%u] -> string(base:10): %s",
            uint32Val, LLBC_Num2Str(uint32Val).c_str());

        long longVal = -1;
        LLBC_PrintLine("LLBC_Num2Str<long>()[%ld] -> string(base:10): %s",
            longVal, LLBC_Num2Str(longVal).c_str());
        llbc::ulong ulongVal = -1;
        LLBC_PrintLine("LLBC_Num2Str<ulong>()[%lu] -> string(base:10): %s",
            ulongVal, LLBC_Num2Str(ulongVal).c_str());

        sint64 sint64Val = -64;
        LLBC_PrintLine("LLBC_Num2Str<sint64>()[%lld] -> string(base:10): %s",
            sint64Val, LLBC_Num2Str(sint64Val).c_str());
        uint64 uint64Val = 64;
        LLBC_PrintLine("LLBC_Num2Str<uint64>()[%llu] -> string(base:10): %s",
            uint64Val, LLBC_Num2Str(uint64Val).c_str());

        float fVal = 1.0f;
        LLBC_PrintLine("LLBC_Num2Str<float>()[%f] -> string: %s",
            fVal, LLBC_Num2Str(fVal).c_str());
        double doubleVal = -1.0f;
        LLBC_PrintLine("LLBC_Num2Str<double>()[%f] -> string: %s",
            doubleVal, LLBC_Num2Str(doubleVal).c_str());

        sint64 intPtrAddr = 0xcdcdcdcd;
        int *intPtr; memcpy(&intPtr, &intPtrAddr, sizeof(int *));
        LLBC_PrintLine("LLBC_Num2Str<int *>()[%p] -> string: %s",
            intPtr, LLBC_Num2Str(intPtr, 16).c_str());

        sint64 voidPtrAddr = 0xfffffffe;
        void *voidPtr; memcpy(&voidPtr, &voidPtrAddr, sizeof(void *));
        LLBC_PrintLine("LLBC_Num2Str<void *>()[%p] -> string: %s",
            voidPtr, LLBC_Num2Str(voidPtr, 16).c_str());
    }

    LLBC_PrintLine("");

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return 0;
}
