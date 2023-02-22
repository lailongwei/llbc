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

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief The object pool block statistic info encapsulation.
 */
class LLBC_ObjectPoolBlockStat
{
public:
    int blockSeq; // block sequence.
    size_t unitMemorySize; // memory unit memory size(in bytes).

    size_t freeUnitsNum; // free memory units number.
    size_t usedUnitsNum; // used memory units number.
    size_t allUnitsNum; // all memory units number.

    size_t freeUnitsMemory; // free units memory(in bytes, approx)
    size_t usedUnitsMemory; // used units memory(in bytes, approx)
    size_t allUnitsMemory; // all units memory(in bytes, approx)
    size_t innerUsedMemory; // inner used memory, for use object pool memory block internal logic(in bytes, approx)

    size_t totalMemory; // total memory(in bytes, approx)

public:
    /**
     * Constructor.
     */
    LLBC_ObjectPoolBlockStat();

public:
    /**
     * Reset statistic info.
     */
    void Reset();


public:
    /**
     * Update string representation.
     */
    void UpdateStrRepr();

    /**
     * Get string representation.
     * @return const LLBC_String & - the string representation.
     */
    const LLBC_String &ToString() const;

private:
    LLBC_String _strRepr; // string representation.
};

/**
 * \brief The object pool instance statistic info encapsulation.
 */
class LLBC_ObjectPoolInstStat
{
public:
    LLBC_String poolInstName; // the object pool instance name

    size_t blockMemorySize; // the object pool block memory size
    size_t unitMemorySize; // the object pool unit memory size
    std::vector<LLBC_ObjectPoolBlockStat> blocks; // the blocks array, all blocks info are stored in here

    size_t freeUnitsNum; // all blocks free memmory units number
    size_t usedUnitsNum; // all blocks used memory units number
    size_t allUnitsNum; // all blocks total memory units number

    size_t freeUnitsMemory; // all blocks free memory(in bytes, approx)
    size_t usedUnitsMemory; // all blocks used memory(in bytes, approx)
    size_t allUnitsMemory; // all blocks total memory(in bytes, approx)
    size_t innerUsedMemory; // inner used memory, for use object pool instance internal logic(in bytes, approx)

    size_t totalMemory; // total memory(in bytes, approx)

public:
    /**
     * Constructor.
     */
    LLBC_ObjectPoolInstStat();

public:
    /**
     * Reset statistic info.
     */
    void Reset();

public:
    /**
     * Update string representation.
     */
    void UpdateStrRepr();

    /**
     * Get string representation.
     * @return const LLBC_String & - the string representation.
     */
    const LLBC_String &ToString() const;

private:
    LLBC_String _strRepr; // string representation.
};

/**
 * \brief Thee object pool statistic info encapsulation.
 */
class LLBC_ObjectPoolStat
{
public:
    // the pool instances array, all pool instances info are stored in here.
    std::map<LLBC_CString, LLBC_ObjectPoolInstStat> poolInsts;

    size_t freeMemory; // all pool instances free memory(in bytes, approx)
    size_t usedMemory; // all pool instances used memory(in bytes, approx)
    size_t innerUsedMemory; // object pool inner used memory, for use object pool internal logic(int bytes, approx)
    size_t totalMemory; // all pool instances total memory(in bytes, approx)

    // Top N pool instance statistics.
    const LLBC_ObjectPoolInstStat *topUsedMemPoolInsts[LLBC_CFG_CORE_OBJECT_POOL_STAT_TOP_N];
    const LLBC_ObjectPoolInstStat *topElemMemPoolInsts[LLBC_CFG_CORE_OBJECT_POOL_STAT_TOP_N];
    const LLBC_ObjectPoolInstStat *topUsedElemsPoolInsts[LLBC_CFG_CORE_OBJECT_POOL_STAT_TOP_N];
    const LLBC_ObjectPoolInstStat *topAllocatedMemPoolInsts[LLBC_CFG_CORE_OBJECT_POOL_STAT_TOP_N];

public:
    /**
     * Constructor.
     */
    LLBC_ObjectPoolStat();

public:
    /**
     * Reset statistic info.
     */
    void Reset();

    /**
     * Get string representation.
     * @param[in] shiftSpaceNum - the shift space number, default is 0.
     * @return const LLBC_String & - the string representation.
     */
    const LLBC_String &ToString(size_t shiftSpaceNum = 0) const;

private:
    /**
     * Update string representation.
     * @param[in] shiftSpaceNum - the shift space number, default is 0.
     */
    void UpdateStrRepr(size_t shiftSpaceNum = 0);

private:
    LLBC_String _strRepr; // string representation.
    size_t _strReprShiftSpaceNum; // shift space number.
};

/**
 * \brief The object pool instance stat info comper encapsulation.
 */
class LLBC_ObjectPoolInstStatComper
{
public:
    static bool CompBy_UsedMem(const LLBC_ObjectPoolInstStat * const &left,
                               const LLBC_ObjectPoolInstStat * const &right);

    static bool CompBy_ElemMem(const LLBC_ObjectPoolInstStat * const &left,
                               const LLBC_ObjectPoolInstStat * const &right);

    static bool CompBy_UsedElems(const LLBC_ObjectPoolInstStat * const &left,
                                 const LLBC_ObjectPoolInstStat * const &right);

    static bool CompBy_AllocatedMem(const LLBC_ObjectPoolInstStat * const &left,
                                    const LLBC_ObjectPoolInstStat * const &right);
};

__LLBC_NS_END

/**
 * Some stream output operator functions(in global ns).
 */
LLBC_EXPORT std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_ObjectPoolBlockStat &st);
LLBC_EXPORT std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_ObjectPoolInstStat &st);
LLBC_EXPORT std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_ObjectPoolStat &st);

#include "llbc/core/objectpool/ObjectPoolStatInl.h"


