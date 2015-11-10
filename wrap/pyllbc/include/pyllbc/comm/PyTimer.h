/**
 * @file    PyTimer.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/09/05
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_COMM_PY_TIMER_H__
#define __PYLLBC_COMM_PY_TIMER_H__

#include "pyllbc/common/Common.h"
#include "pyllbc/core/Core.h"

/**
 * \brief The python c++ wrap timer class encapsulation.
 */
class LLBC_HIDDEN pyllbc_Timer : public LLBC_BaseTimer
{
public:
    /**
     * Constructor.
     * @param[in] pyTimer         - the python layer timer object(steal reference).
     * @param[in] timeoutCallable - the timeout callable python object(normal, not steal reference).
     * @param[in] cancelCallable  - the cancel callable python object(normal, not steal reference).
     */
    pyllbc_Timer(PyObject *pyTimer, PyObject *timeoutCallable, PyObject *cancelCallable = NULL);

    /**
     * Destructor.
     */
    virtual ~pyllbc_Timer();

public:
    /**
     * Make sure ignored dead-weakref object error or not, default is false.
     * @return bool - true if ignored, otherwise return false.
     */
    virtual bool IsIgnoredDeadRef() const;

    /**
     * Set the ignored dead-weakref flag.
     * @param[in] flag - the ignored dead-weakref flag.
     */
    virtual void SetIgnoredDeadRef(bool flag);

public:
    /**
     * Schedule timer(rewrite).
     * @param[in] dueTime - due time, in milli-seconds.
     * @param[in] period  - period value, in milli-seconds.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Schedule(uint64 dueTime, uint64 period);

public:
    /**
     * Get timer string representation.
     * @return LLBC_String - the timer string representation.
     */
    LLBC_String ToString() const;

public:
    /**
     * Timeout event handler.
     * @return bool - return true means readd, otherwise means stop this timer.
     */
    virtual bool OnTimeout();

    /**
     * Cancel event handler.
     */
    virtual void OnCancel();

private:
    PyObject *_pyTimer;
    bool _ignoredDeadRef;

    PyObject *_timeoutCallable;
    PyObject *_boundedTimeOutObj;
    LLBC_String _timeoutCallableDesc;
    LLBC_String _boundedTimeOutObjDesc;

    PyObject *_cancelCallable;
    PyObject *_boundedCancelObj;
    LLBC_String _cancelCallableDesc;
    LLBC_String _boundedCancelObjDesc;
};

#endif // !__PYLLBC_COMM_PY_TIMER_H__
