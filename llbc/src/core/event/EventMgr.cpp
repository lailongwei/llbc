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

#include "llbc/core/os/OS_Atomic.h"

#include "llbc/core/helper/STLHelper.h"

#include "llbc/core/objpool/ThreadSpecObjPool.h"

#include "llbc/core/event/EventFirer.h"
#if LLBC_CFG_CORE_ENABLE_EVENT_HOOK
#include "llbc/core/event/EventHookMgr.h"
#endif // LLBC_CFG_CORE_ENABLE_EVENT_HOOK
#include "llbc/core/event/EventMgr.h"

__LLBC_NS_BEGIN
    sint64 LLBC_EventMgr::_maxListenerStub = 1;

LLBC_EventMgr::_ListenerInfo::_ListenerInfo()
: evId(0)
, stub(0)
, listener(nullptr)
{
}

LLBC_EventMgr::_ListenerInfo::~_ListenerInfo()
{
    if (listener)
        LLBC_Recycle(listener);
}

LLBC_EventMgr::LLBC_EventMgr()
: _firing(0)
, _pendingRemoveAllListeners(false)
#if LLBC_CFG_CORE_ENABLE_EVENT_HOOK
, _eventHookMgr(nullptr)
#endif // LLBC_CFG_CORE_ENABLE_EVENT_HOOK
{
}

LLBC_EventMgr::~LLBC_EventMgr()
{
    #if LLBC_CFG_CORE_ENABLE_EVENT_HOOK
    if(_eventHookMgr)
        LLBC_XRecycle(_eventHookMgr);
    #endif // LLBC_CFG_CORE_ENABLE_EVENT_HOOK

    // Assert: Make sure not in firing when delete event mgr.
    ASSERT(!IsFiring() && "Not allow delete LLBC_EventMgr when event firing");
    // Assert: Make sure pending event operations is empty.
    ASSERT(_pendingEventOps.empty() && "llbc framework internal error: _pendingEventOps is not empty!");

    // Recycle all listener infos.
    for (auto it = _id2ListenerInfos.begin(); it != _id2ListenerInfos.end(); ++it)
        LLBC_STLHelper::RecycleContainer(it->second);
}

#if LLBC_CFG_CORE_ENABLE_EVENT_HOOK
LLBC_EventHookMgr &LLBC_EventMgr::GetEventHookMgr()
{
    if (!_eventHookMgr)
        _eventHookMgr = new LLBC_EventHookMgr();
    return *_eventHookMgr;
}
#endif // LLBC_CFG_CORE_ENABLE_EVENT_HOOK

LLBC_ListenerStub LLBC_EventMgr::AddListener(int id,
                                             const LLBC_Delegate<void(LLBC_Event &)> &listener,
                                             const LLBC_ListenerStub &boundStub)
{
    if (id <= 0 || !listener)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return 0;
    }

    LLBC_ListenerStub stub;
    if (AddListenerCheck(boundStub, stub) != LLBC_OK)
        return 0;

    _ListenerInfo *listenerInfo = new _ListenerInfo;
    listenerInfo->evId = id;
    listenerInfo->stub = stub;
    listenerInfo->deleg = listener;

    AddListenerInfo(listenerInfo);

    return stub;
}

LLBC_ListenerStub LLBC_EventMgr::AddListener(int id,
                                             LLBC_EventListener *listener,
                                             const LLBC_ListenerStub &boundStub)
{
    if (id <= 0 || !listener)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return 0;
    }

    LLBC_ListenerStub stub;
    if (AddListenerCheck(boundStub, stub) != LLBC_OK)
        return 0;

    _ListenerInfo *listenerInfo = new _ListenerInfo;
    listenerInfo->evId = id;
    listenerInfo->stub = stub;
    listenerInfo->listener = listener;

    AddListenerInfo(listenerInfo);

    return stub;
}

