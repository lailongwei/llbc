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

#ifdef __LLBC_CORE_OBJECT_POOL_OBJECT_POOL_INSTANCE_H__

#include "llbc/core/thread/Guard.h"

#include "llbc/core/objectpool/ObjectGuard.h"
#include "llbc/core/objectpool/ObjectManipulator.h"
#include "llbc/core/objectpool/PoolObjectReflection.h"

__LLBC_NS_BEGIN

template <typename ObjectType>
LLBC_FORCE_INLINE LLBC_ObjectPoolInst<ObjectType>::LLBC_ObjectPoolInst(LLBC_IObjectPool *objPool, LLBC_ILock *lock)
: LLBC_IObjectPoolInst(objPool)
, _poolInstName(typeid(ObjectType).name())

, _elemSize((sizeof(MemoryUnit) + 
            (sizeof(ObjectType) % LLBC_CFG_CORE_OBJECT_POOL_MEMORY_ALIGN ? 
                LLBC_CFG_CORE_OBJECT_POOL_MEMORY_ALIGN * (sizeof(ObjectType) / LLBC_CFG_CORE_OBJECT_POOL_MEMORY_ALIGN + 1) : 
                sizeof(ObjectType)) + (LLBC_INL_NS CheckSymbolSize << 1))) // CheckSymbolSize << 1 equivalent to CheckSymbolSize * 2
, _elemCnt(LLBC_CFG_CORE_OBJECT_POOL_MEMORY_BLOCK_SIZE / _elemSize + (LLBC_CFG_CORE_OBJECT_POOL_MEMORY_BLOCK_SIZE % _elemSize != 0 ? 1 : 0))
, _blockSize(_elemSize * _elemCnt)

, _blockCnt(0)
, _block(NULL)
, _memUnitUsageView(NULL)

, _lock(lock)
{
}

template <typename ObjectType>
LLBC_FORCE_INLINE LLBC_ObjectPoolInst<ObjectType>::~LLBC_ObjectPoolInst()
{
    // Lock pool instance.
    _lock->Lock();

    // Notify ObjectType, whis type object pool instance will destroy.
    LLBC_ObjectManipulator::OnPoolInstDestroy<ObjectType>(*this);

    // Destroy objects and recycle memory.
    if (_blockCnt != 0)
    {
        for (int blockIdx = 0; blockIdx != _blockCnt; ++blockIdx)
        {
            MemoryBlock *&memBlock = _block[blockIdx];
            for (int unitIdx = 0; unitIdx != _elemCnt; ++unitIdx)
            {
                MemoryUnit *memUnit = reinterpret_cast<MemoryUnit *>(reinterpret_cast<uint8 *>(memBlock->buff) + _elemSize * unitIdx);
                if (!memUnit->unFlags.flags.inited)
                    continue;

                void *obj = reinterpret_cast<void *>(memUnit->buff + LLBC_INL_NS CheckSymbolSize);
                LLBC_ObjectManipulator::Delete<ObjectType>(obj);
            }

            ::free(memBlock);
            delete _memUnitUsageView[blockIdx];
        }

        LLBC_Free(_block);
        LLBC_Free(_memUnitUsageView);
    }

    // Unlock pool instance and destroy lock.
    _lock->Unlock();
    LLBC_Delete(_lock);
}

template <typename ObjectType>
LLBC_FORCE_INLINE void *LLBC_ObjectPoolInst<ObjectType>::Get()
{
    return Get(false);
}

template <typename ObjectType>
LLBC_FORCE_INLINE void *LLBC_ObjectPoolInst<ObjectType>::GetReferencable()
{
    return Get(true);
}

template <typename ObjectType>
LLBC_FORCE_INLINE ObjectType *LLBC_ObjectPoolInst<ObjectType>::GetObject()
{
    return reinterpret_cast<ObjectType *>(Get(false));
}

template <typename ObjectType>
LLBC_FORCE_INLINE ObjectType *LLBC_ObjectPoolInst<ObjectType>::GetReferencableObject()
{
    return reinterpret_cast<ObjectType *>(Get(true));
}

template <typename ObjectType>
LLBC_FORCE_INLINE LLBC_ObjectGuard<ObjectType> LLBC_ObjectPoolInst<ObjectType>::GetGuarded()
{
    return LLBC_ObjectGuard<ObjectType>(GetObject(), this);
}

