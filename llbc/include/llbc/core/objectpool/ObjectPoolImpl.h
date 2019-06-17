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

#ifdef __LLBC_CORE_OBJECT_POOL_OBJECT_POOL_H__

#include "llbc/core/thread/Guard.h"
#include "llbc/core/helper/Common.h"

__LLBC_NS_BEGIN

template <typename PoolLockType, typename PoolInstLockType>
LLBC_ObjectPool<PoolLockType, PoolInstLockType>::LLBC_ObjectPool()
{
}

template <typename PoolLockType, typename PoolInstLockType>
LLBC_ObjectPool<PoolLockType, PoolInstLockType>::~LLBC_ObjectPool()
{
    LLBC_LockGuard guard(_lock);
    LLBC_STLHelper::DeleteContainer(_poolDict, false, false);
}

template <typename PoolLockType, typename PoolInstLockType>
template <typename ObjectType>
LLBC_ObjectPoolInst<ObjectType, PoolInstLockType> *LLBC_ObjectPool<PoolLockType, PoolInstLockType>::GetPoolInst()
{
    const char *objectType = typeid(ObjectType).name();

    LLBC_IObjectPoolInst *poolInst;
    std::map<const char *, LLBC_IObjectPoolInst *>::iterator it;

    _lock.Lock();
    if ((it = _poolDict.find(objectType)) == _poolDict.end())
        poolInst = _poolDict.insert(std::make_pair(objectType, new LLBC_ObjectPoolInst<ObjectType, PoolInstLockType>())).first->second;
    else
        poolInst = it->second;

    _lock.Unlock();

    return reinterpret_cast<LLBC_ObjectPoolInst<ObjectType, PoolInstLockType>* >(poolInst);
}

template <typename PoolLockType, typename PoolInstLockType>
template <typename ObjectType>
ObjectType *LLBC_ObjectPool<PoolLockType, PoolInstLockType>::Get()
{
    const char *objectType = typeid(ObjectType).name();

    LLBC_IObjectPoolInst *poolInst;
    std::map<const char *, LLBC_IObjectPoolInst *>::iterator it;

    _lock.Lock();
    if ((it = _poolDict.find(objectType)) == _poolDict.end())
        poolInst = _poolDict.insert(std::make_pair(objectType, new LLBC_ObjectPoolInst<ObjectType, PoolInstLockType>())).first->second;
    else
        poolInst = it->second;

    _lock.Unlock();

    return reinterpret_cast<ObjectType *>(poolInst->Get());
}

template <typename PoolLockType, typename PoolInstLockType>
template <typename ObjectType>
LLBC_ObjectGuard<ObjectType> LLBC_ObjectPool<PoolLockType, PoolInstLockType>::GetGuarded()
{
    return LLBC_ObjectGuard<ObjectType>(Get<ObjectType>(), this);
}

template <typename PoolLockType, typename PoolInstLockType>
template <typename ObjectType>
int LLBC_ObjectPool<PoolLockType, PoolInstLockType>::Release(ObjectType *obj)
{
    return this->Release(typeid(ObjectType).name(), obj);
}

template <typename PoolLockType, typename PoolInstLockType>
int LLBC_ObjectPool<PoolLockType, PoolInstLockType>::Release(const char *objectType, void *obj)
{
    LLBC_IObjectPoolInst *poolInst;
    std::map<const char *, LLBC_IObjectPoolInst *>::iterator it;

    _lock.Lock();
    if ((it = _poolDict.find(objectType)) == _poolDict.end())
    {
        _lock.Unlock();

        LLBC_SetLastError(LLBC_ERROR_UNKNOWN);
        return LLBC_FAILED;
    }

    poolInst = it->second;

    _lock.Unlock();

    poolInst->Release(obj);

    return LLBC_OK;
}

__LLBC_NS_END

#endif //!__LLBC_CORE_OBJECT_POOL_OBJECT_POOL_H__
