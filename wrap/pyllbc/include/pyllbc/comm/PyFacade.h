/**
 * @file    PyFacade.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/20
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_COMM_PY_FACADE_H__
#define __PYLLBC_COMM_PY_FACADE_H__

#include "pyllbc/common/Common.h"
#include "pyllbc/core/Core.h"

class pyllbc_Service;

/**
 * \brief The pyllbc facade class encapsulation.
 */
class LLBC_HIDDEN pyllbc_Facade : public LLBC_IFacade
{
public:
    /**
     * Constructor.
     * @param[in] svc - adapted to python c++ wrapped service.
     */
    pyllbc_Facade(pyllbc_Service *svc);

    /**
     * Destructor.
     */
    virtual ~pyllbc_Facade();

public:
    /**
     * When service initialized, will call this event handle function.
     */
    virtual void OnInitialize();

    /**
     * When service will destroy, will call this event handle function.
     */
    virtual void OnDestroy();

    /**
     * Heartbeat function.
     */
    virtual void OnUpdate();

    /**
     * Idle event handler.
     * @param[in] idleTime - idle time, in milliseconds.
     */
    virtual void OnIdle(int idleTime);

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
    virtual void OnUnHandledPacket(int opcode);

public:
    /**
     * data receive handler.
     * @param[in] packet - packet.
     */
    virtual void OnDataReceived(LLBC_Packet &packet);

    /**
     * The data pre-receive handler.
     * @param[in] packet - packet.
     * @return void * - the return value, if return NULL, will remove this session.
     */
    virtual void *OnDataPreReceived(LLBC_Packet &packet);

#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    /**
     * The data unify pre-receive handler.
     * @param[in] packet - packet.
     * @return void * - the return value, if return NULL, will remove this session.
     */
    virtual void *OnDataUnifyPreReceived(LLBC_Packet &packet);
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

    /**
     * Build python layer packet object according to LLBC_Packet object reference.
     * @return PyObject * - the python layer packet object, if error occurred, return NULL.
     */
    PyObject *BuildPyPacket(LLBC_Packet &packet);

    /**
     * Delete the python layer packet.
     */
    void DeletePyPacket(void *pyPacket);

private:
    pyllbc_Service *_svc;
    PyObject *_pySvc;

    const LLBC_IService::Type _svcType;

    PyObject *_methOnInitialize;
    PyObject *_methOnDestroy;
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
    PyObject *_keyCObj;

    PyObject *_pyPacketCls;
};

#endif // !__PYLLBC_COMM_PY_FACADE_H__
