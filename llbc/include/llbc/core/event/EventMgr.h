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

#include "llbc/core/utils/Util_Delegate.h"

__LLBC_NS_BEGIN
class LLBC_Event;
class LLBC_EventFirer;
#if LLBC_CFG_CORE_ENABLE_EVENT_HOOK
class LLBC_EventHookMgr;
#endif // LLBC_CFG_CORE_ENABLE_EVENT_HOOK
__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The event manager class encapsulation.
 */
class LLBC_EXPORT LLBC_EventMgr final
{
public:
    /**
     * Ctor & Dtor.
     */
    LLBC_EventMgr();
    ~LLBC_EventMgr();

public:
    /**
     * Add event deleg.
     * @param[in] id         - event Id.
     * @param[in] obj        - object.
     * @param[in] listener   - deleg.
     * @param[in] boundStub - the binded stub, if not specified, will auto gen stub.
     * @return LLBC_ListenerStub - return LLBC_INVALID_LISTENER_STUB if failed, otherwise return validate stub.
     */
    template <typename ObjectType>
    LLBC_ListenerStub AddListener(int id,
                                  ObjectType *obj,
                                  void (ObjectType::*listener)(LLBC_Event &),
                                  const LLBC_ListenerStub &boundStub = 0);
    /**
     * Add event deleg.
     * @param[in] id        - event Id.
     * @param[in] listener  - event deleg.
     * @param[in] boundStub - the bound stub, if not specified, will auto gen stub.
     * @return LLBC_ListenerStub - return LLBC_INVALID_LISTENER_STUB if failed, otherwise return validate stub.
     */
    LLBC_ListenerStub AddListener(int id,
                                  const LLBC_Delegate<void(LLBC_Event &)> &listener,
                                  const LLBC_ListenerStub &boundStub = 0);

    /**
     * Add event listener.
     * @param[in] id         - event Id.
     * @param[in] listener   - event listener.
     * @param[in]] boundStub - bound listener stub, if specific, will use bound stub.
     * @return LLBC_ListenerStub - return LLBC_INAVLID_LISTENER_STUB if failed, otherwise return validate stub.
     */
    LLBC_ListenerStub AddListener(int id,
                                  LLBC_EventListener *listener,
                                  const LLBC_ListenerStub &boundStub = 0);

    /**
     * Remove event deleg.
     * @param[in] id - event Id.
     * @return int - success if return LLBC_OK, otherwise return LLBC_FAILED.
     *               specially, if return LLBC_FAILED,  and fetch the last error is pending,
     *               it means operation will success on later, but pending at now.
     */
    int RemoveListener(int id);

    /**
     * Remove event deleg using deleg stub.
     * @param[in] stub - event deleg stub.
     * @return int - success if return LLBC_OK, otherwise return LLBC_FAILED,
     *               specially, if return LLBC_FAILED, and fetch the last error is pending,
     *               it means operation will success on later, but pending at now.
     */
    int RemoveListener(const LLBC_ListenerStub &stub);

    /**
     * Remove event deleg using deleg stub and clear the deleg stub.
     * @param[in] stub - event deleg stub.
     * @return int - success if return LLBC_OK, otherwise return LLBC_FAILED,
     *               specially, if return LLBC_FAILED, and fetch the last error is pending,
     *               it means operation will success on later, but pending at now.
     */
    int RemoveListenerX(LLBC_ListenerStub &stub);

    /**
     * Remove all listeners.
     * @return int - return 0 if success, otherwise return -1,
     *               specially, if return LLBC_FAILED, and fetch the last error is pending,
     *               it means operation will success on later, but pending at now.
     */
    int RemoveAllListeners();

public:
    /**
     * Fire event.
     * @param[in] ev - event object.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Fire(LLBC_Event *ev);

    /**
     * Begin fire event.
     * @param[in] evId - the event id.
     * @return LLBC_EventFirer & - the event firer object.
     */
    LLBC_EventFirer BeginFire(int evId);

