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


#include "core/objpool/TestCase_Core_ObjPool.h"

namespace
{

class BaseCls
{
public:
    virtual ~BaseCls() = default;
};

class DerivedCls final : public BaseCls
{
public:
};

class _OD_A final : public LLBC_PoolObj { public: void Reuse() {}; ~_OD_A() override { LLBC_PrintLn("_OD_A destruct"); } };
class _OD_B final : public LLBC_PoolObj { public: void Reuse() {}; ~_OD_B() override { LLBC_PrintLn("_OD_B destruct"); } };
class _OD_C final : public LLBC_PoolObj { public: void Reuse() {}; ~_OD_C() override { LLBC_PrintLn("_OD_C destruct"); } };
class _OD_D final : public LLBC_PoolObj { public: void Reuse() {}; ~_OD_D() override { LLBC_PrintLn("_OD_D destruct"); } };
class _OD_E final : public LLBC_PoolObj { public: void Reuse() {}; ~_OD_E() override { LLBC_PrintLn("_OD_E destruct"); } };
class _OD_F final : public LLBC_PoolObj { public: void Reuse() {}; ~_OD_F() override { LLBC_PrintLn("_OD_F destruct"); } };
class _OD_G final : public LLBC_PoolObj { public: void Reuse() {}; ~_OD_G() override { LLBC_PrintLn("_OD_G destruct"); } };
class _OD_H final : public LLBC_Object { public: ~_OD_H() override { LLBC_PrintLn("_OD_H destruct");}};

struct ReflectMethTest
{
    static bool methCalled_GetStripeCapacity;
    static bool methCalled_OnTypedObjPoolCreated;

    size_t GetStripeCapacity()
    {
        LLBC_PrintLn("GetStripeCapacity() reflection method called");
        methCalled_GetStripeCapacity = true;

        return LLBC_CFG_CORE_OBJPOOL_STRIPE_CAPACITY;
    }

    void OnTypedObjPoolCreated(LLBC_TypedObjPool<ReflectMethTest> *typedObjPool)
    {
        LLBC_PrintLn("OnTypedObjPoolCreated() reflection method called");
        methCalled_OnTypedObjPoolCreated = true;
    }
};

bool ReflectMethTest::methCalled_GetStripeCapacity = false;
bool ReflectMethTest::methCalled_OnTypedObjPoolCreated = false;

}

class SafeObjPoolPrintNameTask : public LLBC_Task 
{
public:
    SafeObjPoolPrintNameTask(int testTimes)
    : _objPool(true)
    , _testTimes(testTimes)
    {
    }

public:
    void Svc() override
    {
        int curId = LLBC_AtomicFetchAndAdd(&_subThreadId, 1);

        if ((curId % 2) == 0)
            _GetName();
        else 
            _SetName();
    }

    void Cleanup() override {  }

    LLBC_ObjPool &GetObjPool() { return _objPool; }

protected:
    void _GetName()
    {
        for (int i = 0; i < _testTimes; i++)
            LLBC_PrintLn("GetName: %s", _objPool.GetName().c_str());
    }

    void _SetName()
    {
        LLBC_String objPoolName;
        for (int i = 0; i < _testTimes; i++)
        {
            LLBC_PrintLn("SetName: %s", objPoolName.format("test_%d", i).c_str());
            _objPool.SetName(objPoolName);
        }
    }

protected:
    LLBC_ObjPool _objPool;
    int _subThreadId = 0;
    int _testTimes = 10;
};

