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
#include "pyllbc/comm/PyComponent.h"
#include "pyllbc/comm/PyComponentEvBuilder.h"

namespace
{
    typedef pyllbc_Component This;
    typedef pyllbc_ComponentEvBuilder _EvBuilder;
}

namespace
{
    void DecRefPyObj(void *obj)
    {
        PyObject *pyObj = reinterpret_cast<PyObject *>(obj);
        Py_DECREF(pyObj);
    }
}

pyllbc_Component::pyllbc_Component(pyllbc_Service *svc)
: LLBC_Component(LLBC_ComponentEvents::AllEvents)
, _svc(svc)
, _pySvc(svc->GetPyService())

, _methOnInit(Py_BuildValue("s", "oninit"))
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
, _keyInlIdleTime(Py_BuildValue("s", "_idletime"))
, _keyCObj(Py_BuildValue("s", "_cobj"))

, _pyPacketCls(nullptr)
#if PYLLBC_CFG_PACKET_REUSE
, _pyReusePacket(nullptr)
, _pyPacketReuseMeth(nullptr)
#endif // PYLLBC_CFG_PACKET_REUSE
, _pyNullCObj(PyInt_FromLong(0))
, _pyPacketCreateArgs(PyTuple_New(7))

, _pyStream(nullptr)
, _nativeStream(nullptr)

, _holdedOnIdleEv(_EvBuilder::BuildIdleEv(_pySvc, 0))
, _holdedOnUpdateEv(_EvBuilder::BuildUpdateEv(_pySvc))

, _compEvCallArgs(PyTuple_New(1))
{
    Py_INCREF(_pySvc);
    PyTuple_SetItem(_pyPacketCreateArgs, 0, _pySvc);
}

pyllbc_Component::~pyllbc_Component()
{
    Py_DECREF(_methOnInit);
    Py_DECREF(_methOnDestroy);
    Py_DECREF(_methOnStart);
    Py_DECREF(_methOnStop);
    Py_DECREF(_methOnUpdate);
    Py_DECREF(_methOnIdle);
    Py_DECREF(_methOnSessionCreate);
    Py_DECREF(_methOnSessionDestroy);
    Py_DECREF(_methOnAsyncConnResult);
    Py_DECREF(_methOnProtoReport);
    Py_DECREF(_methOnUnHandledPacket);

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
    Py_DECREF(_keyInlIdleTime);
    Py_DECREF(_keyCObj);

    Py_XDECREF(_pyPacketCls);
    #if PYLLBC_CFG_PACKET_REUSE
    Py_XDECREF(_pyReusePacket);
    Py_XDECREF(_pyPacketReuseMeth);
    #endif // PYLLBC_CFG_PACKET_REUSE
    Py_XDECREF(_pyNullCObj);
    Py_XDECREF(_pyPacketCreateArgs);

    if (_pyStream)
    {
        Py_DECREF(_pyStream);

        _pyStream = nullptr;
        _nativeStream = nullptr;
    }

    Py_DECREF(_holdedOnIdleEv);
    Py_DECREF(_holdedOnUpdateEv);

    Py_DECREF(_compEvCallArgs);
}

bool pyllbc_Component::OnInit(bool &initFinished)
{
    return CallComponentMeth(_methOnInit, _EvBuilder::BuildInitializeEv(_pySvc), true);
}

void pyllbc_Component::OnDestroy(bool &destroyFinished)
{
    CallComponentMeth(_methOnDestroy, _EvBuilder::BuildDestroyEv(_pySvc), true);
}

bool pyllbc_Component::OnStart(bool &startFinished)
{
    return CallComponentMeth(_methOnStart, _EvBuilder::BuildStartEv(_pySvc), true);
}

void pyllbc_Component::OnStop(bool &stopFinished)
{
    CallComponentMeth(_methOnStop, _EvBuilder::BuildStopEv(_pySvc), true);
}

