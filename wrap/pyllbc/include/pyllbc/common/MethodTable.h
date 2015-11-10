/**
 * @file    MethodTable.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/07/30
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_COM_METHOD_TABLE_H__
#define __PYLLBC_COM_METHOD_TABLE_H__

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

#endif // !__PYLLBC_COM_METHOD_TABLE_H__

