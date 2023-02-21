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

#include "llbc/core/objectpool/ObjectPool.h"

__LLBC_NS_BEGIN

template <typename _Ty>
void LLBC_STLHelper::FreeContainer(std::vector<_Ty *> &vec, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer<_Ty>(vec, LLBC_STLContainerOpcode::Free, clear, reverse);
}

template <typename _Ty>
void LLBC_STLHelper::FreeContainer(std::list<_Ty *> &l, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(l, LLBC_STLContainerOpcode::Free, clear, reverse);
}

template <typename _Ty>
void LLBC_STLHelper::FreeContainer(std::deque<_Ty *> &dq, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(dq, LLBC_STLContainerOpcode::Free, clear, reverse);
}

template <typename _Ty>
void LLBC_STLHelper::FreeContainer(std::stack<_Ty *> &stk, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(stk, LLBC_STLContainerOpcode::Free, clear, reverse);
}

template <typename _Ty>
void LLBC_STLHelper::FreeContainer(std::queue<_Ty *> &q, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(q, LLBC_STLContainerOpcode::Free, clear, reverse);
}

template <typename _Key>
void LLBC_STLHelper::FreeContainer(std::set<_Key *> &s, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(s, LLBC_STLContainerOpcode::Free, clear, reverse);
}

template <typename _Key, typename _Ty>
void LLBC_STLHelper::FreeContainer(std::map<_Key, _Ty *> &m, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(m, LLBC_STLContainerOpcode::Free, clear, reverse);
}

template <typename _Key, typename _Ty>
void LLBC_STLHelper::FreeContainer(std::unordered_map<_Key, _Ty *> &m, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(m, LLBC_STLContainerOpcode::Free, clear, reverse);
}

template <typename _Ty, std::size_t _Nm>
void LLBC_STLHelper::FreeContainer(std::array<_Ty *, _Nm> &arr, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(arr, LLBC_STLContainerOpcode::Free, clear, reverse);
}

template <typename _Ty>
void LLBC_STLHelper::DeleteContainer(std::vector<_Ty *> &vec, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(vec, LLBC_STLContainerOpcode::Delete, clear, reverse);
}

template <typename _Ty>
void LLBC_STLHelper::DeleteContainer(std::list<_Ty *> &l, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(l, LLBC_STLContainerOpcode::Delete, clear, reverse);
}

template <typename _Ty>
void LLBC_STLHelper::DeleteContainer(std::deque<_Ty *> &dq, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(dq, LLBC_STLContainerOpcode::Delete, clear, reverse);
}

template <typename _Ty>
void LLBC_STLHelper::DeleteContainer(std::stack<_Ty *> &stk, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(stk, LLBC_STLContainerOpcode::Delete, clear, reverse);
}

template <typename _Ty>
void LLBC_STLHelper::DeleteContainer(std::queue<_Ty *> &q, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(q, LLBC_STLContainerOpcode::Delete, clear, reverse);
}

template <typename _Key>
void LLBC_STLHelper::DeleteContainer(std::set<_Key *> &s, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(s, LLBC_STLContainerOpcode::Delete, clear, reverse);
}

template <typename _Key, typename _Ty>
void LLBC_STLHelper::DeleteContainer(std::map<_Key, _Ty *> &m, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(m, LLBC_STLContainerOpcode::Delete, clear, reverse);
}

template <typename _Key, typename _Ty>
void LLBC_STLHelper::DeleteContainer(std::unordered_map<_Key, _Ty *> &m, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(m, LLBC_STLContainerOpcode::Delete, clear, reverse);
}

template <typename _Ty, std::size_t _Nm>
void LLBC_STLHelper::DeleteContainer(std::array<_Ty *, _Nm> &arr, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(arr, LLBC_STLContainerOpcode::Delete, clear, reverse);
}

template <typename _Ty>
void LLBC_STLHelper::DeletesContainer(std::vector<_Ty *> &vec, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(vec, LLBC_STLContainerOpcode::Deletes, clear, reverse);
}

template <typename _Ty>
void LLBC_STLHelper::DeletesContainer(std::list<_Ty *> &l, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(l, LLBC_STLContainerOpcode::Deletes, clear, reverse);
}

template <typename _Ty>
void LLBC_STLHelper::DeletesContainer(std::deque<_Ty *> &dq, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(dq, LLBC_STLContainerOpcode::Deletes, clear, reverse);
}

template <typename _Ty>
void LLBC_STLHelper::DeletesContainer(std::stack<_Ty *> &stk, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(stk, LLBC_STLContainerOpcode::Deletes, clear, reverse);
}

template <typename _Key>
void LLBC_STLHelper::DeletesContainer(std::set<_Key *> &s, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(s, LLBC_STLContainerOpcode::Deletes, clear, reverse);
}

