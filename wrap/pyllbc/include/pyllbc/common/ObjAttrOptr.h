/**
 * @file    ObjAttrOptr.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/10
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_COM_OBJ_ATTR_OPTR_H__
#define __PYLLBC_COM_OBJ_ATTR_OPTR_H__

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

#include "pyllbc/common/ObjAttrOptrImpl.h"

#endif // !__PYLLBC_COM_OBJ_ATTR_OPTR_H__
