#ifndef _TEST_TRAITS_H_
#define _TEST_TRAITS_H_

#include "llbc.h"

using TestCaseFactoryFunc = ::llbc::LLBC_ITestCase* (*) ();
using TestCaseFactory = std::pair<const char*, TestCaseFactoryFunc>;

template <class ObjType, typename... Args>
ObjType* __CreateTestCaseIns(Args&&... args)
{
    return new ObjType(std::forward<Args>(args)...);
}

template <int idx>
struct TestCaseTraits
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

template <int i>
struct TraitsLoop
{
    static void Generate()
    {
        TraitsLoop<i - 1>::Generate();
        G_testCaseFactory[ i - 1 ] = TestCaseFactory(TestCaseTraits<i>::Name(), &TestCaseTraits<i>::CreateTestCaseIns);
    }
};

template <>
struct TraitsLoop<0>
{
    static void Generate() {}
};

#define DEF_TEST_CASE_BEGIN                  \
    enum                                     \
    {                                        \
        __TEST_CASE_ID_START = __LINE__ + 1, \
    };

#define DEF_TEST_CASE_END                                    \
    enum                                                     \
    {                                                        \
        __TEST_CASE_COUNT = __LINE__ - __TEST_CASE_ID_START, \
    };                                                       \
    static TestCaseFactory G_testCaseFactory[ __TEST_CASE_COUNT ] = {};

#define DEFINE_TEST_CASE(cls)                              \
    enum                                                   \
    {                                                      \
        __E_##cls = __LINE__ - __TEST_CASE_ID_START,       \
    };                                                     \
    template <>                                            \
    struct TestCaseTraits<__E_##cls>                       \
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

#define TEST_CASE_NAME(idx) G_testCaseFactory[ idx ].first
#define TEST_CASE_FUNC(idx) G_testCaseFactory[ idx ].second

#endif //_TEST_TRAITS_H_
