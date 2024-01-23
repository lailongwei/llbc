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

#include "pyllbc/comm/PyEvent.h"
#include "pyllbc/comm/PyObjCoder.h"
#include "pyllbc/comm/PyPacketHandler.h"
#include "pyllbc/comm/PyComponent.h"
#include "pyllbc/comm/ErrorHooker.h"
#include "pyllbc/comm/PyService.h"

namespace
{
    typedef pyllbc_Service This;

    void ResetMainloopFlag(void *flagPtr)
    {
        bool *flag = reinterpret_cast<bool *>(flagPtr);
        *flag = false;
    }
}

PyObject *pyllbc_Service::_pyEvCls = nullptr;
LLBC_Delegate<void(LLBC_Event *)> pyllbc_Service::_evEnqueueHandler(&pyllbc_Service::EventEnqueueHandler);
LLBC_Delegate<void(LLBC_Event *)> pyllbc_Service::_evDequeueHandler(&pyllbc_Service::EventDequeueHandler);
PyObject *pyllbc_Service::_streamCls = nullptr;
pyllbc_ErrorHooker *pyllbc_Service::_errHooker = new pyllbc_ErrorHooker;

pyllbc_Service::pyllbc_Service(const LLBC_String &name, bool useNormalProtocolFactory, PyObject *pySvc)
: _llbcSvc(nullptr)
, _llbcSvcName(name.c_str(), name.length())
, _useNormalProtocolFactory(useNormalProtocolFactory)

, _pySvc(pySvc)

, _inMainloop()

, _handlers()
, _preHandlers()
#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
, _unifyPreHandler(nullptr)
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

, _pyPacketCls(nullptr)
#if PYLLBC_CFG_PACKET_REUSE
, _pyReusePacket(nullptr)
, _pyPacketReuseMeth(nullptr)
#endif // PYLLBC_CFG_PACKET_REUSE
, _pyNullCObj(PyInt_FromLong(0))
, _pyPacketCreateArgs(PyTuple_New(6))

, _pyStream(nullptr)
, _nativeStream(nullptr)

, _decoders()
, _suppressedCoderNotFoundWarning(false)

, _beforeFrameCallables()
, _afterFrameCallables()

, _handlingBeforeFrameCallables(false)
, _handledBeforeFrameCallables(false)
, _handlingAfterFrameCallables(false)

, _started(false)
, _stoping(false)
{
    // Create llbc library Service object and set some service attributes.
    CreateLLBCService(name, _useNormalProtocolFactory);

    // Get event class.
    if (!This::_pyEvCls)
        This::_pyEvCls = pyllbc_s_TopModule->GetObject("Event"); // Borrowed

    // Get stream class.
    if (!This::_streamCls)
        This::_streamCls = pyllbc_s_TopModule->GetObject("Stream"); // Borrowed
}

pyllbc_Service::~pyllbc_Service()
{
    Stop();
    AfterStop();

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
}

int pyllbc_Service::GetId() const
{
    return _llbcSvc->GetId();
}

const LLBC_String &pyllbc_Service::GetName() const
{
    return _llbcSvc->GetName();
}

PyObject *pyllbc_Service::GetPyService() const
{
    return _pySvc;
}

int pyllbc_Service::GetFPS() const
{
    return _llbcSvc->GetFPS();
}

