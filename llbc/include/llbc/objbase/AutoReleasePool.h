/**
 * @file    AutoReleasePool.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/01/14
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_OBJBASE_AUTO_RELEASE_POOL_H__
#define __LLBC_OBJBASE_AUTO_RELEASE_POOL_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"

#include "llbc/objbase/Object.h"

/* Previous declare some classes. */
__LLBC_NS_BEGIN
class LLBC_Array;
class LLBC_AutoReleasePoolStack;
__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief T he auto release pool class encapsulation.
 */
class LLBC_EXPORT LLBC_AutoReleasePool : public LLBC_Object
{
public:
    /**
     * Constructor & Destructor.
     */
    LLBC_AutoReleasePool();
    virtual ~LLBC_AutoReleasePool();

public:
    /**
     * Add object to pool.
     * @param[in] o - object.
     * @return int - return 0 if success, otherwise return -1.
     */
    int AddObject(LLBC_Object *o);

    /**
     * Remove object from pool.
     * @param[in] o - object.
     * @return int - return 0 if success, otherwise return -1, this method not decrement reference counter.
     */
    int RemoveObject(LLBC_Object *o);

    /**
     * Purge pool.
     */
    void Purge();

public:
    /**
     * Auto release method, use to prevent call.
     * @return int - always return -1, and set last error to permission denied error.
     */
    virtual int AutoRelease();

    /**
     * Clone method, use to prevent call.
     * @return LLBC_Object * - the cloned object, always return NULL, 
     *                         and set last error to permission denied error.
     */
    virtual LLBC_Object *Clone() const;

private:
    /**
     * Declare AutoReleasePoolStack is AutoReleasePool's friend class.
     * Access methods:
     *      GetPoolPrev()/SetPoolPrev()
     *      GetPoolNext()/SetPoolNext()
     */
    friend class LLBC_AutoReleasePoolStack;

    /**
     * Get next auto release pool.
     * @return LLBC_AutoReleasePool * - the next auto release pool.
     */
    LLBC_AutoReleasePool *GetPoolNext();

    /**
     * Set next auto release pool.
     * param[in] next - the next auto release pool.
     */
    void SetPoolNext(LLBC_AutoReleasePool *next);

    LLBC_DISABLE_ASSIGNMENT(LLBC_AutoReleasePool);

private:
    LLBC_Array *_arr;
    LLBC_AutoReleasePool *_next;
};

__LLBC_NS_END

#endif // !__LLBC_OBJBASE_AUTO_RELEASE_POOL_H__
