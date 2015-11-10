/**
 * @file    PyFrameCallable.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/03/16
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_COMM_FRAME_CALLABLE_H__
#define __PYLLBC_COMM_FRAME_CALLABLE_H__

#include "pyllbc/common/Common.h"
#include "pyllbc/core/Core.h"

class pyllbc_FrameCallable
{
public:
    /**
     * Parameter constructor.
     * @param[in] svc      - the service object, steal reference.
     * @param[in] callable - the callable object, normal, not steal reference.
     */
    pyllbc_FrameCallable(PyObject *svc, PyObject *callable);

    /**
     * Destructor.
     */
    virtual ~pyllbc_FrameCallable();

public:
    /**
     * Callable method, call by llbc library.
     * @param[in] llbcSvc - the service pointer.
     */
    void Call(LLBC_IService *llbcSvc);

private:
    PyObject *_svc;
    PyObject *_callable;
};

#endif // !__PYLLBC_COMM_FRAME_CALLABLE_H__
