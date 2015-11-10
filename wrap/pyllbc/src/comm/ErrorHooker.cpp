/**
 * @file    ErrorHooker.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/12/03
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Export.h"
#include "pyllbc/comm/ErrorHooker.h"

namespace
{
    typedef pyllbc_ErrorHooker This;

    typedef LLBC_Delegate0<This> _ClearDeleg;
    typedef LLBC_Delegate4<This, const LLBC_String &, int, PyObject *, PyObject *> _SetDeleg;
}

pyllbc_ErrorHooker::pyllbc_ErrorHooker()
: _installed(false)
, _transfering(false)

, _llbcErrNo(LLBC_ERROR_SUCCESS)
, _errDesc(LLBC_StrError(LLBC_ERROR_SUCCESS))
, _errCls(NULL)
, _tbObj(NULL)
{
}

pyllbc_ErrorHooker::~pyllbc_ErrorHooker()
{
    this->Cleanup();
}

int pyllbc_ErrorHooker::Install()
{
    if (_installed)
    {
        pyllbc_SetError(LLBC_ERROR_REENTRY);
        return LLBC_RTN_FAILED;
    }

    pyllbc_SetErrSetHock(new _SetDeleg(this, &This::Hook_ErrSet));
    pyllbc_SetErrClearHook(new _ClearDeleg(this, &This::Hook_ErrClear));

    _installed = true;
    return LLBC_RTN_OK;
}

void pyllbc_ErrorHooker::Uninstall()
{
    pyllbc_SetErrSetHock(NULL);
    pyllbc_SetErrClearHook(NULL);

    _installed = false;
}

bool pyllbc_ErrorHooker::IsInstalled() const
{
    return _installed;
}

void pyllbc_ErrorHooker::Cleanup()
{
    if (_errCls)
    {
        Py_DECREF(_errCls);
        _errCls = NULL;

        _llbcErrNo = LLBC_ERROR_SUCCESS;
        _errDesc = LLBC_StrError(_llbcErrNo);

        if (_tbObj)
        {
            Py_DECREF(_tbObj);
            _tbObj = NULL;
        }
    }
}

void pyllbc_ErrorHooker::TransferHookedErrorToPython()
{
    if (_errCls && !PyErr_Occurred())
    {
        _transfering = true;

        pyllbc_SetError(_errDesc, _llbcErrNo, _errCls);

        // Link the previous traceback, if we holded.
        if (_tbObj)
        {
            PyObject *errType, *errVal, *errTb;
            PyErr_Fetch(&errType, &errVal, &errTb);
            PyErr_NormalizeException(&errType, &errVal, &errTb);

            if (errTb == NULL)
            {
                errTb = _tbObj;
            }
            else
            {
                PyTracebackObject *perTb = 
                    reinterpret_cast<PyTracebackObject *>(errTb);
                while (perTb->tb_next) perTb = perTb->tb_next;
                perTb->tb_next = reinterpret_cast<PyTracebackObject *>(_tbObj);
            }

            _tbObj = NULL;
            PyErr_Restore(errType, errVal, errTb);
        }

        _transfering = false;

        this->Cleanup();
    }
}

void pyllbc_ErrorHooker::Hook_ErrClear()
{
    this->Cleanup();
}

void pyllbc_ErrorHooker::Hook_ErrSet(const LLBC_String &errDesc, int llbcErr, PyObject *errCls, PyObject *tbObj)
{
    if (_errCls || _transfering)
        return;

    _errCls = errCls;
    Py_INCREF(_errCls);

    if ((_tbObj = tbObj))
        Py_INCREF(_tbObj);

    _llbcErrNo = llbcErr;
    _errDesc.clear();
    _errDesc.append(errDesc);
}
