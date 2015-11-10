/**
 * @file    PyPacketHandler.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/28
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Export.h"

#include "pyllbc/comm/PyPacketHandler.h"

pyllbc_PacketHandler::pyllbc_PacketHandler(int opcode)
: _opcode(opcode)
, _handler(NULL)
{
}

pyllbc_PacketHandler::~pyllbc_PacketHandler()
{
    Py_XDECREF(_handler);
}

int pyllbc_PacketHandler::SetHandler(PyObject *handler)
{
    if (PyCallable_Check(handler))
    {
        _handler = handler;
        Py_INCREF(_handler);

        return LLBC_RTN_OK;
    }
    else if (PyObject_HasAttrString(handler, "handle"))
    {
        _handler = PyObject_GetAttrString(handler, "handle");
        return LLBC_RTN_OK;
    }

    LLBC_String errDesc;
    pyllbc_SetError(errDesc.format("invalid packet handler"
        "(not callable or not exist handle() method): %s", pyllbc_ObjUtil::GetObjStr(handler).c_str()));

    return LLBC_RTN_FAILED;
}

PyObject *pyllbc_PacketHandler::Handle(PyObject *packet)
{
    PyObject *rtn = PyObject_CallFunctionObjArgs(_handler, packet, NULL);
    if (UNLIKELY(rtn == NULL))
    {
        const LLBC_String packetStr = pyllbc_ObjUtil::GetObjStr(packet);
        const LLBC_String handlerStr = pyllbc_ObjUtil::GetObjStr(_handler);
        if (LIKELY(PyErr_Occurred()))
        {
            LLBC_String additionalMsg;
            additionalMsg.append_format("When process packet: %s", packetStr.c_str());
            additionalMsg.append_format(", packet handler: %s", handlerStr.c_str());

            pyllbc_TransferPyError(additionalMsg);
        }

        return NULL;
    }

    return rtn;
}

LLBC_String pyllbc_PacketHandler::ToString() const
{
    return LLBC_String().format("opcode: %d, handler: %s", 
        _opcode, pyllbc_ObjUtil::GetObjStr(_handler).c_str());
}
