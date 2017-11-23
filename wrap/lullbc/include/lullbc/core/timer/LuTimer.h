/**
 * @file    LuTimer.h
 * @author  Longwei lai<lailongwei@126.com>
 * @date    2017-07-07
 * @version 1.0
 *
 * @brief
 */
#ifndef __LULLBC_CORE_TIMER_LUTIMER_H__
#define __LULLBC_CORE_TIMER_LUTIMER_H__

#include "lullbc/common/Common.h"

/**
 * \breif The lullbc timer encapsulation.
 */
class LULLBC_HIDDEN lullbc_Timer : public LLBC_BaseTimer
{
public:
    /**
     * Constructor.
     * @param[in] l - the lua vm.
     */
    explicit lullbc_Timer(lua_State *l);

    /**
     * Destructor.
     */
    virtual ~lullbc_Timer();

public:
    /**
     * Overrided method, when timer timeout, will call this method.
     */
    virtual bool OnTimeout();

    /**
     * Overrided method, when timer cancel, will call this method.
     */
    virtual void OnCancel();

private:
    bool PushCallable();

    int PushPCallErrorHandler();

    void HandleTimerError(const char *callType);

private:
    LLBC_DISABLE_ASSIGNMENT(lullbc_Timer);

private:
    lua_State *_luaState;
    int _callableInfo;
};

#endif // !__LULLBC_CORE_TIMER_LUTIMER_H__