void pyllbc_Component::OnUpdate()
{
    if (UNLIKELY(_svc->_stoping))
        return;

    CallComponentMeth(_methOnUpdate, _holdedOnUpdateEv, false);
}

void pyllbc_Component::OnIdle(const LLBC_TimeSpan &idleTime)
{
    if (UNLIKELY(_svc->_stoping))
        return;

    PyObject *pyIdleTime = PyFloat_FromDouble(
        idleTime.GetTotalMicroSeconds() / static_cast<double>(LLBC_TimeConstant::NumOfMicroSecondsPerSecond));
    PyObject_SetAttr(_holdedOnIdleEv, _keyInlIdleTime, pyIdleTime);
    Py_DECREF(pyIdleTime);

    CallComponentMeth(_methOnIdle, _holdedOnIdleEv, false);
}

void pyllbc_Component::OnSessionCreate(const LLBC_SessionInfo &sessionInfo)
{
    if (UNLIKELY(_svc->_stoping))
        return;

    CallComponentMeth(_methOnSessionCreate, _EvBuilder::BuildSessionCreateEv(_pySvc, sessionInfo), true);
}

void pyllbc_Component::OnSessionDestroy(const LLBC_SessionDestroyInfo &destroyInfo)
{
    if (UNLIKELY(_svc->_stoping))
        return;

    CallComponentMeth(_methOnSessionDestroy, _EvBuilder::BuildSessionDestroyEv(_pySvc, destroyInfo), true);
}

void pyllbc_Component::OnAsyncConnResult(const LLBC_AsyncConnResult &result)
{
    if (UNLIKELY(_svc->_stoping))
        return;

    CallComponentMeth(_methOnAsyncConnResult, _EvBuilder::BuildAsyncConnResultEv(_pySvc, result), true);
}

void pyllbc_Component::OnProtoReport(const LLBC_ProtoReport &report)
{
    if (UNLIKELY(_svc->_stoping))
        return;

    CallComponentMeth(_methOnProtoReport, _EvBuilder::BuildProtoReportEv(_pySvc, report), true);
}

void pyllbc_Component::OnUnHandledPacket(const LLBC_Packet &packet)
{
    if (UNLIKELY(_svc->_stoping))
        return;

    PyObject *pyPacket = BuildPyPacket(packet);
    if (UNLIKELY(pyPacket == nullptr))
        return;

    PyObject *ev = 
        pyllbc_ComponentEvBuilder::BuildUnHandledPacketEv(_pySvc, packet, pyPacket);
    Py_DecRef(pyPacket); pyPacket = nullptr;

    CallComponentMeth(_methOnUnHandledPacket, ev, true);
}

void pyllbc_Component::OnDataReceived(LLBC_Packet &packet)
{
    typedef pyllbc_Service::_PacketHandlers _Handlers;

    // Stopping check.
    if (UNLIKELY(_svc->_stoping))
        return;

    // Find handler.
    _Handlers &handlers = _svc->_handlers;
    _Handlers::iterator handlerIt = handlers.find(packet.GetOpcode());
    if (UNLIKELY(handlerIt == handlers.end()))
        return;

    // Build python layer packet.
    pyllbc_PacketHandler *&handler = handlerIt->second;
    PyObject *pyPacket = reinterpret_cast<PyObject *>(packet.GetPreHandleResult());
    if (!pyPacket)
    {
        if (UNLIKELY(!(pyPacket = BuildPyPacket(packet))))
            return;
    }

    // Handle packet.
    PyObject *ret = handler->Handle(pyPacket);
    if (LIKELY(ret))
        Py_DECREF(ret);

    // Force clear pylayer packet._cobj field.
    PyObject_SetAttr(pyPacket, _keyCObj, _pyNullCObj);

    // Delete packet.
    if (!packet.GetPreHandleResult())
        Py_DECREF(pyPacket);
}

