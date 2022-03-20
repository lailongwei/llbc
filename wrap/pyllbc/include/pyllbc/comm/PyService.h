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

#ifndef __PYLLBC_COMM_PY_SERVICE_H__
#define __PYLLBC_COMM_PY_SERVICE_H__

#include "pyllbc/common/Common.h"
#include "pyllbc/core/Core.h"

class pyllbc_Component;
class pyllbc_PacketHandler;
class pyllbc_ErrorHooker;
class pyllbc_FrameCallable;

/**
 * \brief The service class encapsulation.
 */
class LLBC_HIDDEN pyllbc_Service
{
public:
    /**
     * The codec policy enumeration.
     */
    enum Codec
    {
        JsonCodec,
        BinaryCodec 
    };

public:
    /**
     * Parameter constructor.
     * @param[in] type  - the service type, see enumeration: SvcType.
     * @param[in] name  - the service name.
     * @param[in] pySvc - the python layer service instance(borrow instance).
     */
    pyllbc_Service(LLBC_IService::Type type, const LLBC_String &name, PyObject *pySvc);
    /**
     * Destructor.
     */
    virtual ~pyllbc_Service();

public:
    /**
     * Get the service Id.
     * @return int - the service Id.
     */
    int GetId() const;

    /**
     * Get the python service type.
     * @return SvcType - the service type enumeration.
     */
    LLBC_IService::Type GetType() const;

    /**
     * Get python service.
     * @return PyObject * - the python service(borrow reference).
     */
    PyObject *GetPyService() const;

    /**
     * Get service FPS.
     * @return int - the service FPS.
     */
    int GetFPS() const;

    /**
     * Set service FPS.
     * @param[in] fps - service fps.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetFPS(int fps);

    /**
     * Get service frame interval.
     * @return int - the service frame interval, in milli-seconds.
     */
    int GetFrameInterval() const;

    #if LLBC_CFG_COMM_ENABLE_SERVICE_FRAME_TIMEOUT
    /**
     * Get service frame time out.
     * @return LLBC_TimeSpan - return the service frame-timeout.
     */
    LLBC_TimeSpan GetFrameTimeout() const;

    /**
     * Set service frame time out.
     * @param[in] frameTimeout - the service frame-timeout.
     */
    void SetFrameTimeout(const LLBC_TimeSpan &frameTimeout);
    #endif // LLBC_CFG_COMM_ENABLE_SERVICE_FRAME_TIMEOUT

public:
    /**
     * Suppress coder not found warning in protocol-stack.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SuppressCoderNotFoundWarning();

public:
    /**
     * Get service codec strategy(default is Json).
     * @return  Codec - the codec.
     */
    Codec GetCodec() const;

    /**
     * Set service codec strategy(default is Json).
     * @param[in] codec - the codec.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetCodec(Codec codec);

public:
    /**
     * Startup the service as client.
     * @param[in] pollerCount - the poller count.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Start(int pollerCount = 1);

    /**
     * Check service start or not.
     * @return bool - return true if service running, otherwise return false.
     */
    bool IsStarted() const;

    /**
     * Stop service.
     */
    void Stop();

public:
    /**
     * Register comp.
     * @param[in] comp - the comp instance(normal, not steal reference).
     *      comp methods(all methods are optional):
     *          oninitialize(self, ev): service initialize handler.
     *              ev.svc: service object.
     *          ondestroy(self, ev): service destroy handler.
     *              ev.svc: service object.
     *          onupdate(self, ev): service per-frame update handler.
     *              ev.svc: service object.
     *          onidle(self, ev): service per-frame idle handler.
     *              ev.svc: service object.
     *              ev.idletime: idle time, float type, in seconds.
     *          onsessioncreate(self, ev): session create handler.
     *              ev.svc: service object.
     *              ev.islisten: is listen session or not.
     *              ev.session_id: session Id.
     *              ev.local_ip: local ip address.
     *              ev.local_port: local port number.
     *              ev.peer_ip: peer ip address.
     *              ev.peer_port: peer port number.
     *          onsessiondestroy(self, ev): session destroy handler.
     *              ev.svc: service object.
     *              ev.session_id: session Id.
     *              ev.reason: destroy reason.
     *              ev.destroyed_from_service: destroyed from service flag.
     *              ev.local_ip: local ip address.
     *              ev.local_port: local port number.
     *              ev.peer_ip: peer ip address.
     *              ev.peer_port: peer port number.
     *              ev.socket: socket file descripter.
     *              ev.errno: error number(only available when ev.destroyed_from_service is True).
     *              ev.sub_errno: sub error number(only available when ev.destroyed_from_service is True).
     *          onasyncconnresult(self, ev): async-connect result handler.
     *              ev.svc: service object.
     *              ev.peer_ip: peer ip address.
     *              ev.peer_port: peer port number.
     *              ev.connected: connected flag.
     *              ev.reason: reason describe.
     *          onprotoreport(self, ev): protocol report.
     *              ev.svc: service object.
     *              ev.report_layer: which layer protocol reported.
     *              ev.report_level: report event level(DEBUG, INFO, WARN, ERROR).
     *              ev.report_msg: report message.
     *              ev.session_id: report session_id(optional, maybe is 0).
     *          onunhandledpacket(self, ev): unhandled packet.
     *              ev.svc: service object.
     *              ev.opcode: packet opcode.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RegisterComponent(PyObject *comp);

    /**
     * Register comp from library.
     * @param[in] compName - the comp name.
     * @param[in] libPath  - the library path.
     * @param[in] compCls  - the python layer comp class, can be nullptr.
     * @param[out] comp    - the created comp(new reference).
     * @return int - return 0 if success, otherwise return -1.
     */
    int RegisterComponent(const LLBC_String &compName, const LLBC_String &libPath, PyObject *compCls, PyObject *&comp);

