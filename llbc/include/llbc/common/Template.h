/**
 * @file    Template.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/05
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COM_TEMPLATE_H__
#define __LLBC_COM_TEMPLATE_H__

#include "llbc/common/Macro.h"

__LLBC_NS_BEGIN

// Swap template.
template <typename T>
inline void LLBC_Swap(T &left, T &right)
{
    T temp = left;
    left = right;
    right = temp;
};

__LLBC_NS_END

#endif // !__LLBC_COM_TEMPLATE_H__
