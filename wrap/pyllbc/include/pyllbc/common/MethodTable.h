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
 * \brief The python method table class encapsulation.
 */
class LLBC_HIDDEN pyllbc_MethodTable
{
public:
    /**
     * Default constructor & destructor.
     */
    pyllbc_MethodTable();
    ~pyllbc_MethodTable();

public:
    /**
     * Add python method.
     * @param[in] method - python method.
     * @return int - return 0 if success, otherwise return -1.
     */
    int AddMethod(const PyMethodDef &method);

    /**
     * Remove method by method name.
     * @param[in] name - method name.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveMethod(const LLBC_String &name);

    /**
     * Get method table.
     * @return PyMethodDef * - the method table.
     */
    PyMethodDef *GetMethods();

    /**
     * Get method table(const method).
	 * @return const PyMethodDef * - the method table.
     */
    const PyMethodDef *GetMethods() const;

private:
    /**
     * Readjust the method table store area.
     * @param[in] cap - new capacity.
     */
    void ReCapacity(sint32 cap);

private:
    PyMethodDef *_methods;
    sint32 _size;
    sint32 _capacity;
};
