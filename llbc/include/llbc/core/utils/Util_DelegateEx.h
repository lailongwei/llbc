/**
 * @file    Util_DelegateEx.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/30
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_UTILS_UTIL_DELEGATE_EX_H__
#define __LLBC_CORE_UTILS_UTIL_DELEGATE_EX_H__

/**
 * \brief interface for LLBC_DelegateEx.
 */
template <typename Arg>
class LLBC_IDelegateEx
{
public:
    virtual ~LLBC_IDelegateEx() {  }

public:
    virtual void *Invoke(Arg arg) = 0;
};

template <typename ObjectType, typename Arg>
class LLBC_DelegateEx : public LLBC_IDelegateEx<Arg>
{
    typedef void *(ObjectType::*Method)(Arg arg);

public:
    LLBC_DelegateEx(ObjectType *object, Method method):_object(object), _method(method) {  }

    virtual ~LLBC_DelegateEx() {  }

public:
    virtual void *Invoke(Arg arg) { return (_object->*_method)(arg); }

public:
    ObjectType *_object;
    Method _method;
};

template <typename Arg>
class LLBC_FuncDelegateEx : public LLBC_IDelegateEx<Arg>
{
    typedef void *(*Function)(Arg arg);

public:
    LLBC_FuncDelegateEx(Function function):_function(function) {  }

    virtual ~LLBC_FuncDelegateEx() {  }

public:
    virtual void *Invoke(Arg arg) { return (*_function)(arg); }

public:
    Function _function;
};

#endif // !__LLBC_CORE_UTILS_UTIL_DELEGATE_EX_H__
