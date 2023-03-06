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
 * \brief The python layer object operation util encapsulation.
 */
class LLBC_HIDDEN pyllbc_ObjUtil
{
public:
    /**
     * Get the object string representation.
     * @param[in] obj - the python layer object.
     * @return LLBC_String - the object string representation.
     *                       call PyErr_Occurred() to check this method execute succeed or not.
     */
    static LLBC_String GetObjStr(PyObject *obj);

    /**
     * Convert python object to LLBC_Variant object.
     * @param[in] obj  - the python object.
     * @param[out] var - the converted LLBC_Variant object, if convert failed, will not set this variant.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int Obj2Variant(PyObject *obj, LLBC_Variant &var);

    /**
     * Convert LLBC_Variant object to python object.
     * @param[in] var - the LLBC_Variant object.
     * @return PyObject * - the converted python object(new reference).
     */
    static PyObject *Variant2Obj(const LLBC_Variant &var);
};
