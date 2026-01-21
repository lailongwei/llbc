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

#include "llbc/core/objpool/ObjPool.h"

__LLBC_NS_BEGIN

/**
 * \brief The class simplifies the operations for STL.
 */
class LLBC_EXPORT LLBC_STLHelper
{
public:
    /**
     * Free STL container.
     * @param[in] cont    - stl container.
     * @param[in] reverse - reverse free flag.
     */
    template <typename Cont>
    static void FreeContainer(Cont &cont, bool reverse = false)
    {
        OpCont<Cont, 0>(cont, reverse);
    }

    /**
     * Delete STL container.
     * @param[in] cont    - stl container.
     * @param[in] reverse - reverse delete flag.
     */
    template <typename Cont>
    static void DeleteContainer(Cont &cont, bool reverse = false)
    {
        OpCont<Cont, 1>(cont, reverse);
    }

    /**
     * Deletes STL container.
     * @param[in] cont    - stl container.
     * @param[in] reverse - reverse deletes flag.
     */
    template <typename Cont>
    static void DeletesContainer(Cont &cont, bool reverse = false)
    {
        OpCont<Cont, 2>(cont, reverse);
    }

    /**
     * Recycle STL container.
     * @param[in] cont    - stl container.
     * @param[in] reverse - reverse recycle flag.
     */
    template <typename Cont>
    static void RecycleContainer(Cont &cont, bool reverse = false)
    {
        OpCont<Cont, 3>(cont, reverse);
    }

private:
    template <typename pointer_type, int op>
    struct PointerOperator
    {
        static void Operate(const pointer_type &pointer)
        {
            llbc_assert(false && "Unsupported op");
        }
    };

    template <typename pointer_type>
    struct PointerOperator<pointer_type, 0>
    {
        static void Operate(const pointer_type &pointer) { free(pointer); }
    };

    template <typename pointer_type>
    struct PointerOperator<pointer_type, 1>
    {
        static void Operate(const pointer_type &pointer) { delete pointer; }
    };

    template <typename pointer_type>
    struct PointerOperator<pointer_type, 2>
    {
        static void Operate(const pointer_type &pointer) { delete[] pointer; }
    };

    template <typename pointer_type>
    struct PointerOperator<pointer_type, 3>
    {
        static void Operate(const pointer_type &pointer) { LLBC_Recycle(pointer); }
    };

private:
    template <typename Cont, int Op>
    static
    typename std::enable_if<LLBC_IsTemplSpec<Cont, std::vector>::value ||
                            LLBC_IsTemplSpec<Cont, std::list>::value ||
                            LLBC_IsTemplSpec<Cont, std::set>::value , void>::type
    OpCont(Cont &cont, bool reverse)
    {
        if (!reverse)
        {
            const auto endIt = cont.end();
            for (auto it = cont.begin(); it != endIt; ++it)
                PointerOperator<typename Cont::value_type, Op>::Operate(*it);
        }
        else
        {
            const auto rendIt = cont.rend();
            for (auto rit = cont.rbegin(); rit != rendIt; ++rit)
                PointerOperator<typename Cont::value_type, Op>::Operate(*rit);
        }

        cont.clear();
    }

    template <typename Cont, int Op>
    static
    typename std::enable_if<LLBC_IsTemplSpec<Cont, std::unordered_set>::value, void>::type
    OpCont(Cont &cont, bool)
    {
        const auto endIt = cont.end();
        for (auto it = cont.begin(); it != endIt; ++it)
            PointerOperator<typename Cont::value_type, Op>::Operate(*it);

        cont.clear();
    }

    template <typename Cont, int Op>
    static
    typename std::enable_if<LLBC_IsTemplSpec<Cont, std::stack>::value, void>::type
    OpCont(Cont &cont, bool)
    {
        while (!cont.empty())
        {
            PointerOperator<typename Cont::value_type, Op>::Operate(cont.top());
            cont.pop();
        }
    }

    template <typename Cont, int Op>
    static
    typename std::enable_if<LLBC_IsTemplSpec<Cont, std::queue>::value, void>::type
    OpCont(Cont &cont, bool)
    {
        while (!cont.empty())
        {
            PointerOperator<typename Cont::value_type, Op>::Operate(cont.front());
            cont.pop();
        }
    }

    template <typename Cont, int Op>
    static
    typename std::enable_if<LLBC_IsTemplSpec<Cont, std::deque>::value, void>::type
    OpCont(Cont &cont, bool reverse)
    {
        const int contSize = static_cast<int>(cont.size());
        if (!reverse)
        {
            for (int i = 0; i < contSize; ++i)
                PointerOperator<typename Cont::value_type, Op>::Operate(cont[i]);
        }
        else
        {
            for (int i = contSize - 1; i >= 0; --i)
                PointerOperator<typename Cont::value_type, Op>::Operate(cont[i]);
        }

        cont.clear();
    }

    template <typename Cont, int Op>
    static
    typename std::enable_if<LLBC_IsTemplSpec<Cont, std::map>::value, void>::type
    OpCont(Cont &cont, bool reverse)
    {
        if (!reverse)
        {
            const auto endIt = cont.end();
            for (auto it = cont.begin(); it != endIt; ++it)
                PointerOperator<typename Cont::mapped_type, Op>::Operate(it->second);
        }
        else
        {
            const auto rendIt = cont.rend();
            for (auto rit = cont.rbegin(); rit != rendIt; ++rit)
                PointerOperator<typename Cont::mapped_type, Op>::Operate(rit->second);
        }

        cont.clear();
    }

    template <typename Cont, int Op>
    static
    typename std::enable_if<LLBC_IsTemplSpec<Cont, std::unordered_map>::value, void>::type
    OpCont(Cont &cont, bool)
    {
        const auto endIt = cont.end();
        for (auto it = cont.begin(); it != endIt; ++it)
            PointerOperator<typename Cont::mapped_type, Op>::Operate(it->second);

        cont.clear();
    }

    template <typename Cont, int Op>
    static
    typename std::enable_if<LLBC_IsSTLArraySpec<Cont, std::array>::value, void>::type
    OpCont(Cont &cont, bool reverse)
    {
        if (!reverse)
        {
            for (size_t i = 0; i < cont.size(); ++i)
            {
                auto &elem = cont.at(i);
                PointerOperator<typename Cont::value_type, Op>::Operate(elem);

                elem = nullptr;
            }
        }
        else
        {
            for (int i = static_cast<int>(cont.size()) - 1; i >= 0; --i)
            {
                auto &elem = cont.at(i);
                PointerOperator<typename Cont::value_type, Op>::Operate(elem);

                elem = nullptr;
            }
        }
    }
};

__LLBC_NS_END
