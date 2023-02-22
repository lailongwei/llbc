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
 * \brief The STL container operation code enumeration.
 */
class LLBC_STLContainerOpcode
{
public:
    enum
    {
        Begin,

        Delete = Begin,
        Free,
        Deletes,
        Recycle,

        End
    };
};

/**
 * \brief The class simplifies the operations for STL.
 */
class LLBC_EXPORT LLBC_STLHelper
{
public:
    /**
     * Free STL container.
     * @param[in/out] container - STL container object.
     * @param[in]     clear     - clear container flag, default is true.
     * @param[in]     reverse   - reverse free flag, default is false.
     */
    template <typename _Ty>
    static void FreeContainer(std::vector<_Ty *> &vec, bool clear = true, bool reverse = false);
    template <typename _Ty>
    static void FreeContainer(std::list<_Ty *> &l, bool clear = true, bool reverse = false);
    template <typename _Ty>
    static void FreeContainer(std::deque<_Ty *> &dq, bool clear = true, bool reverse = false);
    template <typename _Ty>
    static void FreeContainer(std::stack<_Ty *> &stk, bool clear = true, bool reverse = false);
    template <typename _Ty>
    static void FreeContainer(std::queue<_Ty *> &q, bool clear = true, bool reverse = false);
    template <typename _Key>
    static void FreeContainer(std::set<_Key *> &s, bool clear = true, bool reverse = false);
    template <typename _Key, typename _Ty>
    static void FreeContainer(std::map<_Key, _Ty *> &m, bool clear = true, bool reverse = false);
    template <typename _Key, typename _Ty>
    static void FreeContainer(std::unordered_map<_Key, _Ty *> &m, bool clear = true, bool reverse = false);
    template <typename _Ty, std::size_t _Nm>
    static void FreeContainer(std::array<_Ty *, _Nm> &arr, bool clear = true, bool reverse = false);

    /**
     * Delete STL container.
     * @param[in/out] container - STL container object.
     * @param[in]     clear     - clear container flag, default is true.
     * @param[in]     reverse   - reverse free flag, default is false.
     */
    template <typename _Ty>
    static void DeleteContainer(std::vector<_Ty *> &vec, bool clear = true, bool reverse = false);
    template <typename _Ty>
    static void DeleteContainer(std::list<_Ty *> &l, bool clear = true, bool reverse = false);
    template <typename _Ty>
    static void DeleteContainer(std::deque<_Ty *> &dq, bool clear = true, bool reverse = false);
    template <typename _Ty>
    static void DeleteContainer(std::stack<_Ty *> &stk, bool clear = true, bool reverse = false);
    template <typename _Ty>
    static void DeleteContainer(std::queue<_Ty *> &q, bool clear = true, bool reverse = false);
    template <typename _Key>
    static void DeleteContainer(std::set<_Key *> &s, bool clear = true, bool reverse = false);
    template <typename _Key, typename _Ty>
    static void DeleteContainer(std::map<_Key, _Ty *> &m, bool clear = true, bool reverse = false);
    template <typename _Key, typename _Ty>
    static void DeleteContainer(std::unordered_map<_Key, _Ty *> &m, bool clear = true, bool reverse = false);
    template <typename _Ty, std::size_t _Nm>
    static void DeleteContainer(std::array<_Ty *, _Nm> &arr, bool clear = true, bool reverse = false);

    /**
     * Deletes STL container.
     * @param[in/out] container - STL container object.
     * @param[in]     clear     - clear container flag, default is true.
     * @param[in]     reverse   - reverse free flag, default is falst.
     */
    template <typename _Ty>
    static void DeletesContainer(std::vector<_Ty *> &vec, bool clear = true, bool reverse = false);
    template <typename _Ty>
    static void DeletesContainer(std::list<_Ty *> &l, bool clear = true, bool reverse = false);
    template <typename _Ty>
    static void DeletesContainer(std::deque<_Ty *> &dq, bool clear = true, bool reverse = false);
    template <typename _Ty>
    static void DeletesContainer(std::stack<_Ty *> &stk, bool clear = true, bool reverse = false);
    template <typename _Ty>
    static void DeletesContainer(std::queue<_Ty *> &stk, bool clear = true, bool reverse = false);
    template <typename _Key>
    static void DeletesContainer(std::set<_Key *> &s, bool clear = true, bool reverse = false);
    template <typename _Key, typename _Ty>
    static void DeletesContainer(std::map<_Key, _Ty *> &m, bool clear = true, bool reverse = false);
    template <typename _Key, typename _Ty>
    static void DeletesContainer(std::unordered_map<_Key, _Ty *> &m, bool clear = true, bool reverse = false);
    template <typename _Ty, std::size_t _Nm>
    static void DeletesContainer(std::array<_Ty *, _Nm> &arr, bool clear = true, bool reverse = false);

