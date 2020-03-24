#ifndef _TEST_TRAITS_H_
#define _TEST_TRAITS_H_

#include "llbc.h"
#include <array>

using TestCaseFactoryFunc = ::llbc::LLBC_ITestCase* (*) ();
using TestCaseFactory = std::pair<const char*, TestCaseFactoryFunc>;

template <class ObjType, typename... Args>
ObjType* createobj(Args&&... args)
{
	return new ObjType(std::forward<Args>(args)...);
}

template <int idx>
struct test_case_traits
{
	static const char* name()
	{
		return nullptr;
	}
	static ::llbc::LLBC_ITestCase* create()
	{
		return nullptr;
	}
};

template <int i>
struct TraitsLoop
{
	static void generate()
	{
		TraitsLoop<i - 1>::generate();
		g_factory[ i - 1 ] = TestCaseFactory(test_case_traits<i>::name(), &test_case_traits<i>::create);
	}
};

template <>
struct TraitsLoop<0>
{
	static void generate() {}
};

#define DEF_TEST_CASE_BEGIN                \
	enum                                   \
	{                                      \
		test_case_id_start = __LINE__ + 1, \
	};

#define DEF_TEST_CASE_END                                \
	enum                                                 \
	{                                                    \
		test_case_count = __LINE__ - test_case_id_start, \
	};                                                   \
	static std::array<TestCaseFactory, test_case_count> g_factory;

#define DEFINE_TEST_CASE(cls)                    \
	enum                                         \
	{                                            \
		E_##cls = __LINE__ - test_case_id_start, \
	};                                           \
	template <>                                  \
	struct test_case_traits<E_##cls>             \
	{                                            \
		static ::llbc::LLBC_ITestCase* create()  \
		{                                        \
			return createobj<cls>();             \
		}                                        \
		static const char* name()                \
		{                                        \
			return #cls;                         \
		}                                        \
	};


#define TEST_CASE_NAME(idx) g_factory[ idx ].first
#define TEST_CASE_FUNC(idx) g_factory[ idx ].second

#endif
