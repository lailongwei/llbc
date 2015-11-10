/**
 * @file    PyTimerScheduler.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/05
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Export.h"

#include "pyllbc/comm/PyTimerScheduler.h"

pyllbc_TimerScheduler::pyllbc_TimerScheduler()
: _scheduler(NULL)
{
}

pyllbc_TimerScheduler::~pyllbc_TimerScheduler()
{
}

void pyllbc_TimerScheduler::Init()
{
    if (_scheduler)
        return;

    _scheduler = LLBC_TimerScheduler::GetCurrentThreadScheduler();
}

void pyllbc_TimerScheduler::Schedule()
{
    _scheduler->SetEnabled(true);
    _scheduler->Update();
    _scheduler->SetEnabled(false);
}
