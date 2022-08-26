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

#include "llbc/core/objectpool/ObjectPool.h"

#include "llbc/core/thread/MessageBlock.h"
#include "llbc/core/thread/MessageQueue.h"

__LLBC_NS_BEGIN

LLBC_MessageQueue::LLBC_MessageQueue()
: _head(nullptr)
, _tail(nullptr)

, _size(0)
{
}

LLBC_MessageQueue::~LLBC_MessageQueue()
{
    Cleanup();
}

bool LLBC_MessageQueue::PopAll(LLBC_MessageBlock *&blocks)
{
    _lock.Lock();
    if (_head)
    {
        blocks = _head;

        _head = _tail = nullptr;
        _size = 0;

        _lock.Unlock();
        return true;
    }

    _lock.Unlock();
    return false;
}

void LLBC_MessageQueue::Cleanup()
{
    _lock.Lock();

    if (!_head)
    {
        _lock.Unlock();
        return;
    }

    while (_head)
    {
        LLBC_MessageBlock *block = _head;
        _head = _head->GetNext();

        LLBC_Recycle(block);
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    _cond.Broadcast();
#else // LLBC_TARGET_PLATFORM_WIN32
    _sem.Post(static_cast<int>(_size));
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

    _size = 0;
    _tail = nullptr;

    _lock.Unlock();
}

void LLBC_MessageQueue::Push(LLBC_MessageBlock *block, bool front)
{
    _lock.Lock();
    front ? PushFrontNonLock(block) : PushBackNonLock(block);
    _lock.Unlock();

#if LLBC_TARGET_PLATFORM_NON_WIN32
    _cond.Notify();
#else // LLBC_TARGET_PLATFORM_WIN32
    _sem.Post();
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

LLBC_FORCE_INLINE void LLBC_MessageQueue::PushFrontNonLock(LLBC_MessageBlock *block)
{
    block->SetPrev(nullptr);
    if (!_head)
    {
        block->SetNext(nullptr);
        _head = _tail = block;
    }
    else
    {
        _head->SetPrev(block);
        block->SetNext(_head);
        _head = block;
    }

    _size += 1;
}

LLBC_FORCE_INLINE void LLBC_MessageQueue::PushBackNonLock(LLBC_MessageBlock *block)
{
    block->SetNext(nullptr);
    if (!_tail)
    {
        block->SetPrev(nullptr);
        _tail = _head = block;
    }
    else
    {
        _tail->SetNext(block);
        block->SetPrev(_tail);
        _tail = block;
    }

    _size += 1;
}

bool LLBC_MessageQueue::Pop(LLBC_MessageBlock *&block, int interval, bool front)
{
    #if LLBC_TARGET_PLATFORM_NON_WIN32
    if (interval != 0)
    {
        block = nullptr;
        while (!block)
        {
            _lock.Lock();
            if (_size > 0)
            {
                front ? PopFrontNonLock(block) : PopBackNonLock(block);
                _lock.Unlock();

                return true;
            }

            _cond.TimedWait(_lock, interval);
            if (_size > 0)
            {
                front ? PopFrontNonLock(block) : PopBackNonLock(block);
                _lock.Unlock();

                return true;
            }

            _lock.Unlock();
            if (interval != LLBC_INFINITE)
                break;
        }

        return false;
    }
    else // interval == 0
    {
        if (_size == 0)
            return false;

        _lock.Lock();
        if (_size > 0)
        {
            front ? PopFrontNonLock(block) : PopBackNonLock(block);
            _lock.Unlock();

            return true;
        }

        _lock.Unlock();
        return false;
    }
    #else // Win32
    if (interval == LLBC_INFINITE)
    {
        while (true)
        {
            _sem.Wait();

            _lock.Lock();
            if (_size > 0)
            {
                front ? PopFrontNonLock(block) : PopBackNonLock(block);
                _lock.Unlock();

                return true;
            }

            _lock.Unlock();
        }
    }
    else if (interval == 0)
    {
        if (_size == 0)
            return false;

        if (_sem.TryWait())
        {
            if (_size == 0)
                return false;

            _lock.Lock();
            if (_size > 0)
            {
                front ? PopFrontNonLock(block) : PopBackNonLock(block);
                _lock.Unlock();

                return true;
            }

            _lock.Unlock();
        }

        return false;
    }
    else // interval != INFINITE && interval != 0
    {
        if (_sem.TimedWait(interval))
        {
            if (_size == 0)
                return false;

            _lock.Lock();
            if (_size > 0)
            {
                front ? PopFrontNonLock(block) : PopBackNonLock(block);
                _lock.Unlock();

                return true;
            }

            _lock.Unlock();
        }

        return false;
    }
    #endif // Non-Win32
}

LLBC_FORCE_INLINE void LLBC_MessageQueue::PopFrontNonLock(LLBC_MessageBlock *&block)
{
    block = _head;
    if (!(_head = _head->GetNext()))
        _tail = nullptr;
    else
        _head->SetPrev(nullptr);

    _size -= 1;
}

LLBC_FORCE_INLINE void LLBC_MessageQueue::PopBackNonLock(LLBC_MessageBlock *&block)
{
    block = _tail;
    if (!(_tail = _tail->GetPrev()))
        _head = nullptr;
    else
        _tail->SetNext(nullptr);

    _size -= 1;
}

__LLBC_NS_END
