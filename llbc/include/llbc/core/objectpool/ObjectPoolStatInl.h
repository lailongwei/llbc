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

__LLBC_NS_BEGIN

inline LLBC_ObjectPoolBlockStat::LLBC_ObjectPoolBlockStat()
: blockSeq(-1)
, unitMemorySize(0)

, freeUnitsNum(0)
, usedUnitsNum(0)
, allUnitsNum(0)

, freeUnitsMemory(0)
, usedUnitsMemory(0)
, allUnitsMemory(0)
, innerUsedMemory(0)

, totalMemory(0)
{
}

inline void LLBC_ObjectPoolBlockStat::Reset()
{
    blockSeq = -1;
    unitMemorySize = 0;

    freeUnitsNum = 0;
    usedUnitsNum = 0;
    allUnitsNum = 0;

    freeUnitsMemory = 0;
    usedUnitsMemory = 0;
    allUnitsMemory = 0;
    innerUsedMemory = 0;

    totalMemory  = 0;

    _strRepr.clear();
}

inline void LLBC_ObjectPoolBlockStat::UpdateStrRepr()
{
    _strRepr.format("seq:%d, unit_mem:%lu, units_num:%lu"
                    "[used:%lu, free:%lu], units_mem:%lu[used:%lu, free:%lu], inner_mem:%lu, total_mem:%lu",
                    blockSeq, 
                    unitMemorySize, 
                    allUnitsNum, usedUnitsNum, freeUnitsNum, 
                    allUnitsMemory, usedUnitsMemory, freeUnitsMemory, 
                    innerUsedMemory, 
                    totalMemory);
}

inline const LLBC_String &LLBC_ObjectPoolBlockStat::ToString() const
{
    return _strRepr;
}

inline LLBC_ObjectPoolInstStat::LLBC_ObjectPoolInstStat()
: blockMemorySize(0)
, unitMemorySize(0)

, freeUnitsNum(0)
, usedUnitsNum(0)
, allUnitsNum(0)

, freeUnitsMemory(0)
, usedUnitsMemory(0)
, allUnitsMemory(0)
, innerUsedMemory(0)

, totalMemory(0)
{
}

inline void LLBC_ObjectPoolInstStat::Reset()
{
    poolInstName.clear();

    blockMemorySize = 0;
    unitMemorySize = 0;
    blocks.clear();

    freeUnitsNum = 0;
    usedUnitsNum = 0;
    allUnitsNum = 0;

    freeUnitsMemory = 0;
    usedUnitsMemory = 0;
    allUnitsMemory = 0;
    innerUsedMemory = 0;

    totalMemory = 0;

    _strRepr.clear();
}

inline void LLBC_ObjectPoolInstStat::UpdateStrRepr()
{
    _strRepr.format("name:%s, block_num:%lu, units_num:%lu"
                    "[used:%lu, free:%lu], units_mem:%lu[used:%lu, free:%lu], inner_mem:%lu, total_mem:%lu",
                    poolInstName.c_str(),
                    blocks.size(),
                    allUnitsNum, usedUnitsNum, freeUnitsNum,
                    allUnitsMemory, usedUnitsMemory, freeUnitsMemory,
                    innerUsedMemory,
                    totalMemory);
}

inline const LLBC_String &LLBC_ObjectPoolInstStat::ToString() const
{
    return _strRepr;
}

inline LLBC_ObjectPoolStat::LLBC_ObjectPoolStat()
: freeMemory(0)
, usedMemory(0)
, innerUsedMemory(0)
, totalMemory(0)

, _strReprShiftSpaceNum(0)
{
    memset(topUsedMemPoolInsts, 0, sizeof(topUsedMemPoolInsts));
    memset(topElemMemPoolInsts, 0, sizeof(topElemMemPoolInsts));
    memset(topUsedElemsPoolInsts, 0, sizeof(topUsedElemsPoolInsts));
    memset(topAllocatedMemPoolInsts, 0, sizeof(topAllocatedMemPoolInsts));
}

inline void LLBC_ObjectPoolStat::Reset()
{
    poolInsts.clear();

    freeMemory = 0;
    usedMemory = 0;
    innerUsedMemory = 0;
    totalMemory = 0;

    memset(topUsedMemPoolInsts, 0, sizeof(topUsedMemPoolInsts));
    memset(topElemMemPoolInsts, 0, sizeof(topElemMemPoolInsts));
    memset(topUsedElemsPoolInsts, 0, sizeof(topUsedElemsPoolInsts));
    memset(topAllocatedMemPoolInsts, 0, sizeof(topAllocatedMemPoolInsts));

    _strRepr.clear();
    _strReprShiftSpaceNum = 0;
}

