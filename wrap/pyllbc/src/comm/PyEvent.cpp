/**
 * @file    PyEvent.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/09/06
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Export.h"

#include "pyllbc/comm/PyEvent.h"

namespace
{
    typedef LLBC_Event Base;
}

pyllbc_Event::pyllbc_Event(int id, PyObject *data)
: Base(id)
, _data(data)
{
    Py_INCREF(_data);
}

pyllbc_Event::~pyllbc_Event()
{
    Py_DECREF(_data);
}

PyObject *pyllbc_Event::GetData()
{
    Py_INCREF(_data);
    return _data;
}