    /**
     * Register codec(only available in CODEC_BINARY).
     * @param[in] opcode - the opcode.
     * @param[in] codec  - the codec class(not steal reference, normal).
     *                     constraints:
     *                      raw types(None, byte, bool, int, long, float, bytearray, buffer, str, unicode) or exist flowwow methods's cls:
     *                          encode(self, stream): Encode data to stream.
     *                          decode(self, stream): Decode data from stream.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RegisterCodec(int opcode, PyObject *codec);

public:
    /**
     * Listen in specified socket address.
     * @param[in] ip   - the ip address.
     * @param[in] port - the port number.
     * @return int - the session id, return 0 means error occurred.
     */
    int Listen(const char *ip, uint16 port);

    /**
     * Connect to peer(synchronous).
     * @param[in] ip   - the ip address.
     * @param[in] port - the port number.
     * @return int - the session id, return 0 means error occurred.
     */
    int Connect(const char *ip, uint16 port);

    /**
     * Connect to peer(asynchronous).
     * @param[in] ip     - the ip address.
     * @param[in] port   - the port number.
     * @return int - return 0 if success, otherwise return -1.
     */
    int AsyncConn(const char *ip, uint16 port);

    /**
     * Check given sessionId is validate or not.
     * @param[in] sessionId - the given session Id.
     * @return bool - return true is given session Id validate, otherwise return false.
     */
    bool IsSessionValidate(int sessionId);

    /**
     * Remove session.
     * Note:
     *      In llbc core library, RemoveSession() method return int value 
     *      to indicate operation success or not, In python layer, we ignore
     *      llbc core library method's return value.
     * @param[in] sessionId - the session Id.
     * @param[in] reason    - the remove reason.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveSession(int sessionId, const char *reason = nullptr);

public:
    /**
     * Send data.
     * @param[in] sessionId - the peer session Id.
     * @param[in] opcode   - the opcode.
     * @param[in] data - send data(normal, not steal reference).
     *                   Note, the object's class must has encode() method and return bytearray.
     * @param[in] status   - the status code, default is 0.
     * @param[in] extData1 - the extend data1, default is 0. 
     * @param[in] extData2 - the extend data2, default is 0. 
     * @param[in] extData3 - the extend data3, default is 0. 
     * @return int - return 0 if success, otherwise return -1.
     */
    int Send(int sessionId, int opcode, PyObject *data, int status = 0, sint64 extData1 = 0, sint64 extData2 = 0, sint64 extData3 = 0);

    /**
     * Multicast data.
     * @param[in] sessionIds - the peer session Ids.
     * @param[in] opcode     - the opcode.
     * @param[in] data       - the data(normal, not steal reference).
     *                         Note, the object's class must has encode() method and return bytearray.
     * @param[in] status     - the status code, default is 0.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Multicast(const LLBC_SessionIdList &sessionIds, int opcode, PyObject *data, int status=0);

    /**
     * Broadcast data.
     * @param[in] opcode     - the opcode.
     * @param[in] data       - the data(normal, not steal reference).
     *                         Note, the object's class must has encode() method and return bytearray.
     * @param[in] status     - the status code, default is 0.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Broadcast(int opcode, PyObject *data, int status=0);

    /**
     * Set specific opcode's packet Subscribe handler, if is RAW type service, the opcode parameter must be 0.
     * @param[in] opcode  - opcode, in RAW type service, ignore.
     * @param[in] handler - the handler object, must has handle(packet) method,
     *                      or rewrite __call__(self, packet) method.
     *                      check order:
     *                          handle(self, packet) -> __call__(self, packet)
     *                      packet describe:
     *                          <dict type>: {'sessionid':1, 'opcode':1, 'status': 0, 'data': <the data>}
     * @param[in] flags   - flags, not used now.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Subscribe(int opcode, PyObject *handler, int flags = 0);

    /**
     * Set specific opcode packet PreSubscribe handler, if is RAW type service, the opcode parameter must be 0.
     * @param[in] opcode     - opcode, in RAW type service, ignore.
     * @param[in] preHandler - the handler object, to see more information about this parameter, see Subscribe() method.
     * @param[in] flags      - flags, not used now.
     * @return int - return 0 if success, otherwise return -1.
     */
    int PreSubscribe(int opcode, PyObject *preHandler, int flags = 0);

#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    /**
     * Set packet unify PreSubscribe handler, this method can pre-subscribe RAW type service's packets.
     * @param[in] preHandler - the handler object, to see more information about this parameter, see Subscribe() method.
     * @param[in] flags      - flags, not used now.
     * @return int - return 0 if success, otherwise return -1.
     */
    int UnifyPreSubscribe(PyObject *preHandler, int flags = 0);
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

public:
    /**
     * Subscribe event.
     * @param[in] event    - the event Id.
     * @param[in] listener - callable object, event handler(not steal ref).
     * @return LLBC_ListenerStub - listener stub, return 0 if failed.
     */
    LLBC_ListenerStub SubscribeEvent(int event, PyObject *listener);

