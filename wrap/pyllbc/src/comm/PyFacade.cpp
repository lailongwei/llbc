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

#include "pyllbc/comm/PyObjCoder.h"
#include "pyllbc/comm/PyPacketHandler.h"
#include "pyllbc/comm/PyService.h"
#include "pyllbc/comm/PyFacade.h"
#include "pyllbc/comm/PyFacadeEvBuilder.h"

namespace
{
    typedef pyllbc_Facade This;
    typedef pyllbc_FacadeEvBuilder _EvBuilder;
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
: LLBC_IFacade(LLBC_FacadeEvents::AllEvents)
, _svc(svc)
, _pySvc(svc->GetPyService())

, _svcType(svc->GetType())

, _methOnInitialize(Py_BuildValue("s", "oninitialize"))
, _methOnDestroy(Py_BuildValue("s", "ondestroy"))
, _methOnStart(Py_BuildValue("s", "onstart"))
, _methOnStop(Py_BuildValue("s", "onstop"))
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
    Py_DECREF(_methOnStart);
    Py_DECREF(_methOnStop);
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

bool pyllbc_Facade::OnInitialize()
{
    return CallFacadeMeth(_methOnInitialize, _EvBuilder::BuildInitializeEv(_pySvc));
}

void pyllbc_Facade::OnDestroy()
{
    CallFacadeMeth(_methOnDestroy, _EvBuilder::BuildDestroyEv(_pySvc));
}

bool pyllbc_Facade::OnStart()
{
    return CallFacadeMeth(_methOnStart, _EvBuilder::BuildStartEv(_pySvc));
}

void pyllbc_Facade::OnStop()
{
    CallFacadeMeth(_methOnStop, _EvBuilder::BuildStopEv(_pySvc));
}

void pyllbc_Facade::OnUpdate()
{
    if (UNLIKELY(_svc->_stoping))
        return;

    CallFacadeMeth(_methOnUpdate, _EvBuilder::BuildUpdateEv(_pySvc));
}

void pyllbc_Facade::OnIdle(int idleTime)
{
    if (UNLIKELY(_svc->_stoping))
        return;

    CallFacadeMeth(_methOnIdle, _EvBuilder::BuildIdleEv(_pySvc, idleTime));
}

void pyllbc_Facade::OnSessionCreate(const LLBC_SessionInfo &sessionInfo)
{
    if (UNLIKELY(_svc->_stoping))
        return;

    CallFacadeMeth(_methOnSessionCreate, _EvBuilder::BuildSessionCreateEv(_pySvc, sessionInfo));
}

void pyllbc_Facade::OnSessionDestroy(const LLBC_SessionDestroyInfo &destroyInfo)
{
    if (UNLIKELY(_svc->_stoping))
        return;

    CallFacadeMeth(_methOnSessionDestroy, _EvBuilder::BuildSessionDestroyEv(_pySvc, destroyInfo));
}

void pyllbc_Facade::OnAsyncConnResult(const LLBC_AsyncConnResult &result)
{
    if (UNLIKELY(_svc->_stoping))
        return;

    CallFacadeMeth(_methOnAsyncConnResult, _EvBuilder::BuildAsyncConnResultEv(_pySvc, result));
}

void pyllbc_Facade::OnProtoReport(const LLBC_ProtoReport &report)
{
    if (UNLIKELY(_svc->_stoping))
        return;

    CallFacadeMeth(_methOnProtoReport, _EvBuilder::BuildProtoReportEv(_pySvc, report));
}

void pyllbc_Facade::OnUnHandledPacket(const LLBC_Packet &packet)
{
    if (UNLIKELY(_svc->_stoping))
        return;

    PyObject *pyPacket = BuildPyPacket(packet);
    if (UNLIKELY(pyPacket == NULL))
        return;

    PyObject *ev = 
        pyllbc_FacadeEvBuilder::BuildUnHandledPacketEv(_pySvc, packet, pyPacket);
    Py_DecRef(pyPacket); pyPacket = NULL;

    CallFacadeMeth(_methOnUnHandledPacket, ev);
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
        if (UNLIKELY(!(pyPacket = BuildPyPacket(packet))))
            return;

    PyObject *ret = handler->Handle(pyPacket);
    if (LIKELY(ret))
        Py_DECREF(ret);

    if (!packet.GetPreHandleResult())
        Py_DECREF(pyPacket);
}

bool pyllbc_Facade::OnDataPreReceived(LLBC_Packet &packet)
{
    typedef pyllbc_Service::_PacketHandlers _Handlers;

    if (UNLIKELY(_svc->_stoping))
        return true;

    _Handlers &handlers = _svc->_preHandlers;
    _Handlers::iterator handlerIt = handlers.find(packet.GetOpcode());
    if (handlerIt == handlers.end())
        return true;

    pyllbc_PacketHandler *handler = handlerIt->second;
    PyObject *pyPacket = BuildPyPacket(packet);
    if (UNLIKELY(!pyPacket))
        return false;

    packet.SetPreHandleResult(pyPacket, this, &This::DeletePyPacket);

    PyObject *ret = handler->Handle(pyPacket);
    if (UNLIKELY(!ret))
        return false;

    const int detectResult = PyObject_IsTrue(ret);
    if (UNLIKELY(detectResult == -1))
    {
        pyllbc_TransferPyError();

        Py_DECREF(ret);
        return false;
    }

    Py_DECREF(ret);
    return detectResult != 0;
}

#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
bool pyllbc_Facade::OnDataUnifyPreReceived(LLBC_Packet &packet)
{
    if (UNLIKELY(_svc->_stoping))
        return true;

    PyObject *pyPacket = BuildPyPacket(packet);
    if (UNLIKELY(!pyPacket))
        return false;

    packet.SetPreHandleResult(pyPacket, this, &This::DeletePyPacket);

    PyObject *ret = _svc->_unifyPreHandler->Handle(pyPacket);
    if (UNLIKELY(!ret))
        return false;

    const int detectResult = PyObject_IsTrue(ret);
    if (UNLIKELY(detectResult == -1))
    {
        pyllbc_TransferPyError();

        Py_DECREF(ret);
        return false;
    }

    Py_DECREF(ret);
    return detectResult != 0;
}
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

PyObject *pyllbc_Facade::BuildPyPacket(const LLBC_Packet &packet)
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
        rawStream.Attach(const_cast<void *>(packet.GetPayload()), packet.GetPayloadLength());

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
        if ((pyllbc_ObjCoder::Decode(j, pyData)) != LLBC_OK)
            return NULL;
    }

    PyObject *pySenderSvcId = PyInt_FromLong(packet.GetSenderServiceId());
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

    PyObject *pyExtData1 = PyLong_FromLongLong(packet.GetExtData1());
    PyObject *pyExtData2 = PyLong_FromLongLong(packet.GetExtData2());
    PyObject *pyExtData3 = PyLong_FromLongLong(packet.GetExtData3());

    PyObject *pyPacketCObj = PyLong_FromUnsignedLongLong(reinterpret_cast<uint64>(&packet));

    PyObject *pyPacket = PyObject_CallFunctionObjArgs(_pyPacketCls,
                                                      _pySvc,
                                                      pySenderSvcId,
                                                      pySessionId,
                                                      pyLocalIp,
                                                      pyLocalPort,
                                                      pyPeerIp,
                                                      pyPeerPort,
                                                      pyOpcode,
                                                      pyStatus,
                                                      pyStatusDesc,
                                                      pyData,
                                                      pyExtData1,
                                                      pyExtData2,
                                                      pyExtData3,
                                                      pyPacketCObj,
                                                      NULL);
    Py_DECREF(pySenderSvcId);
    Py_DECREF(pySessionId);
    Py_DECREF(pyLocalIp);
    Py_DECREF(pyLocalPort);
    Py_DECREF(pyPeerIp);
    Py_DECREF(pyPeerPort);
    Py_DECREF(pyOpcode);
    Py_DECREF(pyStatus);
    Py_DECREF(pyStatusDesc);
    Py_DECREF(pyData);
    Py_DECREF(pyExtData1);
    Py_DECREF(pyExtData2);
    Py_DECREF(pyExtData3);
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

bool pyllbc_Facade::CallFacadeMeth(PyObject *meth, PyObject *ev)
{
    typedef pyllbc_Service::_Facades _Facades;
    LLBC_InvokeGuard guard(&DecRefPyObj, ev);

    _Facades &facades = _svc->_facades;
    for (_Facades::iterator it = facades.begin();
         it != facades.end();
         it++)
    {
        PyObject *facade = *it;
        if (PyObject_HasAttr(facade, meth))
        {
            PyObject *pyRtn = PyObject_CallMethodObjArgs(facade, meth, ev, NULL);
            if (!pyRtn)
            {
                pyllbc_TransferPyError();
                return false;
            }

            Py_DECREF(pyRtn);
        }
    }

    return true;
}
