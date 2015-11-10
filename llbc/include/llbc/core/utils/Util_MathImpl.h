/**
 * @file    Util_MathImpl.h
 * @author  Longwei lai<lailongwei@126.com>
 * @date    2013/09/11
 * @version 1.0
 *
 * @brief
 */
#ifdef __LLBC_CORE_UTILS_UTIL_MATH_H__

__LLBC_NS_BEGIN

template <typename T>
inline T LLBC_Abs(const T &val)
{
    return (val < 0) ? - val : val;
}

template <>
inline float LLBC_Abs(const float &val)
{
    return fabsf(val);
}

template <>
inline double LLBC_Abs(const double &val)
{
    return fabs(val);
}

template <>
inline ldouble LLBC_Abs(const ldouble &val)
{
    return fabsl(val);
}

__LLBC_NS_END

#endif // __LLBC_CORE_UTILS_UTIL_MATH_H__
