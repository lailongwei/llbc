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


#include "common/TestCase_Com_RTTI.h"

namespace
{
    struct UserDefineStruct {};

    class UserDefineClass {};
}

TestCase_Com_RTTI::TestCase_Com_RTTI()
{
}

TestCase_Com_RTTI::~TestCase_Com_RTTI()
{
}

int TestCase_Com_RTTI::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Common/RTTI test:");

    LLBC_PrintLn("Raw type name test:");
    LLBC_PrintLn("  LLBC_GetTypeName(bool): %s", LLBC_GetTypeName(bool));
    LLBC_PrintLn("  LLBC_GetTypeName(sint8): %s", LLBC_GetTypeName(sint8));
    LLBC_PrintLn("  LLBC_GetTypeName(uint8): %s", LLBC_GetTypeName(uint8));
    LLBC_PrintLn("  LLBC_GetTypeName(sint16): %s", LLBC_GetTypeName(sint16));
    LLBC_PrintLn("  LLBC_GetTypeName(uint16): %s", LLBC_GetTypeName(uint16));
    LLBC_PrintLn("  LLBC_GetTypeName(sint32): %s", LLBC_GetTypeName(sint32));
    LLBC_PrintLn("  LLBC_GetTypeName(uint32): %s", LLBC_GetTypeName(uint32));
    LLBC_PrintLn("  LLBC_GetTypeName(sint64): %s", LLBC_GetTypeName(sint64));
    LLBC_PrintLn("  LLBC_GetTypeName(uint64): %s", LLBC_GetTypeName(uint64));
    LLBC_PrintLn("  LLBC_GetTypeName(float): %s", LLBC_GetTypeName(float));
    LLBC_PrintLn("  LLBC_GetTypeName(double): %s", LLBC_GetTypeName(double));
    LLBC_PrintLn("  LLBC_GetTypeName(std::string): %s", LLBC_GetTypeName(std::string));
    LLBC_PrintLn("  LLBC_GetTypeName(LLBC_String): %s", LLBC_GetTypeName(LLBC_String));

    LLBC_PrintLn("\nPoint type test:");
    LLBC_PrintLn("  LLBC_GetTypeName((int *)): %s", LLBC_GetTypeName(int*));
    LLBC_PrintLn("  LLBC_GetTypeName((int *)nullptr): %s", LLBC_GetTypeName((int*)nullptr));
    LLBC_PrintLn("  LLBC_GetTypeName(int **): %s", LLBC_GetTypeName(int**));
    LLBC_PrintLn("  LLBC_GetTypeName((int **)nullptr): %s", LLBC_GetTypeName((int**)nullptr));
    LLBC_PrintLn("  LLBC_GetTypeName((int ***)nullptr): %s", LLBC_GetTypeName((int***)nullptr));

    class BaseCls1 {};
    class DerivedCls1 : public BaseCls1 {};

    class BaseCls2 { public: virtual ~BaseCls2() {} };
    class DerivedCls2 : public BaseCls2 {};

    LLBC_PrintLn("\n complex type constraint test:");
    LLBC_PrintLn("  LLBC_GetTypename(double): %s", LLBC_GetTypeName(double));
    LLBC_PrintLn("  LLBC_GetTypename(const double): %s", LLBC_GetTypeName(const double));
    LLBC_PrintLn("  LLBC_GetTypename(double const): %s", LLBC_GetTypeName(double const));
    LLBC_PrintLn("  LLBC_GetTypename(const double &): %s", LLBC_GetTypeName(const double &));
    LLBC_PrintLn("  LLBC_GetTypename(const double *): %s", LLBC_GetTypeName(const double *));
    LLBC_PrintLn("  LLBC_GetTypename(double const *): %s", LLBC_GetTypeName(double const *));
    LLBC_PrintLn("  LLBC_GetTypename(double * const): %s", LLBC_GetTypeName(double * const));
    LLBC_PrintLn("  LLBC_GetTypename(const double * const): %s", LLBC_GetTypeName(const double * const));
    LLBC_PrintLn("  LLBC_GetTypename(double const * const): %s", LLBC_GetTypeName(double const * const));
    LLBC_PrintLn("  LLBC_GetTypename(volatile double): %s", LLBC_GetTypeName(volatile double));

    BaseCls1 *baseCls1 = new BaseCls1;
    BaseCls1 *derivedCls1 = new DerivedCls1;
    BaseCls2 *baseCls2 = new BaseCls2;
    BaseCls2 *derivedCls2 = new DerivedCls2;
    LLBC_PrintLn("  LLBC_GetTypeName(BaseCls1): %s", LLBC_GetTypeName(BaseCls1));
    LLBC_PrintLn("  LLBC_GetTypeName(DerivedCls1): %s", LLBC_GetTypeName(DerivedCls1));
    LLBC_PrintLn("  LLBC_GetTypeName(BaseCls2): %s", LLBC_GetTypeName(BaseCls2));
    LLBC_PrintLn("  LLBC_GetTypeName(DerivedCls2): %s", LLBC_GetTypeName(DerivedCls2));
    LLBC_PrintLn("  LLBC_GetTypeName(baseCls1): %s", LLBC_GetTypeName(baseCls1));
    LLBC_PrintLn("  LLBC_GetTypeName(derivedCls1): %s", LLBC_GetTypeName(derivedCls1));
    LLBC_PrintLn("  LLBC_GetTypeName(baseCls2): %s", LLBC_GetTypeName(baseCls2));
    LLBC_PrintLn("  LLBC_GetTypeName(derivedCls2): %s", LLBC_GetTypeName(derivedCls2));
    LLBC_PrintLn("  LLBC_GetTypeName(*baseCls1): %s", LLBC_GetTypeName(*baseCls1));
    LLBC_PrintLn("  LLBC_GetTypeName(*derivedCls1): %s", LLBC_GetTypeName(*derivedCls1));
    LLBC_PrintLn("  LLBC_GetTypeName(*baseCls2): %s", LLBC_GetTypeName(*baseCls2));
    LLBC_PrintLn("  LLBC_GetTypeName(*derivedCls2): %s", LLBC_GetTypeName(*derivedCls2));

    LLBC_PrintLn("  LLBC_GetCompName(BaseCls1): %s", LLBC_GetCompName(BaseCls1));
    LLBC_PrintLn("  LLBC_GetCompName(DerivedCls1): %s", LLBC_GetCompName(DerivedCls1));
    LLBC_PrintLn("  LLBC_GetCompName(BaseCls2): %s", LLBC_GetCompName(BaseCls2));
    LLBC_PrintLn("  LLBC_GetCompName(DerivedCls2): %s", LLBC_GetCompName(DerivedCls2));
    LLBC_PrintLn("  LLBC_GetCompName(baseCls1): %s", LLBC_GetCompName(baseCls1));
    LLBC_PrintLn("  LLBC_GetCompName(derivedCls1): %s", LLBC_GetCompName(derivedCls1));
    LLBC_PrintLn("  LLBC_GetCompName(baseCls2): %s", LLBC_GetCompName(baseCls2));
    LLBC_PrintLn("  LLBC_GetCompName(derivedCls2): %s", LLBC_GetCompName(derivedCls2));
    LLBC_PrintLn("  LLBC_GetCompName(*baseCls1): %s", LLBC_GetCompName(*baseCls1));
    LLBC_PrintLn("  LLBC_GetCompName(*derivedCls1): %s", LLBC_GetCompName(*derivedCls1));
    LLBC_PrintLn("  LLBC_GetCompName(*baseCls2): %s", LLBC_GetCompName(*baseCls2));
    LLBC_PrintLn("  LLBC_GetCompName(*derivedCls2): %s", LLBC_GetCompName(*derivedCls2));


    LLBC_PrintLn("\nUser defined classes/structs test:");
    LLBC_PrintLn("  LLBC_GetTypeName(UserDefineStruct): %s", LLBC_GetTypeName(UserDefineStruct));
    LLBC_PrintLn("  LLBC_GetTypeName(UserDefineClass): %s", LLBC_GetTypeName(UserDefineClass));

    LLBC_PrintLn("Press any key to continue...");
    getchar();

    return LLBC_OK;
}


