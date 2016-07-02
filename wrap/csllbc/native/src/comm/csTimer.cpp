/**
 * @file    csTimer.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/06/29
 * @version 1.0
 *
 * @brief   The csharp timer class encapsulation.
 */

#include "csllbc/common/Export.h"

#include "csllbc/comm/csTimer.h"

csllbc_Timer::csllbc_Timer(_TimeoutDeleg timeoutDeleg, _CancelDeleg cancelDeleg)
: _timeoutDeleg(timeoutDeleg)
, _cancelDeleg(cancelDeleg)
{
}

csllbc_Timer::~csllbc_Timer()
{
    _timeoutDeleg = NULL;
    _cancelDeleg = NULL;
}

bool csllbc_Timer::OnTimeout()
{
    return (*_timeoutDeleg)() != 0;
}

void csllbc_Timer::OnCancel()
{
    if (_cancelDeleg)
        (*_cancelDeleg)();
}