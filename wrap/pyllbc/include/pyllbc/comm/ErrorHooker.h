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

#pragma once

#include "pyllbc/common/Common.h"
#include "pyllbc/core/Core.h"

/**
 * \brief The pyllbc error hooker class encapsulation.
 */
class LLBC_HIDDEN pyllbc_ErrorHooker
{
public:
    /**
     * Ctor & Dtor.
     */
    pyllbc_ErrorHooker();
    ~pyllbc_ErrorHooker();

public:
    /**
     * Install this hooker to pyllbc library.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Install();

    /**
     * Uninstall this hooker from pyllbc library.
     */
    void Uninstall();

    /**
     * Determine this hook already install to pyllbc library or not.
     * @return bool - the installed flag.
     */
    bool IsInstalled() const;

public:
    /**
     * Cleanup the hooker's hooked error informations.
     */
    void Cleanup();

    /**
     * Transfer hooked error to python.
     */
    void TransferHookedErrorToPython();

public:
    /**
     * When error cleared, will call this hooker.
     */
    void Hook_ErrClear();

    /**
     * When error set, will call this hooker.
     * @param[in] errDesc - the error describe.
     * @param[in] llbcErr - llbc library error no.
     * @param[in] errCls  - the error class(normal, not steal reference).
     * @param[in] tbObj   - the traceback object(normal, not steal reference).
     */
    void Hook_ErrSet(const LLBC_String &errDesc, int llbcErr, PyObject *errCls, PyObject *tbObj);

private:
    bool _installed;
    bool _transfering;

    int _llbcErrNo;
    LLBC_String _errDesc;
    PyObject *_errCls;
    PyObject *_tbObj;
};