    /**
     * Recycle STL container.
     * @param[in/out] container - STL container object.
     * @param[in]     clear     - clear container flag, default is true.
     * @param[in]     reverse   - reverse free flag, default is falst.
     */
    template <typename _Ty>
    static void RecycleContainer(std::vector<_Ty *> &vec, bool clear = true, bool reverse = false);
    template <typename _Ty>
    static void RecycleContainer(std::list<_Ty *> &l, bool clear = true, bool reverse = false);
    template <typename _Ty>
    static void RecycleContainer(std::deque<_Ty *> &dq, bool clear = true, bool reverse = false);
    template <typename _Ty>
    static void RecycleContainer(std::stack<_Ty *> &stk, bool clear = true, bool reverse = false);
    template <typename _Ty>
    static void RecycleContainer(std::queue<_Ty *> &stk, bool clear = true, bool reverse = false);
    template <typename _Key>
    static void RecycleContainer(std::set<_Key *> &s, bool clear = true, bool reverse = false);
    template <typename _Key, typename _Ty>
    static void RecycleContainer(std::map<_Key, _Ty *> &m, bool clear = true, bool reverse = false);
    template <typename _Key, typename _Ty>
    static void RecycleContainer(std::unordered_map<_Key, _Ty *> &m, bool clear = true, bool reverse = false);
    template <typename _Ty, std::size_t _Nm>
    static void RecycleContainer(std::array<_Ty *, _Nm> &arr, bool clear = true, bool reverse = false);

public:
    /**
     * Operate STL container.
     * @param[in/out] container - STL container object.
     * @param[in]     opcode    - operation code, see LLBC_STLContainerOpCode enumeration class.
     * @param[in]     clear     - clear container flag, default is true.
     * @param[in]     reverse   - reverse free flag, default is false.
     */
    template <typename _Ty>
    static void OperateContainer(std::vector<_Ty *> &vec, int opcode, bool clear, bool reverse);
    template <typename _Ty>
    static void OperateContainer(std::list<_Ty *> &l, int opcode, bool clear, bool reverse);
    template <typename _Ty>
    static void OperateContainer(std::deque<_Ty *> &dq, int opcode, bool clear, bool reverse);
    template <typename _Ty>
    static void OperateContainer(std::stack<_Ty *> &stk, int opcode, bool clear, bool reverse);
    template <typename _Ty>
    static void OperateContainer(std::queue<_Ty *> &q, int opcode, bool clear, bool reverse);
    template <typename _Key>
    static void OperateContainer(std::set<_Key *> &s, int opcode, bool clear, bool reverse);
    template <typename _Key, typename _Ty>
    static void OperateContainer(std::map<_Key, _Ty *> &m, int opcode, bool clear, bool reverse);
    template <typename _Key, typename _Ty>
    static void OperateContainer(std::unordered_map<_Key, _Ty *> &m, int opcode, bool clear, bool reverse);
    template <typename _Ty, std::size_t _Nm>
    static void OperateContainer(std::array<_Ty *, _Nm> &arr, int opcode, bool clear, bool reverse);

private:
    /**
     * Operate STL element.
     * @param[in/out] elem   - element object pointer.
     * @param[in]     opcode - operation code, see LLBC_STLContainerOpcode enumeration class.
     */
    template <typename _Ty>
    static void OperateElem(_Ty *&elem, int opcode);
    template <typename _Ty>
    static void OperateElem(_Ty * const &elem, int opcode);
};

__LLBC_NS_END

#include "llbc/core/helper/STLHelperInl.h"


