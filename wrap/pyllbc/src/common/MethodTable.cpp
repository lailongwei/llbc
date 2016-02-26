/**
 * @file    MethodTable.cpp
 * @author  Longwei Lai
 * @date    2014/07/30
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/LibHeader.h"

#include "pyllbc/common/Errors.h"
#include "pyllbc/common/MethodTable.h"

namespace
{
    typedef pyllbc_MethodTable This;
}

pyllbc_MethodTable::pyllbc_MethodTable()
: _methods(NULL)

, _size(0)
, _capacity(0)
{
    ReCapacity(128);
}

pyllbc_MethodTable::~pyllbc_MethodTable()
{
    LLBC_Free(_methods);
}

int pyllbc_MethodTable::AddMethod(const PyMethodDef &method)
{
    if (!method.ml_name ||
            LLBC_StrLen(method.ml_name) < 1 ||
            !method.ml_meth)
    {
        pyllbc_SetError("method def structure invalid", LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    if (_capacity - _size <= 1)
        ReCapacity(_capacity * 2);

    LLBC_MemCpy(_methods + 1, _methods, sizeof(PyMethodDef) * _size);
    LLBC_MemCpy(_methods, &method, sizeof(PyMethodDef));

    _size += 1;

    return LLBC_OK;
}

int pyllbc_MethodTable::RemoveMethod(const LLBC_String &name)
{
    for (sint32 i = 0; i < _size; i++)
    {
        const PyMethodDef &method = _methods[i];
        if (name == method.ml_name)
        {
            LLBC_MemCpy(_methods + i, _methods + i + 1, _size - i - 1);
            LLBC_MemSet(_methods + _size - 1, 0, sizeof(PyMethodDef));

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

    LLBC_MemSet(_methods + _capacity, 
                0, 
                sizeof(PyMethodDef) * (cap - _capacity));

    _capacity = cap;
}