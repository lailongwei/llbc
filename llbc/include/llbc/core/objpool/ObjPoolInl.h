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

#include "llbc/core/thread/Guard.h"

// The object pool lock operation macros define.
#if LLBC_TARGET_PLATFORM_WIN32
 #define __LLBC_INL_InitObjPoolLock() \
    if (_threadSafe) ::InitializeCriticalSectionAndSpinCount(&_lock, 8000)
 #define __LLBC_INL_DestroyObjPoolLock() \
    if (_threadSafe) ::DeleteCriticalSection(&_lock)
 #define __LLBC_INL_LockObjPool() \
    if (_threadSafe) ::EnterCriticalSection(&_lock)
 #define __LLBC_INL_UnlockObjPool() \
    if (_threadSafe) ::LeaveCriticalSection(&_lock)
#elif LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID
 #define __LLBC_INL_InitObjPoolLock() \
    if (_threadSafe) pthread_spin_init(&_lock, 0)
 #define __LLBC_INL_DestroyObjPoolLock() \
    if (_threadSafe) pthread_spin_destroy(&_lock)
 #define __LLBC_INL_LockObjPool() \
    if (_threadSafe) pthread_spin_lock(&_lock)
 #define __LLBC_INL_UnlockObjPool() \
     if (_threadSafe) pthread_spin_unlock(&_lock)
#elif LLBC_TARGET_PLATFORM_MAC || LLBC_TARGET_PLATFORM_IPHONE
 #define __LLBC_INL_InitObjPoolLock() \
    if (_threadSafe) pthread_mutex_init(&_lock, nullptr)
 #define __LLBC_INL_DestroyObjPoolLock() \
    if (_threadSafe) pthread_mutex_destroy(&_lock)
 #define __LLBC_INL_LockObjPool() \
    if (_threadSafe) pthread_mutex_lock(&_lock)
 #define __LLBC_INL_UnlockObjPool() \
    if (_threadSafe) pthread_mutex_unlock(&_lock)
#endif

__LLBC_NS_BEGIN

template <typename Obj>
LLBC_FORCE_INLINE LLBC_GuardedPoolObj<Obj>::LLBC_GuardedPoolObj(Obj *obj, LLBC_TypedObjPool<Obj> *typedObjPool)
: _obj(obj)
, _typedObjPool(typedObjPool)
{
}

template <typename Obj>
LLBC_FORCE_INLINE LLBC_GuardedPoolObj<Obj>::LLBC_GuardedPoolObj(const LLBC_GuardedPoolObj &other)
: _obj(other._obj)
, _typedObjPool(other._typedObjPool)
{
    other._obj = nullptr;
}

template <typename Obj>
LLBC_FORCE_INLINE LLBC_GuardedPoolObj<Obj>::LLBC_GuardedPoolObj(LLBC_GuardedPoolObj &&other)
: _obj(other._obj)
, _typedObjPool(other._typedObjPool)
{
    other._obj = nullptr;
}

template <typename Obj>
LLBC_FORCE_INLINE LLBC_GuardedPoolObj<Obj>::~LLBC_GuardedPoolObj()
{
    if (_obj)
        _typedObjPool->Release(_obj);
}

template <typename Obj>
LLBC_FORCE_INLINE void LLBC_GuardedPoolObj<Obj>::Reset()
{
    if (_obj)
    {
        _typedObjPool->Release(_obj);
        _obj = nullptr;
    }
}

template <typename Obj>
LLBC_GuardedPoolObj<Obj> &LLBC_GuardedPoolObj<Obj>::operator=(const LLBC_GuardedPoolObj &other)
{
    if (UNLIKELY(this == &other))
        return *this;

    if (UNLIKELY(_obj))
        _typedObjPool->Release(_obj);

    _obj = other._obj;
    _typedObjPool = other._typedObjPool;

    other._obj = nullptr;

    return *this;
}

template <typename Obj>
LLBC_GuardedPoolObj<Obj> &LLBC_GuardedPoolObj<Obj>::operator=(LLBC_GuardedPoolObj &&other)
{
    if (UNLIKELY(this == &other))
        return *this;

    if (UNLIKELY(_obj))
        _typedObjPool->Release(_obj);

    _obj = other._obj;
    _typedObjPool = other._typedObjPool;

    other._obj = nullptr;

    return *this;
}

template <typename Obj>
LLBC_TypedObjPool<Obj>::LLBC_TypedObjPool(LLBC_ObjPool *objPool, bool threadSafe)
: _threadSafe(threadSafe)

, _objPool(objPool)
, _objTypeName(typeid(Obj).name())
, _freeStripes(nullptr)

, _usingObjCount(0)
, _reusableObjCount(0)
{
    // Init lock.
    __LLBC_INL_InitObjPoolLock();
}

