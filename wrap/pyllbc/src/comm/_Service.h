/**
 * @file    _Service.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/25
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/comm/PyService.h"
#include "pyllbc/comm/ErrorHooker.h"

LLBC_EXTERN_C PyObject *_pyllbc_NewService(PyObject *self, PyObject *args)
{
    PyObject *pySvc;
    LLBC_IService::Type svcType;
    if (!PyArg_ParseTuple(args, "Oi", &pySvc, &svcType))
        return NULL;

    if (svcType != LLBC_IService::Normal &&
        svcType != LLBC_IService::Raw)
    {
        pyllbc_SetError("service type invalidate", LLBC_ERROR_INVALID);
        return NULL;
    }

    pyllbc_Service *svc = LLBC_New2(pyllbc_Service, svcType, pySvc);

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

LLBC_EXTERN_C PyObject *_pyllbc_SetPacketHeaderDesc(PyObject *self, PyObject *args)
{
    LLBC_PacketHeaderDesc *desc;
    if (!PyArg_ParseTuple(args, "l", &desc))
        return NULL;

    LLBC_PacketHeaderDesc *copyDesc = LLBC_New1(LLBC_PacketHeaderDesc, *desc);
    if (LLBC_IService::SetPacketHeaderDesc(copyDesc) != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__, "when set packet header desc to service");
        LLBC_Delete(copyDesc);

        return NULL;
    }

    Py_RETURN_NONE;
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
    if (!PyArg_ParseTuple(args, "li", &svc, &sid))
        return NULL;

    svc->RemoveSession(sid);

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_SendData(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;
    PyObject *data;
    int sessionId, opcode, status = 0;
    PyObject *parts = NULL;
    if (!PyArg_ParseTuple(args, "liiO|iO", &svc, &sessionId, &opcode, &data, &status, &parts))
        return NULL;

    if (svc->Send(sessionId, opcode, data, status, parts) != LLBC_OK)
        return NULL;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Multicast(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;

    PyObject *data;
    PyObject *pySessionIds;

    int opcode, status = 0;

    PyObject *parts = NULL;

    if (!PyArg_ParseTuple(args, "lOiO|iO", &svc, &pySessionIds, &opcode, &data, &status, &parts))
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

    if (svc->Multicast(sessionIds, opcode, data, status, parts) != LLBC_OK)
        return NULL;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Broadcast(PyObject *self, PyObject *args)
{
    pyllbc_Service *svc;

    PyObject *data;
    int opcode, status = 0;
    PyObject *parts = NULL;
    if (!PyArg_ParseTuple(args, "liO|iO", &svc, &opcode, &data, &status, &parts))
        return NULL;

    if (svc->Broadcast(opcode, data, status, parts) != LLBC_OK)
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
