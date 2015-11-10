/**
 * @file    EventManager.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/28
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/helper/STLHelper.h"
#include "llbc/core/helper/GUIDHelper.h"

#include "llbc/core/event/Event.h"
#include "llbc/core/event/EventManager.h"

__LLBC_NS_BEGIN

LLBC_EventManager::_Listener::_Listener()
: stub()

, evId(0)
, listener1(NULL)
, listener2(NULL)
{
}

LLBC_EventManager::LLBC_EventManager()
: _firing(0)
{
}

LLBC_EventManager::~LLBC_EventManager()
{
    for (_DelayedOps::iterator it = _delayedOps.begin();
         it != _delayedOps.end();
         it++)
    {
        _Op &op = *it;
        if (op.listener.listener2)
            delete op.listener.listener2;
    }

    for (_ListenersMap::iterator mIt = _listeners.begin();
         mIt != _listeners.end();
         mIt++)
    {
        _Listeners listeners = mIt->second;
        for (_Listeners::iterator lIt = listeners.begin();
             lIt != listeners.end();
             lIt++)
        {
            _Listener &listener = *lIt;
            if (listener.listener2)
                delete listener.listener2;
        }
    }
}

LLBC_ListenerStub LLBC_EventManager::AddListener(int id, Listener listener, const LLBC_ListenerStub &bindedStub)
{
    if (id <= 0 || listener == NULL)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_INVALID_LISTENER_STUB;
    }

    LLBC_String stub;
    if (bindedStub != LLBC_INVALID_LISTENER_STUB)
    {
        if (this->SearchStub(bindedStub))
        {
            LLBC_SetLastError(LLBC_ERROR_REPEAT);
            return LLBC_INVALID_LISTENER_STUB;
        }

        stub = bindedStub;
    }
    else
        stub = LLBC_GUIDHelper::GenStr();

    _Op op;
    op.addOp = true;
    op.listener.evId = id;
    op.listener.listener1 = listener;
    op.listener.stub = stub;

    if (this->IsFiring())
    {
        _delayedOps.push_back(op);

        LLBC_SetLastError(LLBC_ERROR_PENDING);
        return op.listener.stub.c_str();
    }

    if (this->ProcessEventOperation(op) != LLBC_RTN_OK)
        return LLBC_INVALID_LISTENER_STUB;

    return op.listener.stub.c_str();
}

LLBC_ListenerStub LLBC_EventManager::AddListener(int id,
                                                 LLBC_IDelegate1<LLBC_Event *> *listener,
                                                 const LLBC_ListenerStub &bindedStub)
{
    if (id <= 0 || listener == NULL)
    {
        LLBC_XDelete(listener);

        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_INVALID_LISTENER_STUB;
    }

    LLBC_String stub;
    if (bindedStub != LLBC_INVALID_LISTENER_STUB)
    {
        if (this->SearchStub(bindedStub))
        {
            delete listener;

            LLBC_SetLastError(LLBC_ERROR_REPEAT);
            return LLBC_INVALID_LISTENER_STUB;
        }

        stub = bindedStub;
    }
    else
        stub = LLBC_GUIDHelper::GenStr();

    _Op op;
    op.addOp = true;
    op.listener.evId = id;
    op.listener.listener2 = listener;
    op.listener.stub = stub;

    if (this->IsFiring())
    {
        _delayedOps.push_back(op);

        LLBC_SetLastError(LLBC_ERROR_PENDING);
        return LLBC_INVALID_LISTENER_STUB;
    }

    if (this->ProcessEventOperation(op) != LLBC_RTN_OK)
    {
        LLBC_XDelete(listener);
        return LLBC_INVALID_LISTENER_STUB;
    }

    return op.listener.stub.c_str();
}

int LLBC_EventManager::RemoveListener(int id)
{
    if (id <= 0)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }

    _Op op;
    op.addOp = false;
    op.listener.evId = id;

    if (this->IsFiring())
    {
        _delayedOps.push_back(op);

        LLBC_SetLastError(LLBC_ERROR_PENDING);
        return LLBC_RTN_FAILED;
    }

    return this->ProcessEventOperation(op);
}

int LLBC_EventManager::RemoveListener(const LLBC_ListenerStub &stub)
{
    _Op op;
    op.addOp = false;
    op.listener.stub.append(stub);

    if (this->IsFiring())
    {
        _delayedOps.push_back(op);

        LLBC_SetLastError(LLBC_ERROR_PENDING);
        return LLBC_RTN_FAILED;
    }

    return ProcessEventOperation(op);
}

void LLBC_EventManager::FireEvent(LLBC_Event *event)
{
    this->BeforeFireEvent();

    _ListenersMap::iterator mIt = _listeners.find(event->GetId());
    if (mIt != _listeners.end())
    {
        _Listeners &listeners = mIt->second;
        for (_Listeners::iterator lIt = listeners.begin();
             lIt != listeners.end();
             lIt++)
        {
            _Listener &listener = *lIt;
            if (listener.listener1)
                (*listener.listener1)(event);
            else
                listener.listener2->Invoke(event);
        }
    }

    delete event;
    this->AfterFireEvent();
}

bool LLBC_EventManager::IsFiring() const
{
    return _firing > 0;
}

bool LLBC_EventManager::SearchStub(const LLBC_ListenerStub &stub) const
{
    return (_stubListeners.find(stub) != _stubListeners.end()) ? true : false;
}

int LLBC_EventManager::ProcessEventOperation(LLBC_EventManager::_Op &op)
{
    _Listener &listener = op.listener;
    if (op.addOp)
    {
        _ListenersMap::iterator mIt = _listeners.find(listener.evId);
        if (mIt == _listeners.end())
            mIt = _listeners.insert(std::make_pair(listener.evId, std::vector<_Listener>())).first;

        _Listeners &listeners = mIt->second;
        listeners.push_back(listener);

        _stubListeners.insert(std::make_pair(listener.stub, listener));
    }
    else
    {
        if (listener.evId > 0)
        {
            _ListenersMap::iterator mIt = _listeners.find(listener.evId);
            if (mIt == _listeners.end())
            {
                LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
                return LLBC_RTN_FAILED;
            }

            _Listeners &listeners = mIt->second;
            for (_Listeners::iterator lIt = listeners.begin();
                 lIt != listeners.end();
                 lIt++)
            {
                _Listener &l = *lIt;

                _stubListeners.erase(l.stub);
                LLBC_XDelete(l.listener2);
            }

            _listeners.erase(mIt);
        }
        else
        {
            _StubIndexedListeners::iterator stubIt = _stubListeners.find(listener.stub);
            if (stubIt == _stubListeners.end())
            {
                LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
                return LLBC_RTN_FAILED;
            }
            
            const int evId = stubIt->second.evId;
            _ListenersMap::iterator mIt = _listeners.find(evId);

            _Listeners &listeners = mIt->second;
            for (_Listeners::iterator lIt = listeners.begin();
                 lIt != listeners.end();
                 lIt++)
            {
                _Listener &l = *lIt;
                if (l.stub == listener.stub)
                {
                    if (l.listener2)
                        LLBC_XDelete(l.listener2);

                    listeners.erase(lIt);
                    break;
                }
            }

            if (listeners.empty())
                _listeners.erase(mIt);

            _stubListeners.erase(stubIt);
        }
    }

    return LLBC_RTN_OK;
}

void LLBC_EventManager::BeforeFireEvent()
{
    ++_firing;
}

void LLBC_EventManager::AfterFireEvent()
{
    if (--_firing == 0)
    {
        for (size_t i = 0; i < _delayedOps.size(); i++)
            this->ProcessEventOperation(_delayedOps[i]);

        _delayedOps.clear();
    }
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
