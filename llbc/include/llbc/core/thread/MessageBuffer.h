/**
 * @file    MessageBuffer.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/15
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_THREAD_MESSAGE_BUFFER_H__
#define __LLBC_CORE_THREAD_MESSAGE_BUFFER_H__

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

public:
    /**
     * Get the first block.
     * @return LLBC_MessageBlock * - the message block.
     */
    LLBC_MessageBlock *FirstBlock() const;

    /**
     * Merge buffers and detach.
     * @return LLBC_MessageBlock * - merged message block.
     */
    LLBC_MessageBlock *MergeBuffersAndDetach();

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
    LLBC_MessageBlock *_head;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_THREAD_MESSAGE_BUFFER_H__