template <typename Obj>
LLBC_TypedObjPool<Obj>::~LLBC_TypedObjPool()
{
    // Delete all stripes.
    __LLBC_INL_LockObjPool();
    for (auto &stripe : _stripes)
        DeleteStripe(stripe);
    __LLBC_INL_UnlockObjPool();

    // Destroy lock.
    __LLBC_INL_DestroyObjPoolLock();
}

template <typename Obj>
Obj *LLBC_TypedObjPool<Obj>::Acquire()
{
    _WrappedObj *wrappedObj;

    // Lock & Find _WrappedObj.
    __LLBC_INL_LockObjPool();
    auto stripe = FindFreeStripe();
    if (LIKELY(stripe->freeObjs))
    {
        wrappedObj = stripe->freeObjs;
        stripe->freeObjs = wrappedObj->stripeOrNextFreeObj.nextFreeObj;
        ASSERT(!wrappedObj->unFlags.flags.inUsing && "llbc framework internal error");
    }
    else
    {
        wrappedObj = reinterpret_cast<_WrappedObj *>(stripe->objs + stripe->used++);
        wrappedObj->unFlags.flagsVal = 0;
        wrappedObj->magicNum = LLBC_CFG_CORE_OBJPOOL_OBJ_MAGIC_NUMBER;
        wrappedObj->typedObjPool = this;
    }

    // Set wrappedObj owner: stripe
    wrappedObj->stripeOrNextFreeObj.stripe = stripe;

    // If stripe is full, erase from _freeStripes.
    if (!stripe->freeObjs && stripe->used == stripe->cap)
    {
        _freeStripes = stripe->nextFreeStripe;
        #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
        stripe->nextFreeStripe = nullptr;
        #endif
    }

    // Incr using object count.
    ++_usingObjCount;

    // Construct _Obj.
    if (!wrappedObj->unFlags.flags.constructed)
    {
        __LLBC_INL_UnlockObjPool();
        LLBC_ObjReflector::New<Obj>(wrappedObj->buff);
        LLBC_ObjReflector::SetTypedObjPool<Obj>(wrappedObj->buff, this);
        wrappedObj->unFlags.flags.constructed = true;
    }
    else
    {
        --_reusableObjCount;
        __LLBC_INL_UnlockObjPool();
    }

    // Mask obj in using.
    wrappedObj->unFlags.flags.inUsing = true;

    // Return obj.
    return reinterpret_cast<Obj *>(wrappedObj->buff);
}

template <typename Obj>
void LLBC_TypedObjPool<Obj>::Release(Obj *obj)
{
    // Get wrap obj.
    auto wrappedObj = reinterpret_cast<_WrappedObj *>(reinterpret_cast<uint8 *>(obj) - _objOffset);

    // Exec required checks.
    ASSERT(wrappedObj->unFlags.flags.inUsing && "Repeated release object");
    ASSERT(wrappedObj->magicNum == LLBC_CFG_CORE_OBJPOOL_OBJ_MAGIC_NUMBER &&
           "The object is not a objpool object");

    // Reuse/Delete obj.
    if (LLBC_ObjReflector::IsReusable<Obj>())
    {
        LLBC_ObjReflector::Reuse<Obj>(wrappedObj->buff);
    }
    else
    {
        LLBC_ObjReflector::Delete<Obj>(wrappedObj->buff);
        wrappedObj->unFlags.flags.constructed = false;
    }
    wrappedObj->unFlags.flags.inUsing = false;

    // Get stripe.
    auto stripe = wrappedObj->stripeOrNextFreeObj.stripe;

    // Lock.
    __LLBC_INL_LockObjPool();

    // Stat reusable object count.
    if (LLBC_ObjReflector::IsReusable<Obj>())
        ++_reusableObjCount;

    // Link to stripe->freeObjs.
    const auto stripeIsFull = (stripe->used == stripe->cap && !stripe->freeObjs);
    wrappedObj->stripeOrNextFreeObj.nextFreeObj = stripe->freeObjs;
    stripe->freeObjs = wrappedObj;

    // If stripe is full before release obj, add to _freeStripes.
    if (UNLIKELY(stripeIsFull))
    {
        stripe->nextFreeStripe = _freeStripes;
        _freeStripes = stripe;
    }

    // Decr using object count.
    --_usingObjCount;

    // Unlock.
    __LLBC_INL_UnlockObjPool();
}

