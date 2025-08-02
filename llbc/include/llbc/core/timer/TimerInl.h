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

__LLBC_NS_BEGIN

inline const LLBC_Delegate<void(LLBC_Timer *)> &LLBC_Timer::GetTimeoutHandler() const
{
    return _timeoutDeleg;
}

template <typename ObjType>
void LLBC_Timer::SetTimeoutHandler(ObjType *obj, void (ObjType::*method)(LLBC_Timer *))
{
    _timeoutDeleg = LLBC_Delegate<void(LLBC_Timer *)>(obj, method);
}

inline void LLBC_Timer::SetTimeoutHandler(const LLBC_Delegate<void(LLBC_Timer *)> &timeoutDeleg)
{
    _timeoutDeleg = timeoutDeleg;
}

inline const LLBC_Delegate<void(LLBC_Timer *)> &LLBC_Timer::GetCancelHandler() const
{
    return _cancelDeleg ? *_cancelDeleg : _invalidCancelDeleg;
}

template <typename ObjType>
void LLBC_Timer::SetCancelHandler(ObjType *obj, void (ObjType::*method)(LLBC_Timer *))
{
    SetCancelHandler(LLBC_Delegate<void(LLBC_Timer *)>(obj, method));
}

inline void LLBC_Timer::SetCancelHandler(const LLBC_Delegate<void(LLBC_Timer *)> &cancelDeleg)
{
    if (cancelDeleg)
    {
        if (!_cancelDeleg)
            _cancelDeleg = new LLBC_Delegate<void(LLBC_Timer *)>(cancelDeleg);
        else
            *_cancelDeleg = cancelDeleg;
    }
    else if (_cancelDeleg)
    {
        *_cancelDeleg = nullptr;
    }
}

inline void LLBC_Timer::OnTimeout()
{
    if (LIKELY(_timeoutDeleg))
        _timeoutDeleg(this);
}

inline void LLBC_Timer::OnCancel()
{
    if (_cancelDeleg && *_cancelDeleg)
        (*_cancelDeleg)(this);
}

inline int LLBC_Timer::Schedule(const LLBC_Time &firstTime, const LLBC_TimeSpan &period)
{
    return Schedule(firstTime - LLBC_Time::Now(), period);
}

__LLBC_NS_END
