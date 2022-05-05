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

#include "llbc/core/objectpool/ObjectPoolStat.h"
#include "llbc/core/objectpool/IObjectPoolInstFactory.h"

__LLBC_NS_BEGIN

template <typename PoolLockType, typename PoolInstLockType>
LLBC_FORCE_INLINE LLBC_ObjectPool<PoolLockType, PoolInstLockType>::LLBC_ObjectPool()
: LLBC_IObjectPool()
, _orderedDeleteNodes(nullptr)
, _topOrderedDeleteNodes(nullptr)
, _stdStringPoolInst(nullptr)
, _llbcStringPoolInst(nullptr)
{
}

template <typename PoolLockType, typename PoolInstLockType>
LLBC_FORCE_INLINE LLBC_ObjectPool<PoolLockType, PoolInstLockType>::~LLBC_ObjectPool()
{
    // Lock pool.
    LLBC_LockGuard guard(_lock);

    // Delete acquire ordered delete pool instances.
    if (_orderedDeleteNodes)
    {
        for (LLBC_ObjectPoolOrderedDeleteNodes::iterator nodeIt = _topOrderedDeleteNodes->begin();
             nodeIt != _topOrderedDeleteNodes->end();
             ++nodeIt)
            DeleteAcquireOrderedDeletePoolInst(nodeIt->second);

        for (LLBC_ObjectPoolOrderedDeleteNodes::iterator nodeIt = _orderedDeleteNodes->begin();
             nodeIt != _orderedDeleteNodes->end();
             ++nodeIt)
            LLBC_Delete(nodeIt->second);
        LLBC_Delete(_orderedDeleteNodes);
        LLBC_Delete(_topOrderedDeleteNodes);
    }

    // Delete unacquire ordered delete pool instances.
    for (_PoolInsts::iterator poolIt = _poolInsts.begin();
         poolIt != _poolInsts.end();
         ++poolIt)
        LLBC_Delete(poolIt->second);

    LLBC_Delete(_stdStringPoolInst);
    LLBC_Delete(_llbcStringPoolInst);
}

template <typename PoolLockType, typename PoolInstLockType>
template <typename ObjectType>
LLBC_FORCE_INLINE ObjectType *LLBC_ObjectPool<PoolLockType, PoolInstLockType>::Get()
{
    LLBC_ObjectPoolInst<ObjectType> *poolInst = GetPoolInst<ObjectType>();
    return poolInst->GetObject();
}

template <typename PoolLockType, typename PoolInstLockType>
template <typename ObjectType>
LLBC_FORCE_INLINE ObjectType *LLBC_ObjectPool<PoolLockType, PoolInstLockType>::GetReferencable()
{
    LLBC_ObjectPoolInst<ObjectType> *poolInst = GetPoolInst<ObjectType>();
    return reinterpret_cast<ObjectType *>(poolInst->GetReferencable());
}

template <typename PoolLockType, typename PoolInstLockType>
template <typename ObjectType>
LLBC_FORCE_INLINE LLBC_ObjectGuard<ObjectType> LLBC_ObjectPool<PoolLockType, PoolInstLockType>::GetGuarded()
{
    LLBC_IObjectPoolInst *poolInst = GetPoolInst<ObjectType>();
    return LLBC_ObjectGuard<ObjectType>(reinterpret_cast<ObjectType *>(poolInst->Get()), poolInst);
}

template <typename PoolLockType, typename PoolInstLockType>
template <typename ObjectType>
LLBC_FORCE_INLINE int LLBC_ObjectPool<PoolLockType, PoolInstLockType>::Release(ObjectType *obj)
{
    return Release(typeid(ObjectType).name(), obj);
}

template <>
template <>
LLBC_FORCE_INLINE int LLBC_ObjectPool<LLBC_DummyLock, LLBC_DummyLock>::Release(std::string *obj)
{
    if (_stdStringPoolInst != nullptr)
        _stdStringPoolInst->Release(obj);

    return LLBC_OK;
}

