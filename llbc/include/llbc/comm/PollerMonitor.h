/**
 * @file    PollerMonitor.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/14
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_POLLER_MONITOR_H__
#define __LLBC_COMM_POLLER_MONITOR_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

__LLBC_NS_BEGIN

/**
 * \brief The poller monitor class encapsulation.
 */
class LLBC_EXPORT LLBC_PollerMonitor : private LLBC_BaseTask
{
    typedef LLBC_IDelegate0<void> _Deleg;

public:
    /**
     * Parameter constructor.
     * @param[in] deleg - the monitor invoke delegate.
     */
    LLBC_PollerMonitor(_Deleg *deleg);

    /**
     * Destructor.
     */
    virtual ~LLBC_PollerMonitor();

public:
    /**
     * Startup poller monitor.
     * @return int - return 0 if startup success, otherwise return -1.
     */
    int Start();

    /**
     * Stop poller monitor.
     */
    void Stop();

public:
    /**
     * Task entry method.
     */
    virtual void Svc();

    /**
     * Task cleanup method.
     */
    virtual void Cleanup();

private:
    _Deleg *_deleg;

    volatile bool _started;
    volatile bool _stopping;
};

__LLBC_NS_END

#endif // !__LLBC_COMM_POLLER_MONITOR_H__

