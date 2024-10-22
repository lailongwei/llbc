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

class LLBC_HIDDEN pyllbc_ComponentEvBuilder
{
public:
    /**
     * Build Initialize event object.
     * @param[in] svc - service object, not steals reference, normal.
     * @return PyObject * - ServiceEvent object, new reference.
     */
    static PyObject *BuildInitializeEv(PyObject *svc);

    /**
     * Build Destroy event object.
     * @param[in] svc - service object, not steals reference, normal.
     * @return PyObject * - ServiceEvent object, new reference.
     */
    static PyObject *BuildDestroyEv(PyObject *svc);

    /**
     * Build Start event object.
     * @param[in] svc - service object, not steals reference, normal.
     * @return PyObject * - ServiceEvent object, new reference.
     */
    static PyObject *BuildStartEv(PyObject *svc);

    /**
     * Build Stop event object.
     * @param[in] svc - service object, not steals reference, normal.
     * @return PyObject * - ServiceEvent object, new reference.
     */
    static PyObject *BuildStopEv(PyObject *svc);

    /**
     * Build Update event object.
     * @param[in] svc - service object, not steals reference, normal.
     * @return PyObject * - ServiceEvent object, new reference.
     */
    static PyObject *BuildUpdateEv(PyObject *svc);

    /**
     * Build Idle event object.
     * @param[in] svc      - service object, not steals reference, normal.
     * @param[in] idleTime - the idle time, in milli-seconds.
     * return PyObject * - ServiceEvent object, new reference.
     */
    static PyObject *BuildIdleEv(PyObject *svc, int idleTime);

    /**
     * Build SessionCreate event object.
     * @param[in] svc - service object, not steals reference, normal.
     * @param[in] si  - the session info.
     * @return PyObject * - ServiceEvent object, new reference.
     */
    static PyObject *BuildSessionCreateEv(PyObject *svc, const LLBC_SessionInfo &si);

    /**
     * Build SessionDestroy event object.
     * @param[in] svc         - service object, not steals reference, normal.
     * @param[in] destroyInfo - the session destroy info.
     * @return PyObject * - ServiceEvent object, new reference.
     */
    static PyObject *BuildSessionDestroyEv(PyObject *svc, const LLBC_SessionDestroyInfo &destroyInfo);

    /**
     * Build AsyncConnResult event object.
     * @param[in] svc    - service object, not steals reference, normal.
     * @param[in] result - the async-conn result info. 
     * @return PyObject * - the ServiceEvent object, new reference.
     */
    static PyObject *BuildAsyncConnResultEv(PyObject *svc, const LLBC_AsyncConnResult &result);

    /**
     * Build protocol-report event object.
     * @param[in] svc    - the service object, not steals reference, normal.
     * @param[in] report - the protocol-report info.
     * @return PyObject * - the ServiceEvent object, new reference.
     */
    static PyObject *BuildProtoReportEv(PyObject *svc, const LLBC_ProtoReport &report);

    /**
     * Build unhandled packet event object.
     * @param[in] svc        - the service object, not steals reference, normal.
     * @param[in] llbcPacket - the llbc core library packet object.
     * @param[in] packet     - the packet object, not steals reference, normal.
     * @return PyObject *    - the ServiceEvent object, new reference.
     */
    static PyObject *BuildUnHandledPacketEv(PyObject *svc, const LLBC_Packet &llbcPacket, PyObject *packet);

private:
    static PyObject *CreateEv(PyObject *svc);
    
    static void SetAttr(PyObject *ev, PyObject *attr, bool val);
    static void SetAttr(PyObject *ev, PyObject *attr, int val);
    static void SetAttr(PyObject *ev, PyObject *attr, sint64 val);
    static void SetAttr(PyObject *ev, PyObject *attr, double val);
    static void SetAttr(PyObject *ev, PyObject *attr, const LLBC_String &val);
    static void SetAttr(PyObject *ev, PyObject *attr, PyObject *val);

private:
    static PyObject *_evCls;

    static PyObject *_attrSvc;
    static PyObject *_attrSessionId;
    static PyObject *_attrAcceptSessionId;
    static PyObject *_attrIp;
    static PyObject *_attrPort;
    static PyObject *_attrIdleTime;
    static PyObject *_attrConnected;
    static PyObject *_attrReason;
    static PyObject *_attrLocalIp;
    static PyObject *_attrLocalPort;
    static PyObject *_attrPeerIp;
    static PyObject *_attrPeerPort;
    static PyObject *_attrIsListen;
    static PyObject *_attrSocket;
    static PyObject *_attrPacket;
    static PyObject *_attrReportLayer;
    static PyObject *_attrReportLevel;
    static PyObject *_attrReportMsg;
    static PyObject *_attrOpcode;
    static PyObject *_attrDestroyedFromSvc;
    static PyObject *_attrErrNo;
    static PyObject *_attrSubErrNo;
};
