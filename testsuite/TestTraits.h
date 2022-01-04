#ifndef _TEST_TRAITS_H_
#define _TEST_TRAITS_H_

#include "llbc.h"

typedef ::llbc::LLBC_ITestCase * (*__TestCaseFactoryFunc)();
typedef std::pair<const char*, __TestCaseFactoryFunc> __TestCaseFactory;

template <class ObjType>
ObjType* __CreateTestCaseIns()
{
    return new ObjType;
}

template <int idx>
struct __TestCaseTraits
{
    static const char* Name()
    {
        return nullptr;
    }
    static ::llbc::LLBC_ITestCase* CreateTestCaseIns()
    {
        return nullptr;
    }
};

#define __DEF_TEST_CASE_BEGIN            \
    enum                                 \
    {                                    \
        __TEST_CASE_ID_START = __LINE__, \
    };

#define __DEF_TEST_CASE_END                                  \
    enum                                                     \
    {                                                        \
        __TEST_CASE_COUNT = __LINE__ - __TEST_CASE_ID_START, \
    };                                                       \
    static __TestCaseFactory __g_testcaseFactory[ __TEST_CASE_COUNT ] = {};

#define __DEFINE_TEST_CASE(cls)                            \
    enum                                                   \
    {                                                      \
        __E_##cls = __LINE__ - __TEST_CASE_ID_START,       \
    };                                                     \
    template <>                                            \
    struct __TestCaseTraits<__E_##cls>                     \
    {                                                      \
        static ::llbc::LLBC_ITestCase* CreateTestCaseIns() \
        {                                                  \
            return __CreateTestCaseIns<cls>();             \
        }                                                  \
        static const char* Name()                          \
        {                                                  \
            return #cls;                                   \
        }                                                  \
    };

#include "TestSuiteDef.h"

template <int i>
struct __TraitsLoop
{
    static void Generate()
    {
        __TraitsLoop<i - 1>::Generate();
        __g_testcaseFactory[ i - 1 ] = __TestCaseFactory(__TestCaseTraits<i>::Name(), &__TestCaseTraits<i>::CreateTestCaseIns);
    }
};

template <>
struct __TraitsLoop<0>
{
    static void Generate() {}
};

#define __TEST_CASE_NAME(idx) __g_testcaseFactory[ idx ].first
#define __TEST_CASE_FUNC(idx) __g_testcaseFactory[ idx ].second

#endif  //_TEST_TRAITS_H_
