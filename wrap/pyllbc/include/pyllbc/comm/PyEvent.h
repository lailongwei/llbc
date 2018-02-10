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

#ifndef __PYLLBC_COMM_PY_EVENT_H__
#define __PYLLBC_COMM_PY_EVENT_H__

#include "pyllbc/common/Common.h"
#include "pyllbc/core/Core.h"

/**
 * \brief The python event c++ wrap class encapsulation.
 */
class LLBC_HIDDEN pyllbc_Event : public LLBC_Event
{
public:
    /**
     * Constructor.
     * @param[in] id   - the event Id.
     * @param[in] data - the event data, normal, not steal reference to data.
     */
    pyllbc_Event(int id, PyObject *data);

    /**
     * Destructor.
     */
    virtual ~pyllbc_Event();

public:
    /**
     * Get event data(new reference).
     * @return PyObject * - the event data.
     */
    PyObject *GetData();

private:
    PyObject *_data;
};

#endif // !__PYLLBC_COMM_PY_EVENT_H__
