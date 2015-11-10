/**
 * @file    PyService.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/16
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_COMM_PY_SERVICE_H__
#define __PYLLBC_COMM_PY_SERVICE_H__

#include "pyllbc/common/Common.h"
#include "pyllbc/core/Core.h"

class pyllbc_Facade;
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
     * @param[in] pySvc - the python layer service instance(borrow instance).
     */
    pyllbc_Service(LLBC_IService::Type type, PyObject *pySvc);
    /**
     * Destructor.
     */
    virtual ~pyllbc_Service();

public:
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
     * @param[in] - service fps.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetFPS(int fps);

    /**
     * Get service frame interval.
     * @return int - the service frame interval, in milli-seconds.
     */
    int GetFrameInterval() const;

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
     * Register facade.
     * @param[in] facade - the facade instance(normal, not steal reference).
     *                     methods:
     *                      oninitialize(ev): service initialize handler.
     *                        ev.svc: service object.
     *                      ondestroy(ev): service destroy handler.
     *                        ev.svc: service object.
     *                      onupdate(ev): service update handler.
     *                        ev.svc: service object.
     *                      onidle(ev): service idle handler.
     *                        ev.svc: service object.
     *                        ev.idletime: idletime, float type, in seconds.
     *                      onsessioncreate(ev): session create handler.
     *                        ev.svc: service object.
     *                        ev.sessionid: session Id.
     *                        ev.ip: peer Ip address.
     *                        ev.port: peer port number.
     *                      onsessiondestroy(sessionId): session destroy handler.
     *                        ev.svc: service object.
     *                        ev.sessionid: session Id.
     *                      onasyncconnresult(dict): async-connect result handler.
     *                        ev.svc: service object.
     *                        ev.ip:  peer Ip address.
     *                        ev.port: peer port number.
     *                        ev.connected: connected flag.
     *                        ev.reason: reason describe.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RegisterFacade(PyObject *facade);

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
     * Remove session.
     * Note:
     *      In llbc core library, RemoveSession() method return int value 
     *      to indicate operation success or not, In python layer, we ignore
     *      llbc core library method's return value.
     * @param[in] sessionId - the session Id.
     */
    void RemoveSession(int sessionId);

public:
    /**
     * Send data.
     * @param[in] sessionId - the peer session Id.
     * @param[in] opcode - the opcode.
     * @param[in] data - send data(normal, not steal reference).
     *                   Note, the object's class must has encode() method and return bytearray.
     * @param[in] status - the status code, default is 0.
     * @param[in] parts  - the packet header parts object, method will not hold this object instance.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Send(int sessionId, int opcode, PyObject *data, int status = 0, PyObject *parts = NULL);

    /**
     * Multicast data.
     * @param[in] sessionIds - the peer session Ids.
     * @param[in] opcode     - the opcode.
     * @param[in] data       - the data(normal, not steal reference).
     *                         Note, the object's class must has encode() method and return bytearray.
     * @param[in] status     - the status code, default is 0.
     * @param[in] parts  - the packet header parts object, method will not hold this object instance.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Multicast(const LLBC_SessionIdList &sessionIds, int opcode, PyObject *data, int status=0, PyObject *parts = NULL);

    /**
     * Broadcast data.
     * @param[in] sessionIds - the peer session Ids.
     * @param[in] opcode     - the opcode.
     * @param[in] data       - the data(normal, not steal reference).
     *                         Note, the object's class must has encode() method and return bytearray.
     * @param[in] status     - the status code, default is 0.
     * @param[in] parts  - the packet header parts object, method will not hold this object instance.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Broadcast(int opcode, PyObject *data, int status=0, PyObject *parts = NULL);

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
     * @param[in] opcode  - opcode, in RAW type service, ignore.
     * @param[in] handler - the handler object, to see more information about this parameter, see Subscribe() method.
     * @param[in] flags   - flags, not used now.
     * @return int - return 0 if success, otherwise return -1.
     */
    int PreSubscribe(int opcode, PyObject *preHandler, int flags = 0);

#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    /**
     * Set packet unify PreSubscribe handler, this method can pre-subscribe RAW type service's packets.
     * @param[in] handler - the handler object, to see more information about this parameter, see Subscribe() method.
     * @param[in] flags   - flags, not used now.
     * @return int - return 0 if success, otherwise return -1.
     */
    int UnifyPreSubscribe(PyObject *preHandler, int flags = 0);
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

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
     */
    void CreateLLBCService(LLBC_IService::Type svcType);

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
     * Build c++ layer parts object, from python layer parts.
     * @param[in] pyLayerParts - the python layer parts.
     * @return LLBC_Pa cketHeaderParts * - the c++ layer parts.
     */
    LLBC_PacketHeaderParts *BuildCLayerParts(PyObject *pyLayerParts);

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
     * Friend class: pyllbc_Facade.
     * Access all methods and data members.
     */
    friend class pyllbc_Facade;

private:
    LLBC_IService *_llbcSvc;
    LLBC_IService::Type _llbcSvcType;

    PyObject *_pySvc;

    bool _inMainloop;

    pyllbc_Facade *_cppFacade;
    typedef std::set<PyObject *> _Facades;
    _Facades _facades;

    typedef std::map<int, pyllbc_PacketHandler *> _PacketHandlers;
    _PacketHandlers _handlers;
    _PacketHandlers _preHandlers;
#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    pyllbc_PacketHandler *_unifyPreHandler;
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

    Codec _codec;
    typedef std::map<int, PyObject *> _Codecs;
    _Codecs _codecs;

    _FrameCallables _beforeFrameCallables;
    _FrameCallables _afterFrameCallables;

    bool _handlingBeforeFrameCallables;
    bool _handledBeforeFrameCallables;
    bool _handlingAfterFrameCallables;

    bool _started;
    bool _stoping;

private:
    PyObject *_keyCObj;

    static int _maxLLBCSvcId;
    static PyObject *_streamCls;
    static pyllbc_ErrorHooker *_errHooker;
};

#endif // !__PYLLBC_COMM_PY_SERVICE_H__