int TestCase_Core_ObjPool::Run(int argc, char *argv[])
{
    LLBC_ReturnIf(BaseTest() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(AcquireTest() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(ComplexAcquireTest() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(StatTest() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(ReflectTest() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(MemoryLeakTest() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(MultiThreadThread() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(PerfTest() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(OrderedDeleteTest() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(GuardedPoolObjTest() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(LibSupportedObjPoolClassesTest() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(CommonClassTest_Stream() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(RecycleTest() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(SafeObjPoolSetNameTest() != LLBC_OK, LLBC_FAILED);

    return LLBC_OK;
}

int TestCase_Core_ObjPool::BaseTest()
{
    LLBC_PrintLn("Base test:");

    // Acquire()/Release()/GetStatistics()
    LLBC_ObjPool objPool;
    auto str = objPool.Acquire<std::string>();
    *str = "hello world";
    LLBC_PrintLn("- Acquire <std::string> from objpool, str(%p):%s", str, str->c_str());

    auto str2 = objPool.Acquire<std::string>();
    *str2 = "hey judy";
    LLBC_PrintLn("- Acquire <std::string> from objpool, str(%p):%s", str2, str2->c_str());

    LLBC_PrintLn("- Stat objpool(json):\n%s",
                 objPool.GetStatistics(LLBC_ObjPoolStatFormat::Json).c_str());

    LLBC_PrintLn("- Release strs:\n");
    objPool.Release(str2);
    objPool.Release(str);

    LLBC_PrintLn("- After release strs, stat:\n%s",
                 objPool.GetStatistics(LLBC_ObjPoolStatFormat::Json).c_str());

    // Acquire() reused str.
    str = objPool.Acquire<std::string>();
    LLBC_PrintLn("- Acquire reused str, str content(%p):%s", str, str->c_str());
    LLBC_PrintLn("- Release reused str:");
    objPool.Release(str);

    // Multi type acquire release.
    LLBC_PrintLn("- Multiple type acquire/release test:");
    objPool.Release(objPool.Acquire<LLBC_Packet>());

    // Mismatch acquire/release test.
    LLBC_PrintLn("- Mismatch acquire/release test:");
    BaseCls *myObj = objPool.Acquire<DerivedCls>();
    objPool.Release(myObj);

    // Collect()
    LLBC_PrintLn("- Deep collect objpool:");
    objPool.Collect(true);
    LLBC_PrintLn("- After deep collect objpool:\n%s",
                 objPool.GetStatistics(LLBC_ObjPoolStatFormat::Json).c_str());

    LLBC_PrintLn("- Shallow collect objpool:");
    objPool.Release(objPool.Acquire<std::string>());
    LLBC_PrintLn("- After shallow collect objpool:\n%s",
                 objPool.GetStatistics(LLBC_ObjPoolStatFormat::Json).c_str());

    return LLBC_OK;
}

int TestCase_Core_ObjPool::AcquireTest()
{
    LLBC_PrintLn("Acquire test:");
    LLBC_ObjPool objPool;

    // Acquire from objpool.
    std::vector<std::string *> strs;
    for (int i = 0; i < 10; ++i)
        strs.push_back(objPool.Acquire<std::string>());

    // Reverse release to objpool.
    for (auto rit = strs.rbegin(); rit != strs.rend(); ++rit)
        objPool.Release(*rit);

    // Acquire again from objpool.
    std::vector<std::string *> strs2;
    for (int i = 0; i < 10; ++i)
        strs2.push_back(objPool.Acquire<std::string>());

    #if !LLBC_CFG_CORE_OBJPOOL_USE_MALLOC_INSTEAD
    // Compare.
    LLBC_ErrorAndReturnIf(strs != strs2, LLBC_FAILED, "Acquire test failed");
    #endif

    LLBC_PrintLn("Success");

    return LLBC_OK;
}

int TestCase_Core_ObjPool::ComplexAcquireTest()
{
    LLBC_PrintLn("Complex acquire test:");

    std::vector<int> testSeq;
    std::vector<std::string *> strs;

    // Generate test seq(1:Acquire, 1:Release).
    const int acquireTimes = 5000;
    const int preAcquireTimes = acquireTimes / 10;
    for (int i = 0; i < acquireTimes - preAcquireTimes; ++i)
        testSeq.push_back(1);

    const int releaseTimes = 3000;
    for (int i = 0; i < releaseTimes; ++i)
        testSeq.push_back(2);

    LLBC_Random rand;
    rand.Shuffle(testSeq.begin(), testSeq.end());

    // Pre-Acquire.
    LLBC_ObjPool objPool;
    for (int i = 0; i < preAcquireTimes; ++i)
        strs.push_back(objPool.Acquire<std::string>());

    // Test Acquire/Release.
    for (auto &testOp : testSeq)
    {
        if (testOp == 1)
            strs.push_back(objPool.Acquire<std::string>());
        else if (testOp == 2 && !strs.empty())
        {
            auto releaseIdx = rand.Rand(0, static_cast<int>(strs.size() - 1));
            objPool.Release(strs[releaseIdx]);
            strs.erase(strs.begin() + releaseIdx);
        }
    }

    // Post-Release.
    if (strs.size() > 2)
    {
        int postReleaseTimes = static_cast<int>(strs.size()) / 2;
        while (--postReleaseTimes >= 0)
        {
            auto releaseIdx = rand.Rand(0, static_cast<int>(strs.size() - 1));
            objPool.Release(strs[releaseIdx]);
            strs.erase(strs.begin() + releaseIdx);
        }
    }

    LLBC_PrintLn("- After test, objpool stat:\n%s",
                 objPool.GetStatistics(LLBC_ObjPoolStatFormat::CSV).c_str());

    // Colect objPool.
    for (auto &str : strs)
        objPool.Release(str);
    objPool.Collect(true);

    LLBC_PrintLn("- After collect(deep) objpool, stat:\n%s",
        objPool.GetStatistics(LLBC_ObjPoolStatFormat::CSV).c_str());

    return LLBC_OK;
}

int TestCase_Core_ObjPool::StatTest()
{
    LLBC_PrintLn("Stat test:");

    LLBC_ObjPool objPool;
    RandAllocAndRelease<int>(objPool, std::make_pair(10000, 1000), std::make_pair(500, 1000));
    RandAllocAndRelease<double>(objPool, std::make_pair(10000, 1000), std::make_pair(500, 1000));
    RandAllocAndRelease<std::string>(objPool, std::make_pair(10000, 1000), std::make_pair(500, 1000));
    RandAllocAndRelease<LLBC_Packet>(objPool, std::make_pair(10000, 1000), std::make_pair(500, 1000));
    RandAllocAndRelease<LLBC_MessageBlock>(objPool, std::make_pair(10000, 1000), std::make_pair(500, 1000));
    RandAllocAndRelease<std::stack<int> >(objPool, std::make_pair(10000, 1000), std::make_pair(500, 1000));
    RandAllocAndRelease<std::map<int, std::string> >(objPool, std::make_pair(10000, 1000), std::make_pair(500, 1000));


    LLBC_PrintLn("- Stat(CSV):\n%s", objPool.GetStatistics(LLBC_ObjPoolStatFormat::CSV).c_str());
    LLBC_PrintLn("- Stat(json):\n%s", objPool.GetStatistics(LLBC_ObjPoolStatFormat::Json).c_str());

    return LLBC_OK;
}

int TestCase_Core_ObjPool::ReflectTest()
{
    // Basic type reusable reflection.
    LLBC_PrintLn("Basic type IsReusable() Reflect test:");
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<void>(), LLBC_FAILED);
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<bool>(), LLBC_FAILED);
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<sint8>(), LLBC_FAILED);
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<uint8>(), LLBC_FAILED);
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<sint16>(), LLBC_FAILED);
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<uint16>(), LLBC_FAILED);
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<sint32>(), LLBC_FAILED);
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<uint32>(), LLBC_FAILED);
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<long>(), LLBC_FAILED);
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<ulong>(), LLBC_FAILED);
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<sint64>(), LLBC_FAILED);
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<uint64>(), LLBC_FAILED);
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<float>(), LLBC_FAILED);
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<double>(), LLBC_FAILED);
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<void *>(), LLBC_FAILED);
    LLBC_PrintLn("Success");

    // String type reusable reflection.
    LLBC_PrintLn("String type IsReusable() Reflect test:");
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<LLBC_String>() == false, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<std::string>() == false, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<LLBC_CString>(), LLBC_FAILED);
    LLBC_PrintLn("Success");

    // STL containers reusable reflection.
    LLBC_PrintLn("STL containers IsReusable() Reflect test:");
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<std::vector<int> >() == false, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<std::list<int> >() == false, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<std::queue<int> >(), LLBC_FAILED);
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<std::deque<int> >() == false, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<std::stack<int> >(), LLBC_FAILED);
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<std::set<int> >() == false, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<std::unordered_set<int> >() == false, LLBC_FAILED);
    using _TestMap = std::map<int, int>;
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<_TestMap>() == false, LLBC_FAILED);
    using _TestUnorderedMap = std::unordered_map<int, int>;
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<_TestUnorderedMap>() == false, LLBC_FAILED);
    LLBC_PrintLn("Success");

    // IsReusable method class reflection.
    LLBC_PrintLn("IsReusable method class reflect test:");
    class _HasMethodType_reuse { public: void reuse() { } };
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<_HasMethodType_reuse>() != LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_reuse,
                          LLBC_FAILED);
    class _HasMethodType_Reuse { public: void Reuse() { } };
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<_HasMethodType_Reuse>() != LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_Reuse,
                          LLBC_FAILED);

    class _HasMethodType_reset { public: void reset() { } };
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<_HasMethodType_reset>() != LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_reset,
                          LLBC_FAILED);
    class _HasMethodType_Reset { public: void Reset() { } };
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<_HasMethodType_Reset>() != LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_Reset,
                          LLBC_FAILED);

    class _HasMethodType_clear { public: void clear() { } };
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<_HasMethodType_clear>() != LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_clear,
                          LLBC_FAILED);
    class _HasMethodType_Clear { public: void Clear() { } };
    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<_HasMethodType_Clear>() != LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_Clear,
                          LLBC_FAILED);
    LLBC_PrintLn("Success");

    // Reuse method reflection.
    LLBC_PrintLn("Reuse method reflect test:");
    auto intPtr = new int(3);
    LLBC_ObjReflector::Reuse<int>(intPtr);
    auto mapPtr = new std::map<int, int>;
    LLBC_ObjReflector::Reuse<std::map<int, int> >(mapPtr);

    class _ReusableClass
    {
    public:
        _ReusableClass(): _reuseMethCalled(false) { }

        void Reuse() { _reuseMethCalled = true; }

        bool IsReuseMethodCalled() const { return _reuseMethCalled; }

    private:
        bool _reuseMethCalled;
    };

    auto objPtr = new _ReusableClass;
    if (LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_Reuse)
    {
        LLBC_ObjReflector::Reuse<_ReusableClass>(objPtr);
        LLBC_ErrorAndReturnIf(objPtr->IsReuseMethodCalled() == false, LLBC_FAILED);
    }

    // Note: -Wmismatched-new-delete/-Wmismatched-dealloc only available on GCC 11 or later version
    #if (LLBC_CUR_COMP == LLBC_COMP_GCC && LLBC_COMP_MAJOR_VER >= 11) || LLBC_CUR_COMP == LLBC_COMP_CLANG
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wmismatched-new-delete"
    #endif

    LLBC_ObjReflector::Delete<_ReusableClass>(objPtr);
    free(objPtr);

    #if (LLBC_CUR_COMP == LLBC_COMP_GCC && LLBC_COMP_MAJOR_VER >= 11) || LLBC_CUR_COMP == LLBC_COMP_CLANG
    #pragma GCC diagnostic pop
    #endif

    // GetStripeCapacity && OnTypedObjPoolCreated reflection method test
    LLBC_ObjPool objPool;
    auto testObj = objPool.AcquireGuarded<ReflectMethTest>();
    LLBC_PrintLn("- Reflection method: GetStripeCapacity() called:%d",
                 testObj->methCalled_GetStripeCapacity);
    LLBC_PrintLn("- Reflection method: OnTypedObjPoolCreated() called:%d",
                 testObj->methCalled_OnTypedObjPoolCreated);
    ASSERT(testObj->methCalled_GetStripeCapacity &&
           testObj->methCalled_OnTypedObjPoolCreated);

    LLBC_PrintLn("Success");

    return LLBC_OK;
}

