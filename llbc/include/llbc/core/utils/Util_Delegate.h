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

#ifdef __LLBC_CORE_UTILS_UTIL_DELEGATE_IMPL_H__

#define LLBC_DELEGATE_CAT(x, y)   LLBC_DELEGATE_CAT_I(x, y)
#define LLBC_DELEGATE_CAT_I(x, y) x ## y

#define LLBC_IDELEGATE     LLBC_DELEGATE_CAT(LLBC_IDelegate, LLBC_DELEGATE_N)
#define LLBC_DELEGATE      LLBC_DELEGATE_CAT(LLBC_Delegate,  LLBC_DELEGATE_N)
#define LLBC_FUNC_DELEGATE LLBC_DELEGATE_CAT(LLBC_Func, LLBC_DELEGATE_N)

#if LLBC_DELEGATE_N == 0 // ----------------------- LLBC_Delegate0
#define LLBC_IDELEGATE_TEMPLATE     template <typename Rtn>
#define LLBC_DELEGATE_BASE          LLBC_IDELEGATE<Rtn>
#define LLBC_DELEGATE_TEMPLATE      template <typename Rtn, typename ObjectType>
#define LLBC_DELEGATE_ARG           void
#define LLBC_DELEGATE_VALUE 
#define LLBC_DELEGATE_RTN           Rtn
#define LLBC_FUNC_DELEGATE_TEMPLATE LLBC_IDELEGATE_TEMPLATE

#define LLBC_DELEGATE_TEMPLATE_SPEC template <typename ObjectType>
#define LLBC_DELEGATE_BASE_SPEC     LLBC_IDELEGATE<void>
#define LLBC_DELEGATE_SPEC          LLBC_DELEGATE<void, ObjectType>
#define LLBC_FUNC_DELEGATE_TEMPLATE_SPEC template <>
#define LLBC_FUNC_DELEGATE_BASE_SPEC LLBC_DELEGATE_BASE_SPEC
#define LLBC_FUNC_DELEGATE_SPEC     LLBC_FUNC_DELEGATE<void>

#elif LLBC_DELEGATE_N == 1 // ---------------------- LLBC_Delegate1
#define LLBC_IDELEGATE_TEMPLATE     template <typename Rtn, typename Arg1>
#define LLBC_DELEGATE_BASE          LLBC_IDELEGATE<Rtn, Arg1>
#define LLBC_DELEGATE_TEMPLATE      template <typename Rtn, typename ObjectType, typename Arg1>
#define LLBC_DELEGATE_ARG           Arg1 arg1
#define LLBC_DELEGATE_VALUE         arg1
#define LLBC_DELEGATE_RTN           Rtn
#define LLBC_FUNC_DELEGATE_TEMPLATE LLBC_IDELEGATE_TEMPLATE

#define LLBC_DELEGATE_TEMPLATE_SPEC template <typename ObjectType, typename Arg1>
#define LLBC_DELEGATE_BASE_SPEC     LLBC_IDELEGATE<void, Arg1>
#define LLBC_DELEGATE_SPEC          LLBC_DELEGATE<void, ObjectType, Arg1>
#define LLBC_FUNC_DELEGATE_TEMPLATE_SPEC template <typename Arg1>
#define LLBC_FUNC_DELEGATE_BASE_SPEC LLBC_DELEGATE_BASE_SPEC
#define LLBC_FUNC_DELEGATE_SPEC     LLBC_FUNC_DELEGATE<void, Arg1>

#elif LLBC_DELEGATE_N == 2 // ---------------------- LLBC_Delegate2
#define LLBC_IDELEGATE_TEMPLATE     template <typename Rtn, typename Arg1, typename Arg2>
#define LLBC_DELEGATE_BASE          LLBC_IDELEGATE<Rtn, Arg1, Arg2>
#define LLBC_DELEGATE_TEMPLATE      template <typename Rtn, typename ObjectType, typename Arg1, typename Arg2>
#define LLBC_DELEGATE_ARG           Arg1 arg1, Arg2 arg2
#define LLBC_DELEGATE_VALUE         arg1, arg2
#define LLBC_DELEGATE_RTN           Rtn
#define LLBC_FUNC_DELEGATE_TEMPLATE LLBC_IDELEGATE_TEMPLATE