int pyllbc_Service::SetFPS(int fps)
{
    if (_llbcSvc->SetFPS(fps) != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_Service::GetFrameInterval() const
{
    return _llbcSvc->GetFrameInterval();
}

int pyllbc_Service::SuppressCoderNotFoundWarning()
{
    if (_llbcSvc->SuppressCoderNotFoundWarning() != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__, "when suppress service warning");
        return LLBC_FAILED;
    }

    _suppressedCoderNotFoundWarning = true;
    return LLBC_OK;
}

int pyllbc_Service::Start(int pollerCount)
{
    if (_started)
    {
        pyllbc_SetError("service already started", LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    // Force install error hooker
    const bool errHookerInstalled = _errHooker->IsInstalled();
    _errHooker->Install();

    _started = true;
    if (_llbcSvc->Start(pollerCount) != LLBC_OK)
    {
        _started = false;

        // Maybe is native library occurred, force transfer llbc error.
        pyllbc_TransferLLBCError(__FILE__, __LINE__, "when start native Service");
        // Transfer hooked error to python, and Restore error hooker install status.
        _errHooker->TransferHookedErrorToPython();
        if (!errHookerInstalled)
            _errHooker->Uninstall();

        return LLBC_FAILED;
    }

    _errHooker->Cleanup();
    if (!errHookerInstalled)
        _errHooker->Uninstall();

    return LLBC_OK;
}

bool pyllbc_Service::IsStarted() const
{
    return _started;
}

void pyllbc_Service::Stop()
{
    if (!IsStarted())
        return;

    if (_inMainloop)
    {
        _stoping = true;
    }
    else
    {
        AfterStop();
        _started = false;
    }
}

int pyllbc_Service::AddComponent(PyObject *comp)
{
    if (_started)
    {
        pyllbc_SetError("service already started", LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }
    
    auto cppComp = new pyllbc_Component(this, comp);
    return _llbcSvc->AddComponent(cppComp);
}

int pyllbc_Service::AddComponent(const LLBC_String &compName, const LLBC_String &libPath, PyObject *compCls, PyObject *&comp)
{
    // Force reset comp ptr.
    comp = nullptr;

    // Started check.
    if (_started)
    {
        pyllbc_SetError("service already started", LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }

    // Register native comp.
    LLBC_Component *nativeComp;
    int ret = _llbcSvc->AddComponent(libPath, compName, nativeComp);
    if (ret != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__, "When register comp(from dynamic library)");
        return LLBC_FAILED;
    }

    // Get python global env.
    PyObject *pyGbl = PyEval_GetGlobals(); // Borrowed reference.
    if (!pyGbl)
    {
        pyllbc_TransferPyError("When get python global env");
        return LLBC_FAILED;
    }

    // Get native comp native methods.
    typedef LLBC_ComponentMethods::Methods::const_iterator _NativeMethodsIter;
    const LLBC_ComponentMethods::Methods &nativeMeths = nativeComp->GetAllMethods().GetAllMethods();

    // If not specific python comp class, define python layer comp class and compile it.
    if (!compCls || pyllbc_TypeDetector::IsNone(compCls))
    {
        LLBC_String compClsDef;
        compClsDef.append_format("import llbc\n");
        compClsDef.append_format("class %s(llbc.inl.BaseLibComp):\n", compName.c_str());
        compClsDef.append_format("    \"\"\"Dynamic load comp %s(from native dynamic library:%s) define\"\"\"\n", compName.c_str(), libPath.c_str());
        compClsDef.append_format("    def __init__(self, cobj, name, meths):\n");
        compClsDef.append_format("        super(%s, self).__init__(cobj, name, meths)\n", compName.c_str());
        for (_NativeMethodsIter nativeMethIt = nativeMeths.begin();
             nativeMethIt != nativeMeths.end();
             ++nativeMethIt)
        {
            const char *nativeMeth = nativeMethIt->first.c_str();
            compClsDef.append_format("    def %s(self, arg):\n", nativeMeth);
            compClsDef.append_format("        return llbc.inl.CallComponentMethod(self._c_obj, '%s', arg)\n", nativeMeth);
        }

        compClsDef.append_format("\n");
        if (PyRun_SimpleString(compClsDef.c_str()) != 0)
        {
            pyllbc_TransferPyError("When compile comp class");
            return LLBC_FAILED;
        }

        // Get compiled python layer comp class.
        compCls = PyDict_GetItemString(pyGbl, compName.c_str()); // Borrowed reference for return.
        if (!compCls)
        {
            pyllbc_TransferPyError("When get python layer comp class(auto generated by internal routine)");
            return LLBC_FAILED;
        }
    }
    else
    {
        for (_NativeMethodsIter nativeMethIt = nativeMeths.begin();
             nativeMethIt != nativeMeths.end();
             ++nativeMethIt)
        {
            const char *nativeMeth = nativeMethIt->first.c_str();

            LLBC_String compMethDef;
            compMethDef.append_format("def %s(self, arg):\n", nativeMeth);
            compMethDef.append_format("    return llbc.inl.CallComponentMethod(self._c_obj, '%s', arg)\n", nativeMeth);

            if (PyRun_SimpleString(compMethDef.c_str()) != 0)
            {
                pyllbc_TransferPyError("When compile comp method");
                return LLBC_FAILED;
            }

            PyObject *pyCompMeth = PyDict_GetItemString(pyGbl, nativeMeth); // Borrow reference for return.
            if (!pyCompMeth)
            {
                pyllbc_TransferPyError("When get python layer comp meth(auto generated by internal routine)");
                return LLBC_FAILED;
            }

            if (PyObject_SetAttrString(compCls, nativeMeth, pyCompMeth) != 0) // not steal reference.
            {
                pyllbc_TransferPyError("When set auto generated comp meth to comp class");
                return LLBC_FAILED;
            }
        }
    }

    // Create python layer comp instance.
    PyObject *pyCObj = PyLong_FromLongLong(reinterpret_cast<long long>(nativeComp));
    PyObject *pyCompName = PyString_FromString(compName.c_str());
    PyObject *pyMeths = PySet_New(nullptr);
    for (_NativeMethodsIter nativeMethIt = nativeMeths.begin();
         nativeMethIt != nativeMeths.end();
         ++nativeMethIt)
        PySet_Add(pyMeths, PyString_FromString(nativeMethIt->first.c_str())); // Steal referencce for o.

    comp = PyObject_CallFunctionObjArgs(compCls,
                                        pyCObj,
                                        pyCompName,
                                        pyMeths,
                                        nullptr);
    Py_DECREF(pyCObj);
    Py_DECREF(pyCompName);
    Py_DECREF(pyMeths);
    if (!comp)
    {
        pyllbc_TransferPyError("When create python layer comp instance");
        return LLBC_FAILED;
    }

    // Hold python layer comp instances.
    Py_INCREF(comp);

    return LLBC_OK;
}

int pyllbc_Service::AddDecoder(int opcode, PyObject *decoder)
{
    if (!PyCallable_Check(decoder))
    {
        pyllbc_SetError("codec not callable");
        return LLBC_FAILED;
    }

    if (!_decoders.insert(std::make_pair(opcode, decoder)).second)
    {
        LLBC_String err;
        pyllbc_SetError(err.append_format(
            "repeat to register specify opcode's coder class, opcode: %d", opcode));

        return LLBC_FAILED;
    }

    Py_INCREF(decoder);

    return LLBC_OK;
}

int pyllbc_Service::Listen(const char *ip, uint16 port)
{
    int sessionId;
    if ((sessionId = _llbcSvc->Listen(ip, port)) == 0)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__);
        return 0;
    }

    return sessionId;
}

int pyllbc_Service::Connect(const char *ip, uint16 port)
{
    int sessionId;
    if ((sessionId = _llbcSvc->Connect(ip, port)) == 0)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__);
        return 0;
    }

    return sessionId;
}

