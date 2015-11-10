/**
 * @file    PollerMonitor.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/14
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/PollerMonitor.h"

namespace
{
    typedef LLBC_NS LLBC_IDelegate0 _Deleg;
}

__LLBC_NS_BEGIN

LLBC_PollerMonitor::LLBC_PollerMonitor(_Deleg *deleg)
: _deleg(deleg)

, _started(false)
, _stopping(false)
{
}

LLBC_PollerMonitor::~LLBC_PollerMonitor()
{
    this->Stop();
    LLBC_Delete(_deleg);
}

int LLBC_PollerMonitor::Start()
{
    if (_started)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_RTN_FAILED;
    }

    this->Activate();
    _started = true;

    return LLBC_RTN_OK;
}

void LLBC_PollerMonitor::Stop()
{
    if (!_started || _stopping)
        return;

    _stopping = true;
    while (_started)
        LLBC_ThreadManager::Sleep(20);

    _stopping = false;
}

void LLBC_PollerMonitor::Svc()
{
    while (!_started)
        LLBC_Sleep(20);

    while (!_stopping)
        _deleg->Invoke();
}

void LLBC_PollerMonitor::Cleanup()
{
    _started = false;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
