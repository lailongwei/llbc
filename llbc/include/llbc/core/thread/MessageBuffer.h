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

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_MessageBlock;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The message buffer class encapsulation.
 */
class LLBC_EXPORT LLBC_MessageBuffer
{
public:
    LLBC_MessageBuffer();
    ~LLBC_MessageBuffer();

public:
    /**
     * Read data from message buffer.
     * @param[out] buf - data buffer.
     * @param[in]  len - buffer length.
     * @return size_t - read data size.
     */
    size_t Read(void *buf, size_t len);

    /**
     * Write data to message buffer.
     * @param[in] buf - data buffer.
     * @param[in] len - buffer length.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Write(const char *buf, size_t len);

    /**
     * Get message buffer size.
     * @return size_t - the message buffer size.
     */
    size_t GetSize() const;

public:
    /**
     * Get first block.
     * @return const LLBC_MessageBlock * - the message block.
     */
    const LLBC_MessageBlock *FirstBlock() const;

    /**
     * Detach first block.
     * @return LLBC_MessageBlock * - the first message block.
     */
    LLBC_MessageBlock *DetachFirstBlock();

    /**
     * Merge all message blocks and detach.
     * @return LLBC_MessageBlock * - merged message block.
     */
    LLBC_MessageBlock *MergeBlocksAndDetach();

    /**
     * Append new block to buffer.
     * @param[in] block - message block.
     * @return int - return 0 if not error occurred, otherwise return -1.
     */
    int Append(LLBC_MessageBlock *block);

    /**
     * Remove specific length's data.
     * @param[in] length - length.
     * @return size_t - removed size.
     */
    size_t Remove(size_t length);

    /**
     * Cleanup the message buffer.
     */
    void Cleanup();

private:
    /**
     * Delete first message block.
     */
    void DeleteFirstBlock();

private:
    size_t _size;
    LLBC_MessageBlock *_head;
    LLBC_MessageBlock *_tail;
};

__LLBC_NS_END
