/**
 * @file    PyTypeDetector.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/20
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Export.h"

#include "pyllbc/common/Macro.h"
#include "pyllbc/common/PyTypeDetector.h"

namespace
{
    typedef pyllbc_TypeDetector This;
}

pyllbc_ObjType pyllbc_TypeDetector::Detect(PyObject *obj)
{
    if (PyObject_IsInstance(obj, PYLLBC_NONE_CLS))
        return PYLLBC_NONE_OBJ;
    else if (PyBool_Check(obj))
        return PYLLBC_BOOL_OBJ;
    else if (PyInt_Check(obj))
        return PYLLBC_INT_OBJ;
    else if (PyLong_Check(obj))
        return PYLLBC_LONG_OBJ;
    else if (PyFloat_Check(obj))
        return PYLLBC_FLOAT_OBJ;
    else if (PyString_Check(obj))
        return PYLLBC_STR_OBJ;
    else if (PyUnicode_Check(obj))
        return PYLLBC_UNICODE_OBJ;
    else if (PyByteArray_Check(obj))
        return PYLLBC_BYTEARRAY_OBJ;
    else if (PyBuffer_Check(obj))
        return PYLLBC_BUFFER_OBJ;
    else if (PyTuple_Check(obj))
        return PYLLBC_TUPLE_OBJ;
    else if (PyList_Check(obj))
        return PYLLBC_LIST_OBJ;
    else if (PySequence_Check(obj))
        return PYLLBC_SEQ_OBJ;
    else if (PyDict_Check(obj))
        return PYLLBC_DICT_OBJ;

    return PYLLBC_UNKNOWN_OBJ;
}

pyllbc_ObjType pyllbc_TypeDetector::DetectCls(PyObject *cls)
{
    if (cls == PYLLBC_NONE_CLS)
        return PYLLBC_NONE_OBJ;
    else if (cls == PYLLBC_BOOL_CLS)
        return PYLLBC_BOOL_OBJ;
    else if (cls == PYLLBC_INT_CLS)
        return PYLLBC_INT_OBJ;
    else if (cls == PYLLBC_LONG_CLS)
        return PYLLBC_LONG_OBJ;
    else if (cls == PYLLBC_FLOAT_CLS)
        return PYLLBC_FLOAT_OBJ;
    else if (cls == PYLLBC_STR_CLS)
        return PYLLBC_STR_OBJ;
    else if (cls == PYLLBC_UNICODE_CLS)
        return PYLLBC_UNICODE_OBJ;
    else if (cls == PYLLBC_BYTEARRAY_CLS)
        return PYLLBC_BYTEARRAY_OBJ;
    else if (cls == PYLLBC_BUFFER_CLS)
        return PYLLBC_BUFFER_OBJ;
    else if (cls == PYLLBC_TUPLE_CLS)
        return PYLLBC_TUPLE_OBJ;
    else if (cls == PYLLBC_LIST_CLS)
        return PYLLBC_LIST_OBJ;
    else if (cls == PYLLBC_DICT_CLS)
        return PYLLBC_DICT_OBJ;

    return PYLLBC_UNKNOWN_OBJ;
}

bool pyllbc_TypeDetector::IsNone(PyObject *obj)
{
    return This::Detect(obj) == PYLLBC_NONE_OBJ;
}

bool pyllbc_TypeDetector::IsBool(PyObject *obj)
{
    return This::Detect(obj) == PYLLBC_BOOL_OBJ;
}

bool pyllbc_TypeDetector::IsInt(PyObject *obj)
{
    return This::Detect(obj) == PYLLBC_INT_OBJ;
}

bool pyllbc_TypeDetector::IsLong(PyObject *obj)
{
    return This::Detect(obj) == PYLLBC_LONG_OBJ;
}

bool pyllbc_TypeDetector::IsFloat(PyObject *obj)
{
    return This::Detect(obj) == PYLLBC_FLOAT_OBJ;
}

bool pyllbc_TypeDetector::IsStr(PyObject *obj)
{
    return This::Detect(obj) == PYLLBC_STR_OBJ;
}

bool pyllbc_TypeDetector::IsByteArray(PyObject *obj)
{
    return This::Detect(obj) == PYLLBC_BYTEARRAY_OBJ;
}

bool pyllbc_TypeDetector::IsBuffer(PyObject *obj)
{
    return This::Detect(obj) == PYLLBC_BUFFER_OBJ;
}

bool pyllbc_TypeDetector::IsSequence(PyObject *obj)
{
    const pyllbc_ObjType type = This::Detect(obj);
    return type  == PYLLBC_TUPLE_OBJ ||
        type == PYLLBC_LIST_OBJ ||
        type == PYLLBC_SEQ_OBJ;
}

bool pyllbc_TypeDetector::IsDict(PyObject *obj)
{
    return This::Detect(obj) == PYLLBC_DICT_OBJ;
}
