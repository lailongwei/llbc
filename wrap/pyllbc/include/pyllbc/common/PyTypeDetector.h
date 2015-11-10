/**
 * @file    PyTypeDetector.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/20
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_COM_PY_TYPE_DETECTOR_H__
#define __PYLLBC_COM_PY_TYPE_DETECTOR_H__

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

#endif // !__PYLLBC_COM_PY_TYPE_DETECTOR_H__