template <typename Obj>
void LLBC_TypedObjPool<Obj>::Collect(bool deep)
{
    // Lock & Defer unlock.
    __LLBC_INL_LockObjPool();
    LLBC_Defer(__LLBC_INL_UnlockObjPool());

    // Destruct unused objs.
    std::vector<size_t> delayedDelStripeIndexes;
    for (size_t stripeIdx = 0; stripeIdx < _stripes.size(); ++stripeIdx)
    {
        bool hasUsingObjs = false;
        auto &stripe = _stripes[stripeIdx];
        for (uint16 objIdx = 0; objIdx < stripe->used; ++objIdx)
        {
            auto wrappedObj = &stripe->objs[objIdx];
            if (wrappedObj->unFlags.flags.inUsing)
            {
                hasUsingObjs = true;
                continue;
            }

            if (wrappedObj->unFlags.flags.constructed)
            {
                LLBC_ObjReflector::Delete<Obj>(wrappedObj->buff);
                wrappedObj->unFlags.flags.constructed = false;
                --_reusableObjCount;
            }
        }

        // Deep collect: Delete stripe(delay).
        if (!hasUsingObjs && deep)
            delayedDelStripeIndexes.push_back(stripeIdx);
    }

    // Delete free stripes.
    for (int i = static_cast<int>(delayedDelStripeIndexes.size()) - 1; i >= 0; --i)
    {
        // Get stripe, and erase from _stripes.
        const size_t stripeIdx = delayedDelStripeIndexes[i];
        auto stripe = _stripes[stripeIdx];
        _stripes.erase(_stripes.begin() + stripeIdx);

        // Erase from _freeStripes.
        if (stripe == _freeStripes)
        {
            _freeStripes = _freeStripes->nextFreeStripe;
        }
        else
        {
            #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
            bool erasedFromFreeStripes = false;
            #endif

            _ObjStripe *nowFreeStripe;
            _ObjStripe *prevFreeStripe = _freeStripes;
            while ((nowFreeStripe = prevFreeStripe->nextFreeStripe))
            {
                if (nowFreeStripe == stripe)
                {
                    #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
                    erasedFromFreeStripes = true;
                    #endif
                    prevFreeStripe->nextFreeStripe = nowFreeStripe->nextFreeStripe;
                    break;
                }

                prevFreeStripe = nowFreeStripe;
            }

            #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
            ASSERT(erasedFromFreeStripes && "llbc framework internal error");
            #endif
        }

        // Free stripe memory.
        free(stripe);
    }
}

template <typename Obj>
void LLBC_TypedObjPool<Obj>::ReleaseObj_s(void *typedObjPool, void *obj)
{
    reinterpret_cast<LLBC_TypedObjPool<Obj> *>(typedObjPool)->Release(reinterpret_cast<Obj *>(obj));
}

template <typename Obj>
void LLBC_TypedObjPool<Obj>::Destruct_s(void *typedObjPool)
{
    reinterpret_cast<LLBC_TypedObjPool<Obj> *>(typedObjPool)->~LLBC_TypedObjPool();
}

template <typename Obj>
void LLBC_TypedObjPool<Obj>::Collect_s(void *typedObjPool, bool deep)
{
    reinterpret_cast<LLBC_TypedObjPool<Obj> *>(typedObjPool)->Collect(deep);
}

template <typename Obj>
LLBC_Json::Value LLBC_TypedObjPool<Obj>::GetStatistics_s(void *typedObjPool,
                                                         LLBC_Json::MemoryPoolAllocator<> &jsonAlloc)
{
    return reinterpret_cast<LLBC_TypedObjPool<Obj> *>(typedObjPool)->GetStatistics(jsonAlloc);
}

template <typename Obj>
LLBC_FORCE_INLINE
typename LLBC_TypedObjPool<Obj>::_ObjStripe *LLBC_TypedObjPool<Obj>::FindFreeStripe()
{
    if (LIKELY(_freeStripes))
        return _freeStripes;

    const auto stripeCap = LLBC_ObjReflector::GetStripeCapacity<Obj>();
    auto stripe = LLBC_Malloc(_ObjStripe, sizeof(_ObjStripe) + sizeof(_WrappedObj) * stripeCap);
    memset(stripe, 0, sizeof(_ObjStripe));
    stripe->cap = static_cast<uint16>(stripeCap);

    _stripes.push_back(stripe);
    _freeStripes = stripe;

    return stripe;
}

template <typename Obj>
void LLBC_TypedObjPool<Obj>::DeleteStripe(_ObjStripe *stripe)
{
    // Destruct all constructed objs.
    for (sint16 objIdx = 0; objIdx < stripe->used; ++objIdx)
    {
        auto wrappedObj = reinterpret_cast<_WrappedObj *>(stripe->objs + objIdx);
        if (!wrappedObj->unFlags.flags.constructed)
            continue;

        // ASSERT(!wrappedObj->unFlags.flags.inUsing && "Object leak");
        if (wrappedObj->unFlags.flags.inUsing &&
            LLBC_ObjReflector::IsReusable<Obj>())
            LLBC_ObjReflector::Reuse<Obj>(wrappedObj->buff);

        LLBC_ObjReflector::Delete<Obj>(wrappedObj->buff);
    }

    // Delete stripe.
    free(stripe);
}

