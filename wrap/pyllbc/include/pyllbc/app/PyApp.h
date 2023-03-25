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
#include "pyllbc/comm/Comm.h"

/**
 * \brief The python layer native application encapsulation.
 * TODO: for now simple encapsulation.
 */
class LLBC_HIDDEN pyllbc_App : public LLBC_App
{
public:
    /**
     * Ctor & Dtor.
     */
    pyllbc_App(PyObject *pyApp);
    virtual ~pyllbc_App();

public:
    /**
     * Get python layer application.
     * @return PyObject * - the python layer application, borrow reference.
     */
    PyObject *GetPyApp() const;

public:
    /**
     * Application event method: OnStart.
     */
    virtual int OnStart(int argc, char *argv[], bool &startFinished);

    /**
     * Application event method: OnStop.
     */
    virtual void OnStop(bool &stopFinished);

private:
    PyObject *_pyApp; // Borrowed reference.
};
