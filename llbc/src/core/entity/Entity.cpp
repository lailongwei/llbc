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

#include "llbc/core/entity/Entity.h"

__LLBC_INTERNAL_NS_BEGIN

static LLBC_NS LLBC_Variant __g_invalidProperty;

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_Entity::LLBC_Entity()
{
    _id = 0;
}

LLBC_Entity::~LLBC_Entity()
{
    RemoveAllBehaviors();
}

int LLBC_Entity::GetId() const
{
    return _id;
}

void LLBC_Entity::SetId(int id)
{
    _id = id;
}

const LLBC_String &LLBC_Entity::GetName() const
{
    return _name;
}

void LLBC_Entity::SetName(const LLBC_String &name)
{
    _name.clear();
    _name.append(name);
}

int LLBC_Entity::AddProperty(const LLBC_String &name, const LLBC_Variant &value)
{
    if (name.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    std::map<LLBC_String, LLBC_Variant>::const_iterator iter = _properties.find(name);
    if (iter != _properties.end())
    {
        LLBC_SetLastError(LLBC_ERROR_EXIST);
        return LLBC_FAILED;
    }

    _properties.insert(std::make_pair(name, value));

    return LLBC_OK;
}

bool LLBC_Entity::IsExistProperty(const LLBC_String &name) const
{
    if (name.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return false;
    }

    std::map<LLBC_String, LLBC_Variant>::const_iterator iter = _properties.find(name);
    if (iter == _properties.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return false;
    }

    return true;
}

LLBC_Variant &LLBC_Entity::GetProperty(const LLBC_String &name)
{
    if (name.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_INTERNAL_NS __g_invalidProperty;
    }

    std::map<LLBC_String, LLBC_Variant>::iterator iter = _properties.find(name);
    if (iter == _properties.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_INTERNAL_NS __g_invalidProperty;
    }

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    return iter->second;
}

const LLBC_Variant &LLBC_Entity::GetProperty(const LLBC_String &name) const
{
    if (name.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_INTERNAL_NS __g_invalidProperty;
    }

    std::map<LLBC_String, LLBC_Variant>::const_iterator iter = _properties.find(name);
    if (iter == _properties.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_INTERNAL_NS __g_invalidProperty;
    }

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    return iter->second;
}

int LLBC_Entity::RemmoveProperty(const LLBC_String &name)
{
    if (name.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    std::map<LLBC_String, LLBC_Variant>::iterator iter = _properties.find(name);
    if (iter == _properties.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    _properties.erase(iter);

    return LLBC_OK;
}

void LLBC_Entity::RemoveAllProperties()
{
    _properties.erase(_properties.begin(), _properties.end());
}

bool LLBC_Entity::IsExistBehavior(int id) const
{
    std::map<int, LLBC_BehaviorValue>::const_iterator iter = _behaviors.find(id);
    if (iter == _behaviors.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return false;
    }

    return true;
}

bool LLBC_Entity::IsExistBehavior(const LLBC_String &name) const
{
    if (name.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return false;
    }

    std::map<LLBC_String, LLBC_BehaviorValue>::const_iterator iter = _behaviors2.find(name);
    if (iter == _behaviors2.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return false;
    }

    return true;
}

const std::map<int, LLBC_BehaviorValue> &LLBC_Entity::GetIdBehaviorMap() const
{
    return _behaviors;
}

const std::map<LLBC_String, LLBC_BehaviorValue> &LLBC_Entity::GetNameBehaviorMap() const
{
    return _behaviors2;
}

int LLBC_Entity::RemoveBehavior(int id)
{
    std::map<int, LLBC_BehaviorValue>::iterator iter = _behaviors.find(id);
    if (iter == _behaviors.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }


    if (!iter->second.name.empty())
        _behaviors2.erase(iter->second.name);

    delete iter->second.holder;
    _behaviors.erase(iter);

    return LLBC_OK;
}

int LLBC_Entity::RemoveBehavior(const LLBC_String &name)
{
    if (name.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    std::map<LLBC_String, LLBC_BehaviorValue>::iterator iter = _behaviors2.find(name);
    if (iter == _behaviors2.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    _behaviors.erase(iter->second.id);
    delete iter->second.holder;
    _behaviors2.erase(iter);

    return LLBC_OK;
}

void LLBC_Entity::RemoveAllBehaviors()
{
    std::map<int, LLBC_BehaviorValue>::iterator iter = _behaviors.begin();
    for (; iter != _behaviors.end(); ++iter)
        delete iter->second.holder;

    _behaviors.erase(_behaviors.begin(), _behaviors.end());
    _behaviors2.erase(_behaviors2.begin(), _behaviors2.end());
}

__LLBC_NS_END
