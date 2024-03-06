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

pyllbc_Component::pyllbc_Component(pyllbc_Service *svc, PyObject *pyComp)
: LLBC_Component()
, _svc(svc)
, _pySvc(svc->GetPyService())
, _pyComp(pyComp)

, _pyOnInitMeth(nullptr)
, _pyOnDestroyMeth(nullptr)
, _pyOnStartMeth(nullptr)
, _pyOnStopMeth(nullptr)
, _pyOnUpdateMeth(nullptr)
, _pyOnIdleMeth(nullptr)
, _pyOnSessionCreateMeth(nullptr)
, _pyOnSessionDestroyMeth(nullptr)
, _pyOnProtoReportMeth(nullptr)
, _pyOnUnHandledPacketMeth(nullptr)

, _holdedOnIdleEv(_EvBuilder::BuildIdleEv(_pySvc, 0))
, _holdedOnUpdateEv(_EvBuilder::BuildUpdateEv(_pySvc))

, _compEvCallArgs(PyTuple_New(1))
{
    Py_INCREF(_pyComp);
    
    // Get method
    if (PyObject_HasAttrString(_pyComp, "oninit"))
        _pyOnInitMeth = PyObject_GetAttrString(_pyComp, "oninit");
    if (PyObject_HasAttrString(_pyComp, "ondestroy"))
        _pyOnDestroyMeth = PyObject_GetAttrString(_pyComp, "ondestroy");
    if (PyObject_HasAttrString(_pyComp, "onstart"))
        _pyOnStartMeth = PyObject_GetAttrString(_pyComp, "onstart");
    if (PyObject_HasAttrString(_pyComp, "onstop"))
        _pyOnStopMeth = PyObject_GetAttrString(_pyComp, "onstop");
    if (PyObject_HasAttrString(_pyComp, "onupdate"))
        _pyOnUpdateMeth = PyObject_GetAttrString(_pyComp, "onupdate");
    if (PyObject_HasAttrString(_pyComp, "onidle"))
        _pyOnIdleMeth = PyObject_GetAttrString(_pyComp, "onidle");
    if (PyObject_HasAttrString(_pyComp, "onsessioncreate"))
        _pyOnSessionCreateMeth = PyObject_GetAttrString(_pyComp, "onsessioncreate");
    if (PyObject_HasAttrString(_pyComp, "onsessiondestroy"))
        _pyOnSessionDestroyMeth = PyObject_GetAttrString(_pyComp, "onsessiondestroy");
    if (PyObject_HasAttrString(_pyComp, "onasyncconnresult"))
        _pyOnAsyncConnResultMeth = PyObject_GetAttrString(_pyComp, "onasyncconnresult");
    if (PyObject_HasAttrString(_pyComp, "onprotoreport"))
        _pyOnProtoReportMeth = PyObject_GetAttrString(_pyComp, "onprotoreport");
    if (PyObject_HasAttrString(_pyComp, "onunhandledpacket"))
        _pyOnUnHandledPacketMeth = PyObject_GetAttrString(_pyComp, "onunhandledpacket");
}

pyllbc_Component::~pyllbc_Component()
{
    Py_DECREF(_pyComp);

    Py_DECREF(_pyOnInitMeth);
    Py_DECREF(_pyOnDestroyMeth);
    Py_DECREF(_pyOnStartMeth);
    Py_DECREF(_pyOnStopMeth);
    Py_DECREF(_pyOnUpdateMeth);
    Py_DECREF(_pyOnIdleMeth);
    Py_DECREF(_pyOnSessionCreateMeth);
    Py_DECREF(_pyOnSessionDestroyMeth);
    Py_DECREF(_pyOnAsyncConnResultMeth);
    Py_DECREF(_pyOnProtoReportMeth);
    Py_DECREF(_pyOnUnHandledPacketMeth);

    Py_DECREF(_holdedOnIdleEv);
    Py_DECREF(_holdedOnUpdateEv);

    Py_DECREF(_compEvCallArgs);
}

bool pyllbc_Component::OnInit(bool &initFinished)
{
    LLBC_ReturnIf(!_pyOnInitMeth, true);

    PyObject *pyRet = CallComponentMeth(_pyOnInitMeth, _EvBuilder::BuildInitializeEv(_pySvc), true, true);

    return ParsePythonRet(pyRet, initFinished);
}

void pyllbc_Component::OnDestroy(bool &destroyFinished)
{
    LLBC_ReturnIf(!_pyOnDestroyMeth, void());

    PyObject *pyRet = CallComponentMeth(_pyOnDestroyMeth, _EvBuilder::BuildDestroyEv(_pySvc), true, true);

    ParsePythonRet(pyRet, destroyFinished);
}

bool pyllbc_Component::OnStart(bool &startFinished)
{
    LLBC_ReturnIf(!_pyOnStartMeth, true);

    PyObject *pyRet = CallComponentMeth(_pyOnStartMeth, _EvBuilder::BuildStartEv(_pySvc), true, true);

    return ParsePythonRet(pyRet, startFinished);
}

void pyllbc_Component::OnStop(bool &stopFinished)
{
    LLBC_ReturnIf(!_pyOnStopMeth, void());

    PyObject *pyRet = CallComponentMeth(_pyOnStopMeth, _EvBuilder::BuildStopEv(_pySvc), true, true);

    ParsePythonRet(pyRet, stopFinished);
}

void pyllbc_Component::OnUpdate()
{
    LLBC_ReturnIf(!_pyOnUpdateMeth, void());

    LLBC_ReturnIf(UNLIKELY(_svc->_stoping), void());

    CallComponentMeth(_pyOnUpdateMeth, _holdedOnUpdateEv, false, false);
}