template <>
template <>
LLBC_FORCE_INLINE int LLBC_ObjectPool<LLBC_SpinLock, LLBC_DummyLock>::Release(std::string *obj)
{
    if (_stdStringPoolInst != nullptr)
        _stdStringPoolInst->Release(obj);

    return LLBC_OK;
}

template <>
template <>
LLBC_FORCE_INLINE int LLBC_ObjectPool<LLBC_DummyLock, LLBC_SpinLock>::Release(std::string *obj)
{
    if (_stdStringPoolInst != nullptr)
        _stdStringPoolInst->Release(obj);

    return LLBC_OK;
}

template <>
template <>
LLBC_FORCE_INLINE int LLBC_ObjectPool<LLBC_SpinLock, LLBC_SpinLock>::Release(std::string *obj)
{
    if (_stdStringPoolInst != nullptr)
        _stdStringPoolInst->Release(obj);

    return LLBC_OK;
}

template <>
template <>
LLBC_FORCE_INLINE int LLBC_ObjectPool<LLBC_DummyLock, LLBC_DummyLock>::Release(LLBC_String *obj)
{
    if (_llbcStringPoolInst != nullptr)
        _llbcStringPoolInst->Release(obj);

    return LLBC_OK;
}

template <>
template <>
LLBC_FORCE_INLINE int LLBC_ObjectPool<LLBC_SpinLock, LLBC_DummyLock>::Release(LLBC_String *obj)
{
    if (_llbcStringPoolInst != nullptr)
        _llbcStringPoolInst->Release(obj);

    return LLBC_OK;
}

template <>
template <>
LLBC_FORCE_INLINE int LLBC_ObjectPool<LLBC_DummyLock, LLBC_SpinLock>::Release(LLBC_String *obj)
{
    if (_llbcStringPoolInst != nullptr)
        _llbcStringPoolInst->Release(obj);

    return LLBC_OK;
}

template <>
template <>
LLBC_FORCE_INLINE int LLBC_ObjectPool<LLBC_SpinLock, LLBC_SpinLock>::Release(LLBC_String *obj)
{
    if (_llbcStringPoolInst != nullptr)
        _llbcStringPoolInst->Release(obj);

    return LLBC_OK;
}

template <typename PoolLockType, typename PoolInstLockType>
int LLBC_ObjectPool<PoolLockType, PoolInstLockType>::Release(const char *objectType, void *obj)
{
    LLBC_IObjectPoolInst *poolInst;
    std::map<LLBC_CString, LLBC_IObjectPoolInst *>::iterator it;

    _lock.Lock();
    if (UNLIKELY((it = _poolInsts.find(objectType)) == _poolInsts.end()))
    {
        _lock.Unlock();

        LLBC_SetLastError(LLBC_ERROR_NOT_POOL_OBJECT);
        return LLBC_FAILED;
    }

    poolInst = it->second;

    _lock.Unlock();

    poolInst->Release(obj);

    return LLBC_OK;
}

template <typename PoolLockType, typename PoolInstLockType>
template <typename ObjectType>
LLBC_FORCE_INLINE LLBC_ObjectPoolInst<ObjectType> *LLBC_ObjectPool<PoolLockType, PoolInstLockType>::GetPoolInst()
{
    const char *objectType = typeid(ObjectType).name();

    _PoolInsts::iterator it;
    LLBC_ObjectPoolInst<ObjectType> *poolInst;

    _lock.Lock();
    if (UNLIKELY((it = _poolInsts.find(objectType)) == _poolInsts.end()))
    {
        _poolInsts.insert(std::make_pair(objectType, poolInst = new LLBC_ObjectPoolInst<ObjectType>(this, new PoolInstLockType())));
        _lock.Unlock();

        LLBC_ObjectManipulator::OnPoolInstCreate<ObjectType>(*poolInst);
    }
    else
    {
        poolInst = reinterpret_cast<LLBC_ObjectPoolInst<ObjectType> *>(it->second);
        _lock.Unlock();
    }

    return poolInst;
}

