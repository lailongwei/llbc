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
 * \brief The python packet handler class encapsulation.
 */
class LLBC_HIDDEN pyllbc_PacketHandler
{
public:
    /**
     * Constructor & Destructor.
     * @param[in] opcode - the packet opcode.
     */
    pyllbc_PacketHandler(int opcode);
    ~pyllbc_PacketHandler();

public:
    /**
     * Set packet handler.
     * @param[in] handler - the packet handler, normal, not steal reference.
     *                      if handler is bound method, will hold im_self object too.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetHandler(PyObject *handler);

    /**
     * Handle packet.
     * handler search order(HSO):
     *      1> callable check(included function, bound method, callable object)
     *      2> search object has handle() method or not
     * @return PyObject * - the call returned PyObject object, if failed, return nullptr.
     */
    PyObject *Handle(PyObject *packet);

public:
    /**
     * Get object string representation.
     * @return LLBC_String - the object representation.
     */
    LLBC_String ToString() const;

private:
    int _opcode;
    PyObject *_handler;
    PyObject *_callArgs;
};
