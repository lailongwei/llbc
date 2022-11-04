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
    char *svcName = nullptr;
    bool useNormalProtocolFactory;
    if (!PyArg_ParseTuple(args, "Osb", &pySvc, &svcName, &useNormalProtocolFactory))
        return nullptr;

    pyllbc_Service *svc = new pyllbc_Service(svcName, useNormalProtocolFactory, pySvc);

    return Py_BuildValue("l", svc);
}

LLBC_EXTERN_C PyObject *_pyllbc_DelService(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "l", &svc))
        return nullptr;

    delete svc;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_GetServiceId(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "l", &svc))
        return nullptr;

    return PyInt_FromLong(svc->GetId());
}

LLBC_EXTERN_C PyObject *_pyllbc_GetServiceFPS(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "l", &svc))
        return nullptr;

    return PyInt_FromLong(svc->GetFPS());
}

LLBC_EXTERN_C PyObject *_pyllbc_SetServiceFPS(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    int fps = LLBC_CFG_COMM_DFT_SERVICE_FPS;

    if (!PyArg_ParseTuple(args, "l|i", &svc, &fps))
        return nullptr;

    if (svc->SetFPS(fps) != LLBC_OK)
        return nullptr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_GetServiceFrameInterval(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "l", &svc))
        return nullptr;

    return PyFloat_FromDouble(svc->GetFrameInterval() / 1000.0);
}

LLBC_EXTERN_C PyObject *_pyllbc_SuppressServiceCoderNotFoundWarning(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "l", &svc))
        return nullptr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_StartService(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    int pollerCount = 0;

    if (!PyArg_ParseTuple(args, "l|i", &svc, &pollerCount))
        return nullptr;

    if (svc->Start(pollerCount) != LLBC_OK)
        return nullptr;

    if (PyErr_Occurred())
    {
        LLBC_SetLastError(PYLLBC_ERROR_COMMON);
        return nullptr;
    }

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_StopService(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "l", &svc))
        return nullptr;

    svc->Stop();

    if (PyErr_Occurred())
    {
        LLBC_SetLastError(PYLLBC_ERROR_COMMON);
        return nullptr;
    }

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_AddComponent(PyObject *self, PyObject *args)
{
    PyObject *comp;
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "lO", &svc, &comp))
        return nullptr;

    if (svc->AddComponent(comp) != LLBC_OK)
        return nullptr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_AddLibComponent(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    const char *compName, *libPath;
    PyObject *compCls = nullptr;
    if (!PyArg_ParseTuple(args, "lss|O", &svc, &compName, &libPath, &compCls))
        return nullptr;

    PyObject *comp;
    if (svc->AddComponent(compName, libPath, compCls, comp) != LLBC_OK)
        return nullptr;

    return comp;
}

LLBC_EXTERN_C PyObject *_pyllbc_AddDecoder(PyObject *self, PyObject *args)
{
    int opcode;
    PyObject *decoder;
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "liO", &svc, &opcode, &decoder))
        return nullptr;

    if (svc->AddDecoder(opcode, decoder) != LLBC_OK)
        return nullptr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Listen(PyObject *self, PyObject *args)
{
    char *ip;
    int port;
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "lsi", &svc, &ip, &port))
        return nullptr;

    int sessionId;
    if ((sessionId = svc->Listen(ip, port)) == 0)
        return nullptr;

    return PyInt_FromLong(sessionId);
}

LLBC_EXTERN_C PyObject *_pyllbc_Connect(PyObject *self, PyObject *args)
{
    char *ip;
    int port;
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "lsi", &svc, &ip, &port))
        return nullptr;

    int sessionId;
    if ((sessionId = svc->Connect(ip, port)) == 0)
        return nullptr;

    return PyInt_FromLong(sessionId);
}

LLBC_EXTERN_C PyObject *_pyllbc_AsyncConn(PyObject *self, PyObject *args)
{
    char *ip;
    int port;
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "lsi", &svc, &ip, &port))
        return nullptr;

    int sid = svc->AsyncConn(ip, port);
    if (sid == 0)
        return nullptr;

    return PyInt_FromLong(sid);
}

LLBC_EXTERN_C PyObject *_pyllbc_IsSessionValidate(PyObject *self, PyObject *args)
{
    int sid;
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "li", &svc, &sid))
        return nullptr;

    if (svc->IsSessionValidate(sid))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

