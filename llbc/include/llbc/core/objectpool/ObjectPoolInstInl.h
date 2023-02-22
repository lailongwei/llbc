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

#include "llbc/core/objectpool/ObjectGuard.h"
#include "llbc/core/objectpool/ObjectManipulator.h"
#include "llbc/core/objectpool/PoolObjectReflection.h"

#include "llbc/core/objectpool/ObjectPoolStat.h"

__LLBC_NS_BEGIN

template <typename ObjectType>
LLBC_FORCE_INLINE LLBC_ObjectPoolInst<ObjectType>::LLBC_ObjectPoolInst(LLBC_IObjectPool *objPool, LLBC_ILock *lock)
: LLBC_IObjectPoolInst(objPool)
, _poolInstName(typeid(ObjectType).name())

, _elemSize((sizeof(MemoryUnit) + 
            (sizeof(ObjectType) % LLBC_CFG_CORE_OBJECT_POOL_MEMORY_ALIGN ? 
                (LLBC_CFG_CORE_OBJECT_POOL_MEMORY_ALIGN * 
                 (sizeof(ObjectType) / LLBC_CFG_CORE_OBJECT_POOL_MEMORY_ALIGN + 1)) : 
                 (sizeof(ObjectType)) + (LLBC_INL_NS CheckSymbolSize << 1))))
, _elemCnt(static_cast<int>(LLBC_ObjectManipulator::GetPoolInstPerBlockUnitsNum<ObjectType>()))
, _blockSize(_elemSize * _elemCnt)

, _blockCnt(0)
, _blocks(nullptr)

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
            MemoryBlock *&memBlock = _blocks[blockIdx];
            for (int unitIdx = 0; unitIdx != _elemCnt; ++unitIdx)
            {
                MemoryUnit *memUnit =
                    reinterpret_cast<MemoryUnit *>(reinterpret_cast<uint8 *>(memBlock->buff) + _elemSize * unitIdx);
                if (!memUnit->unFlags.flags.inited)
                    continue;

                void *obj = reinterpret_cast<void *>(memUnit->buff + LLBC_INL_NS CheckSymbolSize);
                if (memUnit->unFlags.flags.referencableObj)
                {
                    #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
                    CheckRefCount(obj);
                    #endif
                }
                LLBC_ObjectManipulator::Delete<ObjectType>(obj);
            }

            delete memBlock->freeUnits;
            free(memBlock);
        }

        free(_blocks);
    }

    // Unlock pool instance and destroy lock.
    _lock->Unlock();
    delete _lock;
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
    // Do assert, makesure object is not null.
    #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
    ASSERT(obj != nullptr && "LLBC_ObjectPoolInst::Release() could not release nullptr pointer object!");
    #endif

    // Get memory unit, and do assert, makesure will release object is not referencable object.
    MemoryUnit *memUnit = reinterpret_cast<MemoryUnit *>(
        reinterpret_cast<uint8 *>(obj) - (LLBC_INL_NS CheckSymbolSize + sizeof(MemoryUnit)));
    #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
    ASSERT(!memUnit->unFlags.flags.referencableObj &&
           "LLBC_ObjectPoolInst::Release() could not release referencable object, "
           "please using LLBC_ReferencableObj::Release/AutoRelease methods to complete object release");
    #endif

    // Execute real release.
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
void LLBC_ObjectPoolInst<ObjectType>::Stat(LLBC_ObjectPoolInstStat& stat) const
{
    LLBC_LockGuard guard(*_lock);

    #if LLBC_TARGET_PLATFORM_WIN32
    stat.poolInstName = _poolInstName;
    #else
    stat.poolInstName = __LLBC_CxxDemangle(_poolInstName);
    #endif

    stat.blockMemorySize = _blockSize;
    stat.unitMemorySize = _elemSize;
    stat.blocks.resize(_blockCnt);
    for (int i = 0; i < _blockCnt; ++i)
    {
        // Stat memory block.
        MemoryBlock *&block = _blocks[i];
        LLBC_ObjectPoolBlockStat &blockStat = stat.blocks[i];
        blockStat.blockSeq = block->seq;
        blockStat.unitMemorySize = _elemSize;

        blockStat.freeUnitsNum = block->freeUnits->GetSize();
        blockStat.usedUnitsNum = block->freeUnits->GetCapacity() - block->freeUnits->GetSize();
        blockStat.allUnitsNum = block->freeUnits->GetCapacity();

        blockStat.freeUnitsMemory = blockStat.freeUnitsNum * blockStat.unitMemorySize;
        blockStat.usedUnitsMemory = blockStat.usedUnitsNum *blockStat.unitMemorySize;
        blockStat.allUnitsMemory = blockStat.allUnitsNum * blockStat.unitMemorySize;

        blockStat.innerUsedMemory =
            sizeof(MemoryBlock) + sizeof(LLBC_RingBuffer<MemoryUnit *>) + sizeof(MemoryUnit *) * _elemCnt;

        blockStat.totalMemory = blockStat.allUnitsMemory + blockStat.innerUsedMemory;

        blockStat.UpdateStrRepr();

        // Add memory block stat info to object pool instance stat info.
        stat.freeUnitsNum += blockStat.freeUnitsNum;
        stat.usedUnitsNum += blockStat.usedUnitsNum;
        stat.allUnitsNum += blockStat.allUnitsNum;

        stat.freeUnitsMemory += blockStat.freeUnitsMemory;
        stat.usedUnitsMemory += blockStat.usedUnitsMemory;
        stat.allUnitsMemory += blockStat.allUnitsMemory;

        stat.innerUsedMemory += blockStat.innerUsedMemory;

        stat.totalMemory += blockStat.totalMemory;
    }

    // Stat object pool instance self inner used memory.
    const size_t selfInnerUsedMemory = sizeof(LLBC_ObjectPoolInst) + // this object size.
                                       sizeof(MemoryBlock *) * _blockCnt + // allocated blocks pointer array size.
                                       sizeof(LLBC_RingBuffer<MemoryBlock *>) +
                                           sizeof(MemoryBlock *) * _blockCnt + // block ring-buffer size.
                                       sizeof(*_lock); // Lock object size.

    stat.innerUsedMemory += selfInnerUsedMemory;
    stat.totalMemory += selfInnerUsedMemory;

    stat.UpdateStrRepr();
}

