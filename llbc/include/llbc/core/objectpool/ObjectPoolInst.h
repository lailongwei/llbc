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

#ifndef __LLBC_CORE_OBJECT_POOL_OBJECT_POOL_INSTANCE_H__
#define __LLBC_CORE_OBJECT_POOL_OBJECT_POOL_INSTANCE_H__

#include "llbc/common/Common.h"
#include "llbc/core/thread/DummyLock.h"
#include "llbc/core/objectpool/IObjectPoolInst.h"

// Disable some warnings.
#if LLBC_TARGET_PLATFORM_WIN32
 #pragma warning(push)
 #pragma warning(disable:4200)
#endif // LLBC_TARGET_PLATFORM_WIN32

__LLBC_INTERNAL_NS_BEGIN

// Define bitview element size, in bytes.
const size_t BitViewElemSize = sizeof(LLBC_NS uint64);

// Define 64bit 0&1.
#if LLBC_TARGET_PLATFORM_WIN32
    const LLBC_NS sint64 Zero = 0I64;
    const LLBC_NS sint64 One = 1I64;
#else
    const LLBC_NS sint64 Zero = 0LL;
    const LLBC_NS sint64 One = 1LL;
#endif

// Define BeginingSymbol/EndSymbol, CheckSymbolSize.
#if LLBC_DEBUG
const size_t CheckSymbolSize = 8;

 #if LLBC_TARGET_PLATFORM_WIN32
    const LLBC_NS sint64 BeginingSymbol = 0xcdcdcdcdcdcdcdcdI64;
    const LLBC_NS sint64 EndingSymbol = 0xcdcdcdcdcdcdcdcdI64;
 #else // Non-Win32
    const LLBC_NS sint64 BeginingSymbol = 0xcdcdcdcdcdcdcdcdLL;
    const LLBC_NS sint64 EndingSymbol = 0xcdcdcdcdcdcdcdcdLL;
 #endif // LLBC_TARGET_PLATFORM_WIN32
#else // NDEBUG
const size_t CheckSymbolSize = 0;
#endif // LLBC_DEBUG

__LLBC_INTERNAL_NS_END


__LLBC_NS_BEGIN

/**
* \brief The object pool instance encapsulation.
*/
template <typename ObjectType, typename LockType = LLBC_DummyLock>
class LLBC_ObjectPoolInst : public LLBC_IObjectPoolInst
{
private:
    
    /**
     * The structure of memory unit.
     */
    struct MemoryUnit
    {
        bool inited;            // Object has initialized or not.
        sint32 seq;             // The location seq of memory unit.  

        uint8 buff[0];          // The begin address of buffer.
    };

    /**
    * The structure of memory block.
    */
    struct MemoryBlock
    {
        sint32 seq;            // the seq of memory block
        uint8 buff[0];         // The begin address of buffer.
    };

    /**
     * The structure of memory bit view.
     */
    struct MemoryBitView
    {
        sint32 freeCnt;        // count of free memory unit.
        uint8 bits[0];         // The begin address of buffer.
    };

public:
    LLBC_ObjectPoolInst();
    virtual ~LLBC_ObjectPoolInst();

public:
    /**
     * Get object.
     * @return void * - the object pointer.
     */
    virtual void *Get();

    /**
     * Release object.
     * @param[in] obj - the object pointer.
     */
    virtual void Release(void *obj);

protected:
    /**
     * Allocate a new memory block. 
     */
    void AllocateMemoryBlock();

    /**
     * Find a free object from memory block.
     * @param[in] memBlock - memory block.
     */
    void *FindFreeObj(MemoryBlock *memBlock);

    /**
     * Mask the bit state of bit view .
     * @param[in] bitView - bit view.
     * @param[in] index - bit index.
     * @param[in] used - true means the memory unit is used, otherwise the memory unit is free.
     */
    void MaskBitState(MemoryBitView *bitView, sint32 index, bool used);

    /**
    * Detect idle memory unit for 64bits bit view flag.
    * @param[in] bitView - bit view flag.
    */
    sint32 DetectIdleMemoryUnit(const uint64 &bitView);

    /**
    * Detect idle memory unit for 32bits bit view flag.
    * @param[in] bitView - bit view flag.
    */
    sint32 DetectIdleMemoryUnit(const uint32 &bitView);

    /**
    * Detect idle memory unit for 16bits bit view flag.
    * @param[in] bitView - bit view flag.
    */
    sint32 DetectIdleMemoryUnit(const uint16 &bitView);

private:
    const sint32 _elemSize;
    const sint32 _elemCnt;
    const sint32 _blockSize;
    const size_t _bitViewSize;

    sint32 _blockCnt;
    MemoryBlock **_block;
    MemoryBitView **_blockBitView;

    LockType _lock;
};

__LLBC_NS_END

#include "llbc/core/objectpool/ObjectPoolInstImpl.h"

// Restore some warnings.
#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(pop)
#endif // LLBC_TARGET_PLATFORM_WIN32

#endif // !__LLBC_CORE_OBJECT_POOL_OBJECT_POOL_INSTANCE_H__