int TestCase_Core_ObjPool::MemoryLeakTest()
{
    // Construct test sequence.
    constexpr bool simpleTest = true;

    LLBC_Random rand;
    const int acquireTimes = 500000;
    const int releaseTimes = acquireTimes * 4 / 5;
    std::vector<int> testSeq;
    testSeq.reserve(acquireTimes);
    for (int i = 0; i < acquireTimes; ++i)
        testSeq.emplace_back(1);
    for (int i = 0; i < releaseTimes; ++i)
        testSeq.emplace_back(2);

    if (!simpleTest)
        rand.Shuffle(testSeq.begin(), testSeq.end());

    // Log.
    const int testTimes = simpleTest ? 10 : 1000;
    LLBC_PrintLn("Memory leak test, simpleTest:%d, testTimes:%d, perTime:[acquireTimes:%d, release:%d]",
                 simpleTest, testTimes, acquireTimes, releaseTimes);

    // Test Acquire/Release.
    std::list<LLBC_String *> strs;
    LLBC_ObjPool objPool;
    for (int i = 0; i < testTimes; ++i)
    {
        for (auto &testOp : testSeq)
        {
            if (testOp == 1)
            {
                auto str = objPool.Acquire<LLBC_String>();
                str->append("hello world");
                strs.push_back(str);
            }
            else if (!strs.empty())
            {
                objPool.Release(*strs.begin());
                strs.erase(strs.begin());
            }
        }

        if (strs.size() > acquireTimes)
        {
            for (int i = 0; i < acquireTimes / 2; ++i)
            {
                objPool.Release(*strs.begin());
                strs.erase(strs.begin());
            }
        }
    }

    // Release unreleased strs.
    for (auto &str : strs)
        objPool.Release(str);

    // Log finish.
    LLBC_PrintLn("Test finish, objpool stat:\n%s",
                 objPool.GetStatistics(LLBC_ObjPoolStatFormat::Json).c_str());

    // Collect objpool.
    objPool.Collect(true);
    LLBC_PrintLn("After collect objpool, stat:\n%s",
                 objPool.GetStatistics(LLBC_ObjPoolStatFormat::Json).c_str());

    return LLBC_OK;
}