LLBC_EXTERN_C PyObject *_pyllbc_RemoveSession(PyObject *self, PyObject *args)
{
    int sid;
    pyllbc_Service *svc;
    char *reason = nullptr;
    PyObject *strict = nullptr;
    if (!PyArg_ParseTuple(args, "li|sO", &svc, &sid, &reason, &strict))
        return nullptr;

    if (svc->RemoveSession(sid, reason) != LLBC_OK)
    {
        if (strict &&
                PyObject_IsTrue(strict))
            return nullptr;
    }

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_SendData(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    PyObject *data;
    int sessionId, opcode, status = 0;
    sint64 extData1 = 0, extData2 = 0, extData3 = 0;
    if (!PyArg_ParseTuple(args, "liiO|iLLL", &svc, &sessionId, &opcode, &data, &status, &extData1, &extData2, &extData3))
        return nullptr;

    if (svc->Send(sessionId, opcode, data, status, extData1, extData2, extData3) != LLBC_OK)
        return nullptr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Multicast(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;

    PyObject *data;
    PyObject *pySessionIds;

    int opcode, status = 0;

    if (!PyArg_ParseTuple(args, "lOiO|i", &svc, &pySessionIds, &opcode, &data, &status))
        return nullptr;

    if (!PySequence_Check(pySessionIds))
    {
        pyllbc_SetError("sessionIds not iteratable");
        return nullptr;
    }

    PyObject *fastPySessionIds = PySequence_Fast(pySessionIds, 
        "could not convert seq to FAST seq(PySequence_Fast)"); // New reference.
    if (UNLIKELY(!fastPySessionIds))
    {
        pyllbc_TransferPyError();
        return nullptr;
    }

    const Py_ssize_t sessionCnt = 
        PySequence_Fast_GET_SIZE(fastPySessionIds);
    LLBC_SessionIdList sessionIds(sessionCnt);
    for (Py_ssize_t index = 0; index < sessionCnt; ++index)
    {
        PyObject *pySessionId = PySequence_Fast_GET_ITEM(fastPySessionIds, index); // Borrowed reference.
        if (UNLIKELY(!pySessionId))
        {
            pyllbc_TransferPyError();
            Py_DECREF(fastPySessionIds);

            return nullptr;
        }

        int sessionId = PyInt_AsLong(pySessionId);
        if (UNLIKELY(sessionId == -1 && PyErr_Occurred()))
        {
            pyllbc_TransferPyError();
            Py_DECREF(fastPySessionIds);

            return nullptr;
        }

        sessionIds[index] = sessionId;
    }

    Py_DECREF(fastPySessionIds);

    if (svc->Multicast(sessionIds, opcode, data, status) != LLBC_OK)
        return nullptr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Broadcast(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;

    PyObject *data;
    int opcode, status = 0;
    if (!PyArg_ParseTuple(args, "liO|i", &svc, &opcode, &data, &status))
        return nullptr;

    if (svc->Broadcast(opcode, data, status) != LLBC_OK)
        return nullptr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Subscribe(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    int opcode;
    PyObject *handler;
    if (!PyArg_ParseTuple(args, "liO", &svc, &opcode, &handler))
        return nullptr;

    if (svc->Subscribe(opcode, handler) != LLBC_OK)
        return nullptr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PreSubscribe(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    int opcode;
    PyObject *handler;
    if (!PyArg_ParseTuple(args, "liO", &svc, &opcode, &handler))
        return nullptr;

    if (svc->PreSubscribe(opcode, handler) != LLBC_OK)
        return nullptr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_UnifyPreSubscribe(PyObject *self, PyObject *args)
{
#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    pyllbc_Service *svc;
    PyObject *handler;
    if (!PyArg_ParseTuple(args, "lO", &svc, &handler))
        return nullptr;

    if (svc->UnifyPreSubscribe(handler) != LLBC_OK)
        return nullptr;

    Py_RETURN_NONE;
#else
    pyllbc_SetError("Unify pre-subscribe method not support, please enable this flag and recompile llbc(c++) and pyllbc(python) libraries");
    return nullptr;
#endif
}

LLBC_EXTERN_C PyObject *_pyllbc_SubscribeEvent(PyObject *self, PyObject *args)
{
    int evId;
    pyllbc_Service *svc;
    PyObject *listener;
    if (!PyArg_ParseTuple(args, "liO", &svc, &evId, &listener))
        return nullptr;

    LLBC_ListenerStub stub = svc->SubscribeEvent(evId, listener);
    if (stub == 0)
        return nullptr;

    return PyLong_FromUnsignedLongLong(stub);
}

LLBC_EXTERN_C PyObject *_pyllbc_UnsubscribeEventById(PyObject *self, PyObject *args)
{
    int evId;
    pyllbc_Service *svc;
    if (!PyArg_ParseTuple(args, "li", &svc, &evId))
        return nullptr;

    svc->UnsubscribeEvent(evId);

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_UnsubscribeEventByStub(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    unsigned PY_LONG_LONG stub;
    if (!PyArg_ParseTuple(args, "lK", &svc, &stub))
        return nullptr;

    svc->UnsubscribeEvent(static_cast<LLBC_ListenerStub>(stub));

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_FireEvent(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    PyObject *ev;
    if (!PyArg_ParseTuple(args, "lO", &svc, &ev))
        return nullptr;

    if (svc->FireEvent(ev) != LLBC_OK)
        return nullptr;

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

LLBC_EXTERN_C PyObject *_pyllbc_IsErrHookerInstalled(PyObject *self, PyObject *args)
{
    return PyBool_FromLong(pyllbc_Service::GetErrHooker()->IsInstalled() ? 1 : 0);
}

LLBC_EXTERN_C PyObject *_pyllbc_TransferHookedErrorToPython(PyObject *self, PyObject *args)
{
    pyllbc_Service::GetErrHooker()->TransferHookedErrorToPython();
    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Post(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    PyObject *callable;
    if (!PyArg_ParseTuple(args, "lO", &svc, &callable))
        return nullptr;

    if (svc->Post(callable) != LLBC_OK)
        return nullptr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_CallComponentMethod(PyObject *self, PyObject *args)
{
    LLBC_Component *comp;
    const char *meth;
    PyObject *arg;
    if (!PyArg_ParseTuple(args, "lsO", &comp, &meth, &arg))
        return nullptr;

    LLBC_Variant nativeArg;
    if (pyllbc_ObjUtil::Obj2Variant(arg, nativeArg) != LLBC_OK)
        return nullptr;

    int callMethRet;
    LLBC_Variant nativeRet;
    if ((callMethRet = comp->CallMethod(meth, nativeArg, nativeRet)) != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__, "When call native comp method");
        return nullptr;
    }

    return pyllbc_ObjUtil::Variant2Obj(nativeRet);
}

LLBC_EXTERN_C PyObject *_pyllbc_ServiceMainLoop(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    if (UNLIKELY(!PyArg_ParseTuple(args, "l", &svc)))
        return nullptr;

    PyObject *rtn = svc->MainLoop() ? Py_True : Py_False;
    if (PyErr_Occurred())
        return nullptr;

    Py_INCREF(rtn);

    return rtn;
}