template <typename Obj>
LLBC_Json::Value LLBC_TypedObjPool<Obj>::GetStatistics(LLBC_Json::MemoryPoolAllocator<> &jsonAlloc) const
{
    __LLBC_INL_LockObjPool();
    LLBC_Defer(__LLBC_INL_UnlockObjPool());

    // Meta info:
    // - name.
    LLBC_Json::Value stat(LLBC_Json::kObjectType);
    stat.AddMember("name",
                   LLBC_Json::Value().SetString(LLBC_GetTypeName(Obj), jsonAlloc),
                   jsonAlloc);
    // - reusable.
    stat.AddMember("reusable",
                   LLBC_ObjReflector::IsReusable<Obj>(),
                   jsonAlloc);

    // Object info:
    // - obj_size: object size, in bytes.
    stat.AddMember("obj_size", sizeof(Obj), jsonAlloc);
    // - wrapped_obj_size: wrapped object size, in bytes.
    stat.AddMember("wrapped_obj_size", sizeof(_WrappedObj), jsonAlloc);
    // - obj_count.
    const auto objCountPerStripe = LLBC_ObjReflector::GetStripeCapacity<Obj>();
    const auto objCount = objCountPerStripe * _stripes.size();
    stat.AddMember("obj_count", objCount, jsonAlloc);
    // - using_obj_count.
    stat.AddMember("using_obj_count", _usingObjCount, jsonAlloc);
    // - using_obj_rate.
    stat.AddMember("using_obj_rate",
                   objCount != 0 ? static_cast<double>(_usingObjCount) / objCount : 0.0,
                   jsonAlloc);
    // - reusable_obj_count.
    stat.AddMember("reusable_obj_count", _reusableObjCount, jsonAlloc);
    // - reusable_obj_rate.
    stat.AddMember("reusable_obj_rate",
                   objCount != 0 ? static_cast<double>(_reusableObjCount) / objCount : 0.0,
                   jsonAlloc);
    // - free_obj_count.
    const auto freeObjCount = objCount - _usingObjCount - _reusableObjCount;
    stat.AddMember("free_obj_count", freeObjCount, jsonAlloc);
    // - free_obj_rate.
    stat.AddMember("free_obj_rate",
                   objCount != 0 ? static_cast<double>(freeObjCount) / objCount : 0.0,
                   jsonAlloc);

    // Stripe info:
    // - stripe_size: stripe size, in bytes.
    const auto stripeSize = sizeof(_ObjStripe) + sizeof(_WrappedObj) * objCountPerStripe;
    stat.AddMember("stripe_size", stripeSize, jsonAlloc);
    // - obj_count_per_stripe.
    stat.AddMember("obj_count_per_stripe", objCountPerStripe, jsonAlloc);
    // - stripe_count.
    stat.AddMember("stripe_count", _stripes.size(), jsonAlloc);

    // Memory info:
    // - using_mem: using memory, in bytes
    const auto usingMem = sizeof(Obj) * _usingObjCount;
    stat.AddMember("using_mem", usingMem, jsonAlloc);
    // - reusable_mem: reusable memory, in bytes.
    const auto reusableMem = sizeof(Obj) * _reusableObjCount;
    stat.AddMember("reusable_mem", reusableMem, jsonAlloc);
    // - free_mem: free memory, in bytes.
    const auto totalMem = sizeof(Obj) * objCount;
    stat.AddMember("free_mem", totalMem - usingMem - reusableMem, jsonAlloc);
    // - total_mem: total memory, in bytes.
    stat.AddMember("total_mem", totalMem, jsonAlloc);
    // - total_mem2: total memory, included objpool manage cost.
    stat.AddMember("total_mem2",
                   sizeof(LLBC_ObjPool) + // LLBC_ObjPool memory
                   sizeof(_ObjStripe) * _stripes.size() + // stripes memory
                   sizeof(_WrappedObj) * objCount, // wrapped object memory
                   jsonAlloc);

    return stat;
}

inline LLBC_ObjPool::LLBC_ObjPool(bool threadSafe)
: _threadSafe(threadSafe)

, _orderedDeleteNodes(nullptr)
, _orderedDeleteNodeTree(nullptr)
{
    // Init lock.
    __LLBC_INL_InitObjPoolLock();
}

inline LLBC_ObjPool::~LLBC_ObjPool()
{
    // Lock.
    __LLBC_INL_LockObjPool();

    // Delete acquired ordered delete typed objs.
    OperateOrderedDeleteNodes(false, false);
    // Delete other typed objs.
    for (auto &item : _typedObjPools)
        DelTypedObjPool(item.second);
    _typedObjPools.clear();

    // Delete ordered delete nodes.
    if (_orderedDeleteNodes)
    {
        for (auto it = _orderedDeleteNodes->begin();
             it != _orderedDeleteNodes->end();
             ++it)
            delete it->second;
        delete _orderedDeleteNodes;
        _orderedDeleteNodes = nullptr;

        delete _orderedDeleteNodeTree;
        _orderedDeleteNodeTree = nullptr;
    }

    // Unlock & Destroy lock.
    __LLBC_INL_UnlockObjPool();
    __LLBC_INL_DestroyObjPoolLock();
}

