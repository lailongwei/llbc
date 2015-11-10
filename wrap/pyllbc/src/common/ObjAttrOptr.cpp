/**
 * @file    ObjAttrOptr.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/10
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Export.h"

#include "pyllbc/common/ObjAttrOptr.h"

pyllbc_ObjAttrOptr::pyllbc_ObjAttrOptr(PyObject *obj)
: _obj(obj)
{
    ASSERT(_obj && 
        "pyllbc_ObjAttrOptr::ctor(): obj parameter must be not null!");

    Py_INCREF(_obj);
}

pyllbc_ObjAttrOptr::~pyllbc_ObjAttrOptr()
{
    Py_DECREF(_obj);
}

int pyllbc_ObjAttrOptr::DelAttr(const LLBC_String &name)
{
    if (PyObject_DelAttrString(_obj, name.c_str()) == -1)
    {
        pyllbc_TransferPyError();
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
}

