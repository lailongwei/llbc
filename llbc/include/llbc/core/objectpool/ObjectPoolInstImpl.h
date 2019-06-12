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
, _bitViewSize(_elemCnt / LLBC_INL_NS BitViewElemSize + (_elemCnt % LLBC_INL_NS BitViewElemSize != 0 ? LLBC_INL_NS BitViewElemSize : 0))

, _blockCnt(0)
, _block(NULL)
, _blockBitView(NULL)
{
}

template <typename ObjectType, typename LockType>
inline LLBC_ObjectPoolInst<ObjectType, LockType>::~LLBC_ObjectPoolInst()
{
    LLBC_LockGuard guard(_lock);

    if (_blockCnt != 0)
    {
        for (sint32 blockIdx = 0; blockIdx != _blockCnt; ++blockIdx)
        {
            delete _block[blockIdx];
            delete _blockBitView[blockIdx];
        }

        ::free(_block);
        ::free(_blockBitView);
    }
}

template <typename ObjectType, typename LockType>
inline void *LLBC_ObjectPoolInst<ObjectType, LockType>::Get()
{
    _lock.Lock();

    void *obj;
    int oldBlockCnt = _blockCnt;
    for (sint32 blockIdx = 0; blockIdx < _blockCnt; ++blockIdx)
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
inline void LLBC_ObjectPoolInst<ObjectType, LockType>::Release(void* obj)
{
    _lock.Lock();

    bool destoryed = LLBC_ObjectManipulator::Reset<ObjectType>(obj);
    MemoryUnit *memUnit = reinterpret_cast<MemoryUnit *>(reinterpret_cast<uint8 *>(obj) - (LLBC_INL_NS CheckSymbolSize + sizeof(MemoryUnit)));
    if (destoryed)
        memUnit->inited = false;

    MemoryBlock *memBlock = reinterpret_cast<MemoryBlock *>(reinterpret_cast<uint8 *>(memUnit) - memUnit->seq * _elemSize - sizeof(MemoryBlock));
    MemoryBitView *& bitView = _blockBitView[memBlock->seq];
    ++bitView->freeCnt;
    *(reinterpret_cast<sint64 *>(bitView->bits) + (memUnit->seq >> 6)) &= ~(LLBC_INL_NS One << (memUnit->seq & 0x3f));

    _lock.Unlock();
}

template <typename ObjectType, typename LockType>
inline void LLBC_ObjectPoolInst<ObjectType, LockType>::AllocateMemoryBlock()
{
    // Allocate new block and bitview.
    if (_blockCnt == 0)
    {
        _block = reinterpret_cast<MemoryBlock **>(::malloc(sizeof(MemoryBlock *)));
        _blockBitView = reinterpret_cast<MemoryBitView **>(::malloc(sizeof(MemoryBitView *)));
    }
    else
    {
        _block = reinterpret_cast<MemoryBlock **>(::realloc(_block, sizeof(MemoryBlock *) * (_blockCnt + 1)));
        _blockBitView = reinterpret_cast<MemoryBitView **>(::realloc(_blockBitView, sizeof(MemoryBitView *) * (_blockCnt + 1)));
    }

    // Fill new block content.
    MemoryBlock* memBlock = reinterpret_cast<MemoryBlock *>(::malloc(sizeof(MemoryBlock) + _blockSize));

#if LLBC_DEBUG
    ::memset(memBlock->buff, 0, _blockSize);
#endif

    memBlock->seq = _blockCnt;
    for (sint32 idx = 0; idx < _elemCnt; ++idx)
    {
        MemoryUnit* memUnit = reinterpret_cast<MemoryUnit *>(reinterpret_cast<uint8 *>(memBlock->buff) + _elemSize * idx);
        memUnit->inited = false;
        memUnit->seq = idx;

#if LLBC_DEBUG 
        *(reinterpret_cast<sint64 *>(memUnit->buff)) = LLBC_INL_NS BeginingSymbol;
        *(reinterpret_cast<sint64 *>(reinterpret_cast<uint8 *>(memUnit) + _elemSize - LLBC_INL_NS CheckSymbolSize)) = LLBC_INL_NS EndingSymbol;
#endif

    }

    _block[_blockCnt] = memBlock;

    // Fill new bitview content.
    MemoryBitView *bitView = reinterpret_cast<MemoryBitView *>(::calloc(sizeof(MemoryBitView) + _bitViewSize, 1));
    bitView->freeCnt = _elemCnt;
    _blockBitView[_blockCnt] = bitView;

    // Update block number.
    ++_blockCnt;
}

template <typename ObjectType, typename LockType>
void *LLBC_ObjectPoolInst<ObjectType, LockType>::FindFreeObj(MemoryBlock *memBlock)
{
    MemoryBitView *&bitView = _blockBitView[memBlock->seq];
    if (bitView->freeCnt == 0)
        return NULL;

    sint32 unusedUnitSeq = 0;
    uint8 *viewBits = bitView->bits;
    for (size_t bitViewIdx = 0; bitViewIdx != _bitViewSize; bitViewIdx += LLBC_INL_NS BitViewElemSize)
    {
        uint64 &bitViewVal = *(reinterpret_cast<uint64 *>(viewBits + bitViewIdx));
        sint32 curUnusedUnitSeq = DetectIdleMemoryUnit(bitViewVal);
        if (curUnusedUnitSeq == -1)
            continue;

        unusedUnitSeq = bitViewIdx * 8 + curUnusedUnitSeq;
        if (unusedUnitSeq >= _elemCnt)
            continue;

        // Mask bitview bits.
        --bitView->freeCnt;
        bitViewVal |= (LLBC_INL_NS One << curUnusedUnitSeq);

        MemoryUnit* memUnit = reinterpret_cast<MemoryUnit *>(memBlock->buff + unusedUnitSeq * _elemSize);

#if LLBC_DEBUG 
        ASSERT(*(reinterpret_cast<sint64 *>(memUnit->buff)) == LLBC_INL_NS BeginingSymbol && "LLBC_ObjectPoolInst::Get() memory unit is dirty");
        ASSERT(*(reinterpret_cast<sint64 *>(
            reinterpret_cast<uint8 *>(memUnit) + _elemSize - LLBC_INL_NS CheckSymbolSize)) == LLBC_INL_NS EndingSymbol &&
            "LLBC_ObjectPoolInst::Get() memory unit is dirty");
#endif
        void *obj = memUnit->buff + LLBC_INL_NS CheckSymbolSize;
        if (!memUnit->inited)
        {
            LLBC_ObjectManipulator::New<ObjectType>(obj);
            memUnit->inited = true;
        }

        return obj;
    }

    return NULL;
}

template <typename ObjectType, typename LockType>
sint32 LLBC_ObjectPoolInst<ObjectType, LockType>::DetectIdleMemoryUnit(const uint64 &bitView)
{
    if (bitView == static_cast<uint64>(-1))
        return -1;

    sint32 left = DetectIdleMemoryUnit(static_cast<uint32>(bitView));
    if (left >= 0)
        return left;

    return DetectIdleMemoryUnit(bitView >> (sizeof(uint32) * 8)) + sizeof(uint32) * 8;
}

template <typename ObjectType, typename LockType>
sint32 LLBC_ObjectPoolInst<ObjectType, LockType>::DetectIdleMemoryUnit(const uint32 &bitView)
{
    if (bitView == static_cast<uint32>(-1))
        return -1;

    sint32 left = DetectIdleMemoryUnit(static_cast<uint16>(bitView));
    if (left >= 0)
        return left;

    return DetectIdleMemoryUnit(bitView >> (sizeof(uint16) * 8)) + sizeof(uint16) * 8;
}

template <typename ObjectType, typename LockType>
sint32 LLBC_ObjectPoolInst<ObjectType, LockType>::DetectIdleMemoryUnit(const uint16 &bitView)
{
    if (bitView == 0xffff)
        return -1;

    if ((bitView | 0xfffe) == 0xfffe)
        return 0;
    if ((bitView | 0xfffd) == 0xfffd)
        return 1;
    if ((bitView | 0xfffb) == 0xfffb)
        return 2;
    if ((bitView | 0xfff7) == 0xfff7)
        return 3;
    if ((bitView | 0xffef) == 0xffef)
        return 4;
    if ((bitView | 0xffdf) == 0xffdf)
        return 5;
    if ((bitView | 0xffbf) == 0xffbf)
        return 6;
    if ((bitView | 0xff7f) == 0xff7f)
        return 7;
    if ((bitView | 0xfeff) == 0xfeff)
        return 8;
    if ((bitView | 0xfdff) == 0xfdff)
        return 9;
    if ((bitView | 0xfbff) == 0xfbff)
        return 10;
    if ((bitView | 0xf7ff) == 0xf7ff)
        return 11;
    if ((bitView | 0xefff) == 0xefff)
        return 12;
    if ((bitView | 0xdfff) == 0xdfff)
        return 13;
    if ((bitView | 0xbfff) == 0xbfff)
        return 14;
    if ((bitView | 0x7fff) == 0x7fff)
        return 15;

    ASSERT(false && "llbc framework internal error!");

    return -1;
}

__LLBC_NS_END

#endif //__LLBC_CORE_OBJECT_POOL_OBJECT_POOL_INSTANCE_H__