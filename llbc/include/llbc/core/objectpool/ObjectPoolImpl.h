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

__LLBC_NS_BEGIN

template <typename PoolLockType, typename PoolInstLockType>
LLBC_FORCE_INLINE LLBC_ObjectPool<PoolLockType, PoolInstLockType>::LLBC_ObjectPool()
: LLBC_IObjectPool()
, _orderedDeleteNodes(NULL)
, _topOrderedDeleteNodes(NULL)
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
}

template <typename PoolLockType, typename PoolInstLockType>
template <typename ObjectType>
LLBC_FORCE_INLINE ObjectType *LLBC_ObjectPool<PoolLockType, PoolInstLockType>::Get()
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

    return poolInst->GetObject();
}

template <typename PoolLockType, typename PoolInstLockType>
template <typename ObjectType>
LLBC_FORCE_INLINE ObjectType *LLBC_ObjectPool<PoolLockType, PoolInstLockType>::GetReferencable()
{
    const char *objectType = typeid(ObjectType).name();

    _PoolInsts::iterator it;
    LLBC_ObjectPoolInst<ObjectType> *poolInst;

    _lock.Lock();
    if ((it = _poolInsts.find(objectType)) == _poolInsts.end())
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

    return reinterpret_cast<ObjectType *>(poolInst->GetReferencable());
}

template <typename PoolLockType, typename PoolInstLockType>
template <typename ObjectType>
LLBC_FORCE_INLINE LLBC_ObjectGuard<ObjectType> LLBC_ObjectPool<PoolLockType, PoolInstLockType>::GetGuarded()
{
    const char *objectType = typeid(ObjectType).name();

    _PoolInsts::iterator it;
    LLBC_IObjectPoolInst *poolInst;

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

    return LLBC_ObjectGuard<ObjectType>(reinterpret_cast<ObjectType *>(poolInst->Get()), poolInst);
}

template <typename PoolLockType, typename PoolInstLockType>
template <typename ObjectType>
LLBC_FORCE_INLINE int LLBC_ObjectPool<PoolLockType, PoolInstLockType>::Release(ObjectType *obj)
{
    return Release(typeid(ObjectType).name(), obj);
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
            return NULL;
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
LLBC_FORCE_INLINE int LLBC_ObjectPool<PoolLockType, PoolInstLockType>::AcquireOrderedDeletePoolInst()
{
    const char *frontNodeName = typeid(FrontObjectType).name();
    const char *backNodeName = typeid(BackObjectType).name();
    return AcquireOrderedDeletePoolInst(frontNodeName, backNodeName);
}

template <typename PoolLockType, typename PoolInstLockType>
inline int LLBC_ObjectPool<PoolLockType, PoolInstLockType>::AcquireOrderedDeletePoolInst(const char *frontObjectTypeName, const char *backObjectTypeName)
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
    LLBC_ObjectPoolOrderedDeleteNode *frontNode = NULL;
    LLBC_ObjectPoolOrderedDeleteNodes::iterator nodeIt = _orderedDeleteNodes->find(frontNodeName);
    if (nodeIt != _orderedDeleteNodes->end())
        frontNode = nodeIt->second;

    LLBC_ObjectPoolOrderedDeleteNode *backNode = NULL;
    nodeIt = _orderedDeleteNodes->find(backNodeName);
    if (nodeIt != _orderedDeleteNodes->end())
        backNode = nodeIt->second;

    // Case 1: frontNode & backNode never add to <_orderedDeleteNodes> container.
    if (!frontNode && !backNode)
    {
        frontNode = LLBC_New1(LLBC_ObjectPoolOrderedDeleteNode, frontNodeName);
        backNode = LLBC_New1(LLBC_ObjectPoolOrderedDeleteNode, backNodeName);

        _orderedDeleteNodes->insert(std::make_pair(frontNodeName, frontNode));
        _orderedDeleteNodes->insert(std::make_pair(backNodeName, backNode));

        frontNode->AddBackNode(backNode);
        _topOrderedDeleteNodes->insert(std::make_pair(frontNodeName, frontNode));

        return LLBC_OK;
    }

    // Case 2: frontNode found, backNode not found.
    else if (frontNode && !backNode)
    {
        backNode = LLBC_New1(LLBC_ObjectPoolOrderedDeleteNode, backNodeName);
        _orderedDeleteNodes->insert(std::make_pair(backNodeName, backNode));

        frontNode->AddBackNode(backNode);

        return LLBC_OK;
    }

    // Case 3: frontNode not found, backNode found.
    else if (!frontNode && backNode)
    {
        frontNode = LLBC_New1(LLBC_ObjectPoolOrderedDeleteNode, frontNodeName);
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
            grandFrontNode->RemoveBackNode(backNodeName, false);

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
        if (!backNodeTopParentNode)
            _topOrderedDeleteNodes->erase(backNodeName);
        else
            _topOrderedDeleteNodes->erase(backNodeTopParentNode->GetNodeName());

        frontNode->AdjustBackNodesFrontNode(backNode);
        if (backNodeTopParentNode)
            frontNode->AddBackNode(backNodeTopParentNode);
        else
            frontNode->AddBackNode(backNode);

        return LLBC_OK;
    }
}

template <typename PoolLockType, typename PoolInstLockType>
LLBC_FORCE_INLINE LLBC_IObjectPoolInst * LLBC_ObjectPool<PoolLockType, PoolInstLockType>::TryCreatePoolInstFromFactory(const char *objectType)
{
    _poolInstFactoryLock.Lock();
    std::map<LLBC_CString, LLBC_IObjectPoolInstFactory *>::iterator it = _poolInstFactories.find(objectType);
    if (it == _poolInstFactories.end())
    {
        _poolInstFactoryLock.Unlock();
        return NULL;
    }

    LLBC_IObjectPoolInst *poolInst = it->second->Create(this, new PoolInstLockType());
    _poolInstFactoryLock.Unlock();

    return poolInst;
}

template <typename PoolLockType, typename PoolInstLockType>
LLBC_FORCE_INLINE void LLBC_ObjectPool<PoolLockType, PoolInstLockType>::DeleteAcquireOrderedDeletePoolInst(LLBC_ObjectPoolOrderedDeleteNode *node)
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

__LLBC_NS_END

#endif // __LLBC_CORE_OBJECT_POOL_OBJECT_POOL_H__
