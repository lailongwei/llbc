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

__LLBC_NS_BEGIN

inline int LLBC_EventMgr::AddEventMgrHook(const LLBC_CString &name, LLBC_EventMgrHook *hook)
{
    if (!hook)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    if (!_evMgrHooks.emplace(name, hook).second)
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    hook->SetEventMgr(this);
    return LLBC_OK;
}

inline void LLBC_EventMgr::RemoveEventMgrHook(const LLBC_CString &name)
{
    auto it = _evMgrHooks.find(name);
    if (it == _evMgrHooks.end())
        return;

    auto hook = it->second;
    _evMgrHooks.erase(it);

    hook->OnEventMgrDestroy();
    delete hook;
}

template <typename ObjectType>
LLBC_ListenerStub LLBC_EventMgr::AddListener(int id, 
                                             ObjectType *obj, 
                                             void (ObjectType::*listener)(LLBC_Event &),
                                             const LLBC_ListenerStub &boundStub)
{
    if (!obj || !listener)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return 0;
    }

    return this->AddListener(id, LLBC_Delegate<void(LLBC_Event &)>(obj, listener), boundStub);
}

inline int LLBC_EventMgr::RemoveListenerX(LLBC_ListenerStub &stub)
{
    if (RemoveListener(stub) != LLBC_OK)
    {
        stub = 0;
        return LLBC_FAILED;
    }

    stub = 0;

    return LLBC_OK;
}

inline bool LLBC_EventMgr::IsFiring() const
{
    return _firing > 0;
}

__LLBC_NS_END
