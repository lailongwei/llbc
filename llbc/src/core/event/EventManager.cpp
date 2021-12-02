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
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/helper/STLHelper.h"

#include "llbc/core/event/Event.h"
#include "llbc/core/event/EventManager.h"

__LLBC_NS_BEGIN

LLBC_EventManager::_ListenerInfo::_ListenerInfo()
: evId(0)
, stub(LLBC_INVALID_LISTENER_STUB)
, listener(nullptr)
{
}

LLBC_EventManager::_ListenerInfo::~_ListenerInfo()
{
    if (listener)
        LLBC_Delete(listener);
}

LLBC_EventManager::LLBC_EventManager()
: _firing(0)
, _maxListenerStub(0)
{
}

LLBC_EventManager::~LLBC_EventManager()
{
    for (_Id2Listeners::iterator it = _id2Listeners.begin();
         it != _id2Listeners.end();
         ++it)
        LLBC_STLHelper::DeleteContainer(it->second);
}

LLBC_ListenerStub LLBC_EventManager::AddListener(int id,
                                                 const LLBC_NewDelegate<void(LLBC_Event &)> &listener,
                                                 const LLBC_ListenerStub &boundStub)
{
    if (id <= 0 || !listener)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_INVALID_LISTENER_STUB;
    }

    LLBC_ListenerStub stub;
    if (AddListenerCheck(boundStub, stub) != LLBC_OK)
        return LLBC_INVALID_LISTENER_STUB;

    _ListenerInfo *li = LLBC_New(_ListenerInfo);
    li->evId = id;
    li->stub = stub;
    li->deleg = listener;

    AddListenerInfo(li);

    return stub;
}

LLBC_ListenerStub LLBC_EventManager::AddListener(int id,
                                                 LLBC_EventListener *listener,
                                                 const LLBC_ListenerStub &boundStub)
{
    if (id <= 0 || !listener)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_INVALID_LISTENER_STUB;
    }

    LLBC_ListenerStub stub;
    if (AddListenerCheck(boundStub, stub) != LLBC_OK)
        return LLBC_INVALID_LISTENER_STUB;

    _ListenerInfo *li = LLBC_New(_ListenerInfo);
    li->evId = id;
    li->stub = stub;
    li->listener = listener;

    AddListenerInfo(li);

    return stub;
}

int LLBC_EventManager::RemoveListener(int id)
{
    if (id <= 0)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    _Id2Listeners::iterator idIt = _id2Listeners.find(id);
    if (idIt == _id2Listeners.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    if (IsFiring())
    {
        if (!_pendingRemoveEvIds.insert(id).second)
            LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        else
            LLBC_SetLastError(LLBC_ERROR_PENDING);

        return LLBC_FAILED;
    }

    _Listeners &listeners = idIt->second;
    for (_Listeners::iterator it = listeners.begin();
         it != listeners.end();
         ++it)
        _stub2Listeners.erase((*it)->stub);

    LLBC_STLHelper::DeleteContainer(idIt->second);
    _id2Listeners.erase(idIt);

    return LLBC_OK;
}

int LLBC_EventManager::RemoveListener(const LLBC_ListenerStub &stub)
{
    if (stub == LLBC_INVALID_LISTENER_STUB)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    _Stub2Listeners::iterator stubIt = _stub2Listeners.find(stub);
    if (stubIt == _stub2Listeners.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    if (IsFiring())
    {
        if (!_pendingRemoveEvStubs.insert(stub).second)
            LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        else
            LLBC_SetLastError(LLBC_ERROR_PENDING);

        return LLBC_FAILED;
    }

    int &evId = stubIt->second.first;
    _Id2Listeners::iterator idIt = _id2Listeners.find(evId);
    _Listeners &listeners = idIt->second;

    _Listeners::iterator liIt = stubIt->second.second;
    LLBC_Delete(*liIt);
    listeners.erase(liIt);
    if (listeners.empty())
        _id2Listeners.erase(idIt);

    _stub2Listeners.erase(stubIt);

    return LLBC_OK;
}

void LLBC_EventManager::FireEvent(LLBC_Event *event)
{
    BeforeFireEvent();

    _Id2Listeners::iterator idIt = _id2Listeners.find(event->GetId());
    if (idIt != _id2Listeners.end())
    {
        _Listeners &listeners = idIt->second;
        const _Listeners::const_iterator lItEnd = listeners.end();
        for (_Listeners::const_iterator lIt = listeners.begin();
             lIt != lItEnd;
             ++lIt)
        {
            const _ListenerInfo * const &listener = *lIt;
            if (listener->deleg)
                listener->deleg(*event);
            else
                listener->listener->Invoke(*event);
        }
    }

    if (!event->IsDontDelAfterFire())
        LLBC_Delete(event);

    AfterFireEvent();
}

bool LLBC_EventManager::HasStub(const LLBC_ListenerStub &stub) const
{
    return _stub2Listeners.find(stub) != _stub2Listeners.end();
}

void LLBC_EventManager::BeforeFireEvent()
{
    ++_firing;
}

void LLBC_EventManager::AfterFireEvent()
{
    if (--_firing == 0)
    {
        if (!_pendingRemoveEvIds.empty())
        {
            for (std::set<int>::const_iterator it = _pendingRemoveEvIds.begin();
                 it != _pendingRemoveEvIds.end();
                 ++it)
                RemoveListener(*it);

            _pendingRemoveEvIds.clear();
        }

        if (!_pendingRemoveEvStubs.empty())
        {
            for (std::set<LLBC_ListenerStub>::const_iterator it = _pendingRemoveEvStubs.begin();
                 it != _pendingRemoveEvStubs.end();
                 ++it)
                RemoveListener(*it);

            _pendingRemoveEvStubs.clear();
        }
    }
}

int LLBC_EventManager::AddListenerCheck(const LLBC_ListenerStub &boundStub, LLBC_ListenerStub &stub)
{
    if (boundStub != LLBC_INVALID_LISTENER_STUB)
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
        stub = ++_maxListenerStub;
    }

    return LLBC_OK;
}

void LLBC_EventManager::AddListenerInfo(_ListenerInfo *li)
{
    auto &idListeners = _id2Listeners[li->evId];
    idListeners.push_back(li);

    _stub2Listeners[li->stub] = std::make_pair(li->evId, --idListeners.end());
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
