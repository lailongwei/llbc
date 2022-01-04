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

#include "pyllbc/comm/PyPacketHandler.h"

pyllbc_PacketHandler::pyllbc_PacketHandler(int opcode)
: _opcode(opcode)
, _handler(nullptr)
, _callArgs(PyTuple_New(1))
{
}

pyllbc_PacketHandler::~pyllbc_PacketHandler()
{
    Py_XDECREF(_handler);
    Py_DECREF(_callArgs);
}

int pyllbc_PacketHandler::SetHandler(PyObject *handler)
{
    if (PyCallable_Check(handler))
    {
        _handler = handler;
        Py_INCREF(_handler);

        return LLBC_OK;
    }
    else if (PyObject_HasAttrString(handler, "handle"))
    {
        _handler = PyObject_GetAttrString(handler, "handle");
        return LLBC_OK;
    }

    LLBC_String errDesc;
    pyllbc_SetError(errDesc.format("invalid packet handler"
        "(not callable or not exist handle() method): %s", pyllbc_ObjUtil::GetObjStr(handler).c_str()));

    return LLBC_FAILED;
}

PyObject *pyllbc_PacketHandler::Handle(PyObject *packet)
{
    // Set packet to call args list(steals ref).
    Py_INCREF(packet);
    PyTuple_SetItem(_callArgs, 0, packet);

    // Call.
    PyObject *rtn = PyObject_Call(_handler, _callArgs, nullptr);
    if (UNLIKELY(rtn == nullptr))
    {
        Py_INCREF(Py_None);
        PyTuple_SetItem(_callArgs, 0, Py_None);

        const LLBC_String packetStr = pyllbc_ObjUtil::GetObjStr(packet);
        const LLBC_String handlerStr = pyllbc_ObjUtil::GetObjStr(_handler);
        if (LIKELY(PyErr_Occurred()))
        {
            LLBC_String additionalMsg;
            additionalMsg.append_format("When process packet: %s", packetStr.c_str());
            additionalMsg.append_format(", packet handler: %s", handlerStr.c_str());

            pyllbc_TransferPyError(additionalMsg);
        }

        return nullptr;
    }

    // Release packet from call args.
    Py_INCREF(Py_None);
    PyTuple_SetItem(_callArgs, 0, Py_None);

    return rtn;
}

LLBC_String pyllbc_PacketHandler::ToString() const
{
    return LLBC_String().format("opcode:%d, handler:%s", 
        _opcode, pyllbc_ObjUtil::GetObjStr(_handler).c_str());
}
