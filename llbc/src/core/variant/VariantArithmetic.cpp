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


#include "llbc/common/Export.h"

#include "llbc/core/variant/Variant.h"
#include "llbc/core/variant/VariantArithmetic.h"

__LLBC_NS_BEGIN

void LLBC_VariantArithmetic::Performs(LLBC_Variant &left, const LLBC_Variant &right, int type)
{
    switch (left.GetType())
    {
    case LLBC_VariantType::VT_RAW_BOOL:
        LLBC_VariantArithmetic::Performs_bool_any(left, right, type);
        break;

    case LLBC_VariantType::VT_RAW_SINT8:
        LLBC_VariantArithmetic::Performs_int8_any(left, right, type);
        break;

    case LLBC_VariantType::VT_RAW_UINT8:
        LLBC_VariantArithmetic::Performs_uint8_any(left, right, type);
        break;

    case LLBC_VariantType::VT_RAW_SINT16:
        LLBC_VariantArithmetic::Performs_int16_any(left, right, type);
        break;

    case LLBC_VariantType::VT_RAW_UINT16:
        LLBC_VariantArithmetic::Performs_uint16_any(left, right, type);
        break;

    case LLBC_VariantType::VT_RAW_SINT32:
        LLBC_VariantArithmetic::Performs_int32_any(left, right, type);
        break;

    case LLBC_VariantType::VT_RAW_UINT32:
        LLBC_VariantArithmetic::Performs_uint32_any(left, right, type);
        break;

    case LLBC_VariantType::VT_RAW_LONG:
        LLBC_VariantArithmetic::Performs_long_any(left, right, type);
        break;

    case LLBC_VariantType::VT_RAW_ULONG:
        LLBC_VariantArithmetic::Performs_ulong_any(left, right, type);
        break;

    case LLBC_VariantType::VT_RAW_PTR:
        LLBC_VariantArithmetic::Performs_ptr_any(left, right, type);
        break;

    case LLBC_VariantType::VT_RAW_SINT64:
        LLBC_VariantArithmetic::Performs_int64_any(left, right, type);
        break;

    case LLBC_VariantType::VT_RAW_UINT64:
        LLBC_VariantArithmetic::Performs_uint64_any(left, right, type);
        break;

    case LLBC_VariantType::VT_RAW_FLOAT:
        LLBC_VariantArithmetic::Performs_float_any(left, right, type);
        break;

    case LLBC_VariantType::VT_RAW_DOUBLE:
        LLBC_VariantArithmetic::Performs_double_any(left, right, type);
        break;

    default:
        break;
    }
}

