/**
 * @file    AutoReleasePoolStack.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/01/20
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_OBJBASE_AUTO_RELEASE_POOL_STACK_H__
#define __LLBC_OBJBASE_AUTO_RELEASE_POOL_STACK_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"

/**
 * Previous declare some classes.
 */
__LLBC_NS_BEGIN
class LLBC_Object;
class LLBC_AutoReleasePool;
__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The auto release pool stack class encapsulation.
 *        Use to manage the auto release pool.
 */
class LLBC_HIDDEN LLBC_AutoReleasePoolStack
{
public:
    LLBC_AutoReleasePoolStack();
    ~LLBC_AutoReleasePoolStack();

public:
    /**
     * Add object to pool stack.
     * @param[in] o - the object.
     * @return int - return 0 if success, otherwise return -1.
     */
    int AddObject(LLBC_Object *o);

    /**
     * Remove object from pool stack.
     * @param[in] o - the object.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveObject(LLBC_Object *o);

    /**
     * Push release pool.
     * @param[in] pool - the auto release pool.
     * @return int - return 0 if success, otherwise return -1.
     */
    int PushPool(LLBC_AutoReleasePool *pool);

    /**
     * Pop release pool.
     * @param[in] pool - the will pop pool.
     * @return int - return 0 if success, otherwise return -1.
     */
    int PopPool(LLBC_AutoReleasePool *pool);

    /**
     * Get current release pool.
     * @return LLBC_AutoReleasePool * - the current release pool.
     */
    LLBC_AutoReleasePool *GetCurrentReleasePool();

    /**
     * Purge all pools.
     */
    void Purge();

private:
    LLBC_AutoReleasePool *_head;
};

__LLBC_NS_END

#endif // !__LLBC_OBJBASE_AUTO_RELEASE_POOL_STACK_H__