template <typename Obj>
void LLBC_ObjPool::Release(Obj *obj)
{
    // Get wrapped object.
    auto wrappedObj = reinterpret_cast<typename LLBC_TypedObjPool<Obj>::_WrappedObj *>(
        reinterpret_cast<uint8 *>(obj) - LLBC_TypedObjPool<Obj>::_objOffset);

    // Execute required checks.
    ASSERT(wrappedObj->unFlags.flags.inUsing && "Repeated release object");
    ASSERT(wrappedObj->magicNum == LLBC_CFG_CORE_OBJPOOL_OBJ_MAGIC_NUMBER &&
           "The object is not a objpool object");

    // Release object.
    auto off = _releaseObjMethOffset;
    (*reinterpret_cast<void(**)(void *, void *)>(
        reinterpret_cast<uint8 *>(wrappedObj->typedObjPool) -
            off))(wrappedObj->typedObjPool, obj);
}

inline void LLBC_ObjPool::Collect(bool deep)
{
    // Lock & Defer unlock.
    __LLBC_INL_LockObjPool();
    LLBC_Defer(__LLBC_INL_UnlockObjPool());

    // Exec collect.
    OperateOrderedDeleteNodes(true, deep);
    for (auto &item : _typedObjPools)
    {
        auto &wrappedTypedObjPool = item.second;
        if (!_orderedDeleteNodes ||
            _orderedDeleteNodes->find(wrappedTypedObjPool->rttiName) == _orderedDeleteNodes->end())
            wrappedTypedObjPool->Collect(item.second->typedObjPool, deep);
    }
}

inline LLBC_String LLBC_ObjPool::GetStatistics(int statFmt, bool pretty) const
{
    // Lock & Defer unlock.
    __LLBC_INL_LockObjPool();
    LLBC_Defer(__LLBC_INL_UnlockObjPool());

    // Gen CSV format stat.
    if (statFmt == LLBC_ObjPoolStatFormat::CSV)
    {
        // Add headline.
        LLBC_String stat;
        stat.append("name;reusable;"
                    "obj_size;wrapped_obj_size;obj_count;"
                    "using_obj_count;using_obj_rate;"
                    "reusable_obj_count;reusable_obj_rate;"
                    "free_obj_count;free_obj_rate;"
                    "stripe_size;obj_count_per_stripe;stripe_count;"
                    "using_mem;reusable_mem;free_mem;total_mem;total_mem2");

        // Add typed object pools stat.
        LLBC_Json::Document jsonDoc;
        for (auto &typedObjPoolItem : _typedObjPools)
        {
            auto &wrappedTypedObjPool = typedObjPoolItem.second;
            auto typedObjPoolStat = wrappedTypedObjPool->GetStatistics(wrappedTypedObjPool->typedObjPool,
                                                                       jsonDoc.GetAllocator());
            stat.append_format("\n%s;%d;"
                               "%u;%u;%u;%u;%.3f;%u;%.3f;%u;%.3f;"
                               "%u;%u;%u;"
                               "%u;%u;%u;%u;%u",
                               // Meta info:
                               typedObjPoolStat["name"].GetString(),
                               typedObjPoolStat["reusable"].GetBool(),
                               // Object info:
                               typedObjPoolStat["obj_size"].GetUint(),
                               typedObjPoolStat["wrapped_obj_size"].GetUint(),
                               typedObjPoolStat["obj_count"].GetUint(),
                               typedObjPoolStat["using_obj_count"].GetUint(),
                               typedObjPoolStat["using_obj_rate"].GetDouble(),
                               typedObjPoolStat["reusable_obj_count"].GetUint(),
                               typedObjPoolStat["reusable_obj_rate"].GetDouble(),
                               typedObjPoolStat["free_obj_count"].GetUint(),
                               typedObjPoolStat["free_obj_rate"].GetDouble(),
                               // Stripe info:
                               typedObjPoolStat["stripe_size"].GetUint(),
                               typedObjPoolStat["obj_count_per_stripe"].GetUint(),
                               typedObjPoolStat["stripe_count"].GetUint(),
                               // Memory info:
                               typedObjPoolStat["using_mem"].GetUint(),
                               typedObjPoolStat["reusable_mem"].GetUint(),
                               typedObjPoolStat["free_mem"].GetUint(),
                               typedObjPoolStat["total_mem"].GetUint(),
                               typedObjPoolStat["total_mem2"].GetUint());
        }

        return stat;
    }
    else // Generate default format(json) stat.
    {
        LLBC_Json::Document jsonDoc(LLBC_Json::kObjectType);
        auto &jsonAlloc = jsonDoc.GetAllocator();

        // Get typed object pools stat.
        LLBC_Json::Value typedObjPoolStats(LLBC_Json::kArrayType);
        for (auto &item : _typedObjPools)
        {
            auto &wrappedTypedObjPool = item.second;
            auto &typedObjPool = wrappedTypedObjPool->typedObjPool;
            typedObjPoolStats.PushBack(wrappedTypedObjPool->GetStatistics(typedObjPool, jsonAlloc),
                                       jsonAlloc);
        }
        jsonDoc.AddMember("typed_obj_pools", typedObjPoolStats.Move(), jsonAlloc);

        // Format.
        LLBC_Json::StringBuffer jsonSB;
        if (pretty)
        {
            LLBC_Json::PrettyWriter<LLBC_Json::StringBuffer> jsonWritter(jsonSB);
            jsonDoc.Accept(jsonWritter);
        }
        else
        {
            LLBC_Json::Writer<LLBC_Json::StringBuffer> jsonWritter(jsonSB);
            jsonDoc.Accept(jsonWritter);
        }

        return LLBC_String(jsonSB.GetString(), jsonSB.GetLength());
    }
}

