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
     * @param[in] svc - adapted to python c++ wrapped service.
     */
    pyllbc_Component(pyllbc_Service *svc);

    /**
     * Destructor.
     */
    virtual ~pyllbc_Component();

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

public:
    /**
     * When new session create, will call this event handler.
     * @param[in] sessionInfo - the session info.
     */
    virtual void OnSessionCreate(const LLBC_SessionInfo &sessionInfo);

    /**
     * When session destroy, will call this event handler.
     * @param[in] destroyInfo - the session destroy information.
     */
    virtual void OnSessionDestroy(const LLBC_SessionDestroyInfo &destroyInfo);

    /**
     * When asynchronous connect result, will call this event handler.
     * @param[in] result - the async-conn result info.
     */
    virtual void OnAsyncConnResult(const LLBC_AsyncConnResult &result);

    /**
     * When protocol stack reported, will call this event handler.
     * @param[in] report - the protocol report info.
     */
    virtual void OnProtoReport(const LLBC_ProtoReport &report);

    /**
     * When service receive a unhandled packet, will call this event handler.
     * @param[in] opcode - the opcode.
     */
    virtual void OnUnHandledPacket(const LLBC_Packet &packet);

public:
    /**
     * data receive handler.
     * @param[in] packet - packet.
     */
    virtual void OnDataReceived(LLBC_Packet &packet);

    /**
     * The data pre-receive handler.
     * @param[in] packet - packet.
     * @return bool - the return value, if return false, will remove this session.
     */
    virtual bool OnDataPreReceived(LLBC_Packet &packet);

#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    /**
     * The data unify pre-receive handler.
     * @param[in] packet - packet.
     * @return bool - the return value, if return zero, will remove this session.
     */
    virtual bool OnDataUnifyPreReceived(LLBC_Packet &packet);
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

    /**
     * Build python layer packet object according to LLBC_Packet object reference.
     * @param[in] packet - the core library packet object reference.
     * @return PyObject * - the python layer packet object, if error occurred, return nullptr.
     */
    PyObject *BuildPyPacket(const LLBC_Packet &packet);

    /**
     * Delete the python layer packet.
     */
    void DeletePyPacket(void *pyPacket);

private:
    /**
     * Call python layer comp method.
     * @param[in] meth     - method name, not steal reference, normal.
     * @param[in] ev       - call event object, steal reference.
     * @param[in] decRefEv - decref event.
     * @return bool - return true if call success(no error occurred), otherwise error occurred(error info has been correctly).
     */
    bool CallComponentMeth(PyObject *meth, PyObject *ev, bool decRefEv);

    #if PYLLBC_CFG_PACKET_REUSE
    /**
     * Create reuse python layer packet object.
     * @return PyObject * - the python layer reuse packet(new reference).
     */
    PyObject *CreateReusePyPacket();
    #endif // PYLLBC_CFG_PACKET_REUSE

private:
    pyllbc_Service *_svc;
    PyObject *_pySvc;

    PyObject *_methOnInit;
    PyObject *_methOnDestroy;
    PyObject *_methOnStart;
    PyObject *_methOnStop;
    PyObject *_methOnUpdate;
    PyObject *_methOnIdle;
    PyObject *_methOnSessionCreate;
    PyObject *_methOnSessionDestroy;
    PyObject *_methOnAsyncConnResult;
    PyObject *_methOnProtoReport;
    PyObject *_methOnUnHandledPacket;

    PyObject *_keySVC;
    PyObject *_keyIp;
    PyObject *_keyPort;
    PyObject *_keySessionId;
    PyObject *_keyOpcode;
    PyObject *_keyData;
    PyObject *_keyStatus;
    PyObject *_keyReason;
    PyObject *_keyConnected;
    PyObject *_keyIdleTime;
    PyObject *_keyInlIdleTime;
    PyObject *_keyCObj;

    PyObject *_pyPacketCls;
    #if PYLLBC_CFG_PACKET_REUSE
    PyObject *_pyReusePacket;
    PyObject *_pyPacketReuseMeth;
    #endif // PYLLBC_CFG_PACKET_REUSE
    PyObject *_pyNullCObj;
    PyObject *_pyPacketCreateArgs;

    PyObject *_pyStream;
    pyllbc_Stream *_nativeStream;

    PyObject *_holdedOnIdleEv;
    PyObject *_holdedOnUpdateEv;

    PyObject *_compEvCallArgs;
};
