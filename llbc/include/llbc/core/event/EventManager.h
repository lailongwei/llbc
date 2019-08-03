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

#ifndef __LLBC_CORE_EVENT_EVENT_MANAGER_H__
#define __LLBC_CORE_EVENT_EVENT_MANAGER_H__

#include "llbc/common/Common.h"
#include "llbc/core/utils/Common.h"

__LLBC_NS_BEGIN
class LLBC_Event;
__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The event manager class encapsulation.
 */
class LLBC_EXPORT LLBC_EventManager
{
public:
    /**
     * Event listener function  typedef.
     */
    typedef void (*Listener)(LLBC_Event *event);

public:
    LLBC_EventManager();
    virtual ~LLBC_EventManager();

public:
    /**
     * Add event listener.
     * @param[in] id         - event Id.
     * @param[in] listener   - event listener.
     * @param[in] bindedStub - the binded stub, if not specified, will auto gen stub.
     * @return LLBC_ListenerStub - return if failed, otherwise return validate stub.
     */
    virtual LLBC_ListenerStub AddListener(int id,
                                          Listener listener,
                                          const LLBC_ListenerStub &bindedStub = LLBC_INVALID_LISTENER_STUB);

    /**
     * Add event listener.
     * @param[in] id         - event Id.
     * @param[in] obj        - object.
     * @param[in] listener   - listener.
     * @param[in] bindedStub - the binded stub, if not specified, will auto gen stub.
     * @return LLBC_ListenerStub - return LLBC_INVALID_LISTENER_STUB if failed, otherwise return validate stub.
     */
    template <typename ObjectType>
    LLBC_ListenerStub AddListener(int id,
                                  ObjectType *obj,
                                  void (ObjectType::*listener)(LLBC_Event *),
                                  const LLBC_ListenerStub &bindedStub = LLBC_INVALID_LISTENER_STUB);

    /**
     * Add event listener.
     * @param[in] id         - event Id.
     * @param[in] listener   - event listener.
     * @param[in] bindedStub - the binded stub, if not specified, will auto gen stub.
     * @return LLBC_ListenerStub - return LLBC_INVALID_LISTENER_STUB if failed, otherwise return validate stub.
     */
    virtual LLBC_ListenerStub AddListener(int id,
                                          LLBC_IDelegate1<void, LLBC_Event *> *listener,
                                          const LLBC_ListenerStub &bindedStub = LLBC_INVALID_LISTENER_STUB);

    /**
     * Remove event listener.
     * @param[in] id - event Id.
     * @return int - success if return LLBC_OK, otherwise return LLBC_FAILED.
     *               specially, if return LLBC_FAILED,  and fetch the last error is pending,
     *               it means operation will success on later, but pending at now.
     */
    virtual int RemoveListener(int id);

    /**
     * Remove event listener using listener stub.
     * @param[in] stub - event listener stub.
     * @return int - success if return LLBC_OK, otherwise return LLBC_FAILED,
     *               specially, if return LLBC_FAILED, and fetch the last error is pending,
     *               it means operation will success on later, but pending at now.
     */
    virtual int RemoveListener(const LLBC_ListenerStub &stub);

    /**
     * Remove event listener using listener stub and clear the listener stub.
     * @param[in] stub - event listener stub.
     * @return int - success if return LLBC_OK, otherwise return LLBC_FAILED,
     *               specially, if return LLBC_FAILED, and fetch the last error is pending,
     *               it means operation will success on later, but pending at now.
     */
    virtual int RemoveListenerX(LLBC_ListenerStub &stub);

public:
    /**
     * Fire the event.
     * @param[in] event - event object.
     */
    virtual void FireEvent(LLBC_Event *event);

    /**
     * Check event manager is firing or not.
     * @return bool - firing flag.
     */
    virtual bool IsFiring() const;

protected:
    /**
     * \brief Wrap the event listener.
     */
    struct _Listener
    {
        LLBC_ListenerStub stub;

        int evId;
        Listener listener1;
        LLBC_IDelegate1<void, LLBC_Event *> *listener2;

        _Listener();
    };

    /**
     * \brief Wrap the event operation information.
     */
    struct _Op
    {
        bool addOp;
        _Listener listener;
    };

protected:
    /**
     * Search given listen stub in the event manager.
     */
    bool SearchStub(const LLBC_ListenerStub &stub) const;

    /**
     * Process event operation.
     */
    int ProcessEventOperation(_Op &op);

    /**
     * Before fire event method.
     */
    void BeforeFireEvent();

    /**
     * After fire event method.
     */
    void AfterFireEvent();

protected:
    int _firing;

    typedef std::vector<_Op> _DelayedOps;
    _DelayedOps _delayedOps;

    typedef std::vector<_Listener> _Listeners;
    typedef std::map<int, _Listeners> _ListenersMap;
    _ListenersMap _listeners;

    typedef std::map<LLBC_String, _Listener> _StubIndexedListeners;
    _StubIndexedListeners _stubListeners;
};

__LLBC_NS_END

#include "llbc/core/event/EventManagerImpl.h"

#endif // !__LLBC_CORE_EVENT_EVENT_MANAGER_H__
