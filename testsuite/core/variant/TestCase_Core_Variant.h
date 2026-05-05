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

#pragma once

#include "llbc.h"
using namespace llbc;

class TestCase_Core_Variant final : public LLBC_BaseTestCase
{
public:
    int Run(int argc, char *argv[]) override;

private:
    int VariantTypeTest();

private:
    int ConstructTest();

    int ConstructTest_DftConstruct();
    int ConstructTest_RawType();
    int ConstructTest_StrType();
    int ConstructTest_SeqType();
    template <typename _SeqTy>
    int ConstructTest_OneSeqType(_SeqTy &&seq);
    int ConstructTest_DictType();
    template <typename _DictTy>
    int ConstructTest_OneDictType(_DictTy &&dict);
    int ConstructTest_VariantType();

private:
    int AssignmentTest();

    int AssignmentTest_RawType();
    int AssignmentTest_StrType();

    int AssignmentTest_SeqType();
    template <typename _SeqTy>
    int AssignmentTest_OneSeqType(_SeqTy &&seq);

    int AssignmentTest_DictType();
    template <typename _DictTy>
    int AssignmentTest_OneDictType(_DictTy &&dict);

    int AssignmentTest_VariantType();

    template <typename... _RawTys>
    int AssignmentTest_RawVariantTypes();
    template <typename _RawTy>
    int AssignmentTest_OneRawVariantType();

    template <typename... _StrTys>
    int AssignmentTest_StrVariantTypes();
    template <typename _StrTy>
    int AssignmentTest_OneStrVariantType();

    template <typename... _SeqTys>
    int AssignmentTest_SeqVariantTypes();
    template <typename _SeqTy>
    int AssignmentTest_OneSeqVariantType();

    template <typename... _DictTys>
    int AssignmentTest_DictVariantTypes();
    template <typename _DictTy>
    int AssignmentTest_OneDictVariantType();

    template <typename _RealTy>
    int AssignmentTest_OneVariantType(LLBC_Variant &var);
    template <typename _RealTy, typename _VarTy>
    int AssignmentTest_OneVariantTypeOneRefType(_VarTy &&var);

public:
    int GetTypeTest();

    template <typename... _RawTys>
    int GetTypeTest_RawTypes();
    template <typename _RawTy>
    int GetTypeTest_OneRawType();

    template <typename... _StrTys>
    int GetTypeTest_StrTypes();
    template <typename _StrTy>
    int GetTypeTest_OneStrType();

    template <typename... _SeqTys>
    int GetTypeTest_SeqTypes();
    template <typename _SeqTy>
    int GetTypeTest_OneSeqType();

    template <typename... _DictTys>
    int GetTypeTest_DictTypes();
    template <typename _DictTy>
    int GetTypeTest_OneDictType();

public:
    int IsXxxTest();

    template <typename... _RawTys>
    int IsXxxTest_RawTypes();
    template <typename _RawTy>
    int IsXxxTest_OneRawType();

    template <typename... _StrTys>
    int IsXxxTest_StrTypes();
    template <typename _StrTy>
    int IsXxxTest_OneStrType();

    template <typename... _SeqTys>
    int IsXxxTest_SeqTypes();
    template <typename _SeqTy>
    int IsXxxTest_OneSeqType();

    template <typename... _DictTys>
    int IsXxxTest_DictTypes();
    template <typename _DictTy>
    int IsXxxTest_OneDictType();

public:
    int BecomeTest();

    template <typename... _RawTys>
    int BecomeTest_RawTypes();
    template <typename _RawTy>
    int BecomeTest_OneRawType();

    template <typename... _StrTys>
    int BecomeTest_StrTypes();
    template <typename _StrTy>
    int BecomeTest_OneStrType();

    int BecomeTest_SeqType();

    int BecomeTest_DictType();

public:
    int AsTest();

    template <typename... _RawTys>
    int AsTest_RawTypes();
    template <typename _RawTy>
    int AsTest_OneRawType();

    template <typename... _FloatingTys>
    int AsTest_FloatingTypes();
    template <typename _FloatingTy>
    int AsTest_OneFloatingType();

    int AsTest_StrType();

    int AsTest_SeqType();

    int AsTest_DictType();

public:
    int AsLooseBoolTest();

    template <typename... _RawTys>
    int AsLooseBoolTest_RawTypes();
    template <typename _RawTy>
    int AsLooseBoolTest_OneRawType();

public:
    int ImplicitValueFetchTest();

public:
    int ClearTest();

public:
    int IsEmptyTest();

public:
    int SizeAndCapacityTest();

public:
    int CountAndContainsTest();
};
