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
#include "llbc/core/objectpool/ObjectManipulator.h"

__LLBC_NS_BEGIN

template <typename ObjectType, typename LockType>
inline LLBC_ObjectPoolInst<ObjectType, LockType>::LLBC_ObjectPoolInst()
: _elemSize((sizeof(MemoryUnit) + sizeof(ObjectType) + (LLBC_INL_NS CheckSymbolSize << 1))) // CheckSymbolSize << 1 equivalent to CheckSymbolSize * 2
, _elemCnt(LLBC_CFG_CORE_OBJECT_POOL_MEMORY_BLOCK_SIZE / _elemSize + (LLBC_CFG_CORE_OBJECT_POOL_MEMORY_BLOCK_SIZE % _elemSize != 0 ? 1 : 0))
, _blockSize(_elemSize * _elemCnt)

, _blockCnt(0)
, _block(NULL)
, _memUnitUsageView(NULL)
{
}

template <typename ObjectType, typename LockType>
inline LLBC_ObjectPoolInst<ObjectType, LockType>::~LLBC_ObjectPoolInst()
{
    LLBC_LockGuard guard(_lock);

    if (_blockCnt != 0)
    {
        for (int blockIdx = 0; blockIdx != _blockCnt; ++blockIdx)
        {
            delete _block[blockIdx];
            delete _memUnitUsageView[blockIdx];
        }

        ::free(_block);
        ::free(_memUnitUsageView);
    }
}

template <typename ObjectType, typename LockType>
inline void *LLBC_ObjectPoolInst<ObjectType, LockType>::Get()
{
    _lock.Lock();

    void *obj;
    int oldBlockCnt = _blockCnt;
    for (int blockIdx = 0; blockIdx < oldBlockCnt; ++blockIdx)
    {
        if ((obj = FindFreeObj(_block[blockIdx])))
        {
            _lock.Unlock();
            return obj;
        }
    }

    AllocateMemoryBlock();
    obj = FindFreeObj(_block[oldBlockCnt]);

    _lock.Unlock();

    return obj;
}

template <typename ObjectType, typename LockType>
inline ObjectType *LLBC_ObjectPoolInst<ObjectType, LockType>::GetObject()
{
    return reinterpret_cast<ObjectType *>(Get());
}

template <typename ObjectType, typename LockType>
inline void LLBC_ObjectPoolInst<ObjectType, LockType>::Release(void *obj)
{
    bool destoryed = LLBC_ObjectManipulator::Reset<ObjectType>(obj);

    MemoryUnit *memUnit = reinterpret_cast<MemoryUnit *>(reinterpret_cast<uint8 *>(obj) - (LLBC_INL_NS CheckSymbolSize + sizeof(MemoryUnit)));
    if (destoryed)
        memUnit->inited = false;

    MemoryBlock *memBlock = reinterpret_cast<MemoryBlock *>(reinterpret_cast<uint8 *>(memUnit) - memUnit->seq * _elemSize - sizeof(MemoryBlock));

    _lock.Lock();

    CircularBuffer<MemoryUnit *> *& memUnitView = _memUnitUsageView[memBlock->seq];
    memUnitView->Push(memUnit);

    _lock.Unlock();
}

template <typename ObjectType, typename LockType>
inline void LLBC_ObjectPoolInst<ObjectType, LockType>::ReleaseObject(ObjectType* obj)
{
    Release(obj);
}

template <typename ObjectType, typename LockType>
inline void LLBC_ObjectPoolInst<ObjectType, LockType>::AllocateMemoryBlock()
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

#if LLBC_DEBUG
    ::memset(memBlock->buff, 0, _blockSize);
#endif

    memBlock->seq = _blockCnt;
    for (int idx = 0; idx < _elemCnt; ++idx)
    {
        MemoryUnit *memUnit = reinterpret_cast<MemoryUnit *>(reinterpret_cast<uint8 *>(memBlock->buff) + _elemSize * idx);
        memUnit->inited = false;
        memUnit->seq = idx;

#if LLBC_DEBUG 
        *(reinterpret_cast<sint64 *>(memUnit->buff)) = LLBC_INL_NS BeginingSymbol;
        *(reinterpret_cast<sint64 *>(reinterpret_cast<uint8 *>(memUnit) + _elemSize - LLBC_INL_NS CheckSymbolSize)) = LLBC_INL_NS EndingSymbol;
#endif
        memUnitView->Push(memUnit);
    }

    _block[_blockCnt] = memBlock;
    _memUnitUsageView[_blockCnt] = memUnitView;

    // Update block number.
    ++_blockCnt;
}

template <typename ObjectType, typename LockType>
inline void *LLBC_ObjectPoolInst<ObjectType, LockType>::FindFreeObj(MemoryBlock *memBlock)
{
    CircularBuffer<MemoryUnit *> *& memUnitView = _memUnitUsageView[memBlock->seq];
    if (memUnitView->IsEmpty())
        return NULL;

    MemoryUnit* memUnit = memUnitView->Pop();

#if LLBC_DEBUG 
    ASSERT(*(reinterpret_cast<sint64 *>(memUnit->buff)) == LLBC_INL_NS BeginingSymbol && "LLBC_ObjectPoolInst::Get() memory unit is dirty");
    ASSERT(*(reinterpret_cast<sint64 *>(
        reinterpret_cast<uint8 *>(memUnit) + _elemSize - LLBC_INL_NS CheckSymbolSize)) == LLBC_INL_NS EndingSymbol &&
        "LLBC_ObjectPoolInst::Get() memory unit is dirty");
#endif // LLBC_DEBUG

    void *obj = memUnit->buff + LLBC_INL_NS CheckSymbolSize;
    if (!memUnit->inited)
    {
        LLBC_ObjectManipulator::New<ObjectType>(obj);
        memUnit->inited = true;
    }

    return obj;
}

__LLBC_NS_END

#endif // __LLBC_CORE_OBJECT_POOL_OBJECT_POOL_INSTANCE_H__