template <>
template <>
LLBC_FORCE_INLINE LLBC_ObjectPoolInst<std::string> *LLBC_ObjectPool<LLBC_DummyLock, LLBC_DummyLock>::GetPoolInst()
{
    if (_stdStringPoolInst == nullptr)
    {
        _lock.Lock();
        _stdStringPoolInst = new LLBC_ObjectPoolInst<std::string>(this, new LLBC_DummyLock());
        _lock.Unlock();

        LLBC_ObjectManipulator::OnPoolInstCreate<std::string>(*_stdStringPoolInst);
    }

    return _stdStringPoolInst;
}

template <>
template <>
LLBC_FORCE_INLINE LLBC_ObjectPoolInst<std::string> *LLBC_ObjectPool<LLBC_SpinLock, LLBC_DummyLock>::GetPoolInst()
{
    if (_stdStringPoolInst == nullptr)
    {
        _lock.Lock();
        _stdStringPoolInst = new LLBC_ObjectPoolInst<std::string>(this, new LLBC_DummyLock());
        _lock.Unlock();

        LLBC_ObjectManipulator::OnPoolInstCreate<std::string>(*_stdStringPoolInst);
    }

    return _stdStringPoolInst;
}

template <>
template <>
LLBC_FORCE_INLINE LLBC_ObjectPoolInst<std::string> *LLBC_ObjectPool<LLBC_DummyLock, LLBC_SpinLock>::GetPoolInst()
{
    if (_stdStringPoolInst == nullptr)
    {
        _lock.Lock();
        _stdStringPoolInst = new LLBC_ObjectPoolInst<std::string>(this, new LLBC_SpinLock());
        _lock.Unlock();

        LLBC_ObjectManipulator::OnPoolInstCreate<std::string>(*_stdStringPoolInst);
    }

    return _stdStringPoolInst;
}

template <>
template <>
LLBC_FORCE_INLINE LLBC_ObjectPoolInst<std::string> *LLBC_ObjectPool<LLBC_SpinLock, LLBC_SpinLock>::GetPoolInst()
{
    if (_stdStringPoolInst == nullptr)
    {
        _lock.Lock();
        _stdStringPoolInst = new LLBC_ObjectPoolInst<std::string>(this, new LLBC_SpinLock());
        _lock.Unlock();

        LLBC_ObjectManipulator::OnPoolInstCreate<std::string>(*_stdStringPoolInst);
    }

    return _stdStringPoolInst;
}

template <>
template <>
LLBC_FORCE_INLINE LLBC_ObjectPoolInst<LLBC_String> *LLBC_ObjectPool<LLBC_DummyLock, LLBC_DummyLock>::GetPoolInst()
{
    if (_llbcStringPoolInst == nullptr)
    {
        _lock.Lock();
        _llbcStringPoolInst = new LLBC_ObjectPoolInst<LLBC_String>(this, new LLBC_DummyLock());
        _lock.Unlock();

        LLBC_ObjectManipulator::OnPoolInstCreate<LLBC_String>(*_llbcStringPoolInst);
    }
    return _llbcStringPoolInst;
}

template <>
template <>
LLBC_FORCE_INLINE LLBC_ObjectPoolInst<LLBC_String> *LLBC_ObjectPool<LLBC_SpinLock, LLBC_DummyLock>::GetPoolInst()
{
    if (_llbcStringPoolInst == nullptr)
    {
        _lock.Lock();
        _llbcStringPoolInst = new LLBC_ObjectPoolInst<LLBC_String>(this, new LLBC_DummyLock());
        _lock.Unlock();

        LLBC_ObjectManipulator::OnPoolInstCreate<LLBC_String>(*_llbcStringPoolInst);
    }
    return _llbcStringPoolInst;
}

template <>
template <>
LLBC_FORCE_INLINE LLBC_ObjectPoolInst<LLBC_String> *LLBC_ObjectPool<LLBC_DummyLock, LLBC_SpinLock>::GetPoolInst()
{
    if (_llbcStringPoolInst == nullptr)
    {
        _lock.Lock();
        _llbcStringPoolInst = new LLBC_ObjectPoolInst<LLBC_String>(this, new LLBC_SpinLock());
        _lock.Unlock();

        LLBC_ObjectManipulator::OnPoolInstCreate<LLBC_String>(*_llbcStringPoolInst);
    }
    return _llbcStringPoolInst;
}

