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

#include "llbc/core/thread/MessageBlock.h"
#include "llbc/core/thread/MessageQueue.h"

__LLBC_NS_BEGIN

/**
 * \brief The task queue state enumeration.
 */
class LLBC_EXPORT LLBC_TaskQueue {
public:
    LLBC_TaskQueue() = default;
    virtual ~LLBC_TaskQueue() = default;

private:
    // Disable assignment.
    LLBC_DISABLE_ASSIGNMENT(LLBC_TaskQueue);

public:
    template <template <typename> class Task>
    void InitQueue(std::in_place_type_t<Task<LLBC_TaskQueue>>, int);

    /**
     * Push message block to task.
     * @param[in] block - message block.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Push(LLBC_MessageBlock *block);

    /**
     * Pop message block from task.
     * @param[out] block - message block.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Pop(LLBC_MessageBlock *&block);

    /**
     * Pop all message blocks from task.
     * @param[out] blocks - the message blocks.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int PopAll(LLBC_MessageBlock *&blocks);

    /**
     * Try pop message block from task.
     * @param[out] block - message block.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int TryPop(LLBC_MessageBlock *&block);

    /**
     * Timed pop message block from task.
     * @param[out] block   - message block.
     * @param[in] interval - interval, in milliseconds.
     * @return int -  return 0 if success, otherwise return -1.
     */
    virtual int TimedPop(LLBC_MessageBlock *&block, int interval);

    /**
     * Get unprocessed message size.
     * @return size_t - the unprocessed message size.
     */
    virtual size_t GetMessageSize() const;

    /**
     * Clear cleanup queue.
     */
    virtual void Clear();

private:
    LLBC_MessageQueue _msgQueue;
};

/**
 * \brief The multi thread task queue state enumeration.
 */
class LLBC_EXPORT LLBC_MultiThreadTaskQueue {
public:
    LLBC_MultiThreadTaskQueue();
    virtual ~LLBC_MultiThreadTaskQueue() = default;

private:
    // Disable assignment.
    LLBC_DISABLE_ASSIGNMENT(LLBC_MultiThreadTaskQueue);

public:
    template <template <typename> class Task>  
    void InitQueue(std::in_place_type_t<Task<LLBC_MultiThreadTaskQueue>>, int threadNum);

    /**
     * Push message block to task.
     * @param[in] block - message block.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Push(int hashId, LLBC_MessageBlock *block);

    /**
     * Pop message block from task.
     * @param[out] block - message block.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Pop(LLBC_MessageBlock *&block);

    /**
     * Pop all message blocks from task.
     * @param[out] blocks - the message blocks.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int PopAll(LLBC_MessageBlock *&blocks);

    /**
     * Try pop message block from task.
     * @param[out] block - message block.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int TryPop(LLBC_MessageBlock *&block);

    /**
     * Timed pop message block from task.
     * @param[out] block   - message block.
     * @param[in] interval - interval, in milliseconds.
     * @return int -  return 0 if success, otherwise return -1.
     */
    virtual int TimedPop(LLBC_MessageBlock *&block, int interval);

    /**
     * Clear cleanup queue.
     */
    virtual void Clear();

    /**
     * Get unprocessed message size.
     * @return size_t - the unprocessed message size.
     */
    virtual size_t GetMessageSize() const;

private:
    uint32 _threadNum;
    std::vector<std::unique_ptr<LLBC_MessageQueue>> _msgQueues;

    using ProcessorIdGetter = int (*)();
    ProcessorIdGetter  _processorIdGetter;
};

__LLBC_NS_END

#include "llbc/core/thread/TaskQueueInl.h"
