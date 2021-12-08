// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), to deal in 
// the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
// the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "pyllbc/common/Export.h"
#include "pyllbc/comm/ErrorHooker.h"

namespace
{
    typedef pyllbc_ErrorHooker This;
}

pyllbc_ErrorHooker::pyllbc_ErrorHooker()
: _installed(false)
, _transfering(false)

, _llbcErrNo(LLBC_ERROR_SUCCESS)
, _errDesc(LLBC_StrError(LLBC_ERROR_SUCCESS))
, _errCls(nullptr)
, _tbObj(nullptr)
{
}

pyllbc_ErrorHooker::~pyllbc_ErrorHooker()
{
    Cleanup();
}

int pyllbc_ErrorHooker::Install()
{
    if (_installed)
        return LLBC_OK;

    pyllbc_SetErrSetHock(LLBC_Delegate<void(const LLBC_String &, int, PyObject *, PyObject *)>(this, &This::Hook_ErrSet));
    pyllbc_SetErrClearHook(LLBC_Delegate<void()>(this, &This::Hook_ErrClear));

    _installed = true;
    return LLBC_OK;
}

void pyllbc_ErrorHooker::Uninstall()
{
    pyllbc_SetErrSetHock(nullptr);
    pyllbc_SetErrClearHook(nullptr);

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
        _errCls = nullptr;

        _llbcErrNo = LLBC_ERROR_SUCCESS;
        _errDesc = LLBC_StrError(_llbcErrNo);

        if (_tbObj)
        {
            Py_DECREF(_tbObj);
            _tbObj = nullptr;
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

            if (errTb == nullptr)
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

            _tbObj = nullptr;
            PyErr_Restore(errType, errVal, errTb);
        }

        _transfering = false;

        Cleanup();
    }
}

void pyllbc_ErrorHooker::Hook_ErrClear()
{
    Cleanup();
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
