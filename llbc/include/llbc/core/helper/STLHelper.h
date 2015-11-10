/**
 * @file    STLHelper.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/03/17
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_HELPER_STL_HELPER_H__
#define __LLBC_CORE_HELPER_STL_HELPER_H__

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

#include "llbc/core/helper/STLHelperImpl.h"

#endif // __LLBC_CORE_HELPER_STL_HELPER_H__