int pyllbc_Service::AsyncConn(const char *ip, uint16 port)
{
    int sid = _llbcSvc->AsyncConn(ip, port);
    if (sid == 0)
        pyllbc_TransferLLBCError(__FILE__, __LINE__, LLBC_String().format("async connect to %s:%d failed", ip, port).c_str());

    return sid;
}

bool pyllbc_Service::IsSessionValidate(int sessionId)
{
    return _llbcSvc->IsSessionValidate(sessionId);
}

int pyllbc_Service::RemoveSession(int sessionId, const char *reason)
{
    if (_llbcSvc->RemoveSession(sessionId, reason) != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__, 
            LLBC_String().format("when remove session: %d", sessionId));
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_Service::Send(int sessionId, int opcode, PyObject *data, int status, sint64 extData1, sint64 extData2, sint64 extData3)
{
    // Started check.
    if (UNLIKELY(!IsStarted()))
    {
        pyllbc_SetError("service not start");
        return LLBC_FAILED;
    }

    // Serialize python layer 'data' object to stream.
    LLBC_Stream stream;
    const int ret = SerializePyObj2Stream(data, stream);
    if (UNLIKELY(ret != LLBC_OK))
        return LLBC_FAILED;

    // Build packet & send.
    LLBC_Packet *packet = new LLBC_Packet;
    packet->Write(stream.GetBuf(), stream.GetWritePos());

    packet->SetSessionId(sessionId);
    packet->SetOpcode(opcode);
    packet->SetStatus(status);
    packet->SetExtData1(extData1);
    packet->SetExtData2(extData2);
    packet->SetExtData3(extData3);

    if (UNLIKELY(_llbcSvc->Send(packet) == LLBC_FAILED))
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__, "When sending packet");
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_Service::Multicast(const LLBC_SessionIds &sessionIds, int opcode, PyObject *data, int status)
{
    // Started check.
    if (UNLIKELY(!IsStarted()))
    {
        pyllbc_SetError("service not start");
        return LLBC_FAILED;
    }

    // Session Ids array is empty, done.
    if (sessionIds.empty())
        return LLBC_OK;

    // Serialize python layer 'data' object to stream.
    LLBC_Stream stream;
    if (SerializePyObj2Stream(data, stream) != LLBC_OK)
        return LLBC_FAILED;

    // Send it.
    const void *bytes = stream.GetBuf();
    const size_t len = stream.GetWritePos();
    return _llbcSvc->Multicast(sessionIds, opcode, bytes, len, status);
}

int pyllbc_Service::Broadcast(int opcode, PyObject *data, int status)
{
    // Started check.
    if (UNLIKELY(!IsStarted()))
    {
        pyllbc_SetError("service not start");
        return LLBC_FAILED;
    }

    // Serialize python layer 'data' object to stream.
    LLBC_Stream stream;
    if (SerializePyObj2Stream(data, stream) != LLBC_OK)
        return LLBC_FAILED;

    // Send it.
    const void *bytes = stream.GetBuf();
    const size_t len = stream.GetWritePos();
    return _llbcSvc->Broadcast(opcode, bytes, len, status);
}

int pyllbc_Service::Subscribe(int opcode, PyObject *handler, int flags)
{
    auto it = _handlers.find(opcode);
    if (it != _handlers.end())
    {
        const LLBC_String handlerDesc = pyllbc_ObjUtil::GetObjStr(handler);

        LLBC_String err;
        err.append_format("repeat subscribe, subscribe opcode:%d, handler:%s, ", opcode, handlerDesc.c_str());
        err.append_format("the opcode already subscribed, handler:[%s]", it->second->ToString().c_str());

        pyllbc_SetError(err, LLBC_ERROR_REPEAT);

        return LLBC_FAILED;
    }

    pyllbc_PacketHandler *wrapHandler = new pyllbc_PacketHandler(opcode);
    if (wrapHandler->SetHandler(handler) != LLBC_OK)
    {
        delete wrapHandler;
        return LLBC_FAILED;
    }

    if (_llbcSvc->Subscribe(opcode, this, &pyllbc_Service::OnDataReceived) != LLBC_OK)
    {
        delete wrapHandler;
        pyllbc_TransferLLBCError(__FILE__, __LINE__, "call native Service::Subscribe() failed");

        return LLBC_FAILED;
    }

    _handlers.insert(std::make_pair(opcode, wrapHandler));

    return LLBC_OK;
}

int pyllbc_Service::PreSubscribe(int opcode, PyObject *preHandler, int flags)
{
    auto it = _preHandlers.find(opcode);
    if (it != _preHandlers.end())
    {
        const LLBC_String handlerDesc = pyllbc_ObjUtil::GetObjStr(preHandler);

        LLBC_String err;
        err.append_format("repeat pre-subscribe, pre-subscribe opcode:%d, handler:%s, ", opcode, handlerDesc.c_str());
        err.append_format("the opcode already pre-subscribed, handler:[%s]", it->second->ToString().c_str());

        pyllbc_SetError(err, LLBC_ERROR_REPEAT);

        return LLBC_FAILED;
    }



    pyllbc_PacketHandler *wrapHandler = new pyllbc_PacketHandler(opcode);
    if (wrapHandler->SetHandler(preHandler) != LLBC_OK)
    {
        delete wrapHandler;
        return LLBC_FAILED;
    }

    if (!_preHandlers.insert(std::make_pair(opcode, wrapHandler)).second)
    {
        delete wrapHandler;

        LLBC_String err;
        pyllbc_SetError(err.format(
            "repeat to pre-subscribe opcode: %d, the opcode already pre-subscribed", opcode), LLBC_ERROR_REPEAT);

        return LLBC_FAILED;
    }

    if (_llbcSvc->PreSubscribe(opcode, this, &pyllbc_Service::OnDataPreReceived) != LLBC_OK)
    {
        _preHandlers.erase(opcode);
        delete wrapHandler;
        pyllbc_TransferLLBCError(__FILE__, __LINE__, "call native Service::PreSubscribe() failed");

        return LLBC_FAILED;
    }

    return LLBC_OK;
}

#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
int pyllbc_Service::UnifyPreSubscribe(PyObject *preHandler, int flags)
{
    pyllbc_PacketHandler *wrapHandler = new pyllbc_PacketHandler(0);
    if (wrapHandler->SetHandler(preHandler) != LLBC_OK)
    {
        delete wrapHandler;
        return LLBC_FAILED;
    }

    if (_unifyPreHandler)
    {
        delete wrapHandler;
        pyllbc_SetError("repeat to unify pre-subscribe packet");

        return LLBC_FAILED;
    }

    _unifyPreHandler = wrapHandler;
    if (_llbcSvc->UnifyPreSubscribe(this, &pyllbc_Service::OnDataUnifyPreReceived) != LLBC_OK)
    {
        LLBC_XDelete(_unifyPreHandler);
        pyllbc_TransferLLBCError(__FILE__, __LINE__, "call native Service::UnifyPreSubscribe() failed");

        return LLBC_FAILED;
    }

    return LLBC_OK;
}
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

LLBC_ListenerStub pyllbc_Service::SubscribeEvent(int event, PyObject *listener)
{
    // Create pyllbc layer event delegate.
    pyllbc_EventListener *evListener = new pyllbc_EventListener;
    if (evListener->SetPyListener(listener) != LLBC_OK)
        return 0;

    return _llbcSvc->SubscribeEvent(event, evListener);
}

void pyllbc_Service::UnsubscribeEvent(int event)
{
    _llbcSvc->UnsubscribeEvent(event);
}

void pyllbc_Service::UnsubscribeEvent(LLBC_ListenerStub stub)
{
    _llbcSvc->UnsubscribeEvent(stub);
}

int pyllbc_Service::FireEvent(PyObject *ev)
{
    if (!PyObject_IsInstance(ev, _pyEvCls))
    {
        pyllbc_SetError("event object is not instance of <Event> class");
        return LLBC_FAILED;
    }

    PyObject *nativeEvObj = PyObject_GetAttrString(ev, "cobj"); // New reference.
    if (!nativeEvObj)
    {
        pyllbc_TransferPyError("When get native event object");
        return LLBC_FAILED;
    }

    LLBC_Event *nativeEv = reinterpret_cast<LLBC_Event *>(PyLong_AsUnsignedLongLong(nativeEvObj));
    Py_DECREF(nativeEvObj);

    _llbcSvc->FireEvent(nativeEv, _evEnqueueHandler, _evDequeueHandler);

    return PyErr_Occurred() ? LLBC_FAILED : LLBC_OK;
}

int pyllbc_Service::Post(PyObject *callable)
{
    if (!PyCallable_Check(callable))
    {
        const LLBC_String objDesc = pyllbc_ObjUtil::GetObjStr(callable);
        pyllbc_SetError(LLBC_String().format("frame callable object not callable: %s", objDesc.c_str()));

        return LLBC_FAILED;
    }

    if (_handlingBeforeFrameCallables &&
        _handlingAfterFrameCallables)
    {
        pyllbc_SetError("could not push callable object to service, internal error!");
        return LLBC_FAILED;
    }

    if (_beforeFrameCallables.find(callable) != _beforeFrameCallables.end() ||
        _afterFrameCallables.find(callable) != _afterFrameCallables.end())
    {
        const LLBC_String objDesc = pyllbc_ObjUtil::GetObjStr(callable);
        pyllbc_SetError(LLBC_String().format(
            "repeat to add callable to service, callable: %s", objDesc.c_str()));

        return LLBC_FAILED;
    }

    Py_INCREF(callable);
    if (_handlingBeforeFrameCallables)
    {
        _afterFrameCallables.insert(callable);
    }
    else
    {
        if (!_handledBeforeFrameCallables)
        {
            _beforeFrameCallables.insert(callable);
        }
        else
        {
            if (!_handlingAfterFrameCallables)
                _afterFrameCallables.insert(callable);
            else
                _beforeFrameCallables.insert(callable);
        }
    }

    return LLBC_OK;
}

bool pyllbc_Service::MainLoop()
{
    if (UNLIKELY(!_started))
        return false;

    _inMainloop = true;
    LLBC_InvokeGuard guard(&ResetMainloopFlag, &_inMainloop);

    if (!_stoping)
    {
        HandleFrameCallables(_beforeFrameCallables, _handlingBeforeFrameCallables);
        _handledBeforeFrameCallables = true;
    }

    if (!_stoping)
        _llbcSvc->OnSvc(false);

    if (!_stoping)
    {
        HandleFrameCallables(_afterFrameCallables, _handlingAfterFrameCallables);
    }

    _handledBeforeFrameCallables = false;

    if (_stoping)
    {
        AfterStop();

        _stoping = false;
        _started = false;

        return false;
    }

    // Transfer hooked error to python.
    This::_errHooker->TransferHookedErrorToPython();

    return true;
}

pyllbc_ErrorHooker *pyllbc_Service::GetErrHooker()
{
    return This::_errHooker;
}

void pyllbc_Service::OnDataReceived(LLBC_Packet &packet)
{
    // Stopping check.
    if (UNLIKELY(_stoping))
        return;

    // Find handler.
    std::map<int, pyllbc_PacketHandler *> &handlers = _handlers;
    auto handlerIt = handlers.find(packet.GetOpcode());
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
    PyObject_SetAttrString(pyPacket, "cobj", _pyNullCObj);

    // Delete packet.
    if (!packet.GetPreHandleResult())
        Py_DECREF(pyPacket);
}

bool pyllbc_Service::OnDataPreReceived(LLBC_Packet &packet)
{
    if (UNLIKELY(_stoping))
        return true;

    std::map<int, pyllbc_PacketHandler *> &handlers = _preHandlers;
    auto handlerIt = handlers.find(packet.GetOpcode());
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
bool pyllbc_Service::OnDataUnifyPreReceived(LLBC_Packet &packet)
{
    if (UNLIKELY(_stoping))
        return true;

    PyObject *pyPacket = BuildPyPacket(packet);
    if (UNLIKELY(!pyPacket))
        return false;

    packet.SetPreHandleResult(pyPacket, this, &This::DeletePyPacket);

    PyObject *ret = _unifyPreHandler->Handle(pyPacket);
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

PyObject *pyllbc_Service::BuildPyPacket(const LLBC_Packet &packet)
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
        _pyStream = PyObject_CallObject(_streamCls, tupleArg);
        Py_DECREF(tupleArg);

        // Get pyllbc_Stream object.
        PyObject *cobj = PyObject_GetAttrString(_pyStream, "cobj");
        if (UNLIKELY(!cobj))
        {
            Py_DECREF(_pyStream);
            _pyStream = nullptr; // reset _pyStream
            pyllbc_SetError("could not get llbc.Stream property 'cobj', recv data failed");

            return nullptr;
        }

        PyArg_Parse(cobj, "l", &_nativeStream);
        Py_DECREF(cobj);
    }

    _nativeStream->GetLLBCStream().Attach(
        const_cast<void *>(packet.GetPayload()), packet.GetPayloadLength());

    PyObject *pyData = nullptr;
    const auto &decoders = _decoders;
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

    PyObject *pySessionId = PyInt_FromLong(packet.GetSessionId());

    PyObject *pyOpcode = PyInt_FromLong(packet.GetOpcode());
    PyObject *pyStatus = PyInt_FromLong(packet.GetStatus());

    PyObject *pyPacketCObj = PyLong_FromUnsignedLongLong(reinterpret_cast<uint64>(&packet));

    PyTuple_SetItem(_pyPacketCreateArgs, 1, pySessionId);
    PyTuple_SetItem(_pyPacketCreateArgs, 2, pyOpcode);
    PyTuple_SetItem(_pyPacketCreateArgs, 3, pyStatus);
    PyTuple_SetItem(_pyPacketCreateArgs, 4, pyData);
    PyTuple_SetItem(_pyPacketCreateArgs, 5, pyPacketCObj);
#if PYLLBC_CFG_PACKET_REUSE
    PyObject *reuseRet = PyObject_CallObject(_pyPacketReuseMeth, _pyPacketCreateArgs);

    Py_IncRef(Py_None);
    PyTuple_SetItem(_pyPacketCreateArgs, 4, Py_None); // Only clear pyData item(index 4).

    if (UNLIKELY(!reuseRet))
    {
        pyllbc_TransferPyError();
        return nullptr;
    }

    Py_DecRef(reuseRet);
    Py_IncRef(_pyReusePacket);

    return _pyReusePacket;
#else    
	Py_INCREF(_pySvc);
    PyTuple_SetItem(_pyPacketCreateArgs, 0, _pySvc);
    PyObject *pyPacket = PyObject_CallObject(_pyPacketCls, _pyPacketCreateArgs);

    Py_IncRef(Py_None);
    PyTuple_SetItem(_pyPacketCreateArgs, 0, Py_None); // Only clear pySvc item(index 0).

    Py_IncRef(Py_None);
    PyTuple_SetItem(_pyPacketCreateArgs, 4, Py_None); // Only clear pyData item(index 4).

    if (UNLIKELY(!pyPacket))
    {
        pyllbc_TransferPyError();
        return nullptr;
    }

    return pyPacket;
#endif // PYLLBC_CFG_PACKET_REUSE
}

void pyllbc_Service::DeletePyPacket(void *_)
{
    PyObject *pyPacket = reinterpret_cast<PyObject *>(_);
    Py_DECREF(pyPacket);
}

void pyllbc_Service::CreateLLBCService(const LLBC_String &svcName, bool useNormalProtocolFactory)
{
    ASSERT(!_llbcSvc && "llbc service pointer not nullptr");

    LLBC_IProtocolFactory *protoFactory = nullptr;
    if (useNormalProtocolFactory)
        protoFactory = new LLBC_NormalProtocolFactory;
    else
        protoFactory = new LLBC_RawProtocolFactory;

    _llbcSvc = LLBC_Service::Create(svcName, protoFactory, false);
    _llbcSvc->SetDriveMode(LLBC_ServiceDriveMode::ExternalDrive);
    _llbcSvc->DisableTimerScheduler();
    _llbcSvc->SuppressCoderNotFoundWarning();
}

void pyllbc_Service::AfterStop()
{
    // Recreate service.
    LLBC_XDelete(_llbcSvc);
    CreateLLBCService(_llbcSvcName, _useNormalProtocolFactory);

    // Cleanup all python layer codecs.
    LLBC_Foreach(_decoders, Py_DECREF(item.second));
    _decoders.clear();

    // Cleanup all python layer handlers/prehandlers/unify_prehandlers(if enabled).
    LLBC_STLHelper::DeleteContainer(_handlers);
    LLBC_STLHelper::DeleteContainer(_preHandlers);
#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    LLBC_XDelete(_unifyPreHandler);
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

    // Destroy all frame callables.
    _handledBeforeFrameCallables = false;
    DestroyFrameCallables(_beforeFrameCallables, _handlingBeforeFrameCallables);
    DestroyFrameCallables(_afterFrameCallables, _handlingAfterFrameCallables);
}

void pyllbc_Service::HandleFrameCallables(pyllbc_Service::_FrameCallables &callables, bool &usingFlag)
{
    usingFlag = true;
    for (_FrameCallables::iterator it = callables.begin();
         it != callables.end();
         it++)
    {
        PyObject *callable = *it;
        PyObject *ret = PyObject_CallFunctionObjArgs(callable, _pySvc, nullptr);
        if (ret)
        {
            Py_DECREF(ret);
        }
        else
        {
            const LLBC_String objDesc = pyllbc_ObjUtil::GetObjStr(callable);
            pyllbc_TransferPyError(LLBC_String().format("When call frame-callable: %s", objDesc.c_str()));
            break;
        }
    }

    DestroyFrameCallables(callables, usingFlag);
}

void pyllbc_Service::DestroyFrameCallables(_FrameCallables &callables, bool &usingFlag)
{
    for (_FrameCallables::iterator it = callables.begin();
         it != callables.end();
         it++)
        Py_DECREF(*it);

    callables.clear();
    usingFlag = false;
}

int pyllbc_Service::SerializePyObj2Stream(PyObject *pyObj, LLBC_Stream &stream)
{
    // Create python layer Stream instance.
    PyObject *arg = PyTuple_New(2);
    PyTuple_SetItem(arg, 0, PyInt_FromLong(0)); // stream init size = 0.
        
    Py_INCREF(pyObj);
    PyTuple_SetItem(arg, 1, pyObj); // initWrite = pyObj(steal reference).

    PyObject *pyStreamObj = PyObject_CallObject(This::_streamCls, arg);
    if (UNLIKELY(!pyStreamObj))
    {
        Py_DECREF(arg);
        pyllbc_TransferPyError();

        return LLBC_FAILED;
    }

    // Get cobj property.
    PyObject *cobj = PyObject_GetAttrString(pyStreamObj, "cobj");
    if (UNLIKELY(!cobj))
    {
        Py_DECREF(pyStreamObj);
        Py_DECREF(arg);

        pyllbc_SetError("could not get llbc.Stream property 'cobj'");

        return LLBC_FAILED;
    }

    // Convert to pyllbc_Stream *.
    pyllbc_Stream *cstream = nullptr;
    PyArg_Parse(cobj, "l", &cstream);

    // Let stream attach to inlStream.
    LLBC_Stream &inlStream = cstream->GetLLBCStream();

    stream.Attach(inlStream);
    (void)inlStream.Detach();
    stream.SetAttach(false);

    Py_DECREF(cobj);
    Py_DECREF(pyStreamObj);
    Py_DECREF(arg);

    return LLBC_OK;
}

#if PYLLBC_CFG_PACKET_REUSE
PyObject *pyllbc_Service::CreateReusePyPacket()
{
    PyObject *pyData = Py_None;
    Py_IncRef(pyData);

    PyObject *pySessionId = PyInt_FromLong(0);
    PyObject *pyOpcode = PyInt_FromLong(0);
    PyObject *pyStatus = PyInt_FromLong(0);
    PyObject *pyPacketCObj = PyInt_FromLong(0);

    Py_INCREF(_pySvc);
    PyTuple_SetItem(_pyPacketCreateArgs, 0, _pySvc);
    PyTuple_SetItem(_pyPacketCreateArgs, 1, pySessionId);
    PyTuple_SetItem(_pyPacketCreateArgs, 2, pyOpcode);
    PyTuple_SetItem(_pyPacketCreateArgs, 3, pyStatus);
    PyTuple_SetItem(_pyPacketCreateArgs, 4, pyData);
    PyTuple_SetItem(_pyPacketCreateArgs, 5, pyPacketCObj);

    auto ret = PyObject_CallObject(_pyPacketCls, _pyPacketCreateArgs);

    Py_INCREF(Py_None);
    PyTuple_SetItem(_pyPacketCreateArgs, 0, Py_None);

    return ret;
}
#endif // PYLLBC_CFG_PACKET_REUSE

void pyllbc_Service::EventEnqueueHandler(LLBC_Event *ev)
{
    PyObject *pyEv = reinterpret_cast<PyObject *>(ev->GetExtData());
    Py_INCREF(pyEv);
}

void pyllbc_Service::EventDequeueHandler(LLBC_Event *ev)
{
    PyObject *pyEv = reinterpret_cast<PyObject *>(ev->GetExtData());
    Py_DECREF(pyEv);
}