#define LLBC_DELEGATE_TEMPLATE_SPEC template <typename ObjectType, typename Arg1, typename Arg2>
#define LLBC_DELEGATE_BASE_SPEC     LLBC_IDELEGATE<void, Arg1, Arg2>
#define LLBC_DELEGATE_SPEC          LLBC_DELEGATE<void, ObjectType, Arg1, Arg2>
#define LLBC_FUNC_DELEGATE_TEMPLATE_SPEC template <typename Arg1, typename Arg2>
#define LLBC_FUNC_DELEGATE_BASE_SPEC LLBC_DELEGATE_BASE_SPEC
#define LLBC_FUNC_DELEGATE_SPEC     LLBC_FUNC_DELEGATE<void, Arg1, Arg2>

#elif LLBC_DELEGATE_N == 3 // ---------------------- LLBC_Delegate3
#define LLBC_IDELEGATE_TEMPLATE     template <typename Rtn, typename Arg1, typename Arg2, typename Arg3>
#define LLBC_DELEGATE_BASE          LLBC_IDELEGATE<Rtn, Arg1, Arg2, Arg3>
#define LLBC_DELEGATE_TEMPLATE      template <typename Rtn, typename ObjectType, typename Arg1, typename Arg2, typename Arg3>
#define LLBC_DELEGATE_ARG           Arg1 arg1, Arg2 arg2, Arg3 arg3
#define LLBC_DELEGATE_VALUE         arg1, arg2, arg3
#define LLBC_DELEGATE_RTN           Rtn
#define LLBC_FUNC_DELEGATE_TEMPLATE LLBC_IDELEGATE_TEMPLATE

#define LLBC_DELEGATE_TEMPLATE_SPEC template <typename ObjectType, typename Arg1, typename Arg2, typename Arg3>
#define LLBC_DELEGATE_BASE_SPEC     LLBC_IDELEGATE<void, Arg1, Arg2, Arg3>
#define LLBC_DELEGATE_SPEC          LLBC_DELEGATE<void, ObjectType, Arg1, Arg2, Arg3>
#define LLBC_FUNC_DELEGATE_TEMPLATE_SPEC template <typename Arg1, typename Arg2, typename Arg3>
#define LLBC_FUNC_DELEGATE_BASE_SPEC LLBC_DELEGATE_BASE_SPEC
#define LLBC_FUNC_DELEGATE_SPEC     LLBC_FUNC_DELEGATE<void, Arg1, Arg2, Arg3>

#elif LLBC_DELEGATE_N == 4 // ---------------------- LLBC_Delegate4
#define LLBC_IDELEGATE_TEMPLATE     template <typename Rtn, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
#define LLBC_DELEGATE_BASE          LLBC_IDELEGATE<Rtn, Arg1, Arg2, Arg3, Arg4>
#define LLBC_DELEGATE_TEMPLATE      template <typename Rtn, typename ObjectType, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
#define LLBC_DELEGATE_ARG           Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4
#define LLBC_DELEGATE_VALUE         arg1, arg2, arg3, arg4
#define LLBC_DELEGATE_RTN           Rtn
#define LLBC_FUNC_DELEGATE_TEMPLATE LLBC_IDELEGATE_TEMPLATE

#define LLBC_DELEGATE_TEMPLATE_SPEC template <typename ObjectType, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
#define LLBC_DELEGATE_BASE_SPEC     LLBC_IDELEGATE<void, Arg1, Arg2, Arg3, Arg4>
#define LLBC_DELEGATE_SPEC          LLBC_DELEGATE<void, ObjectType, Arg1, Arg2, Arg3, Arg4>
#define LLBC_FUNC_DELEGATE_TEMPLATE_SPEC template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
#define LLBC_FUNC_DELEGATE_BASE_SPEC LLBC_DELEGATE_BASE_SPEC
#define LLBC_FUNC_DELEGATE_SPEC     LLBC_FUNC_DELEGATE<void, Arg1, Arg2, Arg3, Arg4>

