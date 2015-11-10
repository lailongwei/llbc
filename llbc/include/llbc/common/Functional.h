/**
 * @file    Functional.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/01/17
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COM_FUNCTIONAL_H__
#define __LLBC_COM_FUNCTIONAL_H__

#include "llbc/common/PFConfig.h"

#include "llbc/common/Macro.h"

__LLBC_NS_BEGIN

/**
 * \brief The unary function template class encapsulation.
 */
template <typename _Arg,
          typename _Result>
struct LLBC_UnaryFunction
{
    typedef _Arg Argument_Type;
    typedef _Result Result_Type;
};

/**
 * \brief The binary function template class encapsulation.
 */
template <typename _Arg1, 
          typename _Arg2, 
          typename _Result>
struct LLBC_BinaryFunction
{
    typedef _Arg1 Argument1_Type;
    typedef _Arg2 Argument2_Type;
    typedef _Result Result_Type;
};

__LLBC_NS_END

#endif // !__LLBC_COM_FUNCTIONAL_H__