    /**
     * Unsubscribe event.
     * @param[in] event - the event Id.
     */
    void UnsubscribeEvent(int event);

    /**
     * Unsubscribe event.
     * @param[in] stub - the event stub.
     */
    void UnsubscribeEvent(LLBC_ListenerStub stub);

    /**
     * Fire event.
     * @param[in] ev - the event object(not steal ref).
     * @return int - return 0 if success, otherwise return -1.
     */
    int FireEvent(PyObject *ev);

public:
    /**
     * Post one callable to service, this callable will call in Service OnIdle stage or next Frame.
     * @param[in] callable - the callable object, normal, not steal reference.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Post(PyObject *callable);

    /**
     * Service mainloop method.
     * @return bool - return true means don't need call again.
     */
    bool MainLoop();

public:
    /**
     * Get error hooker.
     * @return pyllbc_ErrorHooker * - the error hooker.
     */
    static pyllbc_ErrorHooker *GetErrHooker();

private:
    /**
     * Create llbc layer service object.
     * @param[in] svcType - the service type.
     * @param[in] svcName - the service name.
     */
    void CreateLLBCService(LLBC_IService::Type svcType, const LLBC_String &svcName);

    /**
     * After stop method, use to purge all python type data.
     */
    void AfterStop();

    /**
     * Frame callable operation methods.
     */
    typedef std::set<PyObject *> _FrameCallables;
    void HandleFrameCallables(_FrameCallables &callables, bool &usingFlag);
    void DestroyFrameCallables(_FrameCallables &callables, bool &usingFlag);

private:
    /**
     * Serialize the python object to packet.
     * @param[in]  pyObj  - the python object.
     * @param[out] stream - the serialized stream.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SerializePyObj2Stream(PyObject *pyObj, LLBC_Stream &stream);

private:
    /**
     * The additional event constructor.
     * @param[in] ev - the event object.
     */
    static void EventEnqueueHandler(LLBC_Event *ev);

    /**
     * The custom event destructor.
     * @param;[in] - the event object.
     */
    static void EventDequeueHandler(LLBC_Event *ev);

private:
    /**
     * Friend class: pyllbc_Component.
     * Access all methods and data members.
     */
    friend class pyllbc_Component;

private:
    LLBC_IService *_llbcSvc;
    LLBC_IService::Type _llbcSvcType;
    LLBC_String _llbcSvcName;

    PyObject *_pySvc;

    bool _inMainloop;

    pyllbc_Component *_cppComp;
    typedef std::vector<PyObject *> _Comps;
    _Comps _comps;

    typedef std::map<int, pyllbc_PacketHandler *> _PacketHandlers;
    _PacketHandlers _handlers;
    _PacketHandlers _preHandlers;
#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    pyllbc_PacketHandler *_unifyPreHandler;
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

    Codec _codec;
    typedef std::map<int, PyObject *> _Codecs;
    _Codecs _codecs;
    bool _suppressedCoderNotFoundWarning;

    static PyObject *_pyEvCls;
    static LLBC_Delegate<void(LLBC_Event *)> _evEnqueueHandler;
    static LLBC_Delegate<void(LLBC_Event *)> _evDequeueHandler;

    _FrameCallables _beforeFrameCallables;
    _FrameCallables _afterFrameCallables;

    bool _handlingBeforeFrameCallables;
    bool _handledBeforeFrameCallables;
    bool _handlingAfterFrameCallables;

    bool _started;
    bool _stoping;

private:
    PyObject *_keyCObj;

    static PyObject *_streamCls;
    static pyllbc_ErrorHooker *_errHooker;
};

#endif // !__PYLLBC_COMM_PY_SERVICE_H__
