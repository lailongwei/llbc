/**
 * @file    _Directory.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/09/06
 * @version 1.0
 *
 * @brief
 */

LLBC_EXTERN_C PyObject *_pyllbc_GetModuleFileName(PyObject *self, PyObject *args)
{
    const LLBC_String mfn = LLBC_GetModuleFileName();
    if (mfn.empty())
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__, "When get the module file name");
        return NULL;
    }

    return PyString_FromStringAndSize(mfn.data(), mfn.size());
}
