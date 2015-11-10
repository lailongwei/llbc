/**
 * @file    PyFacade.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/25
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Export.h"

#include "pyllbc/comm/PyObjCoder.h"
#include "pyllbc/comm/PyPacketHandler.h"
#include "pyllbc/comm/PyService.h"
#include "pyllbc/comm/PyFacade.h"
#include "pyllbc/comm/PyFacadeEvBuilder.h"

namespace
{
    typedef pyllbc_Facade This;
}

namespace
{
    void DecRefPyObj(void *obj)
    {
        PyObject *pyObj = reinterpret_cast<PyObject *>(obj);
        Py_DECREF(pyObj);
    }
}

pyllbc_Facade::pyllbc_Facade(pyllbc_Service *svc)
: _svc(svc)
, _pySvc(svc->GetPyService())

, _svcType(svc->GetType())

, _methOnInitialize(Py_BuildValue("s", "oninitialize"))
, _methOnDestroy(Py_BuildValue("s", "ondestroy"))
, _methOnUpdate(Py_BuildValue("s", "onupdate"))
, _methOnIdle(Py_BuildValue("s", "onidle"))
, _methOnSessionCreate(Py_BuildValue("s", "onsessioncreate"))
, _methOnSessionDestroy(Py_BuildValue("s", "onsessiondestroy"))
, _methOnAsyncConnResult(Py_BuildValue("s", "onasyncconnresult"))
, _methOnProtoReport(Py_BuildValue("s", "onprotoreport"))
, _methOnUnHandledPacket(Py_BuildValue("s", "onunhandledpacket"))

, _keySVC(Py_BuildValue("s", "svc"))
, _keyIp(Py_BuildValue("s", "ip"))
, _keyPort(Py_BuildValue("s", "port"))
, _keySessionId(Py_BuildValue("s", "session_id"))
, _keyOpcode(Py_BuildValue("s", "opcode"))
, _keyData(Py_BuildValue("s", "data"))
, _keyStatus(Py_BuildValue("s", "status"))
, _keyReason(Py_BuildValue("s", "reason"))
, _keyConnected(Py_BuildValue("s", "connected"))
, _keyIdleTime(Py_BuildValue("s", "idletime"))
, _keyCObj(Py_BuildValue("s", "cobj"))

, _pyPacketCls(NULL)
{
}

pyllbc_Facade::~pyllbc_Facade()
{
    Py_DECREF(_methOnInitialize);
    Py_DECREF(_methOnDestroy);
    Py_DECREF(_methOnUpdate);
    Py_DECREF(_methOnIdle);
    Py_DECREF(_methOnSessionCreate);
    Py_DECREF(_methOnSessionDestroy);
    Py_DECREF(_methOnAsyncConnResult);

    Py_DECREF(_keySVC);
    Py_DECREF(_keyIp);
    Py_DECREF(_keyPort);
    Py_DECREF(_keySessionId);
    Py_DECREF(_keyOpcode);
    Py_DECREF(_keyData);
    Py_DECREF(_keyStatus);
    Py_DECREF(_keyReason);
    Py_DECREF(_keyConnected);
    Py_DECREF(_keyIdleTime);
    Py_DECREF(_keyCObj);

    Py_XDECREF(_pyPacketCls);
}

void pyllbc_Facade::OnInitialize()
{
    typedef pyllbc_Service::_Facades _Facades;

    PyObject *ev = pyllbc_FacadeEvBuilder::BuildInitializeEv(_pySvc);
    LLBC_InvokeGuard guard(&DecRefPyObj, ev);

    _Facades &facades = _svc->_facades;
    for (_Facades::iterator it = facades.begin();
         it != facades.end();
         it++)
    {
        PyObject *facade = *it;
        if (PyObject_HasAttr(facade, _methOnInitialize))
        {
            PyObject *pyRtn = PyObject_CallMethodObjArgs(facade, _methOnInitialize, ev, NULL);
            if (!pyRtn)
            {
                pyllbc_TransferPyError();
                return;
            }

            Py_DECREF(pyRtn);
        }
    }
}

void pyllbc_Facade::OnDestroy()
{
    typedef pyllbc_Service::_Facades _Facades;

    PyObject *ev = pyllbc_FacadeEvBuilder::BuildDestroyEv(_pySvc);
    LLBC_InvokeGuard guard(&DecRefPyObj, ev);

    _Facades &facades = _svc->_facades;
    for (_Facades::iterator it = facades.begin();
         it != facades.end();
         it++)
    {
        PyObject *facade = *it;
        if (PyObject_HasAttr(facade, _methOnDestroy))
        {
            PyObject *pyRtn = PyObject_CallMethodObjArgs(facade, _methOnDestroy, ev, NULL);
            if (!pyRtn)
            {
                pyllbc_TransferPyError();
                return;
            }

            Py_DECREF(pyRtn);
        }
    }
}

void pyllbc_Facade::OnUpdate()
{
    typedef pyllbc_Service::_Facades _Facades;

    if (UNLIKELY(_svc->_stoping))
        return;

    PyObject *ev = pyllbc_FacadeEvBuilder::BuildUpdateEv(_pySvc);
    LLBC_InvokeGuard guard(&DecRefPyObj, ev);

    _Facades &facades = _svc->_facades;
    for (_Facades::iterator it = facades.begin();
         it != facades.end();
         it++)
    {
        PyObject *facade = *it;
        if (PyObject_HasAttr(facade, _methOnUpdate))
        {
            PyObject *pyRtn = PyObject_CallMethodObjArgs(facade, _methOnUpdate, ev, NULL);
            if (!pyRtn)
            {
                pyllbc_TransferPyError();
                return;
            }

            Py_DECREF(pyRtn);
        }
    }
}

void pyllbc_Facade::OnIdle(int idleTime)
{
    typedef pyllbc_Service::_Facades _Facades;

    if (UNLIKELY(_svc->_stoping))
        return;

    PyObject *ev = pyllbc_FacadeEvBuilder::BuildIdleEv(_pySvc, idleTime);
    LLBC_InvokeGuard guard(&DecRefPyObj, ev);

    _Facades &facades = _svc->_facades;
    for (_Facades::iterator it = facades.begin();
         it != facades.end();
         it++)
    {
        PyObject *facade = *it;
        if (PyObject_HasAttr(facade, _methOnIdle))
        {

            PyObject *pyRtn = PyObject_CallMethodObjArgs(facade, _methOnIdle, ev, NULL);
            if (!pyRtn)
            {
                pyllbc_TransferPyError();
                return;
            }

            Py_DECREF(pyRtn);
        }
    }
}

void pyllbc_Facade::OnSessionCreate(const LLBC_SessionInfo &si)
{
    typedef pyllbc_Service::_Facades _Facades;

    if (UNLIKELY(_svc->_stoping))
        return;

    PyObject *ev = pyllbc_FacadeEvBuilder::BuildSessionCreateEv(_pySvc, si);
    LLBC_InvokeGuard guard(&DecRefPyObj, ev);

    _Facades &facades = _svc->_facades;
    for (_Facades::iterator it = facades.begin();
         it != facades.end();
         it++)
    {
        PyObject *facade = *it;
        if (PyObject_HasAttr(facade, _methOnSessionCreate))
        {
            PyObject *pyRtn = PyObject_CallMethodObjArgs(
                facade, _methOnSessionCreate, ev, NULL);
            if (!pyRtn)
            {
                pyllbc_TransferPyError();
                return;
            }

            Py_DECREF(pyRtn);
        }
    }
}

void pyllbc_Facade::OnSessionDestroy(int sessionId)
{
    if (UNLIKELY(_svc->_stoping))
        return;

    PyObject *ev = pyllbc_FacadeEvBuilder::BuildSessionDestroyEv(_pySvc, sessionId);
    LLBC_InvokeGuard guard(&DecRefPyObj, ev);

    typedef pyllbc_Service::_Facades _Facades;
    _Facades &facades = _svc->_facades;
    for (_Facades::iterator it = facades.begin();
         it != facades.end();
         it++)
    {
        PyObject *facade = *it;
        if (PyObject_HasAttr(facade, _methOnSessionDestroy))
        {
            PyObject *pyRtn = 
                PyObject_CallMethodObjArgs(facade, _methOnSessionDestroy, ev, NULL);
            if (!pyRtn)
            {
                pyllbc_TransferPyError();
                return;
            }

            Py_DECREF(pyRtn);
        }
    }
}

void pyllbc_Facade::OnAsyncConnResult(const LLBC_AsyncConnResult &result)
{
    if (UNLIKELY(_svc->_stoping))
        return;

    PyObject *ev = pyllbc_FacadeEvBuilder::BuildAsyncConnResultEv(_pySvc, result);
    LLBC_InvokeGuard guard(&DecRefPyObj, ev);

    typedef pyllbc_Service::_Facades _Facades;
    _Facades &facades = _svc->_facades;
    for (_Facades::iterator it = facades.begin();
         it != facades.end();
         it++)
    {
        PyObject *facade = *it;
        if (PyObject_HasAttr(facade, _methOnAsyncConnResult))
        {
            PyObject *pyRtn = 
                PyObject_CallMethodObjArgs(facade, _methOnAsyncConnResult, ev, NULL);
            if (!pyRtn)
            {
                pyllbc_TransferPyError();
                return;
            }

            Py_DECREF(pyRtn);
        }
    }
}

void pyllbc_Facade::OnProtoReport(const LLBC_ProtoReport &report)
{
    if (UNLIKELY(_svc->_stoping))
        return;

    PyObject *ev = pyllbc_FacadeEvBuilder::BuildProtoReportEv(_pySvc, report);
    LLBC_InvokeGuard guard(&DecRefPyObj, ev);

    typedef pyllbc_Service::_Facades _Facades;
    _Facades &facades = _svc->_facades;
    for (_Facades::iterator it = facades.begin();
         it != facades.end();
         it++)
    {
        PyObject *facade = *it;
        if (PyObject_HasAttr(facade, _methOnProtoReport))
        {
            PyObject *pyRtn =
                PyObject_CallMethodObjArgs(facade, _methOnProtoReport, ev, NULL);
            if (!pyRtn)
            {
                pyllbc_TransferPyError();
                return;
            }

            Py_DECREF(pyRtn);
        }
    }
}

void pyllbc_Facade::OnUnHandledPacket(int opcode)
{
    if (UNLIKELY(_svc->_stoping))
        return;

    PyObject *ev = pyllbc_FacadeEvBuilder::BuildUnHandledPacketEv(_pySvc, opcode);
    LLBC_InvokeGuard guard(&DecRefPyObj, ev);

    typedef pyllbc_Service::_Facades _Facades;
    _Facades &facades = _svc->_facades;
    for (_Facades::iterator it = facades.begin();
         it != facades.end();
         it++)
    {
        PyObject *facade = *it;
        if (PyObject_HasAttr(facade, _methOnUnHandledPacket))
        {
            PyObject *pyRtn =
                PyObject_CallMethodObjArgs(facade, _methOnUnHandledPacket, ev, NULL);
            if (!pyRtn)
            {
                pyllbc_TransferPyError();
                return;
            }

            Py_DECREF(pyRtn);
        }
    }
}

void pyllbc_Facade::OnDataReceived(LLBC_Packet &packet)
{
    typedef pyllbc_Service::_PacketHandlers _Handlers;

    if (UNLIKELY(_svc->_stoping))
        return;

    _Handlers &handlers = _svc->_handlers;
    _Handlers::iterator handlerIt = handlers.find(packet.GetOpcode());
    if (handlerIt == handlers.end())
        return;

    pyllbc_PacketHandler *handler = handlerIt->second;
    PyObject *pyPacket = reinterpret_cast<PyObject *>(packet.GetPreHandleResult());
    if (!pyPacket)
        if (UNLIKELY(!(pyPacket = this->BuildPyPacket(packet))))
            return;

    PyObject *ret = handler->Handle(pyPacket);
    if (LIKELY(ret))
        Py_DECREF(ret);

    if (!packet.GetPreHandleResult())
        Py_DECREF(pyPacket);
}

void *pyllbc_Facade::OnDataPreReceived(LLBC_Packet &packet)
{
    static void * const succeedRtn = reinterpret_cast<void *>(0x01);
    static void * const failedRtn = reinterpret_cast<void *>(0x00);

    typedef pyllbc_Service::_PacketHandlers _Handlers;

    if (UNLIKELY(_svc->_stoping))
        return succeedRtn;

    _Handlers &handlers = _svc->_preHandlers;
    _Handlers::iterator handlerIt = handlers.find(packet.GetOpcode());
    if (handlerIt == handlers.end())
        return succeedRtn;

    pyllbc_PacketHandler *handler = handlerIt->second;
    PyObject *pyPacket = this->BuildPyPacket(packet);
    if (UNLIKELY(!pyPacket))
        return failedRtn;

    packet.SetPreHandleResult(pyPacket, this, &This::DeletePyPacket);

    PyObject *ret = handler->Handle(pyPacket);
    if (UNLIKELY(!ret))
        return failedRtn;

    const int detectResult = PyObject_IsTrue(ret);
    if (UNLIKELY(detectResult == -1))
        pyllbc_TransferPyError();

    Py_DECREF(ret);
    return (detectResult == 0 ? failedRtn : succeedRtn);
}

#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
void *pyllbc_Facade::OnDataUnifyPreReceived(LLBC_Packet &packet)
{
    static void * const succeedRtn = reinterpret_cast<void *>(0x01);
    static void * const failedRtn = reinterpret_cast<void *>(0x00);

    if (UNLIKELY(_svc->_stoping))
        return succeedRtn;

    PyObject *pyPacket = this->BuildPyPacket(packet);
    if (UNLIKELY(!pyPacket))
        return failedRtn;

    packet.SetPreHandleResult(pyPacket, this, &This::DeletePyPacket);

    PyObject *ret = _svc->_unifyPreHandler->Handle(pyPacket);
    if (UNLIKELY(!ret))
        return failedRtn;

    const int detectResult = PyObject_IsTrue(ret);
    if (UNLIKELY(detectResult == -1))
        pyllbc_TransferPyError();

    Py_DECREF(ret);
    return (detectResult == 0 ? failedRtn : succeedRtn);
}
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

PyObject *pyllbc_Facade::BuildPyPacket(LLBC_Packet &packet)
{
    if (UNLIKELY(!_pyPacketCls))
    {
        PyObject *modDict = pyllbc_TopModule->GetModuleDict();

        _pyPacketCls = PyDict_GetItemString(modDict, "Packet");
        Py_INCREF(_pyPacketCls);
    }

    PyObject *pyData;
    if (_svcType == LLBC_IService::Raw ||
        _svc->_codec == pyllbc_Service::BinaryCodec)
    {
        // Create python layer instance: llbc.Stream
        PyObject *tupleArg = PyTuple_New(1);
#if (LLBC_CUR_COMP == LLBC_COMP_MSVC) && defined(_M_X64)
        PyTuple_SetItem(tupleArg, 0, PyInt_FromLong(static_cast<long>(packet.GetPayloadLength())));
#else
        PyTuple_SetItem(tupleArg, 0, PyInt_FromLong(packet.GetPayloadLength()));
#endif
        pyData = PyObject_CallObject(_svc->_streamCls, tupleArg);
        Py_DECREF(tupleArg);

        // Get pyllbc_Stream.
        PyObject *cobj = PyObject_GetAttr(pyData, _keyCObj);
        if (UNLIKELY(!cobj))
        {
            Py_DECREF(pyData);
            pyllbc_SetError("could not get llbc.Stream property 'cobj', recv data failed");

            return NULL;
        }

        pyllbc_Stream *cstream = NULL;
        PyArg_Parse(cobj, "l", &cstream);
        Py_DECREF(cobj);

        LLBC_Stream &rawStream = cstream->GetLLBCStream();
        rawStream.WriteBuffer(packet.GetPayload(), packet.GetPayloadLength());
        rawStream.SetPos(0);

        if (_svc->_codec == pyllbc_Service::BinaryCodec)
        {
            pyllbc_Service::_Codecs::iterator decodeIt = 
                    _svc->_codecs.find(packet.GetOpcode());
            if (decodeIt != _svc->_codecs.end())
            {
                PyObject *decoded = cstream->Read(decodeIt->second);
                Py_DECREF(pyData);

                if (!decoded)
                    return NULL;

                pyData = decoded;
            }
        }
    }
    else
    {
        const std::string j(reinterpret_cast<
            const char *>(packet.GetPayload()), packet.GetPayloadLength());
        if ((pyllbc_ObjCoder::Decode(j, pyData)) != LLBC_RTN_OK)
            return NULL;
    }

    PyObject *pySessionId = PyInt_FromLong(packet.GetSessionId());

    const LLBC_SockAddr_IN &localAddr = packet.GetLocalAddr();
    PyObject *pyLocalIp = PyString_FromString(localAddr.GetIpAsString().c_str());
    PyObject *pyLocalPort = PyInt_FromLong(localAddr.GetPort());

    const LLBC_SockAddr_IN &peerAddr = packet.GetPeerAddr();
    PyObject *pyPeerIp = PyString_FromString(peerAddr.GetIpAsString().c_str());
    PyObject *pyPeerPort = PyInt_FromLong(peerAddr.GetPort());

    PyObject *pyOpcode = PyInt_FromLong(packet.GetOpcode());
    PyObject *pyStatus = PyInt_FromLong(packet.GetStatus());

#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
    const LLBC_String &statusDesc = packet.GetStatusDesc();
    PyObject *pyStatusDesc = 
        PyString_FromStringAndSize(statusDesc.c_str(), statusDesc.length());
#else
    PyObject *pyStatusDesc = Py_None;
    Py_INCREF(pyStatusDesc);
#endif

    PyObject *pyPacketCObj = PyLong_FromLong(reinterpret_cast<long>(&packet));

    PyObject *pyPacket = PyObject_CallFunctionObjArgs(_pyPacketCls,
                                                      _pySvc,
                                                      pySessionId,
                                                      pyLocalIp,
                                                      pyLocalPort,
                                                      pyPeerIp,
                                                      pyPeerPort,
                                                      pyOpcode,
                                                      pyStatus,
                                                      pyStatusDesc,
                                                      pyData,
                                                      pyPacketCObj,
                                                      NULL);
    Py_DECREF(pySessionId);
    Py_DECREF(pyLocalIp);
    Py_DECREF(pyLocalPort);
    Py_DECREF(pyPeerIp);
    Py_DECREF(pyPeerPort);
    Py_DECREF(pyOpcode);
    Py_DECREF(pyStatus);
    Py_DECREF(pyStatusDesc);
    Py_DECREF(pyData);
    Py_DECREF(pyPacketCObj);

    if (UNLIKELY(!pyPacket))
    {
        pyllbc_TransferPyError();
        return NULL;
    }

    return pyPacket;
}

void pyllbc_Facade::DeletePyPacket(void *_)
{
    PyObject *pyPacket = reinterpret_cast<PyObject *>(_);
    Py_DECREF(pyPacket);
}
