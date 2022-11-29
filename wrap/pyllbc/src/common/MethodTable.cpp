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

#include "pyllbc/common/LibHeader.h"

#include "pyllbc/common/Errors.h"
#include "pyllbc/common/MethodTable.h"

namespace
{
    typedef pyllbc_MethodTable This;
}

pyllbc_MethodTable::pyllbc_MethodTable()
: _methods(nullptr)

, _size(0)
, _capacity(0)
{
    ReCapacity(128);
}

pyllbc_MethodTable::~pyllbc_MethodTable()
{
    free(_methods);
}

int pyllbc_MethodTable::AddMethod(const PyMethodDef &method)
{
    if (!method.ml_name ||
        strlen(method.ml_name) < 1 ||
        !method.ml_meth)
    {
        pyllbc_SetError("method def structure invalid", LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    if (_capacity - _size <= 1)
        ReCapacity(_capacity * 2);

    memmove(_methods + 1, _methods, sizeof(PyMethodDef) * _size);
    memcpy(_methods, &method, sizeof(PyMethodDef));

    _size += 1;

    return LLBC_OK;
}

int pyllbc_MethodTable::RemoveMethod(const LLBC_String &name)
{
    for (sint32 i = 0; i < _size; ++i)
    {
        const PyMethodDef &method = _methods[i];
        if (name == method.ml_name)
        {
            memmove(_methods + i, _methods + i + 1, _size - i - 1);
            memset(_methods + _size - 1, 0, sizeof(PyMethodDef));

            _size -= 1;

            return LLBC_OK;
        }
    }

    pyllbc_SetError(LLBC_ERROR_NOT_FOUND);
    return LLBC_FAILED;
}

PyMethodDef *pyllbc_MethodTable::GetMethods()
{
    const This *cThis = this;
    return const_cast<PyMethodDef *>(cThis->GetMethods());
}

const PyMethodDef *pyllbc_MethodTable::GetMethods() const
{
    return _methods;
}

void pyllbc_MethodTable::ReCapacity(sint32 cap)
{
    ASSERT(cap > _capacity && 
        "pyllbc_MethodTable::ReCaacity(): new capacity size was invalid!");

    _methods = LLBC_Realloc(PyMethodDef, 
                            _methods, 
                            cap * sizeof(PyMethodDef));

    memset(_methods + _capacity, 0, sizeof(PyMethodDef) * (cap - _capacity));
    _capacity = cap;
}