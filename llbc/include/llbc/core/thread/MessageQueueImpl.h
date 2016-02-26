/**
 * @file    MessageQueueImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/10/27
 * @version 1.0
 *
 * @brief
 */

#ifdef __LLBC_CORE_THREAD_MESSAGE_QUEUE_H__

__LLBC_NS_BEGIN

inline void LLBC_MessageQueue::PushFront(LLBC_MessageBlock *block)
{
    Push(block, true);
}

inline void LLBC_MessageQueue::PushBack(LLBC_MessageBlock *block)
{
    Push(block, false);
}

inline void LLBC_MessageQueue::PopFront(LLBC_MessageBlock *&block)
{
    Pop(block, LLBC_INFINITE, true);
}

inline void LLBC_MessageQueue::PopBack(LLBC_MessageBlock *&block)
{
    Pop(block, LLBC_INFINITE, false);
}

inline bool LLBC_MessageQueue::TryPopFront(LLBC_MessageBlock *&block)
{
    return Pop(block, 0, true);
}

inline bool LLBC_MessageQueue::TryPopBack(LLBC_MessageBlock *&block)
{
    return Pop(block, 0, false);
}

inline bool LLBC_MessageQueue::TimedPopFront(LLBC_MessageBlock *&block, int interval)
{
    return Pop(block, interval, true);
}

inline bool LLBC_MessageQueue::TimedPopBack(LLBC_MessageBlock *&block, int interval)
{
    return Pop(block, interval, false);
}

inline void LLBC_MessageQueue::PushNonLock(LLBC_MessageBlock *block, bool front)
{
    if (front)
        PushFrontNonLock(block);
    else
        PushBackNonLock(block);
}

inline void LLBC_MessageQueue::PopNonLock(LLBC_MessageBlock *&block, bool front)
{
    if (front)
        PopFrontNonLock(block);
    else
        PopBackNonLock(block);
}

__LLBC_NS_END

#endif // __LLBC_CORE_THREAD_MESSAGE_QUEUE_H__