template <>
template <>
LLBC_FORCE_INLINE LLBC_ObjectPoolInst<LLBC_String> *LLBC_ObjectPool<LLBC_SpinLock, LLBC_SpinLock>::GetPoolInst()
{
    if (_llbcStringPoolInst == nullptr)
    {
        _lock.Lock();
        _llbcStringPoolInst = new LLBC_ObjectPoolInst<LLBC_String>(this, new LLBC_SpinLock());
        _lock.Unlock();

        LLBC_ObjectManipulator::OnPoolInstCreate<LLBC_String>(*_llbcStringPoolInst);
    }
    return _llbcStringPoolInst;
}

template <typename PoolLockType, typename PoolInstLockType>
LLBC_FORCE_INLINE LLBC_IObjectPoolInst *LLBC_ObjectPool<PoolLockType, PoolInstLockType>::GetIPoolInst(const char *objectType)
{
    _PoolInsts::iterator it;
    LLBC_IObjectPoolInst *poolInst;

    // Try get pool instance.
    _lock.Lock();
    if (UNLIKELY((it = _poolInsts.find(objectType)) == _poolInsts.end()))
    {
        // Try create object pool instance by pool instance factory if not found pool instance.
        if (!(poolInst = TryCreatePoolInstFromFactory(objectType)))
        {
            _lock.Unlock();
            return nullptr;
        }

        _poolInsts.insert(std::make_pair(poolInst->GetPoolInstName(), poolInst));
        _lock.Unlock();

        return poolInst;
    }

    poolInst = it->second;
    _lock.Unlock();

    return poolInst;
}

template <typename PoolLockType, typename PoolInstLockType>
template <typename FrontObjectType, typename BackObjectType>
int LLBC_ObjectPool<PoolLockType, PoolInstLockType>::AcquireOrderedDeletePoolInst()
{
    return LLBC_IObjectPool::AcquireOrderedDeletePoolInst<FrontObjectType, BackObjectType>();
}

