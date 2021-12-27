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
        LLBC_Recycle(listener);
}

LLBC_EventManager::LLBC_EventManager()
: _firing(0)
, _maxListenerStub(0)
{
}

LLBC_EventManager::~LLBC_EventManager()
{
    for (_Id2ListenerInfos::iterator it = _id2ListenerInfos.begin();
         it != _id2ListenerInfos.end();
         ++it)
        LLBC_STLHelper::RecycleContainer(it->second);
}

LLBC_ListenerStub LLBC_EventManager::AddListener(int id,
                                                 const LLBC_Delegate<void(LLBC_Event &)> &listener,
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

    _Id2ListenerInfos::iterator idIt = _id2ListenerInfos.find(id);
    if (idIt == _id2ListenerInfos.end())
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

    _ListenerInfos &lis = idIt->second;
    for (_ListenerInfos::iterator it = lis.begin();
         it != lis.end();
         ++it)
        _stub2ListenerInfos.erase((*it)->stub);

    LLBC_STLHelper::RecycleContainer(idIt->second);
    _id2ListenerInfos.erase(idIt);

    return LLBC_OK;
}

int LLBC_EventManager::RemoveListener(const LLBC_ListenerStub &stub)
{
    if (stub == LLBC_INVALID_LISTENER_STUB)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    _Stub2ListenerInfos::iterator stubIt = _stub2ListenerInfos.find(stub);
    if (stubIt == _stub2ListenerInfos.end())
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
    _Id2ListenerInfos::iterator idIt = _id2ListenerInfos.find(evId);
    _ListenerInfos &lis = idIt->second;

    _ListenerInfos::iterator &liIt = stubIt->second.second;
    LLBC_Recycle(*liIt);
    lis.erase(liIt);
    if (lis.empty())
        _id2ListenerInfos.erase(idIt);

    _stub2ListenerInfos.erase(stubIt);

    return LLBC_OK;
}

void LLBC_EventManager::FireEvent(LLBC_Event *ev)
{
    BeforeFireEvent();

    _Id2ListenerInfos::iterator idIt = _id2ListenerInfos.find(ev->GetId());
    if (idIt != _id2ListenerInfos.end())
    {
        _ListenerInfos &lis = idIt->second;
        const _ListenerInfos::const_iterator lItEnd = lis.end();
        for (_ListenerInfos::const_iterator lIt = lis.begin();
             lIt != lItEnd;
             ++lIt)
        {
            const _ListenerInfo * const &li = *lIt;
            if (li->deleg)
                li->deleg(*ev);
            else
                li->listener->Invoke(*ev);
        }
    }

    if (!ev->IsDontDelAfterFire())
        LLBC_Recycle(ev);

    AfterFireEvent();
}

bool LLBC_EventManager::HasStub(const LLBC_ListenerStub &stub) const
{
    return _stub2ListenerInfos.find(stub) != _stub2ListenerInfos.end();
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
    auto &lis = _id2ListenerInfos[li->evId];
    lis.push_back(li);

    _stub2ListenerInfos[li->stub] = std::make_pair(li->evId, --lis.end());
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
