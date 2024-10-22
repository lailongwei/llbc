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
 * \brief The object attribute operator template class encapsulation.
 */
class LLBC_HIDDEN pyllbc_ObjAttrOptr
{
public:
    /**
     * Parameter constructor.
     * @param[in] obj - the object pointer.
     */
    pyllbc_ObjAttrOptr(PyObject *obj);

    /**
     * Destructor.
     */
    ~pyllbc_ObjAttrOptr();

public:
    /**
     * Get the object attribute.
     * @param[in]  name - attribute name.
     * @param[out] val  - attribute value, will set when call success.
     * @return int - return 0 if successed, otherwise return -1, see LLBC_GetLastError().
     */
    template <typename _Val>
    int GetAttr(const LLBC_String &name, _Val &val);

    /**
     * Set object attribute.
     * @param[in] name - the attribute name.
     * @param[in] val  - the attribute value.
     *                   Note, this class template method already specialization.
     *                   Support type: bool, LLBC_String, const char *, int/long, float/double.
     * @return int - return 0 if successed, otherwise return -1. see LLBC_GetLastError().
     */
    template <typename _Val>
    int SetAttr(const LLBC_String &name, const _Val &val);

    /**
     * Delete specific name attribute.
     * @param[in] name - the property name.
     * @return int - return 0 if success, otherwise return -1.
     */
    int DelAttr(const LLBC_String &name);

private:
    PyObject *_obj;
};

#include "pyllbc/common/ObjAttrOptrInl.h"


