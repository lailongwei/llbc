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

#if LLBC_CFG_CORE_ENABLE_EVENT_HOOK

#include "llbc/core/utils/Util_Delegate.h"

__LLBC_NS_BEGIN
class LLBC_Event;
__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The fire hook data class encapsulation.
 */
class LLBC_EXPORT LLBC_EventHookMgr final
{
public:
    /**
     * Ctor & Dtor.
     */
    LLBC_EventHookMgr();
    ~LLBC_EventHookMgr();

public:
    /**
     * Add pre-fire hook.
     * @param[in] hookName  - hook name.
     * @param[in] obj       - object.
     * @param[in] hook      - deleg.
     * @return int - success if return LLBC_OK, otherwise return LLBC_FAILED.
     */
    template <typename ObjectType>
    int AddPreFireHook(const LLBC_String &hookName, ObjectType *obj, bool (ObjectType::*hook)(LLBC_Event *));

    /**
     * Add pre-fire hook.
     * @param[in] hookName  - hook name.
     * @param[in] hook      - deleg.
     * @return int - success if return LLBC_OK, otherwise return LLBC_FAILED.
     */
    int AddPreFireHook(const LLBC_String &hookName, const LLBC_Delegate<bool(LLBC_Event *)> &hook);

    /**
     * Remove pre-fire hook using hook name.
     * @param[in] hookName  - hook name.
     */
    void RemovePreFireHook(const LLBC_String &hookName);

    /**
     * Remove all pre-fire hook.
     */
    void RemoveAllPreFireHooks();

    /**
     * Add post-fire hook.
     * @param[in] hookName  - hook name.
     * @param[in] obj       - object.
     * @param[in] hook      - deleg.
     * @return int - success if return LLBC_OK, otherwise return LLBC_FAILED.
     */
    template <typename ObjectType>
    int AddPostFireHook(const LLBC_String &hookName, ObjectType *obj, void (ObjectType::*hook)(LLBC_Event *));

    /**
     * Add post-fire hook.
     * @param[in] hookName  - hook name.
     * @param[in] hook      - deleg.
     * @return int - success if return LLBC_OK, otherwise return LLBC_FAILED.
     */
    int AddPostFireHook(const LLBC_String &hookName, const LLBC_Delegate<void(LLBC_Event *)> &hook);

    /**
     * Remove post-fire hook using hook name.
     * @param[in] hookName  - hook name.
     */
    void RemovePostFireHook(const LLBC_String &hookName);

    /**
     * Remove all post-fire hook.
     */
    void RemoveAllPostFireHooks();

private:
    /**
     * Pre-fire event.
     * @param[in] event - event object.
     * @return int - success if return LLBC_OK, otherwise return LLBC_FAILED.
     */
    int PreFire(LLBC_Event *event);

    /**
     * Post-fire event.
     * @param[in] event - event object.
     */
    void PostFire(LLBC_Event *event);

private:
    // Handle pre-firing hook operations, including the addition and deletion of pre-fire.
    void HandlePreFiringHookOperations();

    // Handle post-firing hook operations, including the addition and deletion of post-fire.
    void HandlePostFiringHookOperations();

private:
    friend class LLBC_EventMgr;

    // Disable assignment&move-assignment.
    LLBC_DISABLE_ASSIGNMENT(LLBC_EventHookMgr);
    LLBC_DISABLE_MOVE_ASSIGNMENT(LLBC_EventHookMgr);

    // Pre-fire info.
    typedef std::pair<LLBC_String, LLBC_Delegate<bool(LLBC_Event *)>> _PreFireInfo;

    // Is pre-firing.
    int _preFiring;
    // Pre-fire hooks.
    std::list<_PreFireInfo> _preFireHookList;
    // Pre-fire hook map.
    std::map<LLBC_String, std::list<_PreFireInfo>::iterator> _preFireHookMap;
    // Removing pre-fire hook's names, these hooks should not process.
    std::set<LLBC_String> _preFireRemovingNameSet;

    // Post-fire info.
    typedef std::pair<LLBC_String, LLBC_Delegate<void(LLBC_Event *)>> _PostFireInfo;

    // Is post-firing.
    int _postFiring;
    // Post-fire hooks.
    std::list<_PostFireInfo> _postFireHookList;
    // Post-fire hook map.
    std::map<LLBC_String, std::list<_PostFireInfo>::iterator> _postFireHookMap;
    // Removing post-fire hook's names, these hooks should not process.
    std::set<LLBC_String> _postFireRemovingNameSet;
};

__LLBC_NS_END

#include "llbc/core/event/EventHookMgrInl.h"

#endif // LLBC_CFG_CORE_ENABLE_EVENT_HOOK
