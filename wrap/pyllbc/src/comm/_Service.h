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


#include "pyllbc/comm/PyService.h"
#include "pyllbc/comm/ErrorHooker.h"

LLBC_EXTERN_C PyObject *_pyllbc_NewService(PyObject *self, PyObject *args)
{
    PyObject *pySvc;
    LLBC_IService::Type svcType;
    char *svcName = NULL;
    if (!PyArg_ParseTuple(args, "Ois", &pySvc, &svcType, &svcName))
        return NULL;

    if (svcType != LLBC_IService::Normal &&
        svcType != LLBC_IService::Raw)
    {
        pyllbc_SetError("service type invalidate", LLBC_ERROR_INVALID);
        return NULL;
    }

    pyllbc_Service *svc = LLBC_New3(pyllbc_Service, svcType, svcName, pySvc);

    return Py_BuildValue("l", svc);
}

LLBC_EXTERN_C PyObject *_pyllbc_DelService(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "l", &svc))
        return NULL;

    LLBC_Delete(svc);

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_GetServiceId(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "l", &svc))
        return NULL;

    return PyInt_FromLong(svc->GetId());
}

LLBC_EXTERN_C PyObject *_pyllbc_GetServiceType(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "l", &svc))
        return NULL;

    return PyInt_FromLong(svc->GetType());
}

LLBC_EXTERN_C PyObject *_pyllbc_GetServiceTypeStr(PyObject *self, PyObject *args)
{
    int type;
    if (UNLIKELY(!PyArg_ParseTuple(args, "i", &type)))
        return NULL;

    const char *str;
    if (type == LLBC_IService::Normal)
        str = "NORMAL";
    else if (type == LLBC_IService::Raw)
        str = "RAW";
    else
        str = "UNKNOWN";

    return PyString_FromString(str);
}

LLBC_EXTERN_C PyObject *_pyllbc_GetServiceFPS(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "l", &svc))
        return NULL;

    return PyInt_FromLong(svc->GetFPS());
}

LLBC_EXTERN_C PyObject *_pyllbc_SetServiceFPS(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    int fps = LLBC_CFG_COMM_DFT_SERVICE_FPS;

    if (!PyArg_ParseTuple(args, "l|i", &svc, &fps))
        return NULL;

    if (svc->SetFPS(fps) != LLBC_OK)
        return NULL;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_GetServiceFrameInterval(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "l", &svc))
        return NULL;

    return PyFloat_FromDouble(svc->GetFrameInterval() / 1000.0);
}

LLBC_EXTERN_C PyObject *_pyllbc_SuppressServiceCoderNotFoundWarning(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "l", &svc))
        return NULL;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_StartService(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    int pollerCount = 0;

    if (!PyArg_ParseTuple(args, "l|i", &svc, &pollerCount))
        return NULL;

    if (svc->Start(pollerCount) != LLBC_OK)
        return NULL;

    if (PyErr_Occurred())
    {
        LLBC_SetLastError(PYLLBC_ERROR_COMMON);
        return NULL;
    }

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_StopService(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "l", &svc))
        return NULL;

    svc->Stop();

    if (PyErr_Occurred())
    {
        LLBC_SetLastError(PYLLBC_ERROR_COMMON);
        return NULL;
    }

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_RegisterFacade(PyObject *self, PyObject *args)
{
    PyObject *facade;
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "lO", &svc, &facade))
        return NULL;

    if (svc->RegisterFacade(facade) != LLBC_OK)
        return NULL;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_RegisterCodec(PyObject *self, PyObject *args)
{
    int opcode;
    PyObject *codec;
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "liO", &svc, &opcode, &codec))
        return NULL;

    if (svc->RegisterCodec(opcode, codec) != LLBC_OK)
        return NULL;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Listen(PyObject *self, PyObject *args)
{
    char *ip;
    int port;
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "lsi", &svc, &ip, &port))
        return NULL;

    int sessionId;
    if ((sessionId = svc->Listen(ip, port)) == 0)
        return NULL;

    return PyInt_FromLong(sessionId);
}

LLBC_EXTERN_C PyObject *_pyllbc_Connect(PyObject *self, PyObject *args)
{
    char *ip;
    int port;
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "lsi", &svc, &ip, &port))
        return NULL;

    int sessionId;
    if ((sessionId = svc->Connect(ip, port)) == 0)
        return NULL;

    return PyInt_FromLong(sessionId);
}

