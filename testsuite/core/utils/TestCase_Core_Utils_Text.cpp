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

namespace
{

enum Util_Text_TestEnum
{
    TestEnumVal = 1,
};

enum class Util_Text_TestEnumCls
{
    TestEnumVal = 2,
};

}

int TestCase_Core_Utils_Text::Run(int argc, char *argv[])
{
    LLBC_PrintLn("core/utils/Util_Text test:");

    LLBC_ErrorAndReturnIf(_Test_Num2Str() != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(_Test_Str2Num() != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(_Test_Str2LooseBool() != LLBC_OK, LLBC_FAILED);

    LLBC_PrintLn("Press any key to continue...");
    getchar();

    return LLBC_OK;
}

int TestCase_Core_Utils_Text::_Test_Num2Str()
{
    LLBC_PrintLn("- Num -> Str test:");

    // Num->Str, bool:
    auto testNum2Str_bool = [](bool boolVal)
    {
        size_t strLen;
        const char *str = LLBC_Num2Str2(boolVal, &strLen);
        LLBC_PrintLn("  - LLBC_Num2Str2(%s, strLen) return:%s, strLen:%lu",
                     boolVal ? "true" : "false", str, strLen);
        llbc_assert(strcmp(str, boolVal ? "1" : "0") == 0);

        const auto &strObj = LLBC_Num2Str(boolVal);
        LLBC_PrintLn("  - LLBC_Num2Str(%s) return:%s",
                     boolVal ? "true" : "false", strObj.c_str());

        llbc_assert(strObj == str && "Test[Num->Str, bool] failed");
    };

    for (auto &boolVal : {true, false})
        testNum2Str_bool(boolVal);

    // Num->Str, enum:
    LLBC_PrintLn("  - LLBC_Num2Str<Enum>() test:");
    auto enumStr = LLBC_Num2Str(Util_Text_TestEnum::TestEnumVal);
    LLBC_PrintLn("    - LLBC_Num2Str(Util_Text_TestEnum::TestEnumVal) return:%s, expect:1", enumStr.c_str());
    llbc_assert(enumStr == "1" && "LLBC_Num2Str<Eunm> test failed");

    enumStr = LLBC_Num2Str(Util_Text_TestEnumCls::TestEnumVal);
    LLBC_PrintLn("    - LLBC_Num2Str(Util_Text_TestEnumCls::TestEnumVal) return:%s, expect:2", enumStr.c_str());
    llbc_assert(enumStr == "2" && "LLBC_Num2Str<EnumCls> test failed");

    // Num->Str, sint8/uint8:
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<sint8, false>() != LLBC_OK), LLBC_FAILED);
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<sint8, true>() != LLBC_OK), LLBC_FAILED);
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<uint8, false>() != LLBC_OK), LLBC_FAILED);
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<uint8, true>() != LLBC_OK), LLBC_FAILED);

    // Num->Str, sint16/uint16:
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<sint16, true>() != LLBC_OK), LLBC_FAILED);
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<sint16, false>() != LLBC_OK), LLBC_FAILED);
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<uint16, true>() != LLBC_OK), LLBC_FAILED);
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<uint16, false>() != LLBC_OK), LLBC_FAILED);

    // Num->Str, sint32/uint32:
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<sint32, true>() != LLBC_OK), LLBC_FAILED);
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<sint32, false>() != LLBC_OK), LLBC_FAILED);
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<uint32, true>() != LLBC_OK), LLBC_FAILED);
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<uint32, false>() != LLBC_OK), LLBC_FAILED);

    // Num->Str, long/ulong:
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<long, true>() != LLBC_OK), LLBC_FAILED);
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<long, false>() != LLBC_OK), LLBC_FAILED);
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<ulong, true>() != LLBC_OK), LLBC_FAILED);
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<ulong, false>() != LLBC_OK), LLBC_FAILED);

    // Num->Str, sint64/uint64:
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<sint64, true>() != LLBC_OK), LLBC_FAILED);
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<sint64, false>() != LLBC_OK), LLBC_FAILED);
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<uint64, true>() != LLBC_OK), LLBC_FAILED);
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<uint64, false>() != LLBC_OK), LLBC_FAILED);

    // Num->Str, float/double/ldouble:
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<float, true>() != LLBC_OK), LLBC_FAILED);
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<float, false>() != LLBC_OK), LLBC_FAILED);
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<double, true>() != LLBC_OK), LLBC_FAILED);
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<double, false>() != LLBC_OK), LLBC_FAILED);
    // LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<ldouble, true>() != LLBC_OK), LLBC_FAILED);
    // LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<ldouble, false>() != LLBC_OK), LLBC_FAILED);

    // Num-Str: pointer:
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<void *, true>() != LLBC_OK), LLBC_FAILED);
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<void *, false>() != LLBC_OK), LLBC_FAILED);
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<int *, true>() != LLBC_OK), LLBC_FAILED);
    LLBC_ErrorAndReturnIf((_Test_Num2StrImpl<int *, false>() != LLBC_OK), LLBC_FAILED);

    return LLBC_OK;
}