inline void LLBC_ObjectPoolStat::UpdateStrRepr(size_t shiftSpaceNum)
{
    const LLBC_String shiftSpaces(shiftSpaceNum, ' ');

    _strRepr.format("%s- summary: inst_num:%lu, mem:%lu/%lu"
                    "[free:%lu, elems_used:%lu, inner_used:%lu], managed_cost:%.03f, mem_usage:%.03f", 
                    shiftSpaces.c_str(), poolInsts.size(),
                    usedMemory + innerUsedMemory, totalMemory, freeMemory, usedMemory, innerUsedMemory,
                    static_cast<double>(innerUsedMemory) / totalMemory,
                    static_cast<double>(usedMemory) / (usedMemory + freeMemory));
    
    _strRepr.append_format("\n%s- top %d used memory pool instances:",
                           shiftSpaces.c_str(),
                           LLBC_CFG_CORE_OBJECT_POOL_STAT_TOP_N);
    for (int i = 0; i != LLBC_CFG_CORE_OBJECT_POOL_STAT_TOP_N; ++i)
    {
        const LLBC_ObjectPoolInstStat *&instStat = topUsedMemPoolInsts[i];
        if (!instStat)
            _strRepr.append_format("\n%s  - %d: <empty>", shiftSpaces.c_str(), i);
        else
            _strRepr.append_format("\n%s  - %d: %s: %lu/%lu[free:%lu, elems_used:%lu, inner_used:%lu]", 
                                   shiftSpaces.c_str(), i, instStat->poolInstName.c_str(),
                                   instStat->usedUnitsMemory + instStat->innerUsedMemory, instStat->totalMemory,
                                   instStat->freeUnitsMemory, instStat->usedUnitsMemory, instStat->innerUsedMemory);
    }

    _strRepr.append_format("\n%s- top %d elems memory pool instances:",
                           shiftSpaces.c_str(),
                           LLBC_CFG_CORE_OBJECT_POOL_STAT_TOP_N);
    for (int i = 0; i != LLBC_CFG_CORE_OBJECT_POOL_STAT_TOP_N; ++i)
    {
        const LLBC_ObjectPoolInstStat *&instStat = topElemMemPoolInsts[i];
        if (!instStat)
            _strRepr.append_format("\n%s  - %d: <empty>", shiftSpaces.c_str(), i);
        else
            _strRepr.append_format("\n%s  - %d: %s: %lu",
                                   shiftSpaces.c_str(),
                                   i,
                                   instStat->poolInstName.c_str(),
                                   instStat->unitMemorySize);
    }


    _strRepr.append_format("\n%s- top %d used elems pool instances:",
                           shiftSpaces.c_str(),
                           LLBC_CFG_CORE_OBJECT_POOL_STAT_TOP_N);
    for (int i = 0; i != LLBC_CFG_CORE_OBJECT_POOL_STAT_TOP_N; ++i)
    {
        const LLBC_ObjectPoolInstStat *&instStat = topUsedElemsPoolInsts[i];
        if (!instStat)
            _strRepr.append_format("\n%s  - %d: <empty>", shiftSpaces.c_str(), i);
        else
            _strRepr.append_format("\n%s  - %d: %s: %lu/%lu",
                                   shiftSpaces.c_str(),
                                   i,
                                   instStat->poolInstName.c_str(),
                                   instStat->usedUnitsNum,
                                   instStat->allUnitsNum);
    }

    _strRepr.append_format("\n%stop %d allocated memory pool instances:",
                           shiftSpaces.c_str(),
                           LLBC_CFG_CORE_OBJECT_POOL_STAT_TOP_N);
    for (int i = 0; i != LLBC_CFG_CORE_OBJECT_POOL_STAT_TOP_N; ++i)
    {
        const LLBC_ObjectPoolInstStat *&instStat = topAllocatedMemPoolInsts[i];
        if (!instStat)
            _strRepr.append_format("\n%s- %d: <empty>", shiftSpaces.c_str(), i);
        else
            _strRepr.append_format("\n%s- %d: %s: %lu",
                                   shiftSpaces.c_str(),
                                   i,
                                   instStat->poolInstName.c_str(),
                                   instStat->totalMemory);
    }

    _strReprShiftSpaceNum = shiftSpaceNum;
}

inline const LLBC_String &LLBC_ObjectPoolStat::ToString(size_t shiftSpaceNum) const
{
    if (_strRepr.empty() || _strReprShiftSpaceNum != shiftSpaceNum)
        const_cast<LLBC_ObjectPoolStat *>(this)->UpdateStrRepr(shiftSpaceNum);

    return _strRepr;
}

inline bool llbc::LLBC_ObjectPoolInstStatComper::CompBy_UsedMem(const LLBC_ObjectPoolInstStat * const &left,
                                                                const LLBC_ObjectPoolInstStat * const &right)
{
    return (left->usedUnitsMemory + left->innerUsedMemory) > (right->usedUnitsMemory + right->innerUsedMemory);
}

inline bool llbc::LLBC_ObjectPoolInstStatComper::CompBy_ElemMem(const LLBC_ObjectPoolInstStat * const &left,
                                                                const LLBC_ObjectPoolInstStat * const &right)
{
    return left->unitMemorySize > right->unitMemorySize;
}

inline bool llbc::LLBC_ObjectPoolInstStatComper::CompBy_UsedElems(const LLBC_ObjectPoolInstStat * const &left,
                                                                  const LLBC_ObjectPoolInstStat * const &right)
{
    return left->usedUnitsNum > right->usedUnitsNum;
}

inline bool llbc::LLBC_ObjectPoolInstStatComper::CompBy_AllocatedMem(const LLBC_ObjectPoolInstStat * const &left,
                                                                     const LLBC_ObjectPoolInstStat * const &right)
{
    return left->totalMemory > right->totalMemory;
}

__LLBC_NS_END