bool pyllbc_Component::OnDataPreReceived(LLBC_Packet &packet)
{
    typedef pyllbc_Service::_PacketHandlers _Handlers;

    if (UNLIKELY(_svc->_stoping))
        return true;

    _Handlers &handlers = _svc->_preHandlers;
    _Handlers::iterator handlerIt = handlers.find(packet.GetOpcode());
    if (UNLIKELY(handlerIt == handlers.end()))
        return true;

    pyllbc_PacketHandler *&handler = handlerIt->second;
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
bool pyllbc_Component::OnDataUnifyPreReceived(LLBC_Packet &packet)
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

PyObject *pyllbc_Component::BuildPyPacket(const LLBC_Packet &packet)
{
    // Get python layer class: llbc.Packet
    if (UNLIKELY(!_pyPacketCls))
    {
        PyObject *modDict = pyllbc_TopModule->GetModuleDict();
        _pyPacketCls = PyDict_GetItemString(modDict, "Packet");
        Py_INCREF(_pyPacketCls);

        #if PYLLBC_CFG_PACKET_REUSE
        _pyReusePacket = CreateReusePyPacket();
        _pyPacketReuseMeth = PyObject_GetAttrString(_pyReusePacket, "_reuse");
        #endif // PYLLBC_CFG_PACKET_REUSE
    }

    // Create python layer instance: llbc.Stream
    if (UNLIKELY(!_pyStream))
    {
        PyObject *tupleArg = PyTuple_New(1);
#if (LLBC_CUR_COMP == LLBC_COMP_MSVC) && defined(_M_X64)
        PyTuple_SetItem(tupleArg, 0, PyInt_FromLong(static_cast<long>(packet.GetPayloadLength())));
#else
        PyTuple_SetItem(tupleArg, 0, PyInt_FromLong(packet.GetPayloadLength()));
#endif
        _pyStream = PyObject_CallObject(_svc->_streamCls, tupleArg);
        Py_DECREF(tupleArg);

        // Get pyllbc_Stream object.
        PyObject *cobj = PyObject_GetAttrString(_pyStream, "cobj");
        if (UNLIKELY(!cobj))
        {
            Py_DECREF(_pyStream);
            pyllbc_SetError("could not get llbc.Stream property 'cobj', recv data failed");

            return nullptr;
        }

        PyArg_Parse(cobj, "l", &_nativeStream);
        Py_DECREF(cobj);
    }
        
    _nativeStream->GetLLBCStream().Attach(
        const_cast<void *>(packet.GetPayload()), packet.GetPayloadLength());

    PyObject *pyData = nullptr;
    const auto &decoders = _svc->_decoders;
    const auto decoderIt = decoders.find(packet.GetOpcode());
    if (decoderIt != decoders.end())
    {
        PyObject *decoded = _nativeStream->Read(decoderIt->second);
        if (!decoded)
            return nullptr;

        pyData = decoded;
    }

    if (!pyData)
    {
        Py_IncRef(_pyStream);
        pyData = _pyStream;
    }

    PyObject *pySenderSvcId = PyInt_FromLong(packet.GetSenderServiceId());
    PyObject *pySessionId = PyInt_FromLong(packet.GetSessionId());

    PyObject *pyOpcode = PyInt_FromLong(packet.GetOpcode());
    PyObject *pyStatus = PyInt_FromLong(packet.GetStatus());

    PyObject *pyPacketCObj = PyLong_FromUnsignedLongLong(reinterpret_cast<uint64>(&packet));

    PyTuple_SetItem(_pyPacketCreateArgs, 1, pySenderSvcId);
    PyTuple_SetItem(_pyPacketCreateArgs, 2, pySessionId);
    PyTuple_SetItem(_pyPacketCreateArgs, 3, pyOpcode);
    PyTuple_SetItem(_pyPacketCreateArgs, 4, pyStatus);
    PyTuple_SetItem(_pyPacketCreateArgs, 5, pyData);
    PyTuple_SetItem(_pyPacketCreateArgs, 6, pyPacketCObj);
    #if PYLLBC_CFG_PACKET_REUSE
    PyObject *reuseRet = PyObject_CallObject(_pyPacketReuseMeth, _pyPacketCreateArgs);

    Py_IncRef(Py_None);
    PyTuple_SetItem(_pyPacketCreateArgs, 5, Py_None); // Only clear pyData item(index 5).

    if (UNLIKELY(!reuseRet))
    {
        pyllbc_TransferPyError();
        return nullptr;
    }

    Py_DecRef(reuseRet);
    Py_IncRef(_pyReusePacket);

    return _pyReusePacket;
    #else
    PyObject *pyPacket = PyObject_CallObject(_pyPacketCls, _pyPacketCreateArgs);

    Py_IncRef(Py_None);
    PyTuple_SetItem(_pyPacketCreateArgs, 5, Py_None); // Only clear pyData item(index 5).

    if (UNLIKELY(!pyPacket))
    {
        pyllbc_TransferPyError();
        return nullptr;
    }

    return pyPacket;
    #endif // PYLLBC_CFG_PACKET_REUSE
}

void pyllbc_Component::DeletePyPacket(void *_)
{
    PyObject *pyPacket = reinterpret_cast<PyObject *>(_);
    Py_DECREF(pyPacket);
}

bool pyllbc_Component::CallComponentMeth(PyObject *meth, PyObject *ev, bool decRefEv)
{
    typedef pyllbc_Service::_Comps _Comps;

    // Set event to call args.
    Py_INCREF(ev);
    PyTuple_SetItem(_compEvCallArgs, 0, ev); // Steals reference.

    _Comps &comps = _svc->_comps;
    for (_Comps::iterator it = comps.begin();
         it != comps.end();
         it++)
    {
        PyObject *&comp = *it;
        if (!PyObject_HasAttr(comp, meth)) // TODO: Optimize
            continue;

        // Get method and call.
        PyObject *pyMeth = PyObject_GetAttr(comp, meth); // New reference.
        PyObject *pyRtn = PyObject_Call(pyMeth, _compEvCallArgs, nullptr);
        if (!pyRtn)
        {
            pyllbc_TransferPyError();

            Py_DECREF(pyMeth);
            Py_INCREF(Py_None);
            PyTuple_SetItem(_compEvCallArgs, 0, Py_None);
            if (decRefEv)
                Py_DECREF(ev);

            return false;
        }

        Py_DECREF(pyMeth);
        Py_DECREF(pyRtn);
    }

    // Clear call args.
    Py_INCREF(Py_None);
    PyTuple_SetItem(_compEvCallArgs, 0, Py_None);

    // Decref event, if acquire.
    if (decRefEv)
        Py_DECREF(ev);

    return true;
}

#if PYLLBC_CFG_PACKET_REUSE
PyObject* pyllbc_Component::CreateReusePyPacket()
{
    PyObject *pyData = Py_None;
    Py_IncRef(pyData);

    PyObject *pySenderSvcId = PyInt_FromLong(0);
    PyObject *pySessionId = PyInt_FromLong(0);
    PyObject *pyOpcode = PyInt_FromLong(0);
    PyObject *pyStatus = PyInt_FromLong(0);
    PyObject *pyPacketCObj = PyInt_FromLong(0);

    PyTuple_SetItem(_pyPacketCreateArgs, 1, pySenderSvcId);
    PyTuple_SetItem(_pyPacketCreateArgs, 2, pySessionId);
    PyTuple_SetItem(_pyPacketCreateArgs, 3, pyOpcode);
    PyTuple_SetItem(_pyPacketCreateArgs, 4, pyStatus);
    PyTuple_SetItem(_pyPacketCreateArgs, 5, pyData);
    PyTuple_SetItem(_pyPacketCreateArgs, 6, pyPacketCObj);

    return PyObject_CallObject(_pyPacketCls, _pyPacketCreateArgs);
}
#endif // PYLLBC_CFG_PACKET_REUSE
