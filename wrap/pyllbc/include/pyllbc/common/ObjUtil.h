/**
 * @file    ObjUtil.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/12/27
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_COM_OBJ_UTIL_H__
#define __PYLLBC_COM_OBJ_UTIL_H__

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
};

#endif // !__PYLLBC_COM_OBJ_UTIL_H__
