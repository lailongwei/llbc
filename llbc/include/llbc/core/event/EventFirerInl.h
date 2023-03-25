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

#include "llbc/core/event/Event.h"
#include "llbc/core/event/EventMgr.h"

__LLBC_NS_BEGIN

inline LLBC_EventFirer::LLBC_EventFirer()
: _ev(nullptr)
, _evMgr(nullptr)
{
}

inline LLBC_EventFirer::~LLBC_EventFirer()
{
    Clear();
}

template <typename KeyType, typename ParamType>
LLBC_EventFirer &LLBC_EventFirer::SetParam(const KeyType &paramKey, const ParamType &param)
{
    _ev->SetParam(paramKey, param);
    return *this;
}

inline void LLBC_EventFirer::Fire()
{
    _evMgr->Fire(_ev);
    _ev = nullptr;
    _evMgr = nullptr;

    LLBC_Recycle(this);
}

inline void LLBC_EventFirer::Clear()
{
    if (_ev)
    {
        LLBC_Recycle(_ev);
        _ev = nullptr;
        _evMgr = nullptr;
    }
}

inline void LLBC_EventFirer::OnPoolInstCreate(LLBC_IObjectPoolInst &poolInst)
{
    LLBC_IObjectPool *objPool = poolInst.GetIObjectPool();
    objPool->AcquireOrderedDeletePoolInst(
        typeid(LLBC_EventFirer).name(), typeid(LLBC_Event).name());
}

inline void LLBC_EventFirer::SetEventInfo(LLBC_Event *ev, LLBC_EventMgr *evMgr)
{
    _ev = ev;
    _evMgr = evMgr;
}

__LLBC_NS_END