    /**
     * Check event manager is firing or not.
     * @return bool - firing flag.
     */
    bool IsFiring() const;

    #if LLBC_CFG_CORE_ENABLE_EVENT_HOOK
    /**
     * Get event hook manager object.
     * @return LLBC_EventHookMgr - create event hook manager object if _eventHookMgr nullptr, and return *_eventHookMgr.
     */
    LLBC_EventHookMgr &GetEventHookMgr();
    #endif // LLBC_CFG_CORE_ENABLE_EVENT_HOOK

protected:
    /**
     * Check given listen stub in the event manager exist or not.
     */
    bool HasStub(const LLBC_ListenerStub &stub) const;

    /**
     * Before fire event method.
     */
    int BeforeFireEvent(LLBC_Event *ev);

    /**
     * After fire event method.
     */
    void AfterFireEvent(LLBC_Event *ev);

protected:
    /**
     * \brief The event listener info encapsulation.
     */
    struct _ListenerInfo
    {
        int evId;
        LLBC_ListenerStub stub;
        LLBC_EventListener *listener;
        LLBC_Delegate<void(LLBC_Event &)> deleg;

        _ListenerInfo();
        ~_ListenerInfo();
    };

    /**
     * \brief The pending event operation structure.
     */
    struct _PendingEventOp
    {
        enum _PendingEventOpType
        {
            AddListener,
            RemoveAllListeners,
            RemoveListenersById,
            RemoveListenerByStub,
        };

        _PendingEventOpType opType;
        union
        {
            _ListenerInfo *listenerInfo;
            int eventId;
            LLBC_ListenerStub eventStub;
        } opInfo;

        explicit _PendingEventOp(_PendingEventOpType opType);
        ~_PendingEventOp();
    };

private:
    // Disable assignment.
    LLBC_DISABLE_ASSIGNMENT(LLBC_EventMgr);

    // Add listener check.
    int AddListenerCheck(const LLBC_ListenerStub &boundStub, LLBC_ListenerStub &stub);

    // Add listener info to event manager.
    int AddListenerInfo(_ListenerInfo *listenerInfo);

protected:
    typedef std::list<_ListenerInfo *> _ListenerInfos; // listener info list.

    int _firing; // Firing flag.
    #if LLBC_CFG_CORE_ENABLE_EVENT_FIRE_DEAD_LOOP_DETECTION 
    std::vector<int> _firingEventIds; // Firing event ids, used for event fire dead loop detection.
    #endif // LLBC_CFG_CORE_ENABLE_EVENT_FIRE_DEAD_LOOP_DETECTION 
    static sint64 _maxListenerStub; // Max listener stub.

    std::map<int, _ListenerInfos> _id2ListenerInfos; // event id 2 listeners.
    std::map<LLBC_ListenerStub, std::pair<int, _ListenerInfos::iterator> > _stub2ListenerInfos; // stub id 2 listeners.

    // Pending operations when event firing.
    // All pending event operations(Add/Remove(by event id)/Remove(by listener stub).
    std::vector<_PendingEventOp *> _pendingEventOps;
    // Pending remove all event listeners flag, used for prevent event firing in event firing.
    bool _pendingRemoveAllListeners;
    // Pending remove event ids, used for prevent event firing in event firing.
    std::set<int> _pendingRemoveEventIds_;
    // Pending remove event stubs, used for prevent event firing in event firing.
    std::set<LLBC_ListenerStub> _pendingRemoveStubs_;

    #if LLBC_CFG_CORE_ENABLE_EVENT_HOOK
    // Event hook manager object, object will create when use GetEventHookMgr.
    LLBC_EventHookMgr *_eventHookMgr;
    #endif // LLBC_CFG_CORE_ENABLE_EVENT_HOOK
};

__LLBC_NS_END

#include "llbc/core/event/EventMgrInl.h"