#elif LLBC_DELEGATE_N == 5 // ---------------------- LLBC_Delegate5
#define LLBC_IDELEGATE_TEMPLATE     template <typename Rtn, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
#define LLBC_DELEGATE_BASE          LLBC_IDELEGATE<Rtn, Arg1, Arg2, Arg3, Arg4, Arg5>
#define LLBC_DELEGATE_TEMPLATE      template <typename Rtn, typename ObjectType, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
#define LLBC_DELEGATE_ARG           Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5
#define LLBC_DELEGATE_VALUE         arg1, arg2, arg3, arg4, arg5
#define LLBC_DELEGATE_RTN           Rtn
#define LLBC_FUNC_DELEGATE_TEMPLATE LLBC_IDELEGATE_TEMPLATE

#define LLBC_DELEGATE_TEMPLATE_SPEC template <typename ObjectType, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
#define LLBC_DELEGATE_BASE_SPEC     LLBC_IDELEGATE<void, Arg1, Arg2, Arg3, Arg4, Arg5>
#define LLBC_DELEGATE_SPEC          LLBC_DELEGATE<void, ObjectType, Arg1, Arg2, Arg3, Arg4, Arg5>
#define LLBC_FUNC_DELEGATE_TEMPLATE_SPEC template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
#define LLBC_FUNC_DELEGATE_BASE_SPEC LLBC_DELEGATE_BASE_SPEC
#define LLBC_FUNC_DELEGATE_SPEC     LLBC_FUNC_DELEGATE<void, Arg1, Arg2, Arg3, Arg4, Arg5>

#elif LLBC_DELEGATE_N == 6 // ---------------------- LLBC_Delegate6
#define LLBC_IDELEGATE_TEMPLATE     template <typename Rtn, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
#define LLBC_DELEGATE_BASE          LLBC_IDELEGATE<Rtn, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6>
#define LLBC_DELEGATE_TEMPLATE      template <typename Rtn, typename ObjectType, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
#define LLBC_DELEGATE_ARG           Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6
#define LLBC_DELEGATE_VALUE         arg1, arg2, arg3, arg4, arg5, arg6
#define LLBC_DELEGATE_RTN           Rtn
#define LLBC_FUNC_DELEGATE_TEMPLATE LLBC_IDELEGATE_TEMPLATE

#define LLBC_DELEGATE_TEMPLATE_SPEC template <typename ObjectType, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
#define LLBC_DELEGATE_BASE_SPEC     LLBC_IDELEGATE<void, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6>
#define LLBC_DELEGATE_SPEC          LLBC_DELEGATE<void, ObjectType, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6>
#define LLBC_FUNC_DELEGATE_TEMPLATE_SPEC template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
#define LLBC_FUNC_DELEGATE_BASE_SPEC LLBC_DELEGATE_BASE_SPEC
#define LLBC_FUNC_DELEGATE_SPEC     LLBC_FUNC_DELEGATE<void, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6>

#elif LLBC_DELEGATE_N == 7 // ---------------------- LLBC_Delegate7
#define LLBC_IDELEGATE_TEMPLATE     template <typename Rtn, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
#define LLBC_DELEGATE_BASE          LLBC_IDELEGATE<Rtn, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7>
#define LLBC_DELEGATE_TEMPLATE      template <typename Rtn, typename ObjectType, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
#define LLBC_DELEGATE_ARG           Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7
#define LLBC_DELEGATE_VALUE         arg1, arg2, arg3, arg4, arg5, arg6, arg7
#define LLBC_DELEGATE_RTN           Rtn
#define LLBC_FUNC_DELEGATE_TEMPLATE LLBC_IDELEGATE_TEMPLATE

#define LLBC_DELEGATE_TEMPLATE_SPEC template <typename ObjectType, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
#define LLBC_DELEGATE_BASE_SPEC     LLBC_IDELEGATE<void, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7>
#define LLBC_DELEGATE_SPEC          LLBC_DELEGATE<void, ObjectType, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7>
#define LLBC_FUNC_DELEGATE_TEMPLATE_SPEC template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
#define LLBC_FUNC_DELEGATE_BASE_SPEC LLBC_DELEGATE_BASE_SPEC
#define LLBC_FUNC_DELEGATE_SPEC     LLBC_FUNC_DELEGATE<void, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7>

#endif

/**
 * \brief Interface for delegate.
 */
LLBC_IDELEGATE_TEMPLATE
class LLBC_IDELEGATE
{
public:
    virtual ~LLBC_IDELEGATE() {  }

public:
    virtual Rtn Invoke(LLBC_DELEGATE_ARG) = 0;
};

/**
 * \brief Delegate class encapsulation.
 *        Storage object and it's method.
 */
