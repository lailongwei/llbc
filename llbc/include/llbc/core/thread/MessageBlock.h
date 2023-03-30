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

#include "llbc/core/objectpool/IObjectPoolInstFactory.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_IObjectPool;
class LLBC_IObjectPoolInst;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief Message block class encapsulation.
 */
class LLBC_EXPORT LLBC_MessageBlock
{
public:
    static const size_t npos = -1;

public:
    /**
     * Init and construct specific size's message block.
     * @param[in] size - block init size.
     */
    explicit LLBC_MessageBlock(size_t size = LLBC_CFG_THREAD_MSG_BLOCK_DFT_SIZE);

    /**
     * Init and construct message block using external buffer.
     * @param[in] buf    - buffer.
     * @param[in] size   - buffer size.
     * @param[in] attach - attach flag, default is true.
     */
    LLBC_MessageBlock(void *buf, size_t size, bool attach = true);

    /**
     * Destructor.
     */
    ~LLBC_MessageBlock();

public:
    /**
     * Allocate new buffer size.
     * @param[in] size - size.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Allocate(size_t size = LLBC_CFG_THREAD_MSG_BLOCK_DFT_SIZE);

public:
    /**
     * Read data from message block.
     * @param[in] buf - buffer, data will store here.
     * @param[in] len - buffer size.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Read(void *buf, size_t len);

    /**
     * Write data to message block.
     * @param[in] buf - buffer, data will store here.
     * @param[in] len - buffer size.
     * @return int  - return 0 if success, otherwise return -1.
     */
    int Write(const void *buf, size_t len);

    /**
     * Release message block.
     */
    void Release();

public:
    /**
     * Object-Pool reflection support: Mark pool object.
     */
    void MarkPoolObject(LLBC_IObjectPoolInst &poolInst);

    /**
     * Object-Pool reflection support: Get pool instance.
     */
    LLBC_IObjectPoolInst *GetPoolInst();

    /**
     * Object-Pool reflection support: get user-defined per-block units number.
     */
    size_t GetPoolInstPerBlockUnitsNum();

    /**
     * Object-Pool reflection support: Clear message block, this operation will clear read&write position information.
     */
    void Clear();

public:
    /**
     * Check the message block's buffer is attach or not.
     * @return bool - attach attribute.
     */
    bool IsAttach() const;

    /**
     * Get message block current buffer.
     * @return void * - buffer pointer.
     */
    void *GetData() const;

    /**
     * Get message block buffer, start with read position.
     * @return void * - buffer pointer.
     */
    void *GetDataStartWithReadPos() const;

    /**
     * Get message block buffer, start with write position.
     * @return void * - buffer pointer.
     */
    void *GetDataStartWithWritePos() const;

    /**
     * Get message block current buffer size.
     * @return size_t - buffer size.
     */
    size_t GetSize() const;

    /**
     * Get message block readable size.
     * @return size_t - message block readable size.
     */
    size_t GetReadableSize() const;

    /**
     * Get message block writable size.
     * @return size_t - messable block writable size.
     */
    size_t GetWritableSize() const;

public:
    /**
     * Get read position.
     * @return size_t - read position.
     */
    size_t GetReadPos() const;

    /**
     * Set read position.
     * @param[in] pos - read position.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetReadPos(size_t pos);

    /**
     * Shift read position.
     * @param[in] off - offset.
     * return int - return 0 if success, otherwise return -1.
     */
    int ShiftReadPos(long off);

    /**
     * Get write position.
     * @reurn size_t - write position.
     */
    size_t GetWritePos() const;

    /**
     * Set write position.
     * @param[in] pos - write position.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetWritePos(size_t pos);

    /**
     * Shift write position.
     * @param[in] off - offset.
     * @return int - return 0 if success, otherwise return -1.
     */
    int ShiftWritePos(long off);

public:
    /**
     * Swap message block.
     * @param[in/out] another - another message block.
     */
    void Swap(LLBC_MessageBlock *another);

    /**
     * Clone message block.
     * @return LLBC_MessageBlock * - new message block.
     */
    LLBC_MessageBlock *Clone() const;

    /**
     * Get previous message block.
     * @return LLBC_MessageBlock * - previous message block.
     */
    LLBC_MessageBlock *GetPrev() const;

    /**
     * Set previous message block.
     * @param[in] prev - the previous message block.
     */
    void SetPrev(LLBC_MessageBlock *prev);

    /**
     * Get next message block.
     * @return LLBC_MessageBlock * - next message block.
     */
    LLBC_MessageBlock *GetNext() const;

    /**
     * Set next message block.
     * @param[in] next - the next message block.
     */
    void SetNext(LLBC_MessageBlock *next);

    /**
     * Adjust the message block's buffer size.
     * @param[in] newSize - new buffer size.
     */
    void Resize(size_t newSize);

    LLBC_DISABLE_ASSIGNMENT(LLBC_MessageBlock);

private:
    bool _attach;

    char *_buf;
    size_t _size;

    size_t _readPos;
    size_t _writePos;

    LLBC_MessageBlock *_prev;
    LLBC_MessageBlock *_next;

    LLBC_IObjectPoolInst *_poolInst;
};

/**
 * \brief The message block object pool instance factory encapsulation.
 */
class LLBC_HIDDEN LLBC_MessageBlockObjectPoolInstFactory : public LLBC_IObjectPoolInstFactory
{
public:
    /**
     * Get object pool instance name(object type name).
     * @return const char * - the object pool instance name.
     */
    virtual const char *GetName() const;

    /**
     * Create object pool instance.
     * @param[in] pool - the object pool.
     * @param[in] lock - the pool instance lock.
     * @return LLBC_IObjectPoolInst * - the object pool instance.
     */
    virtual LLBC_IObjectPoolInst *Create(LLBC_IObjectPool *pool, LLBC_ILock *lock);
};

__LLBC_NS_END

#include "llbc/core/thread/MessageBlockInl.h"


