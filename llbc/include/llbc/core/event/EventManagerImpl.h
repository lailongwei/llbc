/**
 * @file    EventManagerImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/28
 * @version 1.0
 *
 * @brief
 */
#ifdef __LLBC_CORE_EVENT_EVENT_MANAGER_H__

#include "llbc/core/event/Event.h"

__LLBC_NS_BEGIN

template <typename ObjectType>
LLBC_ListenerStub LLBC_EventManager::AddListener(int id, 
                                                 ObjectType *obj, 
                                                 void (ObjectType::*listener)(LLBC_Event *),
                                                 const LLBC_ListenerStub &bindedStub)
{
    if(!obj || !listener)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_INVALID_LISTENER_STUB;
    }

    return this->AddListener(id, new LLBC_Delegate1<ObjectType, LLBC_Event *>(obj, listener), bindedStub);
}

__LLBC_NS_END

#endif // __LLBC_CORE_EVENT_EVENT_MANAGER_H__
