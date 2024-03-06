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

#pragma once

#include "pyllbc/common/Common.h"
#include "pyllbc/core/Core.h"

class pyllbc_Service;

/**
 * \brief The pyllbc comp class encapsulation.
 */
class LLBC_HIDDEN pyllbc_Component : public LLBC_Component
{
public:
    /**
     * Constructor.
     * @param[in] svc    - adapted to python c++ wrapped service.
     * @param[in] pyComp - adapted to python component.
     */
    pyllbc_Component(pyllbc_Service *svc, PyObject *pyComp);

    /**
     * Destructor.
     */
    virtual ~pyllbc_Component();

public:
    /**
     * Get python comp.
     * @return PyObject * - python component.
     */
    const PyObject *GetPyComp() const { return _pyComp; }
public:
    /**
     * When service start and not not init comp before, will call then event handler function.
     */
    virtual bool OnInit(bool &initFinished);

    /**
     * When service destroy, will call this event handler function.
     */
    virtual void OnDestroy(bool &destroyFinished);

    /**
     * When service start, will call this event handler function.
     */
    virtual bool OnStart(bool &startFinished);

    /**
     * When service stop, will call this event handler function.
     */
    virtual void OnStop(bool &stopFinished);

public:
    /**
     * Heartbeat function.
     */
    virtual void OnUpdate();

    /**
     * Idle event handler.
     * @param[in] idleTime - idle time.
     */
    virtual void OnIdle(const LLBC_TimeSpan &idleTime);

    /**
     * Process components' events.
     * @param[in] evIndex
     * @param[in] evArgs
     */
    virtual void OnEvent(LLBC_ComponentEvents::ENUM event, const LLBC_Variant &evArgs);

private:
    /**
     * When new session create, will call this event handler.
     * @param[in] sessionInfo - the session info.
     */
    void OnSessionCreate(const LLBC_SessionInfo &sessionInfo);

    /**
     * When session destroy, will call this event handler.
     * @param[in] destroyInfo - the session destroy information.
     */
    void OnSessionDestroy(const LLBC_SessionDestroyInfo &destroyInfo);

    /**
     * When asynchronous connect result, will call this event handler.
     * @param[in] result - the async-conn result info.
     */
    void OnAsyncConnResult(const LLBC_AsyncConnResult &result);

    /**
     * When protocol stack reported, will call this event handler.
     * @param[in] report - the protocol report info.
     */
    void OnProtoReport(const LLBC_ProtoReport &report);

    /**
     * When service receive a unhandled packet, will call this event handler.
     * @param[in] opcode - the opcode.
     */
    void OnUnHandledPacket(const LLBC_Packet &packet);

private:
    /**
     * Call python layer comp method.
     * @param[in] meth          - method name, not steal reference, normal.
     * @param[in] ev            - call event object, steal reference.
     * @param[in] decRefEv      - decref event.
     * @param[in] isRetRequired - Whether a return value is required.
     * @return PyObject *       - return python return value if isRetRequired is True and call success(no error occurred),
     *                            otherwise return nullptr if error occurred(error info has been correctly processed).
     */
    PyObject *CallComponentMeth(PyObject *meth, PyObject *ev, bool decRefEv, bool isRetRequired);
    /**
     * Parse finished param from pyRet.
     * @param[in] pyRet     - python method return object, steal reference.
     * @param[out] finished - parsed param.
     * @return bool         - return value.
     */
    static bool ParsePythonRet(PyObject *pyRet, bool &finished);

private:
    pyllbc_Service *_svc;
    PyObject *_pySvc;
    PyObject *_pyComp;

    PyObject *_pyOnInitMeth;
    PyObject *_pyOnDestroyMeth;
    PyObject *_pyOnStartMeth;
    PyObject *_pyOnStopMeth;
    PyObject *_pyOnUpdateMeth;
    PyObject *_pyOnIdleMeth;
    PyObject *_pyOnSessionCreateMeth;
    PyObject *_pyOnSessionDestroyMeth;
    PyObject *_pyOnAsyncConnResultMeth;
    PyObject *_pyOnProtoReportMeth;
    PyObject *_pyOnUnHandledPacketMeth;

    PyObject *_holdedOnIdleEv;
    PyObject *_holdedOnUpdateEv;

    PyObject *_compEvCallArgs;
};