int LLBC_EventMgr::RemoveListener(int id)
{
    // Event id check.
    if (UNLIKELY(id <= 0))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    // If in firing, pending operation.
    if (IsFiring())
    {
        _PendingEventOp *pendingOp = new _PendingEventOp(_PendingEventOp::RemoveListenersById);
        pendingOp->opInfo.eventId = id;
        _pendingEventOps.push_back(pendingOp);
        _pendingRemoveEventIds_.insert(id);

        LLBC_SetLastError(LLBC_ERROR_PENDING);

        return LLBC_FAILED;
    }

    // Find event listeners and remove it.
    const auto idIt = _id2ListenerInfos.find(id);
    if (idIt == _id2ListenerInfos.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    _ListenerInfos &listenerInfos = idIt->second;
    for (auto it = listenerInfos.begin(); it != listenerInfos.end(); ++it)
        _stub2ListenerInfos.erase((*it)->stub);

    LLBC_STLHelper::RecycleContainer(listenerInfos);
    _id2ListenerInfos.erase(idIt);

    return LLBC_OK;
}

int LLBC_EventMgr::RemoveListener(const LLBC_ListenerStub &stub)
{
    // Event stub check.
    if (UNLIKELY(stub == 0))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    // If in firing, pending operation.
    if (IsFiring())
    {
        _PendingEventOp *pendingOp = new _PendingEventOp(_PendingEventOp::RemoveListenerByStub);
        pendingOp->opInfo.eventStub = stub;
        _pendingEventOps.push_back(pendingOp);
        _pendingRemoveStubs_.insert(stub);

        LLBC_SetLastError(LLBC_ERROR_PENDING);

        return LLBC_FAILED;
    }

    // Find listener and remove it.
    const auto stubIt = _stub2ListenerInfos.find(stub);
    if (stubIt == _stub2ListenerInfos.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    const int evId = stubIt->second.first;
    const auto idIt = _id2ListenerInfos.find(evId);
    _ListenerInfos &listenerInfos = idIt->second;

    _ListenerInfos::iterator &listenerIt = stubIt->second.second;
    LLBC_Recycle(*listenerIt);
    listenerInfos.erase(listenerIt);
    if (listenerInfos.empty())
        _id2ListenerInfos.erase(idIt);

    _stub2ListenerInfos.erase(stubIt);

    return LLBC_OK;
}

int LLBC_EventMgr::RemoveAllListeners()
{
    if (IsFiring())
    {
        _pendingEventOps.push_back(new _PendingEventOp(_PendingEventOp::RemoveAllListeners));
        _pendingRemoveAllListeners = true;

        LLBC_SetLastError(LLBC_ERROR_PENDING);

        return LLBC_FAILED;
    }

    _stub2ListenerInfos.clear();
    for (auto &listenerInfosItem : _id2ListenerInfos)
        LLBC_STLHelper::RecycleContainer(listenerInfosItem.second);

    return LLBC_OK;
}

int LLBC_EventMgr::Fire(LLBC_Event *ev)
{
    // Prevent use defer syntax to optimize Fire() method performance.
    // LLBC_Defer(LLBC_DoIf(!ev->IsDontDelAfterFire(), LLBC_Recycle(ev)));

    // Firstly, if in firing, check will fire event id has been remove or not.
    if (IsFiring() &&
        (_pendingRemoveAllListeners ||
         (!_pendingRemoveEventIds_.empty() &&
          _pendingRemoveEventIds_.find(ev->GetId()) != _pendingRemoveEventIds_.end())))
    {
        if (!ev->IsDontDelAfterFire())
            LLBC_Recycle(ev);
        return LLBC_OK;
    }

    // Do before fire event logic.
    const int ret = BeforeFireEvent(ev);
    if (UNLIKELY(ret != LLBC_OK))
    {
        if (!ev->IsDontDelAfterFire())
            LLBC_Recycle(ev);
        return LLBC_FAILED;
    }

    // Call all listeners.
    const auto idIt = _id2ListenerInfos.find(ev->GetId());
    if (idIt != _id2ListenerInfos.end() && !idIt->second.empty())
    {
        for (auto &listenerInfo : idIt->second)
        {
            if (!_pendingRemoveStubs_.empty() &&
                _pendingRemoveStubs_.find(listenerInfo->stub) != _pendingRemoveStubs_.end())
                continue;

            if (listenerInfo->deleg)
                listenerInfo->deleg(*ev);
            else
                listenerInfo->listener->Invoke(*ev);
        }
    }

    // Do after fire event logic.
    AfterFireEvent(ev);

    // Recycle event.
    if (!ev->IsDontDelAfterFire())
        LLBC_Recycle(ev);

    return LLBC_OK;
}

LLBC_EventFirer LLBC_EventMgr::BeginFire(int evId)
{
    LLBC_Event *ev = LLBC_ThreadSpecObjPool::UnsafeAcquire<LLBC_Event>();
    ev->SetId(evId);

    return {this, ev};
}

bool LLBC_EventMgr::HasStub(const LLBC_ListenerStub &stub) const
{
    return _stub2ListenerInfos.find(stub) != _stub2ListenerInfos.end();
}

int LLBC_EventMgr::BeforeFireEvent(LLBC_Event *ev)
{
    // If event id already in firing, set <REPEAT> error and return failed.
    #if LLBC_CFG_CORE_ENABLE_EVENT_FIRE_DEAD_LOOP_DETECTION
    if (!_firingEventIds.empty() &&
        std::find(_firingEventIds.begin(), _firingEventIds.end(), ev->GetId()) != _firingEventIds.end())
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }
    #endif // LLBC_CFG_CORE_ENABLE_EVENT_FIRE_DEAD_LOOP_DETECTION

    // The event hook manager do pre-fire, stop continuing fire when the pre-fire execution fails.
    #if LLBC_CFG_CORE_ENABLE_EVENT_HOOK
    if (_eventHookMgr && _eventHookMgr->PreFire(ev) == LLBC_FAILED)
        return LLBC_FAILED;
    #endif // LLBC_CFG_CORE_ENABLE_EVENT_HOOK

    // Increase firing flag.
    ++_firing;

    // Add event id to _firingEventIds set.
    #if LLBC_CFG_CORE_ENABLE_EVENT_FIRE_DEAD_LOOP_DETECTION
    _firingEventIds.push_back(ev->GetId());
    #endif // LLBC_CFG_CORE_ENABLE_EVENT_FIRE_DEAD_LOOP_DETECTION
    return LLBC_OK;
}

void LLBC_EventMgr::AfterFireEvent(LLBC_Event *ev)
{
    // Remove event id from _firingEventIds list.
    #if LLBC_CFG_CORE_ENABLE_EVENT_FIRE_DEAD_LOOP_DETECTION
    _firingEventIds.resize(_firingEventIds.size() - 1);
    #endif // LLBC_CFG_CORE_ENABLE_EVENT_FIRE_DEAD_LOOP_DETECTION

    // Decrease firing flag.
    if (--_firing != 0)
        return;

    // Assert: make sure _firing >= 0.
    ASSERT(_firing >= 0 && "llbc framework internal error: LLBC_EventMgr._firing < 0");
    // Assert: make sure _firingEventIds is empty.
    #if LLBC_CFG_CORE_ENABLE_EVENT_FIRE_DEAD_LOOP_DETECTION
    ASSERT(_firingEventIds.empty() && "llbc framework internal error: LLBC_EventMgr._firingEventIds is not empty!");
    #endif // LLBC_CFG_CORE_ENABLE_EVENT_FIRE_DEAD_LOOP_DETECTION

    // The event hook manager do post-fire.
    #if LLBC_CFG_CORE_ENABLE_EVENT_HOOK
    if (_eventHookMgr)
        _eventHookMgr->PostFire(ev);
    #endif // LLBC_CFG_CORE_ENABLE_EVENT_HOOK


    // Process pending event ops(add/remove).
    for (auto &pendingEventOp : _pendingEventOps)
    {
        const auto opType = pendingEventOp->opType;
        if (opType == _PendingEventOp::AddListener)
        {
            AddListenerInfo(pendingEventOp->opInfo.listenerInfo);
            pendingEventOp->opInfo.listenerInfo = nullptr;
        }
        else if (opType == _PendingEventOp::RemoveAllListeners)
        {
            RemoveAllListeners();
        }
        else if (opType == _PendingEventOp::RemoveListenersById)
        {
            RemoveListener(pendingEventOp->opInfo.eventId);
        }
        else // RemoveListenerByStub
        {
            RemoveListener(pendingEventOp->opInfo.eventStub);
        }
    }

    LLBC_STLHelper::DeleteContainer(_pendingEventOps);
    _pendingRemoveAllListeners = false;
    _pendingRemoveEventIds_.clear();
    _pendingRemoveStubs_.clear();
}

LLBC_EventMgr::_PendingEventOp::_PendingEventOp(_PendingEventOpType opType)
: opType(opType)
, opInfo{}
{
}

LLBC_EventMgr::_PendingEventOp::~_PendingEventOp()
{
    if (opType == AddListener && opInfo.listenerInfo)
        delete opInfo.listenerInfo;
}

int LLBC_EventMgr::AddListenerCheck(const LLBC_ListenerStub &boundStub, LLBC_ListenerStub &stub)
{
    if (boundStub != 0)
    {
        if (HasStub(boundStub))
        {
            LLBC_SetLastError(LLBC_ERROR_REPEAT);
            return LLBC_FAILED;
        }

        stub = boundStub;
    }
    else
    {
        stub = static_cast<LLBC_ListenerStub>(
            LLBC_AtomicFetchAndAdd(&_maxListenerStub, 1));
        ASSERT(stub != 0 && "Event stubs are exhausted");
    }

    return LLBC_OK;
}

int LLBC_EventMgr::AddListenerInfo(_ListenerInfo *listenerInfo)
{
    if (IsFiring())
    {
        _PendingEventOp *pendingOp = new _PendingEventOp(_PendingEventOp::AddListener);
        pendingOp->opInfo.listenerInfo = listenerInfo;
        _pendingEventOps.push_back(pendingOp);

        LLBC_SetLastError(LLBC_ERROR_PENDING);

        return LLBC_FAILED;
    }

    auto &listenerInfos = _id2ListenerInfos[listenerInfo->evId];
    listenerInfos.push_back(listenerInfo);

    _stub2ListenerInfos[listenerInfo->stub] = std::make_pair(listenerInfo->evId, --listenerInfos.end());

    return LLBC_OK;
}

__LLBC_NS_END
