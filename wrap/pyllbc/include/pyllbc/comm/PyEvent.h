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
 * \brief The event listener class encapsulation.
 */
class LLBC_HIDDEN pyllbc_EventListener : public LLBC_EventListener
{
public:
    /**
     * Delegate ctor&dtor.
     */
    pyllbc_EventListener();
    virtual ~pyllbc_EventListener();

public:
    /**
     * Set python layer listener.
     * @param[in] pyListener - the python layer listener, not steal ref.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetPyListener(PyObject *pyListener);

    /**
     * Listener invoke method.
     * @param[in] ev - the event object.
     */
    virtual void Invoke(LLBC_Event &ev);

private:
    PyObject *_pyListenerObj;
    PyObject *_pyListenerMeth;

    static PyObject *_pyEvCls;
};