template <typename Obj>
LLBC_FORCE_INLINE LLBC_TypedObjPool<Obj> *LLBC_ObjPool::GetTypedObjPool()
{
    typedef LLBC_TypedObjPool<Obj> _TypedObjPool;
    static const char *rttiName = typeid(Obj).name();

    // Lock.
    __LLBC_INL_LockObjPool();

    // Find typed object pool and return(if found).
    const auto it = _typedObjPools.find(rttiName);
    if (LIKELY(it != _typedObjPools.end()))
    {
        __LLBC_INL_UnlockObjPool();
        return reinterpret_cast<_TypedObjPool *>(it->second->typedObjPool);
    }

    // Create new typed object pool.
    auto wrappedTypedObjPool = 
        LLBC_Malloc(_WrappedTypedObjPool, sizeof(_WrappedTypedObjPool) + sizeof(_TypedObjPool));
    wrappedTypedObjPool->rttiName = rttiName;
    wrappedTypedObjPool->ReleaseObj = &_TypedObjPool::ReleaseObj_s;
    wrappedTypedObjPool->Destruct = &_TypedObjPool::Destruct_s;
    wrappedTypedObjPool->Collect = &_TypedObjPool::Collect_s;
    wrappedTypedObjPool->GetStatistics = &_TypedObjPool::GetStatistics_s;
    new (wrappedTypedObjPool->typedObjPool) _TypedObjPool(this, _threadSafe);
    _typedObjPools.emplace(rttiName, wrappedTypedObjPool);

    // Unlock.
    __LLBC_INL_UnlockObjPool();

    // Call typed object pool created event handler.
    _TypedObjPool *typedObjPool = reinterpret_cast<_TypedObjPool *>(wrappedTypedObjPool->typedObjPool);
    LLBC_ObjReflector::OnTypedObjPoolCreated<Obj>(typedObjPool);

    // Return new typed object pool.
    return typedObjPool;
}