template <typename _Key, typename _Ty>
void LLBC_STLHelper::DeletesContainer(std::map<_Key, _Ty *> &m, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(m, LLBC_STLContainerOpcode::Deletes, clear, reverse);
}

template <typename _Key, typename _Ty>
void LLBC_STLHelper::DeletesContainer(std::unordered_map<_Key, _Ty *> &m, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(m, LLBC_STLContainerOpcode::Deletes, clear, reverse);
}

template <typename _Ty, std::size_t _Nm>
void LLBC_STLHelper::DeletesContainer(std::array<_Ty *, _Nm> &arr, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(arr, LLBC_STLContainerOpcode::Deletes, clear, reverse);
}

template <typename _Ty>
void LLBC_STLHelper::RecycleContainer(std::vector<_Ty *> &vec, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(vec, LLBC_STLContainerOpcode::Recycle, clear, reverse);
}

template <typename _Ty>
void LLBC_STLHelper::RecycleContainer(std::list<_Ty *> &l, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(l, LLBC_STLContainerOpcode::Recycle, clear, reverse);
}

template <typename _Ty>
void LLBC_STLHelper::RecycleContainer(std::deque<_Ty *> &dq, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(dq, LLBC_STLContainerOpcode::Recycle, clear, reverse);
}

template <typename _Ty>
void LLBC_STLHelper::RecycleContainer(std::stack<_Ty *> &stk, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(stk, LLBC_STLContainerOpcode::Recycle, clear, reverse);
}

template <typename _Key>
void LLBC_STLHelper::RecycleContainer(std::set<_Key *> &s, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(s, LLBC_STLContainerOpcode::Recycle, clear, reverse);
}

template <typename _Key, typename _Ty>
void LLBC_STLHelper::RecycleContainer(std::map<_Key, _Ty *> &m, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(m, LLBC_STLContainerOpcode::Recycle, clear, reverse);
}

template <typename _Key, typename _Ty>
void LLBC_STLHelper::RecycleContainer(std::unordered_map<_Key, _Ty *> &m, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(m, LLBC_STLContainerOpcode::Recycle, clear, reverse);
}

template <typename _Ty, std::size_t _Nm>
void LLBC_STLHelper::RecycleContainer(std::array<_Ty *, _Nm> &arr, bool clear, bool reverse)
{
    LLBC_STLHelper::OperateContainer(arr, LLBC_STLContainerOpcode::Recycle, clear, reverse);
}

template <typename _Ty>
void LLBC_STLHelper::OperateContainer(std::vector<_Ty *> &vec, int opcode, bool clear, bool reverse)
{
    if (vec.empty())
        return;

    typedef typename std::vector<_Ty *>::size_type SizeType;

    const SizeType vecSize = vec.size();
    if (!reverse)
    {
        for (SizeType i = 0; i < vecSize; ++i)
            LLBC_STLHelper::OperateElem(vec[i], opcode);
    }
    else
    {
        for (SizeType i = vecSize - 1; i != SizeType(-1); --i)
            LLBC_STLHelper::OperateElem(vec[i], opcode);
    }

    if (LIKELY(clear))
        vec.clear();
}

template <typename _Ty>
void LLBC_STLHelper::OperateContainer(std::list<_Ty *> &l, int opcode, bool clear, bool reverse)
{
    if (l.empty())
        return;

    typedef typename std::list<_Ty *>::iterator ListIter;
    typedef typename std::list<_Ty *>::reverse_iterator ListReverseIter;

    if (!reverse)
    {
        for (ListIter iter = l.begin(); iter != l.end(); ++iter)
            LLBC_STLHelper::OperateElem(*iter, opcode);
    }
    else
    {
        for (ListReverseIter riter = l.rbegin(); riter != l.rend(); ++riter)
            LLBC_STLHelper::OperateElem(*riter, opcode);
    }

    if (LIKELY(clear))
        l.clear();
}

template <typename _Ty>
void LLBC_STLHelper::OperateContainer(std::deque<_Ty *> &dq, int opcode, bool clear, bool reverse)
{
    if (dq.empty())
        return;

    typedef typename std::deque<_Ty *>::size_type SizeType;

    const SizeType dqSize = dq.size();
    if (!reverse)
    {
        for (SizeType i = 0; i < dqSize; ++i)
            LLBC_STLHelper::OperateElem(dq[i], opcode);
    }
    else
    {
        for (SizeType i = dqSize - 1; i != SizeType(-1); --i)
            LLBC_STLHelper::OperateElem(dq[i], opcode);
    }

    if (LIKELY(clear))
        dq.clear();
}

