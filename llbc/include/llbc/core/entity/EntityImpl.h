/**
 * @file    EntityImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/13
 * @version 1.0
 *
 * @brief
 */
#ifdef __LLBC_CORE_ENTITY_ENTITY_H__

#include "llbc/core/entity/Holder.h"

__LLBC_NS_BEGIN

template <typename BehaviorType>
int LLBC_Entity::AddBehavior(int id, BehaviorType *value, const LLBC_String &name)
{
    if (!value)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }

    if (this->IsExistBehavior(id) || 
        (!name.empty() && this->IsExistBehavior(name)))
    {
        LLBC_SetLastError(LLBC_ERROR_EXIST);
        return LLBC_RTN_FAILED;
    }

    LLBC_BehaviorValue behaviorValue;
    behaviorValue.id = id;
    behaviorValue.name = name;
    behaviorValue.holder = new LLBC_Holder<BehaviorType>(value);

    _behaviors.insert(std::make_pair(id, behaviorValue));
    if (!name.empty())
        _behaviors2.insert(std::make_pair(name, behaviorValue));

    return LLBC_RTN_OK;
}

template <typename BehaviorType>
BehaviorType *LLBC_Entity::GetBehavior(int id)
{
    std::map<int, LLBC_BehaviorValue>::iterator iter = _behaviors.find(id);
    if (iter == _behaviors.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return NULL;
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
        return NULL;
    }

    return static_cast<const BehaviorType *>(iter->second.holder->GetValue());
}

template <typename BehaviorType>
BehaviorType *LLBC_Entity::GetBehavior(const LLBC_String &name)
{
    if (name.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return NULL;
    }

    std::map<LLBC_String, LLBC_BehaviorValue>::iterator iter = _behaviors2.find(name);
    if (iter == _behaviors2.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return NULL;
    }

    return static_cast<BehaviorType *>(iter->second.holder->GetValue());
}

template <typename BehaviorType>
const BehaviorType *LLBC_Entity::GetBehavior(const LLBC_String &name) const
{
    if (name.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return NULL;
    }

    std::map<LLBC_String, LLBC_BehaviorValue>::const_iterator iter = _behaviors2.find(name);
    if (iter == _behaviors2.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return NULL;
    }

    return static_cast<const BehaviorType *>(iter->second.holder->GetValue());
}

__LLBC_NS_END

#endif // __LLBC_CORE_ENTITY_ENTITY_H__