int TestCase_Core_Utils_Text::_Test_Str2Num()
{
    LLBC_PrintLn("- Str -> Num test:");

    // Str->Num, bool:
    auto boolTestFunc = [](const LLBC_String &str, int base, bool expect)
    {
        const bool boolVal = LLBC_Str2Num<bool>(str.c_str(), base);
        LLBC_PrintLn("  - LLBC_Str2Num<bool>(""%s"", %d): return:%s, expect:%s",
                     str.c_str(),
                     base,
                     boolVal ? "true" : "false",
                     expect ? "true" : "false");
        if (boolVal != expect)
        {
            LLBC_FilePrintLn(stderr, "    - Test failed");
            llbc_assert(false && "LLBC_Str2Num<bool>() test failed");
            return LLBC_FAILED;
        }

        return LLBC_OK;
    };

    // Str->Num, enum:
    LLBC_PrintLn("  - LLBC_Str2Num<Enum>() test:");
    auto enumVal = LLBC_Str2Num<Util_Text_TestEnum>("1");
    LLBC_PrintLn("    - LLBC_Str2Num<Util_Text_TestEnum>(""1"") return:%d, except:%d",
                      enumVal, Util_Text_TestEnum::TestEnumVal);
    llbc_assert(enumVal == Util_Text_TestEnum::TestEnumVal && "LLBC_Str2Num<Enum>() test failed");

    auto enumVal2 = LLBC_Str2Num<Util_Text_TestEnumCls>("2");
    LLBC_PrintLn("    - LLBC_Str2Num<Util_Text_TestEnumCls>(""2"") return:%d, except:%d",
                      static_cast<int>(enumVal2), static_cast<int>(Util_Text_TestEnumCls::TestEnumVal));
    llbc_assert(enumVal2 == Util_Text_TestEnumCls::TestEnumVal && "LLBC_Str2Num<EnumCls>() test failed");

    for (auto &boolStr : {"0", "  0", "0    ", "   0   ", "", "   "})
        LLBC_ErrorAndReturnIf(boolTestFunc(boolStr, 10, false) != LLBC_OK, LLBC_FAILED);
    for (auto &boolStr : {"0x0", "  0x0", "0x0    ", "   0x0   "})
        LLBC_ErrorAndReturnIf(boolTestFunc(boolStr, 16, false) != LLBC_OK, LLBC_FAILED);
    for (auto &boolStr : {"1", "123", "-123", "1.35", "-1.35"})
        LLBC_ErrorAndReturnIf(boolTestFunc(boolStr, 10, true) != LLBC_OK, LLBC_FAILED);
    for (auto &boolStr : {"0xfd", "0xdddd"})
        LLBC_ErrorAndReturnIf(boolTestFunc(boolStr, 16, true) != LLBC_OK, LLBC_FAILED);

    // Str->Num, sint8/uint8:
    LLBC_ErrorAndReturnIf(_Test_Str2NumImpl<sint8>() != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(_Test_Str2NumImpl<uint8>() != LLBC_OK, LLBC_FAILED);

    // Str->Num, sint16/uint16:
    LLBC_ErrorAndReturnIf(_Test_Str2NumImpl<sint16>() != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(_Test_Str2NumImpl<uint16>() != LLBC_OK, LLBC_FAILED);

    // Str->Num, sint32/uint32:
    LLBC_ErrorAndReturnIf(_Test_Str2NumImpl<sint32>() != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(_Test_Str2NumImpl<uint32>() != LLBC_OK, LLBC_FAILED);

    // Str->Num, long/ulong:
    LLBC_ErrorAndReturnIf(_Test_Str2NumImpl<long>() != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(_Test_Str2NumImpl<ulong>() != LLBC_OK, LLBC_FAILED);

    // Str->Num, sint64/uint64:
    LLBC_ErrorAndReturnIf(_Test_Str2NumImpl<sint64>() != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(_Test_Str2NumImpl<uint64>() != LLBC_OK, LLBC_FAILED);

    // Str->Num, float/double/ldouble:
    LLBC_ErrorAndReturnIf(_Test_Str2NumImpl<float>() != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(_Test_Str2NumImpl<double>() != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(_Test_Str2NumImpl<ldouble>() != LLBC_OK, LLBC_FAILED);

    // Str->Num, void */int *:
    LLBC_ErrorAndReturnIf(_Test_Str2NumImpl<void *>() != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(_Test_Str2NumImpl<int *>() != LLBC_OK, LLBC_FAILED);

    // Str->Num exception test:
    LLBC_PrintLn("  - Str->Num exception test:");
    _Test_Str2NumExceptionTest<sint32>("9999999999999999", LLBC_ERROR_CLIB, ERANGE);
    _Test_Str2NumExceptionTest<uint32>("9999999999999999", LLBC_ERROR_CLIB, ERANGE);
    _Test_Str2NumExceptionTest<long>("999999999999999999999999999", LLBC_ERROR_CLIB, ERANGE);
    _Test_Str2NumExceptionTest<ulong>("999999999999999999999999999", LLBC_ERROR_CLIB, ERANGE);
    _Test_Str2NumExceptionTest<sint64>("999999999999999999999999999", LLBC_ERROR_CLIB, ERANGE);
    _Test_Str2NumExceptionTest<uint64>("999999999999999999999999999", LLBC_ERROR_CLIB, ERANGE);

    _Test_Str2NumExceptionTest<sint64>("xxxxxxxxxxxx", LLBC_ERROR_INVALID, 0);
    _Test_Str2NumExceptionTest<uint64>("xxxxxxxxxxxx", LLBC_ERROR_INVALID, 0);
    _Test_Str2NumExceptionTest<uint64>("123xxxxxxx", LLBC_ERROR_PARTIAL_PARSED, 0);

    _Test_Str2NumExceptionTest<float>("XXXXXXXX", LLBC_ERROR_INVALID, 0);
    _Test_Str2NumExceptionTest<float>("3.5xxxxxxxx", LLBC_ERROR_PARTIAL_PARSED, 0);

    // Str->Num, error effect test.
    {
        LLBC_PrintLn("  - LLBC_Str2Num() error effect test:");

        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        auto ret = LLBC_Str2Num<sint64>("0");
        LLBC_PrintLn("    LLBC_SetLastError(LLBC_ERROR_NOT_FOUND), LLBC_Str2Num<sint64>(""0""): return:%lld, "
                     "err[errno:%x, subErrno:%d, errStr:%s], expectErr[errno:%x, subErrno:%d, errStr:%s]",
                     ret,
                     LLBC_GetLastError(),
                     LLBC_GetSubErrorNo(),
                     LLBC_FormatLastError(),
                     LLBC_ERROR_SUCCESS,
                     0,
                     LLBC_StrErrorEx(LLBC_ERROR_SUCCESS, 0));
        if (LLBC_GetLastError() != LLBC_ERROR_SUCCESS ||
            LLBC_GetSubErrorNo() != 0 ||
            ret != 0)
        {
            llbc_assert(false && "LLBC_Str2Num() error effect test faild");
            return LLBC_FAILED;
        }

        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        ret = LLBC_Str2Num<sint64>("10086");
        LLBC_PrintLn("    LLBC_SetLastError(LLBC_ERROR_NOT_FOUND), LLBC_Str2Num<sint64>(""10086""): return:%lld, "
                     "err[errno:%x, subErrno:%d, errStr:%s], expectErr[errno:%x, subErrno:%d, errStr:%s]",
                     ret,
                     LLBC_GetLastError(),
                     LLBC_GetSubErrorNo(),
                     LLBC_FormatLastError(),
                     LLBC_ERROR_NOT_FOUND,
                     0,
                     LLBC_StrErrorEx(LLBC_ERROR_NOT_FOUND, 0));
        if (LLBC_GetLastError() != LLBC_ERROR_NOT_FOUND ||
            LLBC_GetSubErrorNo() != 0 ||
            ret != 10086)
        {
            llbc_assert(false && "LLBC_Str2Num() error effect test faild");
            return LLBC_FAILED;
        }
    }

    return LLBC_OK;
}

int TestCase_Core_Utils_Text::_Test_Str2LooseBool()
{
    // Log.
    LLBC_PrintLn("- Str -> LooseBool test:");

    // Define test lambda function.
    auto testFunc = [](const LLBC_String &str, int base, bool recognizeTrueAndYes, bool expect)
    {
        const auto ret = LLBC_Str2LooseBool(str.c_str(), base, recognizeTrueAndYes);
        LLBC_PrintLn("  - LLBC_Str2LooseBool(""%s"", %d, %s) return:%s, expect:%s",
                     str.c_str(),
                     base,
                     recognizeTrueAndYes ? "true" : "false",
                     ret ? "true" : "false",
                     expect ? "true" : "false");
        if (ret != expect)
        {
            LLBC_FilePrintLn(stderr, "    - Test failed");
            llbc_assert(false && "LLBC_Str2LooseBool() test failed");

            return LLBC_FAILED;
        }

        return LLBC_OK;
    };

    LLBC_ErrorAndReturnIf(testFunc("", 10, false, false) != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(testFunc("    ", 10, false, false) != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(testFunc("    0", 10, false, false) != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(testFunc("0    ", 10, false, false) != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(testFunc("    1    ", 10, false, true) != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(testFunc("    1    ", 16, false, true) != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(testFunc("    0x0    ", 10, false, false) != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(testFunc("    0x1234    ", 10, false, true) != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(testFunc("    0X1234    ", 10, false, true) != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(testFunc("    0x1234    ", 16, false, true) != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(testFunc("    0X1234    ", 16, false, true) != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(testFunc("    TruE ", 10, false, false) != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(testFunc("    TruE ", 10, true, true) != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(testFunc("    YeS  ", 10, false, false) != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(testFunc("    YeS  ", 10, true, true) != LLBC_OK, LLBC_FAILED);

    return LLBC_OK;
}

template <typename _NumTy, bool _HexFormat>
int TestCase_Core_Utils_Text::_Test_Num2StrImpl()
{
    // Log:
    LLBC_PrintLn("  - Test LLBC_Num2Str<%s, %s>():",
                 typeid(_NumTy).name(), _HexFormat ? "true" : "false");

    // Define test lambda function.
    auto testFunc = [](_NumTy num, const LLBC_String &expect)
    {
        // LLBC_Num2Str2 test.
        LLBC_String funcDesc = LLBC_String().format("LLBC_Num2Str2<%s, %s>(%s, strLen)",
                                                    typeid(_NumTy).name(),
                                                    _HexFormat ? "true" : "false",
                                                    LLBC_Variant(num).ValueToString().c_str());

        size_t strLen;
        const char *str = LLBC_Num2Str2<_NumTy, _HexFormat>(num, &strLen);
        LLBC_PrintLn("    - %s expect:%s, return:%s, returnStrLen:%lu",
                     funcDesc.c_str(), expect.c_str(), str, strLen);
        if (!expect.empty() && expect != str)
        {
            LLBC_FilePrintLn(stderr, "      - Test Failed");
            llbc_assert(false && "LLBC_Num2Str2() test failed");

            return LLBC_FAILED;
        }

        // LLBC_Num2Str test.
        funcDesc = LLBC_String().format("LLBC_Num2Str<%s, %s>(%s)",
                                        typeid(_NumTy).name(),
                                        _HexFormat ? "true" : "false",
                                        LLBC_Variant(num).ValueToString().c_str());
        auto strObj = LLBC_Num2Str<_NumTy, _HexFormat>(num);
        LLBC_PrintLn("    - %s expect:%s, return:%s",
                     funcDesc.c_str(), expect.c_str(), strObj.c_str());
        if (!expect.empty() && expect != strObj)
        {
            LLBC_FilePrintLn(stderr, "      - Test failed");
            llbc_assert(false && "LLBC_Num2Str() test failed");

            return LLBC_FAILED;
        }

        return LLBC_OK;
    };

    // Test.
    LLBC_String expect;
    typedef std::numeric_limits<_NumTy> _NumTyLmt;
    if constexpr (std::is_pointer_v<_NumTy>)
    {
        for (uint64 ptrVal : {0x0llu, 0xfedccdefllu, 0xffffffllu, 0xabcdefllu})
        {
            _NumTy ptr;
            expect.format(_HexFormat ? "0x%llx" : "%llu", ptrVal);
            memcpy(&ptr, &ptrVal, std::min(sizeof(ptrVal), sizeof(ptr)));
            LLBC_ErrorAndReturnIf(testFunc(ptr, expect) != LLBC_OK, LLBC_FAILED);
        }
    }
    else
    {
        _NumTy vals[]{_NumTyLmt::min(),
                      _NumTyLmt::min() + static_cast<_NumTy>(50),
                      _NumTyLmt::min() + static_cast<_NumTy>(100),
                      _NumTy(),
                      static_cast<_NumTy>(3.1415926),
                      _NumTyLmt::max() - static_cast<_NumTy>(50),
                      _NumTyLmt::max() - static_cast<_NumTy>(100),
                      _NumTyLmt::max()};
        for (_NumTy val : vals)
        {
            if constexpr (std::is_same_v<_NumTy, sint8> ||
                          std::is_same_v<_NumTy, sint16> ||
                          std::is_same_v<_NumTy, sint32>)
            {
                if (_HexFormat)
                    expect.format(val < 0 ? "-0x%x" : "0x%x", std::abs(val));
                else
                    expect.format("%d", val);
            }
            else if constexpr (std::is_same_v<_NumTy, uint8> ||
                               std::is_same_v<_NumTy, uint16> ||
                               std::is_same_v<_NumTy, uint32>)
            {
                expect.format(_HexFormat ? "0x%x" : "%u", val);
            }
            else if constexpr (std::is_same_v<_NumTy, long>)
            {
                if (_HexFormat)
                    expect.format(val < 0 ? "-0x%lx" : "0x%lx", std::abs(val));
                else
                    expect.format("%l", val);
            }
            else if constexpr (std::is_same_v<_NumTy, ulong>)
            {
                expect.format(_HexFormat ? "0x%lx" : "%lu", val);
            }
            else if constexpr (std::is_same_v<_NumTy, sint64>)
            {
                if (_HexFormat)
                    expect.format(val < 0 ? "-0x%llx" : "0x%llx", std::abs(val));
                else
                    expect.format("%ll", val);
            }
            else if constexpr (std::is_same_v<_NumTy, uint64>)
            {
                expect.format(_HexFormat ? "0x%llx" : "%llu", val);
            }
            // else if constexpr (std::is_same_v<_NumTy, float>)
            // {
            //     expect.format("%.f", val);
            // }
            // else if constexpr (std::is_same_v<_NumTy, double>)
            // {
            //     expect.format("%.f", val);
            // }
            // else if constexpr (std::is_same_v<_NumTy, ldouble>)
            // {
            //     expect.format("%Lf", val);
            // }

            LLBC_ErrorAndReturnIf(testFunc(val, expect) != LLBC_OK, LLBC_FAILED);
        }
    }

    return LLBC_OK;
}


template <typename _NumTy>
int TestCase_Core_Utils_Text::_Test_Str2NumImpl()
{
    // Log.
    LLBC_PrintLn("  - Test LLBC_Str2Num<%s>():", typeid(_NumTy).name());

    // Define test lambda function.
    auto testFunc = [](const LLBC_String &str, int base, _NumTy expect)
    {
        _NumTy num = LLBC_Str2Num<_NumTy>(str.c_str(), base);
        LLBC_PrintLn("    - Test LLBC_Str2Num<%s>(%s, %d): return:%s, expect:%s",
            typeid(_NumTy).name(),
            str.c_str(),
            base,
            base == 16 ? LLBC_Num2Str<_NumTy, true>(num).c_str() : LLBC_Num2Str<_NumTy, false>(num).c_str(),
            base == 16 ? LLBC_Num2Str<_NumTy, true>(expect).c_str() : LLBC_Num2Str<_NumTy, false>(expect).c_str());

        bool equal;
        if constexpr (std::is_floating_point_v<_NumTy>)
            equal = LLBC_IsFloatAlmostEqual(num, expect);
        else
            equal = num == expect;

        if (!equal)
        {
            LLBC_FilePrintLn(stderr, "      - Test failed");
            llbc_assert(false && "LLBC_Str2Num() test failed");
            return LLBC_FAILED;
        }

        return LLBC_OK;
    };

    // Test.
    if constexpr (std::is_pointer_v<_NumTy>)
    {
        _NumTy ptr = nullptr;
        uint64 ptrVal = 0x0;
        std::map<LLBC_String, _NumTy> testPtrs;
        memcpy(&ptr, &ptrVal, std::min(sizeof(ptr), sizeof(ptrVal)));
        testPtrs["0x0"] = ptr;
        testPtrs["0x00"] = ptr;
        testPtrs["0x00000"] = ptr;

        ptr = nullptr;
        ptrVal = 0x123456;
        memcpy(&ptr, &ptrVal, std::min(sizeof(ptr), sizeof(ptrVal)));
        testPtrs["0x123456"] = ptr;

        for (auto &[testPtrStr, expect] : testPtrs)
        {
            LLBC_ErrorAndReturnIf(testFunc(testPtrStr, 10, expect) != LLBC_OK, LLBC_FAILED);
            LLBC_ErrorAndReturnIf(testFunc(testPtrStr, 16, expect) != LLBC_OK, LLBC_FAILED);
        }
    }
    else
    {
        typedef std::numeric_limits<_NumTy> _NumTyLmt;
        _NumTy testNums[]
        {
            _NumTyLmt::min(),
            _NumTyLmt::min() + static_cast<_NumTy>(50),
            _NumTyLmt::min() + static_cast<_NumTy>(100),
            _NumTy(),
            static_cast<_NumTy>(3.5),
        };

        for (auto testNum : testNums)
        {
            LLBC_String numStr;
            numStr = LLBC_Num2Str<_NumTy, false>(testNum);
            LLBC_ErrorAndReturnIf(testFunc(numStr, 10, testNum) != LLBC_OK, LLBC_FAILED);
            numStr = LLBC_Num2Str<_NumTy, true>(testNum);
            LLBC_ErrorAndReturnIf(testFunc(numStr, 16, testNum) != LLBC_OK, LLBC_FAILED);
        }
    }

    return LLBC_OK;
}

template <typename _NumTy>
int TestCase_Core_Utils_Text::_Test_Str2NumExceptionTest(const char *str,
                                                         int expectErrno,
                                                         int expectSubErrno)
{
    auto num = LLBC_Str2Num<_NumTy>(str);
    LLBC_PrintLn("    - LLBC_Num2Text<%s>(""%s"") exception test: return:%s, "
                 "retErr[errno:%x, subErrno:%d, errStr:%s], "
                 "expectErr[errno:%x, subErrno:%d, errStr:%s]",
                 typeid(_NumTy).name(),
                 str,
                 LLBC_Num2Str2<_NumTy>(num),
                 LLBC_GetLastError(),
                 LLBC_GetSubErrorNo(),
                 LLBC_FormatLastError(),
                 expectErrno,
                 expectSubErrno,
                 LLBC_StrErrorEx(expectErrno, expectSubErrno));

    if (LLBC_GetLastError() != expectErrno ||
        LLBC_GetSubErrorNo() != expectSubErrno)
    {
        LLBC_FilePrintLn(stderr,
                         "      - Test failed, errno:%x, subErrno:%d",
                         LLBC_GetLastError(), LLBC_GetSubErrorNo());
        llbc_assert(false && "LLBC_Str2Num() exception test failed");
        return LLBC_FAILED;
    }

    return LLBC_OK;
}