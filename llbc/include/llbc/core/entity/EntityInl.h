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

#include "llbc/core/entity/Holder.h"

__LLBC_NS_BEGIN

template <typename BehaviorType>
int LLBC_Entity::AddBehavior(int id, BehaviorType *value, const LLBC_String &name)
{
    if (!value)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    if (this->IsExistBehavior(id) ||
        (!name.empty() && this->IsExistBehavior(name)))
    {
        LLBC_SetLastError(LLBC_ERROR_EXIST);
        return LLBC_FAILED;
    }

    LLBC_BehaviorValue behaviorValue;
    behaviorValue.id = id;
    behaviorValue.name = name;
    behaviorValue.holder = new LLBC_Holder<BehaviorType>(value);

    _behaviors.insert(std::make_pair(id, behaviorValue));
    if (!name.empty())
        _behaviors2.insert(std::make_pair(name, behaviorValue));

    return LLBC_OK;
}

template <typename BehaviorType>
BehaviorType *LLBC_Entity::GetBehavior(int id)
{
    std::map<int, LLBC_BehaviorValue>::iterator iter = _behaviors.find(id);
    if (iter == _behaviors.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return nullptr;
    }

    return static_cast<BehaviorType *>(iter->second.holder->GetValue());
}

template <typename BehaviorType>
const BehaviorType *LLBC_Entity::GetBehavior(int id) const
{
    std::map<int, LLBC_BehaviorValue>::const_iterator iter = _behaviors.find(id);
    if (iter == _behaviors.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return nullptr;
    }

    return static_cast<const BehaviorType *>(iter->second.holder->GetValue());
}

template <typename BehaviorType>
BehaviorType *LLBC_Entity::GetBehavior(const LLBC_String &name)
{
    if (name.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return nullptr;
    }

    std::map<LLBC_String, LLBC_BehaviorValue>::iterator iter = _behaviors2.find(name);
    if (iter == _behaviors2.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return nullptr;
    }

    return static_cast<BehaviorType *>(iter->second.holder->GetValue());
}

template <typename BehaviorType>
const BehaviorType *LLBC_Entity::GetBehavior(const LLBC_String &name) const
{
    if (name.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return nullptr;
    }

    std::map<LLBC_String, LLBC_BehaviorValue>::const_iterator iter = _behaviors2.find(name);
    if (iter == _behaviors2.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return nullptr;
    }

    return static_cast<const BehaviorType *>(iter->second.holder->GetValue());
}

__LLBC_NS_END
