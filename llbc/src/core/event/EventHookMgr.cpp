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

#include "llbc/core/helper/STLHelper.h"

#include "llbc/core/event/Event.h"
#include "llbc/core/event/EventHookMgr.h"

#if LLBC_CFG_CORE_ENABLE_EVENT_HOOK

__LLBC_NS_BEGIN

LLBC_EventHookMgr::LLBC_EventHookMgr()
: _preFiring(0)
, _postFiring(0)
{
}

LLBC_EventHookMgr::~LLBC_EventHookMgr()
{
}

int LLBC_EventHookMgr::AddPreFireHook(const LLBC_String &hookName, const LLBC_Delegate<bool(LLBC_Event *)> &hook)
{
    if (hookName.empty() || !hook)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    if (_preFireHookMap.find(hookName) != _preFireHookMap.end())
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    _preFireHookList.emplace_back(hookName, hook);
    _preFireHookMap[hookName] = std::prev(_preFireHookList.end());

    return LLBC_OK;
}

void LLBC_EventHookMgr::RemovePreFireHook(const LLBC_String &hookName)
{
    const auto it = _preFireHookMap.find(hookName);
    if (it == _preFireHookMap.end())
        return;

    // When pre-firing, recorded in the set, these hooks are not executed.
    if (_preFiring > 0)
    {
        _preFireRemovingNameSet.emplace(hookName);
        return;
    }

    // Real delete the hook.
    _preFireHookList.erase(it->second);
    _preFireHookMap.erase(it);
}

void LLBC_EventHookMgr::RemoveAllPreFireHooks()
{
    // When pre-firing, recorded in the set, these hooks are not executed.
    if (_preFiring > 0)
    {
        LLBC_Foreach(_preFireHookList, _preFireRemovingNameSet.emplace(item.first));
        return;
    }

    _preFireRemovingNameSet.clear();
    _preFireHookMap.clear();
    _preFireHookList.clear();
}

int LLBC_EventHookMgr::AddPostFireHook(const LLBC_String &hookName, const LLBC_Delegate<void(LLBC_Event *)> &hook)
{
    if (hookName.empty() || !hook)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    if (_postFireHookMap.find(hookName) != _postFireHookMap.end())
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    _postFireHookList.emplace_front(hookName, hook);
    _postFireHookMap[hookName] = _postFireHookList.begin();

    return LLBC_OK;
}

void LLBC_EventHookMgr::RemovePostFireHook(const LLBC_String &hookName)
{
    const auto it = _postFireHookMap.find(hookName);
    if (it == _postFireHookMap.end())
        return;

    // When post-firing, recorded in the set, these hooks are not executed.
    if (_postFiring > 0)
    {
        _postFireRemovingNameSet.emplace(hookName);
        return;
    }

    _postFireHookList.erase(it->second);
    _postFireHookMap.erase(it);
}

void LLBC_EventHookMgr::RemoveAllPostFireHooks()
{
    // When post-firing, recorded in the set, these hooks are not executed.
    if (_postFiring > 0)
    {
        LLBC_Foreach(_postFireHookList, _postFireRemovingNameSet.emplace(item.first));
        return;
    }

    _postFireRemovingNameSet.clear();
    _postFireHookMap.clear();
    _postFireHookList.clear();
}

int LLBC_EventHookMgr::PreFire(LLBC_Event *event)
{
    // pre-fire do not delete the event obj.
    const bool oldDontDelAfterFire = event->IsDontDelAfterFire();
    event->SetDontDelAfterFire(true);

    // Increase pre-firing flag.
    ++_preFiring;

    for (auto &[name, preHook] : _preFireHookList)
    {
        if (_preFireRemovingNameSet.find(name) == _preFireRemovingNameSet.end() && !preHook(event))
        {
            // Decrease pre-firing flag.
            --_preFiring;

            // After pre-fire, event reset the old DONT DEL AFTER FIRE option.
            event->SetDontDelAfterFire(oldDontDelAfterFire);

            // Handle hook addition and deletion after fire().
            HandlePreFiringHookOperations();

            // Set error code.
            LLBC_SetLastError(LLBC_ERROR_EVENT_MANAGER_PRE_FIRE_FAILED);
            return LLBC_FAILED;
        }
    }

    // Decrease pre-firing flag.
    --_preFiring;

    // After pre-fire, event reset the old DONT DEL AFTER FIRE option.
    event->SetDontDelAfterFire(oldDontDelAfterFire);

    // Handle hook addition and deletion after fire().
    HandlePreFiringHookOperations();
    return LLBC_OK;
}

void LLBC_EventHookMgr::PostFire(LLBC_Event *event)
{
    // Post-fire do not delete the event obj.
    const bool oldDontDelAfterFire = event->IsDontDelAfterFire();
    event->SetDontDelAfterFire(true);

    // Increase post-firing flag.
    ++_postFiring;

    for (auto &[name, postHook] : _postFireHookList)
    {
        if (_postFireRemovingNameSet.find(name) == _postFireRemovingNameSet.end())
            postHook(event);
    }

    // Decrease post-firing flag.
    --_postFiring;

    // Handle hook addition and deletion after post-fire().
    HandlePostFiringHookOperations();

    // After post-fire, event reset the old DONT DEL AFTER FIRE option.
    event->SetDontDelAfterFire(oldDontDelAfterFire);
}

#if LLBC_DEBUG
LLBC_NO_INLINE
#else
LLBC_FORCE_INLINE
#endif
void LLBC_EventHookMgr::HandlePreFiringHookOperations()
{
    if (_preFiring > 0)
        return;

    // Delete removing pre-fire hooks.
    if (_preFireHookList.size() != _preFireRemovingNameSet.size())
    {
        for (auto &name : _preFireRemovingNameSet)
            RemovePreFireHook(name);
        _preFireRemovingNameSet.clear();
    }
    else
    {
        RemoveAllPreFireHooks();
    }
}

#if LLBC_DEBUG
LLBC_NO_INLINE
#else
LLBC_FORCE_INLINE
#endif
void LLBC_EventHookMgr::HandlePostFiringHookOperations()
{
    if (_postFiring > 0)
        return;

    // Delete removing post-fire hooks.
    if (_postFireHookList.size() != _postFireRemovingNameSet.size())
    {
        for (auto &name : _postFireRemovingNameSet)
            RemovePostFireHook(name);
        _postFireRemovingNameSet.clear();
    }
    else
    {
        RemoveAllPostFireHooks();
    }
}

__LLBC_NS_END

#endif // LLBC_CFG_CORE_ENABLE_EVENT_HOOK