void pyllbc_Component::OnIdle(const LLBC_TimeSpan &idleTime)
{
    LLBC_ReturnIf(!_pyOnIdleMeth, void());

    LLBC_ReturnIf(UNLIKELY(_svc->_stoping), void());

    PyObject *pyIdleTime = PyFloat_FromDouble(
        idleTime.GetTotalMicros() / static_cast<double>(LLBC_TimeConst::numOfMicrosPerSecond));
    PyObject_SetAttrString(_holdedOnIdleEv, "_idletime", pyIdleTime);
    Py_DECREF(pyIdleTime);

    CallComponentMeth(_pyOnIdleMeth, _holdedOnIdleEv, false, false);
}

void pyllbc_Component::OnEvent(LLBC_ComponentEvents::ENUM event, const LLBC_Variant &evArgs)
{
    switch(event)
    {
        case LLBC_ComponentEvents::SessionCreate:
        {
            OnSessionCreate(*evArgs.AsPtr<LLBC_SessionInfo>());
            break;
        }
        case LLBC_ComponentEvents::SessionDestroy:
        {
            OnSessionDestroy(*evArgs.AsPtr<LLBC_SessionDestroyInfo>());
            break;
        }
        case LLBC_ComponentEvents::AsyncConnResult:
        {
            OnAsyncConnResult(*evArgs.AsPtr<LLBC_AsyncConnResult>());
            break;
        }
        case LLBC_ComponentEvents::ProtoReport:
        {
            OnProtoReport(*evArgs.AsPtr<LLBC_ProtoReport>());
            break;
        }
        case LLBC_ComponentEvents::UnHandledPacket:
        {
            OnUnHandledPacket(*evArgs.AsPtr<LLBC_Packet>());
            break;
        }
        default: break;
    }
}

void pyllbc_Component::OnSessionCreate(const LLBC_SessionInfo &sessionInfo)
{
    LLBC_ReturnIf(!_pyOnSessionCreateMeth, void());

    LLBC_ReturnIf(UNLIKELY(_svc->_stoping), void());

    CallComponentMeth(_pyOnSessionCreateMeth, _EvBuilder::BuildSessionCreateEv(_pySvc, sessionInfo), true, false);
}

void pyllbc_Component::OnSessionDestroy(const LLBC_SessionDestroyInfo &destroyInfo)
{
    LLBC_ReturnIf(!_pyOnSessionDestroyMeth, void());

    LLBC_ReturnIf(UNLIKELY(_svc->_stoping), void());

    CallComponentMeth(_pyOnSessionDestroyMeth, _EvBuilder::BuildSessionDestroyEv(_pySvc, destroyInfo), true, false);
}

void pyllbc_Component::OnAsyncConnResult(const LLBC_AsyncConnResult &result)
{
    LLBC_ReturnIf(!_pyOnAsyncConnResultMeth, void());

    LLBC_ReturnIf(UNLIKELY(_svc->_stoping), void());

    CallComponentMeth(_pyOnAsyncConnResultMeth, _EvBuilder::BuildAsyncConnResultEv(_pySvc, result), true, false);
}

void pyllbc_Component::OnProtoReport(const LLBC_ProtoReport &report)
{
    LLBC_ReturnIf(!_pyOnProtoReportMeth, void());

    LLBC_ReturnIf(UNLIKELY(_svc->_stoping), void());

    CallComponentMeth(_pyOnProtoReportMeth, _EvBuilder::BuildProtoReportEv(_pySvc, report), true, false);
}

void pyllbc_Component::OnUnHandledPacket(const LLBC_Packet &packet)
{
    LLBC_ReturnIf(!_pyOnUnHandledPacketMeth, void());

    LLBC_ReturnIf(UNLIKELY(_svc->_stoping), void());

    PyObject *pyPacket = _svc->BuildPyPacket(packet);
    LLBC_ReturnIf(UNLIKELY(pyPacket == nullptr), void());

    PyObject *ev = 
        pyllbc_ComponentEvBuilder::BuildUnHandledPacketEv(_pySvc, packet, pyPacket);
    Py_DecRef(pyPacket); pyPacket = nullptr;

    CallComponentMeth(_pyOnUnHandledPacketMeth, ev, true, false);
}

PyObject *pyllbc_Component::CallComponentMeth(PyObject *meth, PyObject *ev, bool decRefEv, bool isRetRequired)
{
    // Set event to call args.
    Py_INCREF(ev);
    PyTuple_SetItem(_compEvCallArgs, 0, ev); // Steals reference.
    PyObject *pyRet = PyObject_Call(meth, _compEvCallArgs, nullptr);
    LLBC_DoIf(!pyRet, pyllbc_TransferPyError());

    // Clear call args.
    Py_INCREF(Py_None);
    PyTuple_SetItem(_compEvCallArgs, 0, Py_None);

    // Decref event, if acquire.
    LLBC_DoIf(decRefEv, Py_DECREF(ev));

    if (!isRetRequired)
    {
        LLBC_DoIf(pyRet, Py_DECREF(pyRet));
        return nullptr;
    }

    return pyRet;
}

bool pyllbc_Component::ParsePythonRet(PyObject *pyRet, bool &finished)
{
    LLBC_ReturnIf(UNLIKELY(!pyRet), false);

    finished = false;

    // default is successful and finished
    if (pyRet == Py_None)
    {
        finished = true;
        Py_DecRef(pyRet);
        return true;
    }

    // parse return
    if (PyBool_Check(pyRet))
    {
        finished = PyObject_IsTrue(pyRet);
        Py_DecRef(pyRet);
        return true;
    }
    else
    {
        pyllbc_SetError(LLBC_String().format("Invalid ret type:%s", pyRet->ob_type->tp_name));
        Py_DecRef(pyRet);
        return false;
    }
}