template <typename PoolLockType, typename PoolInstLockType>
int LLBC_ObjectPool<PoolLockType, PoolInstLockType>::AcquireOrderedDeletePoolInst(const char *frontObjectTypeName, const char *backObjectTypeName)
{
    // Make node name reference(const) to shorten the param name.
    const LLBC_CString frontNodeName(frontObjectTypeName);
    const LLBC_CString backNodeName(backObjectTypeName);

    // Same object type check.
    if (UNLIKELY(frontNodeName == backNodeName))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);
        return LLBC_FAILED;
    }

    // Lock pool.
    LLBC_LockGuard guard(_lock);

    // Check and create nodes containers.
    if (!_orderedDeleteNodes)
    {
        _orderedDeleteNodes = LLBC_New(LLBC_ObjectPoolOrderedDeleteNodes);
        _topOrderedDeleteNodes = LLBC_New(LLBC_ObjectPoolOrderedDeleteNodes);
    }

    // Try fetch frontNode & backNode.
    LLBC_ObjectPoolOrderedDeleteNode *frontNode = nullptr;
    LLBC_ObjectPoolOrderedDeleteNodes::iterator nodeIt = _orderedDeleteNodes->find(frontNodeName);
    if (nodeIt != _orderedDeleteNodes->end())
        frontNode = nodeIt->second;

    LLBC_ObjectPoolOrderedDeleteNode *backNode = nullptr;
    nodeIt = _orderedDeleteNodes->find(backNodeName);
    if (nodeIt != _orderedDeleteNodes->end())
        backNode = nodeIt->second;

    // Case 1: frontNode & backNode never add to <_orderedDeleteNodes> container.
    if (!frontNode && !backNode)
    {
        frontNode = LLBC_New(LLBC_ObjectPoolOrderedDeleteNode, frontNodeName);
        backNode = LLBC_New(LLBC_ObjectPoolOrderedDeleteNode, backNodeName);

        _orderedDeleteNodes->insert(std::make_pair(frontNodeName, frontNode));
        _orderedDeleteNodes->insert(std::make_pair(backNodeName, backNode));

        frontNode->AddBackNode(backNode);
        _topOrderedDeleteNodes->insert(std::make_pair(frontNodeName, frontNode));

        return LLBC_OK;
    }

    // Case 2: frontNode found, backNode not found.
    else if (frontNode && !backNode)
    {
        backNode = LLBC_New(LLBC_ObjectPoolOrderedDeleteNode, backNodeName);
        _orderedDeleteNodes->insert(std::make_pair(backNodeName, backNode));

        frontNode->AddBackNode(backNode);

        return LLBC_OK;
    }

    // Case 3: frontNode not found, backNode found.
    else if (!frontNode && backNode)
    {
        frontNode = LLBC_New(LLBC_ObjectPoolOrderedDeleteNode, frontNodeName);
        _orderedDeleteNodes->insert(std::make_pair(frontNodeName, frontNode));

        // backNode is top node.
        if (!backNode->GetFrontNode())
        {
            _topOrderedDeleteNodes->erase(backNodeName);

            frontNode->AddBackNode(backNode);
            _topOrderedDeleteNodes->insert(std::make_pair(frontNodeName, frontNode));
        }
        else // backNode is not top node.
        {
            LLBC_ObjectPoolOrderedDeleteNode *grandFrontNode = backNode->GetFrontNode();
            grandFrontNode->RemoveBackNode(backNodeName, false, true);

            frontNode->AddBackNode(backNode);
            grandFrontNode->AddBackNode(frontNode);
        }

        return LLBC_OK;
    }

    // Case 4: frontNode & backNode found.
    else
    {
        // Node relationship has been set to right order check.
        if (frontNode->IsBackNode(backNode->GetNodeName()))
        {
            return LLBC_OK;
        }

        // Node relationship between inversion check.
        if (frontNode->IsFrontNode(backNode->GetNodeName()))
        {
            LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);
            return LLBC_FAILED;
        }

        // Exec adjust.
        // eg: 
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
        LLBC_ObjectPoolOrderedDeleteNode *backNodeTopParentNode = backNode->GetTopFrontNode();
        LLBC_ObjectPoolOrderedDeleteNode *frontNodeTopParentNode = frontNode->GetTopFrontNode();
        if (!backNodeTopParentNode)
            _topOrderedDeleteNodes->erase(backNodeName);
        else if (frontNodeTopParentNode != backNodeTopParentNode)
            _topOrderedDeleteNodes->erase(backNodeTopParentNode->GetNodeName());
        else
            backNode->GetFrontNode()->RemoveBackNode(backNodeName, false, true);

        frontNode->AdjustBackNodesFrontNode(backNode);
        if (backNodeTopParentNode &&
            frontNodeTopParentNode != backNodeTopParentNode)
            frontNode->AddBackNode(backNodeTopParentNode);
        else
            frontNode->AddBackNode(backNode);

        return LLBC_OK;
    }
}

template <typename PoolLockType, typename PoolInstLockType>
void LLBC_ObjectPool<PoolLockType, PoolInstLockType>::Stat(LLBC_ObjectPoolStat &stat) const
{
    typedef typename std::map<LLBC_CString, LLBC_ObjectPoolInstStat>::iterator _InstStatIt;

    stat.Reset();

    LLBC_LockGuard guard(const_cast<LLBC_ObjectPool *>(this)->_lock);
    if (_poolInsts.empty())
        return;

    std::vector<const LLBC_ObjectPoolInstStat *> instStats;
    for (_PoolInsts::const_iterator it = _poolInsts.begin();
         it != _poolInsts.end();
         ++it)
    {
        _InstStatIt instStatIt = stat.poolInsts.insert(
            std::make_pair(it->first, LLBC_ObjectPoolInstStat())).first;

        LLBC_ObjectPoolInstStat &instStat = instStatIt->second;
        instStats.push_back(&instStat);

        it->second->Stat(instStat);
        stat.freeMemory += instStat.freeUnitsMemory;
        stat.usedMemory += instStat.usedUnitsMemory;
        stat.innerUsedMemory += instStat.innerUsedMemory;
        stat.totalMemory += instStat.totalMemory;
    }

    const size_t selfInnerUsedMemory = sizeof(LLBC_ObjectPool);
    stat.innerUsedMemory += selfInnerUsedMemory;
    stat.totalMemory += selfInnerUsedMemory;

    StatTopNPoolInstStats(stat, instStats);
}