template <typename ObjectType>
LLBC_FORCE_INLINE void LLBC_ObjectPoolInst<ObjectType>::ReleaseReferencable(void *obj)
{
    #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
    ASSERT(obj != nullptr && "LLBC_ObjectPoolInst::ReleaseReferencable() could not release nullptr pointer object!");
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
    if (UNLIKELY(_blockCnt == 0))
        _blocks = LLBC_Malloc(MemoryBlock *, sizeof(MemoryBlock *));
    else
        _blocks = LLBC_Realloc(MemoryBlock *, _blocks, sizeof(MemoryBlock *) * (_blockCnt + 1));
    LLBC_RingBuffer<MemoryUnit *> *freeUnits = new LLBC_RingBuffer<MemoryUnit *>(_elemCnt);

    // Fill new block content.
    MemoryBlock* memBlock = reinterpret_cast<MemoryBlock *>(malloc(sizeof(MemoryBlock) + _blockSize));

    #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
    memset(memBlock->buff, 0, _blockSize);
    #endif

    memBlock->seq = _blockCnt;
    #if LLBC_64BIT_PROCESSOR
    memBlock->unused = 0;
    #endif // 64bit-processor
    memBlock->freeUnits = freeUnits;

    for (int idx = 0; idx < _elemCnt; ++idx)
    {
        MemoryUnit *memUnit =
            reinterpret_cast<MemoryUnit *>(reinterpret_cast<uint8 *>(memBlock->buff) + _elemSize * idx);
        memUnit->block = memBlock;
        #if !LLBC_CFG_CORE_OBJECT_POOL_DEBUG
        memUnit->unFlags.flagsVal = 0;
        #endif // !LLBC_CFG_CORE_OBJECT_POOL_DEBUG
        memUnit->seq = idx;

        #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
        *(reinterpret_cast<sint64 *>(memUnit->buff)) = LLBC_INL_NS BeginingSymbol;
        *(reinterpret_cast<sint64 *>(reinterpret_cast<
          uint8 *>(memUnit) + _elemSize - LLBC_INL_NS CheckSymbolSize)) = LLBC_INL_NS EndingSymbol;
        #endif // LLBC_CFG_CORE_OBJECT_POOL_DEBUG
        freeUnits->Push(memUnit);
    }

    _blocks[_blockCnt] = memBlock;
    _freeBlocks.Push(memBlock);

    // Update block number.
    ++_blockCnt;
}

