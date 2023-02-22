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

#pragma once

#include "lullbc/common/Common.h"

/**
 * \breif The lullbc timer encapsulation.
 */
class LULLBC_HIDDEN lullbc_Timer : public LLBC_Timer
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
    virtual void OnTimeout();

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