void LLBC_VariantArithmetic::Performs_bool_any(
    LLBC_Variant &left, const LLBC_Variant &right, int type)
{
    switch (right.GetType())
    {
    case LLBC_VariantType::VT_RAW_BOOL:
        left = Performs_raw_operation(left.AsBool(), right.AsBool(), type);
        break;

    case LLBC_VariantType::VT_RAW_SINT8:
        left = Performs_raw_operation(left.AsInt8(), right.AsInt8(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT8:
        left = Performs_raw_operation(left.AsUInt8(), right.AsUInt8(), type);
        break;

    case LLBC_VariantType::VT_RAW_SINT16:
        left = Performs_raw_operation(left.AsInt16(), right.AsInt16(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT16:
        left = Performs_raw_operation(left.AsUInt16(), right.AsUInt16(), type);
        break;

    case LLBC_VariantType::VT_RAW_SINT32:
        left = Performs_raw_operation(left.AsInt32(), right.AsInt32(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT32:
        left = Performs_raw_operation(left.AsUInt32(), right.AsUInt32(), type);
        break;

    case LLBC_VariantType::VT_RAW_LONG:
        left = Performs_raw_operation(left.AsLong(), right.AsLong(), type);
        break;

    case LLBC_VariantType::VT_RAW_ULONG:
        left = Performs_raw_operation(left.AsULong(), right.AsULong(), type);
        break;

    case LLBC_VariantType::VT_RAW_PTR:
        left = Performs_raw_operation(left.AsPtr<void>(), right.AsPtr<void>(), type);
        break;

    case LLBC_VariantType::VT_RAW_SINT64:
        left = Performs_raw_operation(left.AsInt64(), right.AsInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT64:
        left = Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_FLOAT:
    case LLBC_VariantType::VT_RAW_DOUBLE:
        left = Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
        break;

    default:
        break;
    }
}

void LLBC_VariantArithmetic::Performs_int8_any(
    LLBC_Variant &left, const LLBC_Variant &right, int type)
{
    switch (right.GetType())
    {
    case LLBC_VariantType::VT_RAW_BOOL:
    case LLBC_VariantType::VT_RAW_SINT8:
        left = Performs_raw_operation(left.AsInt8(), right.AsInt8(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT8:
        left = Performs_raw_operation(left.AsUInt8(), right.AsUInt8(), type);
        break;

    case LLBC_VariantType::VT_RAW_SINT16:
        left = Performs_raw_operation(left.AsInt16(), right.AsInt16(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT16:
        left = Performs_raw_operation(left.AsUInt16(), right.AsUInt16(), type);
        break;

    case LLBC_VariantType::VT_RAW_SINT32:
        left = Performs_raw_operation(left.AsInt32(), right.AsInt32(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT32:
        left = Performs_raw_operation(left.AsUInt32(), right.AsUInt32(), type);
        break;

    case LLBC_VariantType::VT_RAW_LONG:
        left = Performs_raw_operation(left.AsLong(), right.AsLong(), type);
        break;

    case LLBC_VariantType::VT_RAW_ULONG:
        left = Performs_raw_operation(left.AsULong(), right.AsULong(), type);
        break;

    case LLBC_VariantType::VT_RAW_PTR:
        left = Performs_raw_operation(left.AsPtr<void>(), right.AsPtr<void>(), type);
        break;

    case LLBC_VariantType::VT_RAW_SINT64:
        left = Performs_raw_operation(left.AsInt64(), right.AsInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT64:
        left = Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_FLOAT:
    case LLBC_VariantType::VT_RAW_DOUBLE:
        left = Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
        break;

    default:
        break;
    }
}

void LLBC_VariantArithmetic::Performs_uint8_any(
    LLBC_Variant &left, const LLBC_Variant &right, int type)
{
    switch (right.GetType())
    {
    case LLBC_VariantType::VT_RAW_BOOL:
    case LLBC_VariantType::VT_RAW_SINT8:
    case LLBC_VariantType::VT_RAW_UINT8:
        left = Performs_raw_operation(left.AsUInt8(), right.AsUInt8(), type);
        break;

    case LLBC_VariantType::VT_RAW_SINT16:
        left = Performs_raw_operation(left.AsInt16(), right.AsInt16(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT16:
        left = Performs_raw_operation(left.AsUInt16(), right.AsUInt16(), type);
        break;

    case LLBC_VariantType::VT_RAW_SINT32:
        left = Performs_raw_operation(left.AsInt32(), right.AsInt32(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT32:
        left = Performs_raw_operation(left.AsUInt32(), right.AsUInt32(), type);
        break;

    case LLBC_VariantType::VT_RAW_LONG:
        left = Performs_raw_operation(left.AsLong(), right.AsLong(), type);
        break;

    case LLBC_VariantType::VT_RAW_ULONG:
        left = Performs_raw_operation(left.AsULong(), right.AsULong(), type);
        break;

    case LLBC_VariantType::VT_RAW_PTR:
        left = Performs_raw_operation(left.AsPtr<void>(), right.AsPtr<void>(), type);
        break;

    case LLBC_VariantType::VT_RAW_SINT64:
        left = Performs_raw_operation(left.AsInt64(), right.AsInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT64:
        left = Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_FLOAT:
    case LLBC_VariantType::VT_RAW_DOUBLE:
        left = Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
        break;

    default:
        break;
    }
}

void LLBC_VariantArithmetic::Performs_int16_any(
    LLBC_Variant &left, const LLBC_Variant &right, int type)
{
    switch (right.GetType())
    {
    case LLBC_VariantType::VT_RAW_BOOL:
    case LLBC_VariantType::VT_RAW_SINT8:
    case LLBC_VariantType::VT_RAW_UINT8:
    case LLBC_VariantType::VT_RAW_SINT16:
        left = Performs_raw_operation(left.AsInt16(), right.AsInt16(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT16:
        left = Performs_raw_operation(left.AsUInt16(), right.AsUInt16(), type);
        break;

    case LLBC_VariantType::VT_RAW_SINT32:
        left = Performs_raw_operation(left.AsInt32(), right.AsInt32(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT32:
        left = Performs_raw_operation(left.AsUInt32(), right.AsUInt32(), type);
        break;

    case LLBC_VariantType::VT_RAW_LONG:
        left = Performs_raw_operation(left.AsLong(), right.AsLong(), type);
        break;

    case LLBC_VariantType::VT_RAW_ULONG:
        left = Performs_raw_operation(left.AsULong(), right.AsULong(), type);
        break;

    case LLBC_VariantType::VT_RAW_PTR:
        left = Performs_raw_operation(left.AsPtr<void>(), right.AsPtr<void>(), type);
        break;

    case LLBC_VariantType::VT_RAW_SINT64:
        left = Performs_raw_operation(left.AsInt64(), right.AsInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT64:
        left = Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_FLOAT:
    case LLBC_VariantType::VT_RAW_DOUBLE:
        left = Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
        break;

    default:
        break;
    }
}

void LLBC_VariantArithmetic::Performs_uint16_any(
    LLBC_Variant &left, const LLBC_Variant &right, int type)
{
    switch (right.GetType())
    {
    case LLBC_VariantType::VT_RAW_BOOL:
    case LLBC_VariantType::VT_RAW_SINT8:
    case LLBC_VariantType::VT_RAW_UINT8:
    case LLBC_VariantType::VT_RAW_SINT16:
    case LLBC_VariantType::VT_RAW_UINT16:
        left = Performs_raw_operation(left.AsUInt16(), right.AsUInt16(), type);
        break;

    case LLBC_VariantType::VT_RAW_SINT32:
        left = Performs_raw_operation(left.AsInt32(), right.AsInt32(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT32:
        left = Performs_raw_operation(left.AsUInt32(), right.AsUInt32(), type);
        break;

    case LLBC_VariantType::VT_RAW_LONG:
        left = Performs_raw_operation(left.AsLong(), right.AsLong(), type);
        break;

    case LLBC_VariantType::VT_RAW_ULONG:
        left = Performs_raw_operation(left.AsULong(), right.AsULong(), type);
        break;

    case LLBC_VariantType::VT_RAW_PTR:
        left = Performs_raw_operation(left.AsPtr<void>(), right.AsPtr<void>(), type);
        break;

    case LLBC_VariantType::VT_RAW_SINT64:
        left = Performs_raw_operation(left.AsInt64(), right.AsInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT64:
        left = Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_FLOAT:
    case LLBC_VariantType::VT_RAW_DOUBLE:
        left = Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
        break;

    default:
        break;
    }
}

void LLBC_VariantArithmetic::Performs_int32_any(
    LLBC_Variant &left, const LLBC_Variant &right, int type)
{
    switch (right.GetType())
    {
    case LLBC_VariantType::VT_RAW_BOOL:
    case LLBC_VariantType::VT_RAW_SINT8:
    case LLBC_VariantType::VT_RAW_UINT8:
    case LLBC_VariantType::VT_RAW_SINT16:
    case LLBC_VariantType::VT_RAW_UINT16:
    case LLBC_VariantType::VT_RAW_SINT32:
        left = Performs_raw_operation(left.AsInt32(), right.AsInt32(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT32:
        left = Performs_raw_operation(left.AsUInt32(), right.AsUInt32(), type);
        break;

    case LLBC_VariantType::VT_RAW_LONG:
        left = Performs_raw_operation(left.AsLong(), right.AsLong(), type);
        break;

    case LLBC_VariantType::VT_RAW_ULONG:
        left = Performs_raw_operation(left.AsULong(), right.AsULong(), type);
        break;

    case LLBC_VariantType::VT_RAW_PTR:
        left = Performs_raw_operation(left.AsPtr<void>(), right.AsPtr<void>(), type);
        break;

    case LLBC_VariantType::VT_RAW_SINT64:
        left = Performs_raw_operation(left.AsInt64(), right.AsInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT64:
        left = Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_FLOAT:
    case LLBC_VariantType::VT_RAW_DOUBLE:
        left = Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
        break;

    default:
        break;
    }
}

void LLBC_VariantArithmetic::Performs_uint32_any(
    LLBC_Variant &left, const LLBC_Variant &right, int type)
{
    switch (right.GetType())
    {
    case LLBC_VariantType::VT_RAW_BOOL:
    case LLBC_VariantType::VT_RAW_SINT8:
    case LLBC_VariantType::VT_RAW_UINT8:
    case LLBC_VariantType::VT_RAW_SINT16:
    case LLBC_VariantType::VT_RAW_UINT16:
    case LLBC_VariantType::VT_RAW_SINT32:
    case LLBC_VariantType::VT_RAW_UINT32:
        left = Performs_raw_operation(left.AsUInt32(), right.AsUInt32(), type);
        break;

    case LLBC_VariantType::VT_RAW_LONG:
        left = Performs_raw_operation(left.AsLong(), right.AsLong(), type);
        break;

    case LLBC_VariantType::VT_RAW_ULONG:
        left = Performs_raw_operation(left.AsULong(), right.AsULong(), type);
        break;

    case LLBC_VariantType::VT_RAW_PTR:
        left = Performs_raw_operation(left.AsPtr<void>(), right.AsPtr<void>(), type);
        break;

    case LLBC_VariantType::VT_RAW_SINT64:
        left = Performs_raw_operation(left.AsInt64(), right.AsInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT64:
        left = Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_FLOAT:
    case LLBC_VariantType::VT_RAW_DOUBLE:
        left = Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
        break;

    default:
        break;
    }
}

void LLBC_VariantArithmetic::Performs_long_any(
    LLBC_Variant &left, const LLBC_Variant &right, int type)
{
    switch (right.GetType())
    {
    case LLBC_VariantType::VT_RAW_BOOL:
    case LLBC_VariantType::VT_RAW_SINT8:
    case LLBC_VariantType::VT_RAW_UINT8:
    case LLBC_VariantType::VT_RAW_SINT16:
    case LLBC_VariantType::VT_RAW_UINT16:
    case LLBC_VariantType::VT_RAW_SINT32:
    case LLBC_VariantType::VT_RAW_UINT32:
    case LLBC_VariantType::VT_RAW_LONG:
        left = Performs_raw_operation(left.AsLong(), right.AsLong(), type);
        break;

    case LLBC_VariantType::VT_RAW_ULONG:
        left = Performs_raw_operation(left.AsULong(), right.AsULong(), type);
        break;

    case LLBC_VariantType::VT_RAW_PTR:
        left = Performs_raw_operation(left.AsPtr<void>(), right.AsPtr<void>(), type);
        break;

    case LLBC_VariantType::VT_RAW_SINT64:
        left = Performs_raw_operation(left.AsInt64(), right.AsInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT64:
        left = Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_FLOAT:
    case LLBC_VariantType::VT_RAW_DOUBLE:
        left = Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
        break;

    default:
        break;
    }
}

void LLBC_VariantArithmetic::Performs_ulong_any(
    LLBC_Variant &left, const LLBC_Variant &right, int type)
{
    switch (right.GetType())
    {
    case LLBC_VariantType::VT_RAW_BOOL:
    case LLBC_VariantType::VT_RAW_SINT8:
    case LLBC_VariantType::VT_RAW_UINT8:
    case LLBC_VariantType::VT_RAW_SINT16:
    case LLBC_VariantType::VT_RAW_UINT16:
    case LLBC_VariantType::VT_RAW_SINT32:
    case LLBC_VariantType::VT_RAW_UINT32:
    case LLBC_VariantType::VT_RAW_LONG:
    case LLBC_VariantType::VT_RAW_ULONG:
        left = Performs_raw_operation(left.AsULong(), right.AsULong(), type);
        break;

    case LLBC_VariantType::VT_RAW_PTR:
        left = Performs_raw_operation(left.AsPtr<void>(), right.AsPtr<void>(), type);
        break;

    case LLBC_VariantType::VT_RAW_SINT64:
        left = Performs_raw_operation(left.AsInt64(), right.AsInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT64:
        left = Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_FLOAT:
    case LLBC_VariantType::VT_RAW_DOUBLE:
        left = Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
        break;

    default:
        break;
    }
}

void LLBC_VariantArithmetic::Performs_ptr_any(
    LLBC_Variant &left, const LLBC_Variant &right, int type)
{
    switch (right.GetType())
    {
    case LLBC_VariantType::VT_RAW_BOOL:
    case LLBC_VariantType::VT_RAW_SINT8:
    case LLBC_VariantType::VT_RAW_UINT8:
    case LLBC_VariantType::VT_RAW_SINT16:
    case LLBC_VariantType::VT_RAW_UINT16:
    case LLBC_VariantType::VT_RAW_SINT32:
    case LLBC_VariantType::VT_RAW_UINT32:
    case LLBC_VariantType::VT_RAW_LONG:
    case LLBC_VariantType::VT_RAW_ULONG:
    case LLBC_VariantType::VT_RAW_PTR:
        left = Performs_raw_operation(left.AsPtr<void>(), right.AsPtr<void>(), type);
        break;

    case LLBC_VariantType::VT_RAW_SINT64:
        left = Performs_raw_operation(left.AsInt64(), right.AsInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT64:
        left = Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_FLOAT:
    case LLBC_VariantType::VT_RAW_DOUBLE:
        left = Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
        break;

    default:
        break;
    }
}

void LLBC_VariantArithmetic::Performs_int64_any(
    LLBC_Variant &left, const LLBC_Variant &right, int type)
{
    switch (right.GetType())
    {
    case LLBC_VariantType::VT_RAW_BOOL:
    case LLBC_VariantType::VT_RAW_SINT8:
    case LLBC_VariantType::VT_RAW_UINT8:
    case LLBC_VariantType::VT_RAW_SINT16:
    case LLBC_VariantType::VT_RAW_UINT16:
    case LLBC_VariantType::VT_RAW_SINT32:
    case LLBC_VariantType::VT_RAW_UINT32:
    case LLBC_VariantType::VT_RAW_LONG:
    case LLBC_VariantType::VT_RAW_ULONG:
    case LLBC_VariantType::VT_RAW_PTR:
    case LLBC_VariantType::VT_RAW_SINT64:
        left = Performs_raw_operation(left.AsInt64(), right.AsInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_UINT64:
        left = Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_FLOAT:
    case LLBC_VariantType::VT_RAW_DOUBLE:
        left = Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
        break;

    default:
        break;
    }
}

void LLBC_VariantArithmetic::Performs_uint64_any(
    LLBC_Variant &left, const LLBC_Variant &right, int type)
{
    switch (right.GetType())
    {
    case LLBC_VariantType::VT_RAW_BOOL:
    case LLBC_VariantType::VT_RAW_SINT8:
    case LLBC_VariantType::VT_RAW_UINT8:
    case LLBC_VariantType::VT_RAW_SINT16:
    case LLBC_VariantType::VT_RAW_UINT16:
    case LLBC_VariantType::VT_RAW_SINT32:
    case LLBC_VariantType::VT_RAW_UINT32:
    case LLBC_VariantType::VT_RAW_LONG:
    case LLBC_VariantType::VT_RAW_ULONG:
    case LLBC_VariantType::VT_RAW_PTR:
    case LLBC_VariantType::VT_RAW_SINT64:
    case LLBC_VariantType::VT_RAW_UINT64:
        left = Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
        break;

    case LLBC_VariantType::VT_RAW_FLOAT:
    case LLBC_VariantType::VT_RAW_DOUBLE:
        left = Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
        break;

    default:
        break;
    }
}

void LLBC_VariantArithmetic::Performs_float_any(
    LLBC_Variant &left, const LLBC_Variant &right, int type)
{
    left = static_cast<float>(Performs_raw_operation(left.AsDouble(), right.AsDouble(), type));
}

void LLBC_VariantArithmetic::Performs_double_any(
    LLBC_Variant &left, const LLBC_Variant &right, int type)
{
    left = Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
}

__LLBC_NS_END
