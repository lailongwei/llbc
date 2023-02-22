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
 * \brief The python c++ wrap timer class encapsulation.
 */
class LLBC_HIDDEN pyllbc_Timer : public LLBC_Timer
{
public:
    /**
     * Constructor.
     * @param[in] pyTimer         - the python layer timer object(steal reference).
     * @param[in] timeoutCallable - the timeout callable python object(normal, not steal reference).
     * @param[in] cancelCallable  - the cancel callable python object(normal, not steal reference).
     */
    pyllbc_Timer(PyObject *pyTimer, PyObject *timeoutCallable, PyObject *cancelCallable = nullptr);

    /**
     * Destructor.
     */
    virtual ~pyllbc_Timer();

public:
    /**
     * Make sure ignored dead-weakref object error or not, default is false.
     * @return bool - true if ignored, otherwise return false.
     */
    virtual bool IsIgnoredDeadRef() const;

    /**
     * Set the ignored dead-weakref flag.
     * @param[in] flag - the ignored dead-weakref flag.
     */
    virtual void SetIgnoredDeadRef(bool flag);

public:
    /**
     * Schedule timer(rewrite).
     * @param[in] dueTime - due time, in milli-seconds.
     * @param[in] period  - period value, in milli-seconds.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Schedule(sint64 dueTime, sint64 period);

public:
    /**
     * Get timer string representation.
     * @return LLBC_String - the timer string representation.
     */
    LLBC_String ToString() const;

public:
    /**
     * Timeout event handler.
     * @return bool - return true means readd, otherwise means stop this timer.
     */
    virtual void OnTimeout();

    /**
     * Cancel event handler.
     */
    virtual void OnCancel();

private:
    PyObject *_pyTimer;
    bool _ignoredDeadRef;

    PyObject *_timeoutCallable;
    PyObject *_boundedTimeOutObj;
    LLBC_String _timeoutCallableDesc;
    LLBC_String _boundedTimeOutObjDesc;

    PyObject *_cancelCallable;
    PyObject *_boundedCancelObj;
    LLBC_String _cancelCallableDesc;
    LLBC_String _boundedCancelObjDesc;
};