template <typename ObjectType>
LLBC_FORCE_INLINE void LLBC_ObjectPoolInst<ObjectType>::Release(void *obj)
{
    #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
    ASSERT(obj != NULL && "LLBC_ObjectPoolInst::Release() could not release NULL pointer object!");
    #endif

    MemoryUnit *memUnit = reinterpret_cast<MemoryUnit *>(
        reinterpret_cast<uint8 *>(obj) - (LLBC_INL_NS CheckSymbolSize + sizeof(MemoryUnit)));
    #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
    ASSERT(!memUnit->unFlags.flags.referencableObj && 
           "LLBC_ObjectPoolInst::Release() could not release referencable object, "
           "please using LLBC_ReferencableObj::Release/AutoRelease methods to complete object release");
    #endif
    Release(memUnit, obj);
}

template <typename ObjectType>
LLBC_FORCE_INLINE void LLBC_ObjectPoolInst<ObjectType>::ReleaseObject(ObjectType* obj)
{
    Release(obj);
}

template <typename ObjectType>
const char * LLBC_ObjectPoolInst<ObjectType>::GetPoolInstName()
{
    return _poolInstName;
}

template <typename ObjectType>
LLBC_FORCE_INLINE bool LLBC_ObjectPoolInst<ObjectType>::IsThreadSafety() const
{
    return !_lock->IsDummyLock();
}

template <typename ObjectType>
LLBC_FORCE_INLINE void LLBC_ObjectPoolInst<ObjectType>::ReleaseReferencable(void *obj)
{
    #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
    ASSERT(obj != NULL && "LLBC_ObjectPoolInst::ReleaseReferencable() could not release NULL pointer object!");
    #endif

    MemoryUnit *memUnit = reinterpret_cast<MemoryUnit *>(
        reinterpret_cast<uint8 *>(obj) - (LLBC_INL_NS CheckSymbolSize + sizeof(MemoryUnit)));
    #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
    ASSERT(memUnit->unFlags.flags.referencableObj && 
           "LLBC_ObjectPoolInst::Release() could not release referencable object, "
           "please using LLBC_ReferencableObj::Release/AutoRelease methods to complete object release");
    #endif

    Release(memUnit, obj);
}

template <typename ObjectType>
LLBC_FORCE_INLINE void LLBC_ObjectPoolInst<ObjectType>::AllocateMemoryBlock()
{
    // Allocate new block and memory unit usage view.
    if (_blockCnt == 0)
    {
        _block = reinterpret_cast<MemoryBlock **>(::malloc(sizeof(MemoryBlock *)));
        _memUnitUsageView = reinterpret_cast<CircularBuffer<MemoryUnit *> **>(::malloc(sizeof(CircularBuffer<MemoryUnit *> *)));
    }
    else
    {
        _block = reinterpret_cast<MemoryBlock **>(::realloc(_block, sizeof(MemoryBlock *) * (_blockCnt + 1)));
        _memUnitUsageView = reinterpret_cast<CircularBuffer<MemoryUnit *> **>(::realloc(_memUnitUsageView, sizeof(CircularBuffer<MemoryUnit *> *) * (_blockCnt + 1)));
    }

    CircularBuffer<MemoryUnit *> *memUnitView = new CircularBuffer<MemoryUnit *>(_elemCnt);

    // Fill new block content.
    MemoryBlock* memBlock = reinterpret_cast<MemoryBlock *>(::malloc(sizeof(MemoryBlock) + _blockSize));

    #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
    ::memset(memBlock->buff, 0, _blockSize);
    #endif

    memBlock->seq = _blockCnt;
    #if LLBC_64BIT_PROCESSOR
    memBlock->unused = 0;
    #endif // 64bit-processor

    for (int idx = 0; idx < _elemCnt; ++idx)
    {
        MemoryUnit *memUnit = reinterpret_cast<MemoryUnit *>(reinterpret_cast<uint8 *>(memBlock->buff) + _elemSize * idx);
        memUnit->block = memBlock;
        #if !LLBC_CFG_CORE_OBJECT_POOL_DEBUG
        memUnit->unFlags.flagsVal = 0;
        #endif // !LLBC_CFG_CORE_OBJECT_POOL_DEBUG
        memUnit->seq = idx;

        #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
        *(reinterpret_cast<sint64 *>(memUnit->buff)) = LLBC_INL_NS BeginingSymbol;
        *(reinterpret_cast<sint64 *>(reinterpret_cast<uint8 *>(memUnit) + _elemSize - LLBC_INL_NS CheckSymbolSize)) = LLBC_INL_NS EndingSymbol;
        #endif // LLBC_CFG_CORE_OBJECT_POOL_DEBUG
        memUnitView->Push(memUnit);
    }

    _block[_blockCnt] = memBlock;
    _memUnitUsageView[_blockCnt] = memUnitView;

    // Update block number.
    ++_blockCnt;
}

