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

#if LLBC_TARGET_PLATFORM_NON_WIN32
#include "llbc/core/thread/SimpleLock.h"
#include "llbc/core/thread/ConditionVariable.h"
#else // LLBC_TARGET_PLATFORM_WIN32
#include "llbc/core/thread/SpinLock.h"
#include "llbc/core/thread/Semaphore.h"
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

__LLBC_NS_BEGIN
class LLBC_MessageBlock;
__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The thread message queue class encapsulation.
 */
class LLBC_EXPORT LLBC_MessageQueue
{
public:
    LLBC_MessageQueue();
    ~LLBC_MessageQueue();

public:
    /**
     * Insert new message block at the begin of the controlled sequence.
     * @param[in] block - message block.
     */
    void PushFront(LLBC_MessageBlock *block);

    /**
     * Insert new message block at the end of the controlled sequence.
     * @param[in] block - message block.
     */
    void PushBack(LLBC_MessageBlock *block);

public:
    /**
     * Pop all message blocks.
     * @param[out] blocks - the message blocks.
     * @return bool - return true if has block(s), otherwise return false.
     */
    bool PopAll(LLBC_MessageBlock *&blocks);

    /**
     * Fetch and remove the first message block of the controlled sequence.
     * @param[out] block - message block.
     */
    void PopFront(LLBC_MessageBlock *&block);

    /**
     * Fetch and remove the last message block of the controlled sequence.
     * @param[out] block - message block.
     */
    void PopBack(LLBC_MessageBlock *&block);

    /**
     * Try fetch and remove the first message block.
     * @param[out] block - message block.
     * @return bool - return true if success, otherwise return false.
     */
    bool TryPopFront(LLBC_MessageBlock *&block);

    /**
     * Try fetch and remove the last message block.
     * @param[out] block - message block.
     * @return bool - return true if success, otherwise return false.
     */
    bool TryPopBack(LLBC_MessageBlock *&block);

    /**
     * Timed fetch and remove the first message block.
     * @param[out] block   - message block.
     * @param[in] interval - interval, in milliseconds.
     * @return bool - return true if success, otherwise return false.
     */
    bool TimedPopFront(LLBC_MessageBlock *&block, int interval);

    /**
     * Time fetch and remove the last message block.
     * @param[out] block   - message block.
     * @param[in] interval - interval, in milliseconds.
     * @return bool - return true if success, otherwise return false.
     */
    bool TimedPopBack(LLBC_MessageBlock *&block, int interval);

public:
    /**
     * Get the message block current size.
     * @return size_t - current size.
     */
    size_t GetSize() const;

    /**
     * Cleanup the message queue.
     */
    void Cleanup();

private:
    /**
     * Insert the message of the controlled sequence.
     * @param[in] block    - message block.
     * @param[in] front    - front flag, if false, will append to tail.
     */
    void Push(LLBC_MessageBlock *block, bool front);

    /**
     * Insert the message block at the begin of the controlled sequence, non lock.
     * @param[in] block - message block.
     */
    void PushFrontNonLock(LLBC_MessageBlock *block);

    /**
     * Insert the message block at the end of the controlled sequence, non lock.
     * @param[in] block - message block.
     */
    void PushBackNonLock(LLBC_MessageBlock *block);

private:
    /**
     * Popt the message block of the controlled sequence.
     * @param[out] block    - message block.
     * @param[in]  interval - interval value, in milliseconds.
     * @param[in]  front    - front flag, if false, will fetch from tail.
     * @return bool - return true if success, otherwise return false.
     */
    bool Pop(LLBC_MessageBlock *&block, int interval, bool front);

    /**
     * Pop the first mesage block of the controlled sequence, non lock.
     * @param[out] block - message block.
     */
    void PopFrontNonLock(LLBC_MessageBlock *&block);

    /**
     * Pop the last message block of the controlled sequence, non lock.
     * @param[out] block - message block.
     */
    void PopBackNonLock(LLBC_MessageBlock *&block);

public:
#if LLBC_TARGET_PLATFORM_NON_WIN32
    LLBC_SimpleLock _lock;
    LLBC_ConditionVariable _cond;
#else // LLBC_TARGET_PLATFORM_WIN32
    LLBC_SpinLock _lock;
    LLBC_Semaphore _sem;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

    LLBC_MessageBlock *_head;
    LLBC_MessageBlock *_tail;

    volatile size_t _size;
};

__LLBC_NS_END

#include "llbc/core/thread/MessageQueueInl.h"