LLBC_DELEGATE_TEMPLATE
class LLBC_DELEGATE : public LLBC_DELEGATE_BASE
{
    typedef Rtn (ObjectType::*Method)(LLBC_DELEGATE_ARG);
public:
    LLBC_DELEGATE(ObjectType *object, Method method, bool destroyObjectWhenDestruct = false):
        _object(object), _method(method), _destroyObjectWhenDestruct(destroyObjectWhenDestruct) {  }

    ~LLBC_DELEGATE() { if (_destroyObjectWhenDestruct) LLBC_XDelete(_object); }

public:
    Rtn Invoke(LLBC_DELEGATE_ARG) { return (_object->*_method)(LLBC_DELEGATE_VALUE); }

private:
    ObjectType *_object;
    Method _method;
    bool _destroyObjectWhenDestruct;
};

/**
 * \brief Return type partial specialized(to void) class encapsulation.
 *        Storage object and it's method.
 */
LLBC_DELEGATE_TEMPLATE_SPEC
class LLBC_DELEGATE_SPEC : public LLBC_DELEGATE_BASE_SPEC
{
    typedef void (ObjectType::*Method)(LLBC_DELEGATE_ARG);
public:
    LLBC_DELEGATE(ObjectType *object, Method method, bool destroyObjectWhenDestruct = false):
        _object(object), _method(method), _destroyObjectWhenDestruct(destroyObjectWhenDestruct) {  }

    ~LLBC_DELEGATE() { if (_destroyObjectWhenDestruct) LLBC_XDelete(_object); }

public:
    void Invoke(LLBC_DELEGATE_ARG) { (_object->*_method)(LLBC_DELEGATE_VALUE); }

private:
    ObjectType *_object;
    Method _method;
    bool _destroyObjectWhenDestruct;
};

/**
 * \brief Function specific delegate class encapsulation.
 *        Storage function.
 */
LLBC_FUNC_DELEGATE_TEMPLATE
class LLBC_FUNC_DELEGATE : public LLBC_DELEGATE_BASE
{
    typedef Rtn (*Function)(LLBC_DELEGATE_ARG);
public:
    explicit LLBC_FUNC_DELEGATE(Function function):
        _function(function) {  }

    ~LLBC_FUNC_DELEGATE() {  }

public:
    Rtn Invoke(LLBC_DELEGATE_ARG) { return (*_function)(LLBC_DELEGATE_VALUE); }

private:
    Function _function;
};

/**
 * \brief Return type partial specialized(to void) function delegate class encapsulation.
 *        Storage function.
 */
LLBC_FUNC_DELEGATE_TEMPLATE_SPEC
class LLBC_FUNC_DELEGATE_SPEC : public LLBC_FUNC_DELEGATE_BASE_SPEC
{
    typedef void (*Function)(LLBC_DELEGATE_ARG);
public:
    explicit LLBC_FUNC_DELEGATE(Function function):
        _function(function) {  }

    ~LLBC_FUNC_DELEGATE() {  }

public:
    void Invoke(LLBC_DELEGATE_ARG) { (*_function)(LLBC_DELEGATE_VALUE); }

private:
    Function _function;
};

#undef LLBC_DELEGATE_CAT
#undef LLBC_DELEGATE_CAT_I
#undef LLBC_IDELEGATE
#undef LLBC_DELEGATE
#undef LLBC_IDELEGATE_TEMPLATE
#undef LLBC_DELEGATE_BASE
#undef LLBC_DELEGATE_TEMPLATE
#undef LLBC_DELEGATE_ARG
#undef LLBC_DELEGATE_VALUE
#undef LLBC_DELEGATE_RTN
#undef LLBC_FUNC_DELEGATE
#undef LLBC_FUNC_DELEGATE_TEMPLATE
#undef LLBC_DELEGATE_TEMPLATE_SPEC
#undef LLBC_DELEGATE_BASE_SPEC
#undef LLBC_DELEGATE_SPEC
#undef LLBC_FUNC_DELEGATE_TEMPLATE_SPEC
#undef LLBC_FUNC_DELEGATE_BASE_SPEC
#undef LLBC_FUNC_DELEGATE_SPEC

#endif // __LLBC_CORE_UTILS_UTIL_DELEGATE_IMPL_H__