template <typename ObjectType>
LLBC_FORCE_INLINE void *LLBC_ObjectPoolInst<ObjectType>::FindFreeObj(MemoryBlock *&memBlock,
                                                                     const bool &referencableObj)
{
    // Do assert(makesure given block has free units).
    #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
    ASSERT(!memBlock->freeUnits->IsEmpty() && "Try pop empty memory block!");
    #endif

    // Pop free unit, and then not exist any free units after pop, pop this block from _freeBlocks.
    LLBC_RingBuffer<MemoryUnit *> *&freeUnits = memBlock->freeUnits;
    MemoryUnit *memUnit = freeUnits->Pop();
    if (UNLIKELY(freeUnits->IsEmpty()))
    {
        #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
        MemoryBlock *popBlock = _freeBlocks.Pop();
        ASSERT(popBlock == memBlock && "Object pool instance internal error, memory blocks dismatch!");
        #else
        _freeBlocks.Pop();
        #endif
    }

    // Unlock pool instance.
    _lock->Unlock();

    #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
    ASSERT(*(reinterpret_cast<sint64 *>(memUnit->buff)) ==
           LLBC_INL_NS BeginingSymbol && "LLBC_ObjectPoolInst::Get() memory unit is dirty");
    ASSERT(*(reinterpret_cast<sint64 *>(
        reinterpret_cast<uint8 *>(memUnit) + _elemSize - LLBC_INL_NS CheckSymbolSize)) == LLBC_INL_NS EndingSymbol &&
        "LLBC_ObjectPoolInst::Get() memory unit is dirty");
    if (memUnit->unFlags.flags.inited)
        ASSERT(memUnit->unFlags.flags.referencableObj ==
               referencableObj && "LLBC_ObjectPoolInst::Get() memory unit referencable flag conflict");
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
        LLBC_PoolObjectReflection::MarkPoolObject<ObjectType>(typeObj, this, referencableObj);
        if (referencableObj)
            memUnit->unFlags.flags.referencableObj = true;

        memUnit->unFlags.flags.inited = true;
    }

    // Mark using flag.
    memUnit->unFlags.flags.inUsing = true;

    return obj;
}

template <typename ObjectType>
LLBC_FORCE_INLINE void *LLBC_ObjectPoolInst<ObjectType>::Get(const bool &referencableObj)
{
    _lock->Lock();
    if (UNLIKELY(_freeBlocks.IsEmpty()))
        AllocateMemoryBlock();

    void *obj = FindFreeObj(_freeBlocks.Front(), referencableObj);

    // Unlocked in FindFreeObj() method.
    // _lock->Unlock();

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

    // Push back to freeUnits.
    _lock->Lock();

    LLBC_RingBuffer<MemoryUnit *> *&freeUnits = memUnit->block->freeUnits;
    if (UNLIKELY(freeUnits->IsEmpty())) // If the block has not free units before release object,
                                        // push this memory block to _freeBlocks.
        _freeBlocks.Push(memUnit->block);

    // Makesure ring-buffer is not full.
    #if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
    ASSERT(!freeUnits->IsFull() && "Try repeat release object!");
    #endif

    freeUnits->Push(memUnit);
    _lock->Unlock();
}

__LLBC_NS_END
