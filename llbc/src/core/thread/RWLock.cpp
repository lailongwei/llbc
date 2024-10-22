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


#include "llbc/common/Export.h"

#include "llbc/core/thread/RWLock.h"

__LLBC_NS_BEGIN;
static const int __g_valid = 0xfaebdc;
__LLBC_NS_END

__LLBC_NS_BEGIN

LLBC_RWLock::LLBC_RWLock()
{
    _holder.r_wait = 0;
    _holder.w_wait = 0;
    _holder.r_active = 0;
    _holder.w_active = 0;

    _holder.vaild = __g_valid;
}

LLBC_RWLock::~LLBC_RWLock()
{
    if (_holder.vaild != __g_valid)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return;
    }

    _holder.lock.Lock();
    if (_holder.r_active > 0 || _holder.w_active)
    {
        _holder.lock.Unlock();
        LLBC_SetLastError(LLBC_ERROR_BUSY);
        return;
    }

    if (_holder.r_wait > 0 || _holder.w_wait > 0)
    {
        _holder.lock.Unlock();
        LLBC_SetLastError(LLBC_ERROR_BUSY);
        return;
    }

    _holder.vaild = 0;
    _holder.lock.Unlock();

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
}

void LLBC_RWLock::ReadLock()
{
    _holder.lock.Lock();
    if (_holder.w_active)
    {
        _holder.r_wait ++;
        while (_holder.w_active)
        {
            _holder.readCond.Wait(_holder.lock);
        }
    }

    _holder.r_active ++;
    _holder.lock.Unlock();
}

bool LLBC_RWLock::ReadTryLock()
{
    if (_holder.vaild != __g_valid)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return false;
    }

    bool ret = true;
    _holder.lock.Lock();
    if (_holder.w_active)
    {
        ret = false;
        LLBC_SetLastError(LLBC_ERROR_BUSY);
    }
    else
    {
        _holder.r_active ++;
        LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    }

    _holder.lock.Unlock();

    return ret;
}

void LLBC_RWLock::ReadUnlock()
{
    _holder.lock.Lock();
    _holder.r_active --;
    if (_holder.r_active == 0 && _holder.w_wait > 0)
    {
        _holder.writeCond.Notify();
    }

    _holder.lock.Unlock();
}

void LLBC_RWLock::WriteLock()
{
    _holder.lock.Lock();
    if (_holder.w_active || _holder.r_active > 0)
    {
        _holder.w_wait ++;
        while (_holder.w_active || _holder.r_active > 0)
        {
            _holder.writeCond.Wait(_holder.lock);
        }

        _holder.w_wait --;
    }

    _holder.w_active = 1;
    _holder.lock.Unlock();
}

bool LLBC_RWLock::WriteTryLock()
{
    bool ret = true;
    _holder.lock.Lock();
    if (_holder.w_active || _holder.r_active > 0)
    {
        ret = false;
        LLBC_SetLastError(LLBC_ERROR_BUSY);
    }
    else
    {
        _holder.w_active = 1;
        LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    }

    _holder.lock.Unlock();

    return ret;
}

void LLBC_RWLock::WriteUnlock()
{
    _holder.lock.Lock();

    _holder.w_active = 0;
    if (_holder.w_wait > 0)
    {
        _holder.writeCond.Notify();
    }
    else if (_holder.r_wait > 0)
    {
        _holder.readCond.Broadcast();
    }

    _holder.lock.Unlock();
}

__LLBC_NS_END