template <typename PoolLockType, typename PoolInstLockType>
LLBC_FORCE_INLINE LLBC_IObjectPoolInst * LLBC_ObjectPool<PoolLockType, PoolInstLockType>::TryCreatePoolInstFromFactory(const char *objectType)
{
    _poolInstFactoryLock.Lock();
    std::map<LLBC_CString, LLBC_IObjectPoolInstFactory *>::iterator it = _poolInstFactories.find(objectType);
    if (it == _poolInstFactories.end())
    {
        _poolInstFactoryLock.Unlock();
        return nullptr;
    }

    LLBC_IObjectPoolInst *poolInst = it->second->Create(this, new PoolInstLockType());
    _poolInstFactoryLock.Unlock();

    return poolInst;
}

template <typename PoolLockType, typename PoolInstLockType>
void LLBC_ObjectPool<PoolLockType, PoolInstLockType>::DeleteAcquireOrderedDeletePoolInst(LLBC_ObjectPoolOrderedDeleteNode *node)
{
    // Delete node pool instance.
    _PoolInsts::iterator instIt = _poolInsts.find(node->GetNodeName());
    if (instIt != _poolInsts.end())
    {
        LLBC_Delete(instIt->second);
        _poolInsts.erase(instIt);
    }

    // Delete back nodes pool instances.
    const LLBC_ObjectPoolOrderedDeleteNodes *backNodes = node->GetBackNodes();
    if (!backNodes)
        return;

    for (LLBC_ObjectPoolOrderedDeleteNodes::const_iterator nodeIt = backNodes->begin();
         nodeIt != backNodes->end();
         ++nodeIt)
        DeleteAcquireOrderedDeletePoolInst(nodeIt->second);
}

template <typename PoolLockType, typename PoolInstLockType>
void LLBC_ObjectPool<PoolLockType, PoolInstLockType>::StatTopNPoolInstStats(LLBC_ObjectPoolStat &stat,
                                                                            std::vector<const LLBC_ObjectPoolInstStat *> &instStats) const
{
    // top N used memory array:
    std::sort(instStats.begin(), instStats.end(), LLBC_ObjectPoolInstStatComper::CompBy_UsedMem);
    for (size_t i = 0; i < instStats.size() && i < LLBC_CFG_CORE_OBJECT_POOL_STAT_TOP_N; ++i)
        stat.topUsedMemPoolInsts[i] = instStats[i];

    // top N elem memory array:
    std::sort(instStats.begin(), instStats.end(), LLBC_ObjectPoolInstStatComper::CompBy_ElemMem);
    for (size_t i = 0; i < instStats.size() && i < LLBC_CFG_CORE_OBJECT_POOL_STAT_TOP_N; ++i)
        stat.topElemMemPoolInsts[i] = instStats[i];

    // top N used elems array:
    std::sort(instStats.begin(), instStats.end(), LLBC_ObjectPoolInstStatComper::CompBy_UsedElems);
    for (size_t i = 0; i < instStats.size() && i < LLBC_CFG_CORE_OBJECT_POOL_STAT_TOP_N; ++i)
        stat.topUsedElemsPoolInsts[i] = instStats[i];

    // top N allocted memory array:
    std::sort(instStats.begin(), instStats.end(), LLBC_ObjectPoolInstStatComper::CompBy_AllocatedMem);
    for (size_t i = 0; i < instStats.size() && i < LLBC_CFG_CORE_OBJECT_POOL_STAT_TOP_N; ++i)
        stat.topAllocatedMemPoolInsts[i] = instStats[i];
}

#undef __LLBC_OBJECT_POOL_EXPAND_TOPN_INSERT

__LLBC_NS_END

#endif // __LLBC_CORE_OBJECT_POOL_OBJECT_POOL_H__
