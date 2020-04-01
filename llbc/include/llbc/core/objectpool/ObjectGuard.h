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

#ifndef __LLBC_CORE_OBJECT_POOL_OBJECT_GUARD_H__
#define __LLBC_CORE_OBJECT_POOL_OBJECT_GUARD_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

// Pre-declare some classes.
class LLBC_IObjectPoolInst;

/**
* \brief The pool object guard class encapsulation.
*/
template <typename ObjectType>
class LLBC_ObjectGuard
{
public:
    LLBC_ObjectGuard(ObjectType *obj, LLBC_IObjectPoolInst *poolInst);
    LLBC_ObjectGuard(const LLBC_ObjectGuard<ObjectType> &another);

    ~LLBC_ObjectGuard();

public:
    /**
     * Get guarded object pointer.
     * @return ObjectType *& - the guarded object pointer.
     */
    ObjectType *&operator ->();
    /**
     * Get guarded object pointer.
     * @return const ObjectType * const & - the guarded object pointer.
     */
    const ObjectType * const &operator ->() const;

public:
    /**
     * Get guarded object.
     * @return ObjectType & - the guarded object reference.
     */
    ObjectType &operator *();
    /**
     * Get guarded object.
     * @return const const ObjectType & - the guarded object reference.
     */
    const ObjectType &operator *() const;

public:
    /**
     * Implicit type convert to ObjectType *.
     */
    operator ObjectType *();
    /**
     * Implicit type convert to const ObjectType *.
     */
    operator const ObjectType *() const;

public:
    /**
     * Get guarded object.
     * @return ObjectType * - the guarded object.
     */
    ObjectType *GetObj();
    /**
     * Get guarded object.
     * @return ObjectType & - the guarded object.
     */
    const ObjectType *GetObj() const;

    /**
     * Delete guarded object.
     * @return ObjectType * - the detached guarded object.
     */
    ObjectType *DetachObj();

private:
    /**
     * Make guarded object weakref.
     */
    void MakeWeakRef() const;

private:
    /**
     * Disable assignment.
     */
    LLBC_ObjectGuard &operator =(const LLBC_ObjectGuard &another);

private:
    ObjectType *_obj;
    LLBC_IObjectPoolInst *_poolInst;

    bool _weakRef;
};

__LLBC_NS_END

#include "llbc/core/objectpool/ObjectGuardImpl.h"

#endif // !__LLBC_CORE_OBJECT_POOL_OBJECT_GUARD_H__