int TestCase_Core_ObjPool::MultiThreadThread()
{
    LLBC_PrintLn("Multi thread test:");

    // Define test task.
    class ObjPoolMultiThreadTestTask : public LLBC_Task
    {
    public:
        ObjPoolMultiThreadTestTask(bool threadSafe, size_t needTestTimes)
        : _objPool(threadSafe)
        , _needTestTimes(needTestTimes)
        {
        }

    public:
        void Svc() override
        {
            auto threadId = LLBC_GetCurrentThreadId();
            LLBC_PrintLn("Thread %d begin test objpool...", threadId);

            _testedTimes.SetValue(new size_t(0));
            while (*_testedTimes != _needTestTimes)
            {
                ++*_testedTimes;
                auto stream = _objPool.Acquire<LLBC_Stream>();
                *stream << *_testedTimes;

                _objPool.Release(stream);
            }

            LLBC_PrintLn("Thread %d test finished", threadId);
        }

        void Cleanup() override
        {
            LLBC_PrintLn("All threads test finished, objPool stat:\n%s",
                         _objPool.GetStatistics(LLBC_ObjPoolStatFormat::PrettyJson).c_str());
        }

    public:
        LLBC_ObjPool _objPool;
        volatile size_t _needTestTimes;
        LLBC_Tls<size_t> _testedTimes;
    };

    // Begin test.
    LLBC_PrintLn("- Create multi-thread test task...");
    auto task = new ObjPoolMultiThreadTestTask(true, 1000000);
    LLBC_Defer(delete task);

    LLBC_PrintLn("- Active multi-thread test task...");
    if (task->Activate(10) != LLBC_OK)
    {
        LLBC_FilePrintLn(stderr, "- Active test task failed, err:%s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    LLBC_PrintLn("- Waiting for test task finished...");
    if (task->Wait() != LLBC_OK)
    {
        LLBC_FilePrintLn(stderr, "- Waiting for test task finished failed, err:%s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    LLBC_PrintLn("Multi-thread test success");

    return LLBC_OK;
}

int TestCase_Core_ObjPool::PerfTest()
{
    LLBC_PrintLn("Begin object pool performance test:");

    struct TestObj
    {
        std::string strVal;
        std::vector<double> dblList;
        std::map<int, int> infoMap;
        char buf[1024];

        void Reuse()
        {
            strVal.clear();
            dblList.clear();
            infoMap.clear();
        }
    };

    #if LLBC_DEBUG
    constexpr int testTimes = 1000;
    #else
    constexpr int testTimes = 10000;
    #endif
    constexpr int perTimeAcquireTimes = 100;
    TestObj *objs[perTimeAcquireTimes];

    // Define object operate macro.
    #define __LLBC_Inl_DoSomeThingForObj(obj)        \
        obj->strVal.append("hello world, hey judy"); \
        for (int i = 0; i < 50; ++i)                 \
            obj->dblList.push_back(i);               \
        for (int i = 0; i < 50; ++i)                 \
            obj->infoMap.emplace(i, i * 1000)

    // Exec default new/malloc test.
    LLBC_PrintLn("New/Delete perf test:");
    LLBC_Stopwatch sw;
    for (int i = 0; i < testTimes; ++i)
    {
        for (int j = 0; j < perTimeAcquireTimes; ++j)
        {
            auto obj = new TestObj();
            // __LLBC_Inl_DoSomeThingForObj(obj);
            objs[j] = obj;
        }
    
        for (int j = 0; j < perTimeAcquireTimes; ++j)
        {
            delete objs[j];
        }
    }

    sw.Pause();
    LLBC_PrintLn("New/Delete test fin, cost: %lld us", sw.Elapsed().GetTotalMicros());

    // Execute object pool acquire/release test.
    LLBC_PrintLn("ObjPool New/Delete perf test:");
    LLBC_ObjPool objPool(false);

    objPool.Release(objPool.Acquire<std::vector<int>>());

    // auto testObjPool = objPool.GetTypedObjPool<TestObj>();
    LLBC_Stopwatch sw2;
    for (int i = 0; i < testTimes; ++i)
    {
        for (int j = 0; j < perTimeAcquireTimes; ++j)
        {
            // auto obj = testObjPool->Acquire();
            auto obj = objPool.Acquire<TestObj>();
            // __LLBC_Inl_DoSomeThingForObj(obj);
            objs[j] = obj;
        }

        for (int j = 0; j < perTimeAcquireTimes; ++j)
        {
            objPool.Release(objs[j]);
            // testObjPool->Release(objs[j]);
        }
    }

    sw2.Pause();
    LLBC_PrintLn("ObjPool New/Delete test fin, cost: %lld us, perf improvement: %.3f",
                 sw2.Elapsed().GetTotalMicros(),
                 (sw.ElapsedTicks() - sw2.ElapsedTicks()) / static_cast<double>(sw2.ElapsedTicks()));

    getchar();

    // Undef object operate macro.
    #undef __LLBC_Inl_DoSomeThingForObj

    return LLBC_OK;
}

int TestCase_Core_ObjPool::OrderedDeleteTest()
{
    LLBC_PrintLn("Ordered delete test:");

    // Test empty ordered delete tree:
    LLBC_ObjPool objPool;
    LLBC_PrintLn("- Empty ordered delete tree:%s", objPool.GetOrderedDeleteTree().c_str());

    // Let _OD_A <- _OD_B:
    objPool.EnsureDeletionBefore<_OD_A, _OD_B>();
    LLBC_PrintLn("- Let _OD_A <- _OD_B:\n%s",
                 objPool.GetOrderedDeleteTree(true).c_str());

    // Let _OD_A <- _OD_C:
    objPool.EnsureDeletionBefore<_OD_A, _OD_C>();
    LLBC_PrintLn("- Let _OD_A <- _OD_C:\n%s",
                 objPool.GetOrderedDeleteTree(true).c_str());

    // Let _OD_D <- _OD_E:
    objPool.EnsureDeletionBefore<_OD_D, _OD_E>();
    LLBC_PrintLn("- Let _OD_D <- _OD_E:\n%s",
                 objPool.GetOrderedDeleteTree(true).c_str());

    // Let _OD_E <- _OD_F:
    objPool.EnsureDeletionBefore<_OD_E, _OD_F>();
    LLBC_PrintLn("- Let _OD_E <- _OD_F:\n%s",
                 objPool.GetOrderedDeleteTree(true).c_str());

    // Let _OD_D <- _OD_E:
    objPool.EnsureDeletionBefore<_OD_D, _OD_G>();
    LLBC_PrintLn("- Let _OD_D <- _OD_G:\n%s",
                 objPool.GetOrderedDeleteTree(true).c_str());

    // Let _OD_B <- _OD_E:
    objPool.EnsureDeletionBefore<_OD_B, _OD_E>();
    LLBC_PrintLn("- Let _OD_B <- _OD_E:\n%s",
                 objPool.GetOrderedDeleteTree(true).c_str());

    // Let _OD_C <- _OD_E:
    objPool.EnsureDeletionBefore<_OD_C, _OD_E>();
    LLBC_PrintLn("- Let _OD_C <- _OD_E:\n%s",
                 objPool.GetOrderedDeleteTree(true).c_str());

    // Let _OD_F <- _OD_G:
    objPool.EnsureDeletionBefore<_OD_F, _OD_G>();
    LLBC_PrintLn("- Let _OD_F <- _OD_G:\n%s",
                 objPool.GetOrderedDeleteTree(true).c_str());

    objPool.Release(objPool.Acquire<_OD_A>());
    objPool.Release(objPool.Acquire<_OD_B>());
    objPool.Release(objPool.Acquire<_OD_C>());
    objPool.Release(objPool.Acquire<_OD_D>());
    objPool.Release(objPool.Acquire<_OD_E>());
    objPool.Release(objPool.Acquire<_OD_F>());
    objPool.Release(objPool.Acquire<_OD_G>());

    return LLBC_OK;
}

int TestCase_Core_ObjPool::GuardedPoolObjTest()
{
    LLBC_PrintLn("GuardedPoolObj test:");

    class GuardTestA
    {
    public:
        GuardTestA()
        {
            LLBC_PrintLn("GuardTestA::GuardTestA() called, this:%p", this);
        }

        ~GuardTestA()
        {
            LLBC_PrintLn("GuardTestA::~GuardTestA() called, this:%p", this);
        }

        void Foo()
        {
            LLBC_PrintLn("GuardTestA::Foo() called, this:%p", this);
        }

        void Reuse()
        {
            LLBC_PrintLn("GuardTestA::Reuse() called, this:%p", this);
        }
    };

    LLBC_ObjPool objPool;

    // Basic func test.
    {
        // Alloc guarded pool obj: GuardTestA.
        auto guard1 = objPool.AcquireGuarded<GuardTestA>();
        // Method call test.
        guard1->Foo(); // operator->
        guard1.Get()->Foo(); // Get()

        // Test copy construct.
        auto guard2 = guard1;
        LLBC_ErrorAndReturnIf(guard1 || !guard2, LLBC_FAILED, "GuardedPoolObj copy construct test failed!");
        // guard1->Foo(); // Raise crash.
        // Test move construct.
        auto guard3 = std::move(guard2);
        LLBC_ErrorAndReturnIf(guard2 || !guard3, LLBC_FAILED, "GuardedPoolObj move construct test failed!");
        // guard2->Foo(); // Raise crash.

        // Test copy assignment.
        auto guard4(guard1);
        guard4 = guard3;
        LLBC_ErrorAndReturnIf(guard3 || !guard4, LLBC_FAILED, "GuardedPoolObj copy assignment test failed!");
        // guard3->Foo(); // Raise crash.
        // Test move assignment.
        auto guard5(guard1);
        guard5 = std::move(guard4);
        LLBC_ErrorAndReturnIf(guard4 || !guard5, LLBC_FAILED, "GuardedPoolObj move assignment test failed!");
        // guard4->Foo(); // Raise crash.

        // Test detach.
        auto guard6 = objPool.AcquireGuarded<GuardTestA>();
        auto obj = guard6.Detach();
        LLBC_ErrorAndReturnIf(guard6 || !obj || guard6.GetTypedObjPool(), LLBC_FAILED, "GuardedPoolObj detach test failed!");
        objPool.Release(obj);

        // Test reset.
        auto guard7 = objPool.AcquireGuarded<GuardTestA>();
        guard7.Reset();
        LLBC_ErrorAndReturnIf(guard7, LLBC_FAILED, "GuardedPoolObj reset test failed!");
    }

    // Complex test.
    std::vector <LLBC_GuardedPoolObj<GuardTestA> > guardObjs;
    for (int i = 0; i < 10; ++i)
        guardObjs.push_back(objPool.AcquireGuarded<GuardTestA>());

    return LLBC_OK;
}

template <typename Obj, bool SupportedReuse, bool SupportedReflection>
int _CheckObjPoolSupport()
{
    LLBC_PrintLn("- class: %s", LLBC_GetTypeName(Obj));

    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsReusable<Obj>() != SupportedReuse, LLBC_FAILED);
                          LLBC_PrintLn("  - supported reuse: %s",
                                       LLBC_ObjReflector::IsReusable<Obj>() ? "true" : "false");

    LLBC_ErrorAndReturnIf(LLBC_ObjReflector::IsSupportedObjPoolReflection<Obj>() != SupportedReflection, LLBC_FAILED);
                          LLBC_PrintLn("  - supported reflection: %s",
                                       LLBC_ObjReflector::IsSupportedObjPoolReflection<Obj>() ? "true" : "false");

    return LLBC_OK;
}

int TestCase_Core_ObjPool::LibSupportedObjPoolClassesTest()
{
    LLBC_PrintLn("llbc core library supported object pool classes test:");

    // Reuse: unsupported, Reflection: unsupported.
    if (_CheckObjPoolSupport<LLBC_PacketProtocol, false, false>() != LLBC_OK)
        return LLBC_FAILED;

    // Reuse: supported, Reflection: unsupported.
    if (_CheckObjPoolSupport<LLBC_String, true, false>() != LLBC_OK)
        return LLBC_FAILED;
    if (_CheckObjPoolSupport<LLBC_Heap<int>, false, false>() != LLBC_OK)
        return LLBC_FAILED;

    // Reuse: supported, Reflection: supported.
    if (_CheckObjPoolSupport<LLBC_Event, true, true>() != LLBC_OK)
        return LLBC_FAILED;
    if (_CheckObjPoolSupport<LLBC_Stream, true, true>() != LLBC_OK)
        return LLBC_FAILED;
    if (_CheckObjPoolSupport<LLBC_MessageBlock, true, true>() != LLBC_OK)
        return LLBC_FAILED;
    if (_CheckObjPoolSupport<LLBC_Packet, true, true>() != LLBC_OK)
        return LLBC_FAILED;
    if (_CheckObjPoolSupport<LLBC_LogData, true, true>() != LLBC_OK)
        return LLBC_FAILED;

    return LLBC_OK;
}

int TestCase_Core_ObjPool::CommonClassTest_Stream()
{
    LLBC_PrintLn("CommonClass - Stream test:");

    LLBC_ObjPool objPool;

    // Acquire LLBC_Stream from obj pool.
    auto stream1 = objPool.Acquire<LLBC_Stream>();
    LLBC_PrintLn("- Acquire from objpool, stream:%s, ptr:%p", stream1->ToString().c_str(), stream1);
    #if !LLBC_CFG_CORE_OBJPOOL_USE_MALLOC_INSTEAD
    LLBC_ErrorAndReturnIf(stream1->GetTypedObjPool() != objPool.GetTypedObjPool<LLBC_Stream>(),
                          LLBC_FAILED,
                          "  - Acquired stream object internal error!");
    #endif // !LLBC_CFG_CORE_OBJPOOL_USE_MALLOC_INSTEAD

    // Serialize.
    LLBC_PrintLn("- Serialize some data to stream...");
    *stream1 << "hello world" << 9527 << false;
    LLBC_PrintLn("  - After serialized, stream:%s", stream1->ToString().c_str());

    // Recycle.
    LLBC_PrintLn("- Recycle...");
    LLBC_Recycle(stream1);

    // Acquire new stream.
    auto stream2 = objPool.Acquire<LLBC_Stream>();
    LLBC_PrintLn("- Acquire new stream(stream2): stream2 == stream?: %s, stream2:%s, ptr:%p",
                 stream2 == stream1 ? "true" : "false",
                 stream2->ToString().c_str(),
                 stream2);
    LLBC_ErrorAndReturnIf(stream2 != stream1, LLBC_FAILED, "ObjPool acquire internal error");

    // Acquire new stream too.
    auto stream3 = objPool.Acquire<LLBC_Stream>();
    LLBC_PrintLn("- Acquire new stream again(stream3): stream3 == stream2?: %s, stream3:%s, ptr:%p",
                 stream3 == stream2 ? "true" : "false",
                 stream3->ToString().c_str(),
                 stream3);
    LLBC_ErrorAndReturnIf(stream3 == stream2, LLBC_FAILED, "ObjPool acquire internal error");

    // Recycle stream2 & stream3.
    LLBC_Recycle(stream3);
    LLBC_Recycle(stream2);
    // LLBC_Recycle(stream2); // Raise crash.

    return LLBC_OK;
}

template <typename Obj>
void TestCase_Core_ObjPool::RandAllocAndRelease(LLBC_ObjPool &objPool,
                                                const std::pair<int, int> allocRange,
                                                const std::pair<int, int> &releaseRange)
{
    auto allocTimes = LLBC_Rand(allocRange.first, allocRange.second);
    auto releaseTimes = LLBC_Rand(releaseRange.first, releaseRange.second);
    releaseTimes = std::min(releaseTimes, allocTimes);

    std::vector<Obj *> objs;
    objs.reserve(allocTimes);
    while (--allocTimes >= 0)
        objs.push_back(objPool.Acquire<Obj>());

    while (--releaseTimes >= 0)
        objPool.Release(objs[releaseTimes]);
}

int TestCase_Core_ObjPool::RecycleTest()
{
    // obj from pool recycle test
    LLBC_ObjPool objPool;
    auto* g = objPool.Acquire<_OD_G>();
    LLBC_Recycle(g);

    // obj by new
    // TODO():no way to judge，just test
    auto str = new std::string("obj by new test");
    LLBC_Recycle(str);
    
    // obj as a LLBC_Object
    auto* obj = new _OD_H;
    // obj delegate to auto-gc pool, call this only once
    obj->AutoRelease();

    // add ref
    obj->Retain();
    LLBC_ReturnIf(obj->GetAutoRefCount() != 1, LLBC_FAILED)
    LLBC_ReturnIf(obj->GetRefCount() != 2, LLBC_FAILED)
    // sub ref
    LLBC_Recycle(obj);

    // only for test, frame will call Purge automatically
    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    auto poolStack = reinterpret_cast<LLBC_AutoReleasePoolStack *>(tls->objbaseTls.poolStack);
    poolStack->Purge();

    return LLBC_OK;
}

int TestCase_Core_ObjPool::SafeObjPoolSetNameTest()
{
    const int testTimes = 20;

    LLBC_String expectedName;
    expectedName.format("test_%d", testTimes - 1);

    SafeObjPoolPrintNameTask printNametask(testTimes);

    auto &objPool = printNametask.GetObjPool();
    LLBC_PrintLn("safe-obj-pool: %s", objPool.GetName().c_str());

    printNametask.Activate(8);

    printNametask.Wait();

    auto objPoolFinalName = objPool.GetName();

    LLBC_ErrorAndReturnIf(expectedName != objPoolFinalName, 
                          LLBC_FAILED, 
                          "objPoolFinalName(%s) is not expected(%s)", 
                          objPoolFinalName.c_str(), 
                          expectedName.c_str());

    return LLBC_OK;
}
