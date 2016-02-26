/**
 * @fil     _TestCaseOut.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/13
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Common.h"

/**
 * \brief The pyllbc testcase encapsulation.
 */
class LLBC_HIDDEN pyllbc_TestCase : public LLBC_BaseTestCase
{
public:
    pyllbc_TestCase() {  }
    virtual ~pyllbc_TestCase() {  }

public:
    virtual int Run(int argc, char *argv[])
    {
        PyObject *pyObj = reinterpret_cast<PyObject *>(LLBC_Str2Ptr(argv[0]));

        PyObject *callable = NULL;
        pyllbc_ObjAttrOptr optr(pyObj);
        if (optr.GetAttr("run", callable) != LLBC_OK)
            return LLBC_FAILED;

        if (!PyCallable_Check(callable))
        {
            Py_DECREF(callable);

            pyllbc_SetError("object not callable");
            return LLBC_FAILED;
        }
    
        Py_DECREF(callable);

        PyObject *arg = reinterpret_cast<PyObject *>(LLBC_Str2Ptr(argv[1]));
        PyObject *rtn = PyObject_CallMethod(pyObj, const_cast<char *>("run"), const_cast<char *>("N"), arg);
        if (!rtn)
            return LLBC_FAILED;

        int cRtn = 0;
        if (!PyArg_Parse(rtn, "i", &cRtn))
            cRtn = LLBC_FAILED;

        Py_DECREF(rtn);
        return cRtn;
    }
};


LLBC_EXTERN_C PyObject *_pyllbc_NewTestCase(PyObject *self, PyObject *args)
{
    char *tcName;
    if (!PyArg_ParseTuple(args, "s", &tcName))
        return NULL;

    pyllbc_TestCase *tc = new pyllbc_TestCase;
    tc->SetName(tcName);

    return Py_BuildValue("l", tc);
}

LLBC_EXTERN_C PyObject *_pyllbc_DelTestCase(PyObject *self, PyObject *args)
{
    pyllbc_TestCase *tc;
    if (!PyArg_ParseTuple(args, "l", &tc))
        return NULL;

    delete tc;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_GetTestCaseName(PyObject *self, PyObject *args)
{
    pyllbc_TestCase *tc;
    if (!PyArg_ParseTuple(args, "l", &tc))
        return NULL;

    return PyString_FromString(tc->GetName().c_str());
}

LLBC_EXTERN_C PyObject *_pyllbc_SetTestCaseName(PyObject *self, PyObject *args)
{
    char *tcName;
    pyllbc_TestCase *tc;
    if (!PyArg_ParseTuple(args, "ls", &tc, &tcName))
        return NULL;

    tc->SetName(tcName);

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_RunTestCase(PyObject *self, PyObject *args)
{
    PyObject *argObj;
    pyllbc_TestCase *tc;
    if (!PyArg_ParseTuple(args, "lN", &tc, &argObj))
        return NULL;

    char *tcArgs[2] = {NULL, NULL};
    tcArgs[0] = const_cast<char *>(LLBC_Num2Str(self).c_str());
    tcArgs[1] = const_cast<char *>(LLBC_Num2Str(argObj).c_str());

    return Py_BuildValue("i", tc->Run(2, tcArgs));
}
