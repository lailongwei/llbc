/**
 * @file    Util_Math.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/09/11
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_UTILS_UTIL_MATH_H__
#define __LLBC_CORE_UTILS_UTIL_MATH_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Absolute value of given number.
 * @param[in] value - number value.
 * @return T - absolute value.
 */
template <typename T>
T LLBC_Abs(const T &val);

__LLBC_NS_END

#include "llbc/core/utils/Util_MathImpl.h"

#endif // !__LLBC_CORE_UTILS_UTIL_MATH_H__

