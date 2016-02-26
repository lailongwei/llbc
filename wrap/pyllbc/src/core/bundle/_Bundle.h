/**
 * @file    _Bundle.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/12
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Common.h"

LLBC_EXTERN_C PyObject *_pyllbc_NewBundle(PyObject *self, PyObject *args)
{
    LLBC_Bundle *b = new LLBC_Bundle();
    return Py_BuildValue("l", b);
}

LLBC_EXTERN_C PyObject *_pyllbc_DelBundle(PyObject *self, PyObject *args)
{
    LLBC_Bundle *bundlePtr;
    if (!PyArg_ParseTuple(args, "l", &bundlePtr))
        return NULL;

    delete bundlePtr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_GetMainBundle(PyObject *self, PyObject *args)
{
    const LLBC_Bundle *b = LLBC_Bundle::GetMainBundle();
    return PyLong_FromLong(reinterpret_cast<long>(b));
}

LLBC_EXTERN_C PyObject *_pyllbc_GetBundlePath(PyObject *self, PyObject *args)
{
    LLBC_Bundle *bundlePtr;
    if (!PyArg_ParseTuple(args, "l", &bundlePtr))
        return NULL;

    return PyString_FromString(bundlePtr->GetBundlePath().c_str());
}

LLBC_EXTERN_C PyObject *_pyllbc_GetBundleName(PyObject *self, PyObject *args)
{
    LLBC_Bundle *bundlePtr;
    if (!PyArg_ParseTuple(args, "l", &bundlePtr))
        return NULL;

    return PyString_FromString(bundlePtr->GetBundleName().c_str());
}

LLBC_EXTERN_C PyObject *_pyllbc_InitBundle(PyObject *self, PyObject *args)
{
    const char *path;
    LLBC_Bundle *bundle;
    if (!PyArg_ParseTuple(args, "ls", &bundle, &path))
        return NULL;

    if (bundle->Initialize(path) != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__);
        return NULL;
    }

    Py_RETURN_TRUE;
}

LLBC_EXTERN_C PyObject *_pyllbc_GetBundleResPath(PyObject *self, PyObject *args)
{
    LLBC_Bundle *bundle;
    const char *name;
    const char *intDir = "";
    const char *extension = "";
    if (!PyArg_ParseTuple(
        args, "ls|ss", &bundle, &name, &extension, &intDir))
        return NULL;

    LLBC_String resPath = bundle->GetResPath(name, extension, intDir);
    if (resPath.empty())
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__);
        return NULL;
    }

    return PyString_FromString(resPath.c_str());
}