template <typename ObjA, typename ObjB>
int LLBC_ObjPool::EnsureDeletionBefore()
{
    // Same object type check.
    const LLBC_CString frontName(typeid(ObjA).name());
    const LLBC_CString backName(typeid(ObjB).name());
    LLBC_SetErrAndReturnIf(frontName == backName, LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    // Lock & defer unlock.
    __LLBC_INL_LockObjPool();
    LLBC_Defer(__LLBC_INL_UnlockObjPool());

    // Defer create ordered delete node containers.
    if (!_orderedDeleteNodes)
    {
        _orderedDeleteNodes = new std::remove_pointer<decltype(_orderedDeleteNodes)>::type;
        _orderedDeleteNodeTree = new std::remove_pointer<decltype(_orderedDeleteNodeTree)>::type;
    }

    // Try fetch frontNode & backNode.
    _OrderedDeleteNode *frontNode = nullptr;
    auto it = _orderedDeleteNodes->find(frontName);
    LLBC_DoIf(it != _orderedDeleteNodes->end(), frontNode = it->second);

    _OrderedDeleteNode *backNode = nullptr;
    it = _orderedDeleteNodes->find(backName);
    LLBC_DoIf(it != _orderedDeleteNodes->end(), backNode = it->second);

    // Case 1: frontNode & backNode never add to container.
    if (!frontNode && !backNode)
    {
        frontNode = new _OrderedDeleteNode(frontName);
        backNode = new _OrderedDeleteNode(backName);
        _orderedDeleteNodes->emplace(frontName, frontNode);
        _orderedDeleteNodes->emplace(backName, backNode);

        frontNode->AddBack(backNode);
        _orderedDeleteNodeTree->emplace(frontName, frontNode);

        return LLBC_OK;
    }

    // Case 2: frontNode found, backNode not found.
    if (frontNode && !backNode)
    {
        backNode = new _OrderedDeleteNode(backName);
        _orderedDeleteNodes->emplace(backName, backNode);

        frontNode->AddBack(backNode);

        return LLBC_OK;
    }

    // Case 3: frontNode not found, backNode found.
    if (!frontNode && backNode)
    {
        frontNode = new _OrderedDeleteNode(frontName);
        _orderedDeleteNodes->emplace(frontName, frontNode);
        if (!backNode->GetFront())
        {
            _orderedDeleteNodeTree->erase(backName);

            frontNode->AddBack(backNode);
            _orderedDeleteNodeTree->emplace(frontName, frontNode);
        }
        else
        {
            auto grandFrontNode = backNode->GetFront();
            ASSERT(grandFrontNode->RemoveBack(backName) == LLBC_OK && "llbc framework internal error");

            frontNode->AddBack(backNode);
            grandFrontNode->AddBack(frontNode);
        }

        return LLBC_OK;
    }

    // Case 4: frontNode & backNode found.
    // Adjust eg: 
    //   before adjust nodes graph:
    //      A --> X --> Y1 --> Z1
    //             |--> Y2 --> Z2
    //      B --> I --> J1 --> K1
    //             |--> J2 --> K2
    // If caller acquire X before I delete(X --> I), below graph is the adjusted nodes graph:
    //      A --> X --> B -- >I --> Y1 --> Z1
    //                         |--> Y2 --> Z2
    //                         |--> J1 --> K1
    //                         |--> J2 --> K2
    LLBC_ReturnIf(frontNode->IsBack(backName), LLBC_OK);
    LLBC_SetErrAndReturnIf(frontNode->IsFront(backName), LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    auto backRootNode = backNode->GetRoot();
    auto frontRootNode = frontNode->GetRoot();
    if (!backRootNode)
        _orderedDeleteNodeTree->erase(backName);
    else if (frontRootNode != backRootNode)
        _orderedDeleteNodeTree->erase(backRootNode->GetName());
    else
        backNode->GetFront()->RemoveBack(backName);

    for (auto &item : frontNode->GetBacks())
        backNode->AddBack(item.second);

    if (backRootNode && frontRootNode != backRootNode)
        frontNode->AddBack(backRootNode);
    else
        frontNode->AddBack(backNode);

    return LLBC_OK;
}

inline LLBC_String LLBC_ObjPool::GetOrderedDeleteTree(bool pretty) const
{
    LLBC_Json::Document jsonDoc(LLBC_Json::kArrayType);
    auto &jsonAlloc = jsonDoc.GetAllocator();

    __LLBC_INL_LockObjPool();
    if (_orderedDeleteNodeTree)
    {
        for (auto it = _orderedDeleteNodeTree->begin();
             it != _orderedDeleteNodeTree->end();
             ++it)
            jsonDoc.PushBack(
                             it->second->GetOrderedDeleteTree( jsonDoc),
                             jsonAlloc);
    }
    __LLBC_INL_UnlockObjPool();

    LLBC_Json::StringBuffer jsonSB;
    if (pretty)
    {
        LLBC_Json::PrettyWriter<LLBC_Json::StringBuffer> jsonWritter(jsonSB);
        jsonDoc.Accept(jsonWritter);
    }
    else
    {
        LLBC_Json::Writer<LLBC_Json::StringBuffer> jsonWriter(jsonSB);
        jsonDoc.Accept(jsonWriter);
    }

    return LLBC_String(jsonSB.GetString(), jsonSB.GetLength());
}

inline LLBC_ObjPool::_OrderedDeleteNode::_OrderedDeleteNode(const LLBC_CString &name)
: _name(name)
, _front(nullptr)
, _backs(nullptr)
{
}

inline LLBC_ObjPool::_OrderedDeleteNode::~_OrderedDeleteNode()
{
    LLBC_DoIf(_backs, delete _backs);
}

inline LLBC_ObjPool::_OrderedDeleteNode *LLBC_ObjPool::_OrderedDeleteNode::GetRoot() const
{
    if (!_front)
        return nullptr;

    _OrderedDeleteNode *root = _front;
    while (root->_front)
        root = root->_front;

    return root;
}

inline void LLBC_ObjPool::DelTypedObjPool(_WrappedTypedObjPool *wrappedTypedObjPool)
{
    wrappedTypedObjPool->Destruct(wrappedTypedObjPool->typedObjPool);
    free(wrappedTypedObjPool);
}

inline const std::map<LLBC_CString, LLBC_ObjPool::_OrderedDeleteNode *> &
LLBC_ObjPool::_OrderedDeleteNode::GetBacks() const
{
    static const std::map<LLBC_CString, _OrderedDeleteNode *> emptyNodes;
    return _backs ? *_backs : emptyNodes;
}

inline int LLBC_ObjPool::_OrderedDeleteNode::AddBack(_OrderedDeleteNode *backNode)
{
    LLBC_SetErrAndReturnIf(IsBack(backNode->GetName()), LLBC_ERROR_REPEAT, LLBC_FAILED);
    LLBC_DoIf(!_backs, _backs = new std::remove_pointer<decltype(_backs)>::type);

    backNode->_front = this;
    _backs->emplace(backNode->GetName(), backNode);

    return LLBC_OK;
}

inline int LLBC_ObjPool::_OrderedDeleteNode::RemoveBack(const LLBC_CString &name)
{
    LLBC_SetErrAndReturnIf(!_backs, LLBC_ERROR_NOT_FOUND, LLBC_FAILED);
    for (auto backNodeIt = _backs->begin(); backNodeIt != _backs->end(); ++backNodeIt)
    {
        auto backNode = backNodeIt->second;
        if (backNode->GetName() != name)
        {
            LLBC_ReturnIf(backNode->RemoveBack(name) == LLBC_OK, LLBC_OK);
            continue;
        }

        if (backNode->_backs && !backNode->_backs->empty())
        {
            for (auto &item : *backNode->_backs)
            {
                item.second->_front = this;
                _backs->insert(item);
            }

            backNode->_backs->clear();
        }

        _backs->erase(backNodeIt);
        backNode->_front = nullptr;

        return LLBC_OK;
    }

    LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
    return LLBC_FAILED;
}

inline bool LLBC_ObjPool::_OrderedDeleteNode::IsFront(const LLBC_CString &name) const
{
    LLBC_ReturnIf(!_front, false);
    return _front->_name == name ? true : _front->IsFront(name);
}

inline bool LLBC_ObjPool::_OrderedDeleteNode::IsBack(const LLBC_String &name) const
{
    LLBC_ReturnIf(!_backs || _backs->empty(), false);
    LLBC_Foreach(*_backs,
                 LLBC_ReturnIf(item.second->_name == name || item.second->IsBack(name), true));
    return false;
}

inline LLBC_Json::Value LLBC_ObjPool::_OrderedDeleteNode::GetOrderedDeleteTree(LLBC_Json::Document &jsonDoc) const
{
    LLBC_Json::Value nodeJson(LLBC_Json::kObjectType);
    nodeJson.AddMember("type",
                       LLBC_Json::Value().SetString(_name.c_str(),
                                                    _name.size(), 
                                                    jsonDoc.GetAllocator()),
                       jsonDoc.GetAllocator());

    if (_backs && !_backs->empty())
    {
        LLBC_Json::Value postNodesJson(LLBC_Json::kArrayType);
        for (auto it = _backs->begin(); it != _backs->end(); ++it)
            postNodesJson.PushBack(it->second->GetOrderedDeleteTree(jsonDoc), jsonDoc.GetAllocator());
        nodeJson.AddMember("post_delete_types", postNodesJson, jsonDoc.GetAllocator());
    }

    return nodeJson;
}

inline void LLBC_ObjPool::OperateOrderedDeleteNodes(bool isCollect, bool deepCollect)
{
    LLBC_ReturnIf(!_orderedDeleteNodeTree || _orderedDeleteNodeTree->empty(), void());

    for (auto &item: *_orderedDeleteNodeTree)
        OperateOneOrderedDeleteNode(item.second, isCollect, deepCollect);
}

inline void LLBC_ObjPool::OperateOneOrderedDeleteNode(_OrderedDeleteNode *orderedDelNode,
                                                      bool isCollect,
                                                      bool deepCollect)
{
    const auto it = std::find_if(_typedObjPools.begin(),
                                 _typedObjPools.end(),
                                 [orderedDelNode](const std::pair<const char *, _WrappedTypedObjPool *> &item) {
        return orderedDelNode->GetName() == item.first;
    });

    if (it != _typedObjPools.end())
    {
        auto wrappedTypedObjPool = it->second;
        if (isCollect)
        {
            wrappedTypedObjPool->Collect(wrappedTypedObjPool->typedObjPool, deepCollect);
        }
        else
        {
            DelTypedObjPool(wrappedTypedObjPool);
            _typedObjPools.erase(it);
        }
    }

    for (auto &item : orderedDelNode->GetBacks())
        OperateOneOrderedDeleteNode(item.second, isCollect, deepCollect);
}

__LLBC_NS_END

#undef __LLBC_INL_InitObjPoolLock
#undef __LLBC_INL_DestroyObjPoolLock
#undef __LLBC_INL_LockObjPool
#undef __LLBC_INL_UnlockObjPool

