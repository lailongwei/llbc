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

#include "pyllbc/common/Common.h"
#include "pyllbc/core/Core.h"

LLBC_EXTERN_C PyObject *_pyllbc_Packet_GetLocalIp(PyObject *self, PyObject *args)
{
    LLBC_Packet *pkt;
    if (!PyArg_ParseTuple(args, "l", &pkt))
        return nullptr;

    const LLBC_String &ip = pkt->GetLocalAddr().GetIpAsString();
    return PyString_FromStringAndSize(ip.c_str(), ip.size());
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_GetLocalPort(PyObject *self, PyObject *args)
{
    LLBC_Packet *pkt;
    if (!PyArg_ParseTuple(args, "l", &pkt))
        return nullptr;

    return PyInt_FromLong(pkt->GetLocalAddr().GetPort());
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_GetPeerIp(PyObject *self, PyObject *args)
{
    LLBC_Packet *pkt;
    if (!PyArg_ParseTuple(args, "l", &pkt))
        return nullptr;

    const LLBC_String &ip = pkt->GetPeerAddr().GetIpAsString();
    return PyString_FromStringAndSize(ip.c_str(), ip.size());
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_GetPeerPort(PyObject *self, PyObject *args)
{
    LLBC_Packet *pkt;
    if (!PyArg_ParseTuple(args, "l", &pkt))
        return nullptr;

    return PyInt_FromLong(pkt->GetPeerAddr().GetPort());
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_GetStatusDesc(PyObject *self, PyObject *args)
{
    LLBC_Packet *pkt;
    if (!PyArg_ParseTuple(args, "l", &pkt))
        return nullptr;

#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
    const LLBC_String &statusDesc = pkt->GetStatusDesc();
    return PyString_FromStringAndSize(statusDesc.c_str(), statusDesc.length());
#else
    return PyString_FromString("");
#endif
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_GetExtData1(PyObject *self, PyObject *args)
{
    LLBC_Packet *pkt;
    if (!PyArg_ParseTuple(args, "l", &pkt))
        return nullptr;

    return PyLong_FromLongLong(pkt->GetExtData1());
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_GetExtData2(PyObject *self, PyObject *args)
{
    LLBC_Packet *pkt;
    if (!PyArg_ParseTuple(args, "l", &pkt))
        return nullptr;

    return PyLong_FromLongLong(pkt->GetExtData2());
}

LLBC_EXTERN_C PyObject *_pyllbc_Packet_GetExtData3(PyObject *self, PyObject *args)
{
    LLBC_Packet *pkt;
    if (!PyArg_ParseTuple(args, "l", &pkt))
        return nullptr;

    return PyLong_FromLongLong(pkt->GetExtData3());
}

