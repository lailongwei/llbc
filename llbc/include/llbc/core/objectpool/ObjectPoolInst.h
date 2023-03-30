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

#include "llbc/core/algo/RingBuffer.h"
#include "llbc/core/thread/DummyLock.h"
#include "llbc/core/objectpool/IObjectPoolInst.h"

// Disable some warnings.
#if LLBC_TARGET_PLATFORM_WIN32
 #pragma warning(push)
 #pragma warning(disable:4200)
#endif // LLBC_TARGET_PLATFORM_WIN32


__LLBC_INTERNAL_NS_BEGIN

// Define BeginingSymbol/EndSymbol, CheckSymbolSize.
#if LLBC_CFG_CORE_OBJECT_POOL_DEBUG
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
#endif // LLBC_CFG_CORE_OBJECT_POOL_DEBUG

__LLBC_INTERNAL_NS_END


__LLBC_NS_BEGIN

// Pre-declare some classes
template <typename ObjectType>
class LLBC_ObjectGuard;

/**
* \brief The object pool instance encapsulation.
*/
template <typename ObjectType>
class LLBC_ObjectPoolInst : public LLBC_IObjectPoolInst
{
public:
    /**
     * using definition.
     */
    using value_type = ObjectType;

private:
    /**
     * The structure of memory unit.
     */
    struct MemoryBlock;
    struct MemoryUnit
    {
        MemoryBlock *block;     // ownered this memory unit's block.

        union
        {
            struct
            {
                bool inited;            // Object has initialized or not.
                bool referencableObj;   // Indicate object is referencable object or not.
                bool inUsing;           // Using flag.
            } flags;
            uint32 flagsVal;            // all flags.
        } unFlags;
        sint32 seq;             // The location seq of memory unit.

        uint8 buff[0];          // The begin address of buffer.
    };

    /**
    * The structure of memory block.
    */
    struct MemoryBlock
    {
        sint32 seq;            // the seq of memory block.
        #if LLBC_64BIT_PROCESSOR
        sint32 unused;         // if in 64bit arch, use for memory align.
        #endif
        LLBC_RingBuffer<MemoryUnit *> *freeUnits; // the free units for the memory block.
        uint8 buff[0];         // The begin address of buffer.
    };

public:
    LLBC_ObjectPoolInst(LLBC_IObjectPool *objPool, LLBC_ILock *lock);
    virtual ~LLBC_ObjectPoolInst();

public:
    /**
    * Get object.
    * @return void * - the object pointer.
    */
    virtual void *Get();
    /**
     * Get referencable object.
     * @return void * - the object pointer.
     */
    virtual void *GetReferencable();

    /**
     * Get object.
     * @return ObjectType * - the object pointer.
     */
    ObjectType *GetObject();

    /**
     * Get object.
     * @return ObjectType * - the object pointer.
     */
    ObjectType *GetReferencableObject();

    /**
     * Get guarded object.
     * @return LLBC_ObjectGuard<ObjectType> - the guarded object.
     */
    LLBC_ObjectGuard<ObjectType> GetGuarded();

    /**
     * Release object.
     * @param[in] obj - the object pointer.
     */
    virtual void Release(void *obj);

    /**
    * Release object.
    * @param[in] obj - the object pointer.
    */
    void ReleaseObject(ObjectType *obj);

    /**
     * Get pool instance name.
     * @return const char * - the pool instance name.
     */
    virtual const char *GetPoolInstName();

    /**
     * Check this object pool instance is thread safety or not.
     * @return bool - return true if is thread safety, otherwise thread unsafety.
     */
    virtual bool IsThreadSafety() const;

public:
    /**
     * Perform object pool statistic.
     * @param[out] stat - the statstic info.
     */
    virtual void Stat(LLBC_ObjectPoolInstStat &stat) const;

protected:
    /**
     * Release referencable object.
    * @param[in] obj - the object pointer.
     */
    virtual void ReleaseReferencable(void *obj);

private:
    /**
     * Allocate a new memory block. 
     */
    void AllocateMemoryBlock();

    /**
     * Find a free object from memory block.
     * @param[in] memBlock        - memory block.
     * @param[in] referencableObj - is referencable object or not.
     * @return void * - the object pointer.
     */
    void *FindFreeObj(MemoryBlock *&memBlock, const bool &referencableObj);

    /**
     * Internal get object implement.
     * @param[in] referencableObj - specific get referencable object or not.
     * @return void * - the object pointer.
     */
    virtual void *Get(const bool &referencableObj);

    /**
     * Release object.
     * @param[in] memUnit - the object memory unit.
     * @param[in] obj     - the object pointer.
     */
    virtual void Release(MemoryUnit *memUnit, void *obj);

private:
    // Disable assignment.
    LLBC_DISABLE_ASSIGNMENT(LLBC_ObjectPoolInst);

private:
    const char *_poolInstName;

    const int _elemSize;
    const int _elemCnt;
    const int _blockSize;

    int _blockCnt;
    MemoryBlock **_blocks;
    LLBC_RingBuffer<MemoryBlock *> _freeBlocks;

    LLBC_ILock *_lock;
};

__LLBC_NS_END

#include "llbc/core/objectpool/ObjectPoolInstInl.h"

// Restore some warnings.
#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(pop)
#endif // LLBC_TARGET_PLATFORM_WIN32