template <typename ObjectType>
LLBC_FORCE_INLINE void *LLBC_ObjectPoolInst<ObjectType>::FindFreeObj(MemoryBlock *memBlock, const bool &referencableObj)
{
    CircularBuffer<MemoryUnit *> *&memUnitView = _memUnitUsageView[memBlock->seq];
    if (memUnitView->IsEmpty())
        return NULL;

    MemoryUnit *memUnit = memUnitView->Pop();
    #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
    ASSERT(*(reinterpret_cast<sint64 *>(memUnit->buff)) == LLBC_INL_NS BeginingSymbol && "LLBC_ObjectPoolInst::Get() memory unit is dirty");
    ASSERT(*(reinterpret_cast<sint64 *>(
        reinterpret_cast<uint8 *>(memUnit) + _elemSize - LLBC_INL_NS CheckSymbolSize)) == LLBC_INL_NS EndingSymbol &&
        "LLBC_ObjectPoolInst::Get() memory unit is dirty");
    if (memUnit->unFlags.flags.inited)
        ASSERT(memUnit->unFlags.flags.referencableObj == referencableObj && "LLBC_ObjectPoolInst::Get() memory unit referencable flag conflict");
    #endif

    #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
    void *obj = memUnit->buff + LLBC_INL_NS CheckSymbolSize;
    #else // !LLBC_CFG_CORE_OBJECT_POOL_DEBUG
    void *obj = memUnit->buff; //! Implic CheckSymbolSize is zero.
    #endif // LLBC_CFG_CORE_OBJECT_POOL_DEBUG
    if (!memUnit->unFlags.flags.inited)
    {
        LLBC_ObjectManipulator::New<ObjectType>(obj);
        ObjectType *typeObj = reinterpret_cast<ObjectType *>(obj);
        LLBC_PoolObjectReflection::MarkPoolObject<ObjectType>(typeObj, this);
        if (referencableObj)
        {
            memUnit->unFlags.flags.referencableObj = true;
            SetPoolInstToReferencablePoolObj(obj);
        }

        memUnit->unFlags.flags.inited = true;
    }

    // Mark using flag.
    memUnit->unFlags.flags.inUsing = true;

    return obj;
}

template <typename ObjectType>
LLBC_FORCE_INLINE void *LLBC_ObjectPoolInst<ObjectType>::Get(const bool &referencableObj)
{
    void *obj;

    _lock->Lock();

    int oldBlockCnt = _blockCnt;
    for (int blockIdx = 0; blockIdx < oldBlockCnt; ++blockIdx)
    {
        if ((obj = FindFreeObj(_block[blockIdx], referencableObj)))
        {
            _lock->Unlock();
            return reinterpret_cast<ObjectType *>(obj);
        }
    }

    AllocateMemoryBlock();
    obj = FindFreeObj(_block[oldBlockCnt], referencableObj);

    _lock->Unlock();

    return obj;
}

template <typename ObjectType>
LLBC_FORCE_INLINE void LLBC_ObjectPoolInst<ObjectType>::Release(MemoryUnit *memUnit, void *obj)
{
    // Repeated release check(only available when LLBC_DEBUG/LLBC_CFG_CORE_OBJECT_POOL_DEBUG enabled).
    #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
    ASSERT(memUnit->unFlags.flags.inUsing && "Repeated release object to object-pool!");
    #endif

    // Reset object data(call object pool clear method or delete object).
    if (LLBC_ObjectManipulator::Reset<ObjectType>(obj))
        memUnit->unFlags.flags.inited = false;

    // Reset using flag.
    memUnit->unFlags.flags.inUsing = false;

    // Push back to Circular buffer.
    _lock->Lock();
    _memUnitUsageView[memUnit->block->seq]->Push(memUnit);
    _lock->Unlock();
}

__LLBC_NS_END

#endif // __LLBC_CORE_OBJECT_POOL_OBJECT_POOL_INSTANCE_H__