LLBC_EXTERN_C PyObject *_pyllbc_AsyncConn(PyObject *self, PyObject *args)
{
    char *ip;
    int port;
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "lsi", &svc, &ip, &port))
        return NULL;

    if (svc->AsyncConn(ip, port) != LLBC_OK)
        return NULL;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_RemoveSession(PyObject *self, PyObject *args)
{
    int sid;
    pyllbc_Service *svc;
    char *reason = NULL;
    PyObject *strict = NULL;
    if (!PyArg_ParseTuple(args, "li|sO", &svc, &sid, &reason, &strict))
        return NULL;

    if (svc->RemoveSession(sid, reason) != LLBC_OK)
    {
        if (strict &&
                PyObject_IsTrue(strict))
            return NULL;
    }

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_SendData(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    PyObject *data;
    int sessionId, opcode, status = 0;
    if (!PyArg_ParseTuple(args, "liiO|i", &svc, &sessionId, &opcode, &data, &status))
        return NULL;

    if (svc->Send(sessionId, opcode, data, status) != LLBC_OK)
        return NULL;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Multicast(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;

    PyObject *data;
    PyObject *pySessionIds;

    int opcode, status = 0;

    if (!PyArg_ParseTuple(args, "lOiO|i", &svc, &pySessionIds, &opcode, &data, &status))
        return NULL;

    if (!PySequence_Check(pySessionIds))
    {
        pyllbc_SetError("sessionIds not iteratable");
        return NULL;
    }

    PyObject *fastPySessionIds = PySequence_Fast(pySessionIds, 
        "could not convert seq to FAST seq(PySequence_Fast)"); // New reference.
    if (UNLIKELY(!fastPySessionIds))
    {
        pyllbc_TransferPyError();
        return NULL;
    }

    const Py_ssize_t sessionCnt = 
        PySequence_Fast_GET_SIZE(fastPySessionIds);
    LLBC_SessionIdList sessionIds(sessionCnt);
    for (register Py_ssize_t index = 0; index < sessionCnt; index++)
    {
        PyObject *pySessionId = PySequence_Fast_GET_ITEM(fastPySessionIds, index); // Borrowed reference.
        if (UNLIKELY(!pySessionId))
        {
            pyllbc_TransferPyError();
            Py_DECREF(fastPySessionIds);

            return NULL;
        }

        int sessionId = PyInt_AsLong(pySessionId);
        if (UNLIKELY(sessionId == -1 && PyErr_Occurred()))
        {
            pyllbc_TransferPyError();
            Py_DECREF(fastPySessionIds);

            return NULL;
        }

        sessionIds[index] = sessionId;
    }

    Py_DECREF(fastPySessionIds);

    if (svc->Multicast(sessionIds, opcode, data, status) != LLBC_OK)
        return NULL;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Broadcast(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;

    PyObject *data;
    int opcode, status = 0;
    if (!PyArg_ParseTuple(args, "liO|i", &svc, &opcode, &data, &status))
        return NULL;

    if (svc->Broadcast(opcode, data, status) != LLBC_OK)
        return NULL;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Subscribe(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    int opcode;
    PyObject *handler;
    if (!PyArg_ParseTuple(args, "liO", &svc, &opcode, &handler))
        return NULL;

    if (svc->Subscribe(opcode, handler) != LLBC_OK)
        return NULL;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PreSubscribe(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    int opcode;
    PyObject *handler;
    if (!PyArg_ParseTuple(args, "liO", &svc, &opcode, &handler))
        return NULL;

    if (svc->PreSubscribe(opcode, handler) != LLBC_OK)
        return NULL;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_UnifyPreSubscribe(PyObject *self, PyObject *args)
{
#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    pyllbc_Service *svc;
    PyObject *handler;
    if (!PyArg_ParseTuple(args, "lO", &svc, &handler))
        return NULL;

    if (svc->UnifyPreSubscribe(handler) != LLBC_OK)
        return NULL;

    Py_RETURN_NONE;
#else
    pyllbc_SetError("Unify pre-subscribe method not support, please enable this flag and recompile llbc(c++) and pyllbc(python) libraries");
    return NULL;
#endif
}

LLBC_EXTERN_C PyObject *_pyllbc_GetServiceCodec(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "l", &svc))
        return NULL;

    return PyInt_FromLong(svc->GetCodec());
}

LLBC_EXTERN_C PyObject *_pyllbc_SetServiceCodec(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    pyllbc_Service::Codec codec;
    if (!PyArg_ParseTuple(args, "li", &svc, &codec))
        return NULL;

    if (svc->SetCodec(codec) != LLBC_OK)
        return NULL;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_InstallErrHooker(PyObject *self, PyObject *args)
{
    pyllbc_Service::GetErrHooker()->Install();
    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_UninstallErrHooker(PyObject *self, PyObject *args)
{
    pyllbc_Service::GetErrHooker()->Uninstall();
    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_ClearHookedErrors(PyObject *self, PyObject *args)
{
    pyllbc_ClearError();
    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Post(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    PyObject *callable;
    if (!PyArg_ParseTuple(args, "lO", &svc, &callable))
        return NULL;

    if (svc->Post(callable) != LLBC_OK)
        return NULL;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_ServiceMainLoop(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    if (UNLIKELY(!PyArg_ParseTuple(args, "l", &svc)))
        return NULL;

    PyObject *rtn = svc->MainLoop() ? Py_True : Py_False;
    if (PyErr_Occurred())
        return NULL;

    Py_INCREF(rtn);

    return rtn;
}