template <typename _Ty>
void LLBC_STLHelper::OperateContainer(std::stack<_Ty *> &stk, int opcode, bool clear, bool reverse)
{
    while (!stk.empty())
    {
        LLBC_STLHelper::OperateElem(stk.top(), opcode);
        stk.pop();
    }
}

template <typename _Ty>
void LLBC_STLHelper::OperateContainer(std::queue<_Ty *> &q, int opcode, bool clear, bool reverse)
{
    while (!q.empty())
    {
        LLBC_STLHelper::OperateElem(q.front(), opcode);
        q.pop();
    }
}

template <typename _Key>
void LLBC_STLHelper::OperateContainer(std::set<_Key *> &s, int opcode, bool clear, bool reverse)
{
    if (s.empty())
        return;

    typedef typename std::set<_Key *>::iterator SetIter;
    typedef typename std::set<_Key *>::reverse_iterator SetReverseIter;

    if (!reverse)
    {
        for (SetIter iter = s.begin(); iter != s.end(); ++iter)
        {
            _Key *const &elem = *iter;
            LLBC_STLHelper::OperateElem(elem, opcode);
        }
    }
    else
    {
        for (SetReverseIter riter = s.rbegin(); riter != s.rend(); ++riter)
        {
            _Key *const &elem = *riter;
            LLBC_STLHelper::OperateElem(elem, opcode);
        }
    }

    if (LIKELY(clear))
        s.clear();
}

template <typename _Key, typename _Ty>
void LLBC_STLHelper::OperateContainer(std::map<_Key, _Ty *> &m, int opcode, bool clear, bool reverse)
{
    if (m.empty())
        return;

    typedef typename std::map<_Key, _Ty *>::iterator MapIter;
    typedef typename std::map<_Key, _Ty *>::reverse_iterator MapReverseIter;

    if (!reverse)
    {
        for (MapIter iter = m.begin(); iter != m.end(); ++iter)
            LLBC_STLHelper::OperateElem(iter->second, opcode);
    }
    else
    {
        for (MapReverseIter riter = m.rbegin(); riter != m.rend(); ++riter)
            LLBC_STLHelper::OperateElem(riter->second, opcode);
    }

    if (LIKELY(clear))
        m.clear();
}

template <typename _Key, typename _Ty>
void LLBC_STLHelper::OperateContainer(std::unordered_map<_Key, _Ty *> &m, int opcode, bool clear, bool reverse)
{
    if (m.empty())
        return;

    typedef typename std::unordered_map<_Key, _Ty *>::iterator MapIter;

    for (MapIter iter = m.begin(); iter != m.end(); ++iter)
        LLBC_STLHelper::OperateElem(iter->second, opcode);

    if (LIKELY(clear))
        m.clear();
}

template <typename _Ty, std::size_t _Nm>
void LLBC_STLHelper::OperateContainer(std::array<_Ty *, _Nm> &arr, int opcode, bool clear, bool reverse)
{
    if (arr.empty())
        return;

    typedef typename std::array<_Ty *, _Nm>::size_type SizeType;

    const SizeType arrSize = arr.size();
    if (!reverse)
    {
        for (SizeType i = 0; i < arrSize; ++i)
            LLBC_STLHelper::OperateElem(arr[i], opcode);
    }
    else
    {
        for (SizeType i = arrSize - 1; i != SizeType(-1); --i)
            LLBC_STLHelper::OperateElem(arr[i], opcode);
    }

    if (LIKELY(clear))
        arr.fill(nullptr);
}

template <typename _Ty>
inline void LLBC_STLHelper::OperateElem(_Ty *&elem, int opcode)
{
    switch (opcode)
    {
    case LLBC_STLContainerOpcode::Delete:
        delete elem;
        break;

    case LLBC_STLContainerOpcode::Free:
        free(elem);
        break;

    case LLBC_STLContainerOpcode::Deletes:
        delete[] elem;
        break;

    case LLBC_STLContainerOpcode::Recycle:
        LLBC_Recycle(elem);
        break;

    default:
        ASSERT(false && "LLBC_STLHelper::OperateElem(): invalid operation code");
        break;
    }
}

template <typename _Ty>
inline void LLBC_STLHelper::OperateElem(_Ty * const &elem, int opcode)
{
    if (UNLIKELY(!elem))
        return;

    switch (opcode)
    {
    case LLBC_STLContainerOpcode::Delete:
        delete elem;
        break;

    case LLBC_STLContainerOpcode::Free:
        free(elem);
        break;

    case LLBC_STLContainerOpcode::Deletes:
        delete[] elem;
        break;

    case LLBC_STLContainerOpcode::Recycle:
        LLBC_Recycle(elem);
        break;

    default:
        ASSERT(false && "LLBC_STLHelper::OperateElem(): invalid operation code");
        break;
    }
}

__LLBC_NS_END
