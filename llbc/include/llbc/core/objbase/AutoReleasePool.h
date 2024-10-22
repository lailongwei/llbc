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

#include "llbc/core/objbase/Object.h"

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
     * @return LLBC_Object * - the cloned object, always return nullptr, 
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

#include "llbc/core/objbase/AutoReleasePoolInl.h"


