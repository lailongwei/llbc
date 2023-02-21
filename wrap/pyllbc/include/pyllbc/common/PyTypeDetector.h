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

#include "pyllbc/common/LibHeader.h"

/**
 * \brief The obj type enumeration.
 */
enum pyllbc_ObjType
{
    PYLLBC_NONE_OBJ,
    PYLLBC_BOOL_OBJ,
    PYLLBC_INT_OBJ,
    PYLLBC_LONG_OBJ,
    PYLLBC_FLOAT_OBJ,
    PYLLBC_STR_OBJ,
    PYLLBC_UNICODE_OBJ,
    PYLLBC_BYTEARRAY_OBJ,
    PYLLBC_BUFFER_OBJ,
    PYLLBC_TUPLE_OBJ,
    PYLLBC_LIST_OBJ,
    PYLLBC_SEQ_OBJ,
    PYLLBC_DICT_OBJ,
    PYLLBC_UNKNOWN_OBJ
};

/**
 * \brief The python object type detector encapsulation.
 */
class LLBC_HIDDEN pyllbc_TypeDetector
{
public:
    /**
     * Detect given object type.
     * @return pyllbc_ObjType - the object type.
     */
    static pyllbc_ObjType Detect(PyObject *obj);
    static pyllbc_ObjType DetectCls(PyObject *cls);

public:
    /**
     * Specific type detectors.
     * @param[in] obj - will detect object.
     * @return bool - true means is specific type object, otherwise return false.
     */
    static bool IsNone(PyObject *obj);
    static bool IsBool(PyObject *obj);
    static bool IsInt(PyObject *obj);
    static bool IsLong(PyObject *obj);
    static bool IsFloat(PyObject *obj);
    static bool IsStr(PyObject *obj);
    static bool IsByteArray(PyObject *obj);
    static bool IsBuffer(PyObject *obj);
    static bool IsSequence(PyObject *obj);
    static bool IsDict(PyObject *obj);
};
