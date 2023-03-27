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


#include "core/objectpool/TestCase_Core_ObjectPool.h"

namespace
{
    // Define some test configs.
    #if LLBC_DEBUG
    const int TestTimes = 1000;
    const int ComplexTestTimes = TestTimes / 10;
    const int ListSize = 100;
    #else
    const int TestTimes = 100000;
    const int ComplexTestTimes = TestTimes / 10;
    const int ListSize = 100;
    #endif

    // Ordered delete test classes.
    class OD_A {};
    class OD_X
    {
    public:
        OD_X()
        : _a(nullptr)
        , _pool(nullptr)
        {
        }

        ~OD_X()
        {
            if (_pool && _a)
                _pool->Release(_a);
        }

        void Init(LLBC_UnsafetyObjectPool *pool)
        {
            _pool = pool;
            _a = pool->Get<OD_A>();
        }

    private:
        OD_A *_a;
        LLBC_UnsafetyObjectPool *_pool;
    };
    class OD_Y1 {};
    class OD_Z1 {};
    class OD_Y2 {};
    class OD_Z2 {};
    class OD_B {};
    class OD_I {};
    class OD_J1 {};
    class OD_K1 {};
    class OD_J2 {};
    class OD_K2 {};

    // Define some test classes.
    class TestObj
    {
    public:
        TestObj()
        {
            LLBC_PrintLn("  ->[ptr:0x%p]%s: Called!", this, __FUNCTION__);
        }

        ~TestObj()
        {
            LLBC_PrintLn("  ->[ptr:0x%p]%s: Called!", this, __FUNCTION__);
        }
    };

    class ReflectionableTestObj
    {
    public:
        ReflectionableTestObj()
        : _poolInst(nullptr)
        {
            LLBC_PrintLn("  ->[ptr:0x%p]%s: Called!", this, __FUNCTION__);
        }

        ~ReflectionableTestObj()
        {
            LLBC_PrintLn("  ->[ptr:0x%p]%s: Called!", this, __FUNCTION__);
        }

    public:
        void MarkPoolObject(LLBC_IObjectPoolInst &poolInst)
        {
            LLBC_PrintLn("  ->[ptr:0x%p]%s: Called!", this, __FUNCTION__);
            _poolInst = &poolInst;
        }

        LLBC_IObjectPoolInst *GetPoolInst()
        {
            return _poolInst;
        }

        bool HasBeenGiveBackToPool() const
        {
            return _poolInst == nullptr;
        }

        void OnPoolInstCreate(LLBC_IObjectPoolInst &poolInst)
        {
            LLBC_Print("  ->[ptr:0x%p]%s: Called, pool inst:%p!", this, __FUNCTION__, &poolInst);
        }

        void OnPoolInstDestroy(LLBC_IObjectPoolInst &poolInst)
        {
            LLBC_Print("  ->[ptr:0x%p]%s: Called, pool inst:%p!", this, __FUNCTION__, &poolInst);
        }

        void Clear()
        {
            _poolInst = nullptr;
        }

    private:
        LLBC_IObjectPoolInst *_poolInst;
    };

    class ClearableTestObj
    {
    public:
        ClearableTestObj()
        {
            LLBC_PrintLn("  ->[ptr:0x%p]%s: Called!", this, __FUNCTION__);
        }

        ~ClearableTestObj()
        {
            LLBC_PrintLn("  ->[ptr:0x%p]%s: Called!", this, __FUNCTION__);
        }

    public:
        void Clear()
        {
            LLBC_PrintLn("  ->[ptr:0x%p]%s: Called!", this, __FUNCTION__);
        }
    };

    class ReferencableTestObj : public LLBC_ReferencablePoolObj
    {
    public:
        ReferencableTestObj()
        {
            LLBC_PrintLn("  ->[ptr:0x%p]%s: Called!", this, __FUNCTION__);
        }

        virtual ~ReferencableTestObj()
        {
            LLBC_PrintLn("  ->[ptr:0x%p]%s: Called!", this, __FUNCTION__);
        }

    public:
        void Clear()
        {
            LLBC_PrintLn("  ->[ptr:0x%p]%s: Called!", this, __FUNCTION__);
        }
    };

    class ObjectReflectionBaseTestObj : public LLBC_PoolObject
    {
    public:
        ObjectReflectionBaseTestObj()
        {
            LLBC_PrintLn("  ->[ptr:0x%p]%s: Called!", this, __FUNCTION__);
        }

        virtual ~ObjectReflectionBaseTestObj()
        {
            LLBC_PrintLn("  ->[ptr:0x%p]%s: Called!", this, __FUNCTION__);
        }

        void OnPoolInstCreate(LLBC_IObjectPoolInst &poolInst)
        {
            LLBC_Print("  ->[ptr:0x%p]%s: Called, pool inst:%p!", this, __FUNCTION__, &poolInst);
        }

        void OnPoolInstDestroy(LLBC_IObjectPoolInst &poolInst)
        {
            LLBC_Print("  ->[ptr:0x%p]%s: Called, pool inst:%p!", this, __FUNCTION__, &poolInst);
        }

    public:
        void Clear()
        {
            LLBC_PrintLn("  ->[ptr:0x%p]%s: Called!", this, __FUNCTION__);
        }
    };

    class ComplexObj
    {
    public:
        ComplexObj()
        {
            strdictptr = nullptr;
            strdict2ptr = nullptr;

            buf1 = nullptr;
            buf2 = nullptr;
            buf3 = nullptr;
            buf4 = nullptr;
            buf5 = nullptr;
        }

        ~ComplexObj()
        {
            LLBC_XDelete(strdictptr);
            LLBC_XDelete(strdict2ptr);

            LLBC_XFree(buf1);
            LLBC_XFree(buf2);
            LLBC_XFree(buf3);
            LLBC_XFree(buf4);
            LLBC_XFree(buf5);
        }

        void Clear()
        {
            u64vec.clear();
            strvec.clear();
            strdict.clear();
            if (strdictptr)
                strdictptr->clear();

            strdict2.clear();
            if (strdict2ptr)
                strdict2ptr->clear();
        }

    public:
        std::vector<uint64> u64vec;
        std::vector<LLBC_String> strvec;
        std::map<uint64, LLBC_String> strdict;
        std::map<uint64, LLBC_String> *strdictptr;

        std::map<LLBC_String, LLBC_String> strdict2;
        std::map<LLBC_String, LLBC_String> *strdict2ptr;

        char *buf1;
        char *buf2;
        char *buf3;
        char *buf4;
        char *buf5;
    };

    /**
    * \brief Object pool test task encapsulation.
    */
    class ObjectPoolTestTask : public LLBC_Task
    {
    public:
        ObjectPoolTestTask()
        : _repeatCount(TestTimes)
        , _pool(new LLBC_SafetyObjectPool())
        , _poolInst(_pool->GetPoolInst<std::vector<double> >())
        {
            LLBC_Random rand;
            rand.Seed(static_cast<int>(LLBC_Time::NowTimestampInSecs()));

            for (int i = 0; i < TestTimes; ++i)
            {
                _randTimes[i] = LLBC_Abs(rand.Rand(ListSize));
                _pushElems[i] = LLBC_Abs(rand.Rand(ListSize));
            }
        }

        virtual ~ObjectPoolTestTask() {  }

    public:
        virtual void Svc()
        {
            std::vector<double> *poolObjs[ListSize];

            int runTimes;
            while (true)
            {
                _lock.Lock();
                if (_repeatCount == 0)
                {
                    _lock.Unlock();
                    break;
                }

                runTimes = --_repeatCount;
                _lock.Unlock();

                const int newDelTimes = _randTimes[runTimes];
                for (int j = 0; j < newDelTimes; ++j)
                {
                    // poolObjs[j] = _pool->Get<std::vector<double> >();
                    poolObjs[j] = _poolInst->GetObject();
                    for (int k = 0; k < _pushElems[runTimes]; ++k)
                        poolObjs[j]->push_back(k);
                }

                for (int j = 0; j < newDelTimes; ++j)
                    // _pool->Release(poolObjs[j]);
                    _poolInst->ReleaseObject(poolObjs[j]);
            }
        }

        virtual void Cleanup()
        {
            LLBC_XDelete(_pool);
        }

    private:
        int _randTimes[TestTimes];
        int _pushElems[TestTimes];

        volatile int _repeatCount;

        LLBC_FastLock _lock;
        LLBC_SafetyObjectPool  *_pool;
        LLBC_ObjectPoolInst<std::vector<double> > *_poolInst;
    };
}

TestCase_Core_ObjectPool::TestCase_Core_ObjectPool()
{
}

TestCase_Core_ObjectPool::~TestCase_Core_ObjectPool()
{
}

int TestCase_Core_ObjectPool::Run(int argc, char *argv[])
{
    LLBC_PrintLn("core/objectpool test:");

    DoBasicTest();
    DoStatTest();
    DoOrderedDeleteTest();
    DoConverienceMethodsTest();
    DoPerfTest();
    DoComplexObjPerfTest();
    DoPoolDebugAssetTest();
    DoStringSpecificPoolInstTest();

    LLBC_PrintLn("Press any key to continue ...");
    getchar();

    return 0;
}

void TestCase_Core_ObjectPool::DoBasicTest()
{
    LLBC_PrintLn("Begin object pool basic test:");

    // Test Get/Release
    {
        LLBC_ObjectPool<> pool;

        std::set<int> *set1 = pool.Get<std::set<int> >();
        pool.Release(set1);

        std::vector<int> *vec1 = pool.Get<std::vector<int> >();
        pool.Release(vec1);

        const int testTimes = 10;
        LLBC_PrintLn("LLBC_ObjectPool<>:Get/Release test(test times:%d):", testTimes);
        for (int i = 0; i < testTimes; ++i)
        {
            LLBC_PrintLn("  -> Get TestObj...");
            TestObj *obj = pool.Get<TestObj>();

            LLBC_PrintLn("  -> Release TestObj...");
            pool.Release(obj);
        }

        LLBC_PrintLn("LLBC_ObjectPool<>:Get/Release test(clearable obj)(test times:%d):", testTimes);
        for (int i = 0; i < testTimes; ++i)
        {
            LLBC_PrintLn("  -> Get ClearableTestObj...");
            ClearableTestObj *obj = pool.Get<ClearableTestObj>();

            LLBC_PrintLn("  -> Release ClearableTestObj...");
            pool.Release(obj);
        }

        LLBC_PrintLn("LLBC_ObjectPool<>:GetGuarded test(clearable obj)(test times:%d):", testTimes);
        for (int i = 0; i < testTimes; ++i)
        {
            LLBC_PrintLn("  -> Get(Guarded) ClearableTestObj...");
            LLBC_ObjectGuard<ClearableTestObj> obj = pool.GetGuarded<ClearableTestObj>();
        }
    }

    // Test Reflectionable pool object.
    {
        LLBC_ObjectPool<> pool;

        const int testTimes = 10;
        LLBC_PrintLn("ReflectionTestObj object test(times:%d)", testTimes);
        LLBC_PrintLn("Is <ReflectionableTestObj> supported pool-object reflection:%s", 
                       LLBC_PoolObjectReflection::IsSupportedPoolObjectReflection<ReflectionableTestObj>() ? "True" : "False");

        ReflectionableTestObj *ro = pool.Get<ReflectionableTestObj>();
        LLBC_PrintLn("Reflection method support:IsPoolObject(): %s", LLBC_PoolObjectReflection::IsPoolObject(ro) ? "True" : "False");
        LLBC_PrintLn("Reflection method support:GetPoolInst(): %p", LLBC_PoolObjectReflection::GetPoolInst(ro));
        LLBC_PrintLn("Reflection method support:Recycle():");
        LLBC_PoolObjectReflection::Recycle(ro);

        LLBC_ObjectGuard<LLBC_Packet> pkt = pool.GetGuarded<LLBC_Packet>();
        LLBC_ObjectGuard<LLBC_MessageBlock> block = pool.GetGuarded<LLBC_MessageBlock>();
        pkt->Write("hello world");

        for (int i = 0; i < testTimes; ++i)
        {
            ReflectionableTestObj *obj = pool.Get<ReflectionableTestObj>();
            pool.Release(obj);
        }
    }

    {
        LLBC_ObjectPool<> pool;

        const int testTimes = 10;
        LLBC_PrintLn("ObjectReflectionBaseTestObj object test(times:%d)", testTimes);
        LLBC_PrintLn("Is <ObjectReflectionBaseTestObj supported pool-object reflection:%s",
                       LLBC_PoolObjectReflection::IsSupportedPoolObjectReflection<ObjectReflectionBaseTestObj>() ? "True" : "False");

        ObjectReflectionBaseTestObj *ro = pool.Get<ObjectReflectionBaseTestObj>();
        LLBC_PrintLn("Reflection method support:IsPoolObject(): %s", LLBC_PoolObjectReflection::IsPoolObject(ro) ? "True" : "False");
        LLBC_PrintLn("Reflection method support:GetPoolInst(): %p", LLBC_PoolObjectReflection::GetPoolInst(ro));
        LLBC_PrintLn("Reflection method support:Recycle():");
        LLBC_PoolObjectReflection::Recycle(ro);

        LLBC_ObjectGuard<LLBC_Packet> pkt = pool.GetGuarded<LLBC_Packet>();
        LLBC_ObjectGuard<LLBC_MessageBlock> block = pool.GetGuarded<LLBC_MessageBlock>();
        pkt->Write("hello world");

        for (int i = 0; i < testTimes; ++i)
        {
            ObjectReflectionBaseTestObj *obj = pool.Get<ObjectReflectionBaseTestObj>();
            pool.Release(obj);
        }
    }

    // Test Referencable object
    {
        LLBC_ObjectPool<> pool;

        const int testTimes = 10;
        LLBC_PrintLn("Referencable object test(times: %d)", testTimes);
        for (int i = 0; i < testTimes; ++i)
        {
            ReferencableTestObj *obj = pool.GetReferencable<ReferencableTestObj>();
            LLBC_Release(obj);
        }
    }

    LLBC_PrintLn("Object pool basic test finished");
}

void TestCase_Core_ObjectPool::DoStatTest()
{
    LLBC_PrintLn("Object pool stat test:");
    LLBC_UnsafetyObjectPool ob;
    ob.GetGuarded<char>();
    ob.GetGuarded<sint16>();
    ob.GetGuarded<std::vector<int> >();
    ob.GetGuarded<LLBC_String>();
    ob.GetGuarded<LLBC_Stream>();
    ob.GetGuarded<LLBC_Packet>();

    LLBC_ObjectPoolStat stat;
    ob.Stat(stat);

    LLBC_PrintLn("After stat(use 2 shift spaces):\n%s", stat.ToString(2).c_str());
}

void TestCase_Core_ObjectPool::DoOrderedDeleteTest()
{
    LLBC_PrintLn("Do ordered delete test(Please use debug tools too check test result):");

    LLBC_PrintLn("Test case 1: <FrontNode == nullptr && BackNode == nullptr>:");
    {
        LLBC_UnsafetyObjectPool pool;
        pool.AcquireOrderedDeletePoolInst<OD_A, OD_B>();
    }

    LLBC_PrintLn("Test case 2: <FrontNode != nullptr && BackNode == nullptr>:");
    {
        {
            // Before:
            //     OD_A --> OD_B
            // Let:
            //     OD_B --> OD_I
            // After:
            //     OD_A --> OD_B --> OD_I
            LLBC_UnsafetyObjectPool pool;
            pool.AcquireOrderedDeletePoolInst<OD_A, OD_B>();
            pool.AcquireOrderedDeletePoolInst<OD_B, OD_I>();
        }

        {
            // Before:
            //    OD_A --> OD_B
            // Let:
            //    OD_A --> OD_I
            // After:
            //    OD_A --> OD_B
            //        |--> OD_I
            LLBC_UnsafetyObjectPool pool;
            pool.AcquireOrderedDeletePoolInst<OD_A, OD_B>();
            pool.AcquireOrderedDeletePoolInst<OD_A, OD_I>();
        }
    }

    LLBC_PrintLn("Test case 3: <FrontNode == nullptr && BackNode != nullptr>:");
    {
        {
            // Before:
            //    OD_B --> OD_X
            // Let:
            //    OD_A --> OD_B
            // After:
            //   OD_A --> OD_B -->OD_X
            LLBC_UnsafetyObjectPool pool;
            pool.AcquireOrderedDeletePoolInst<OD_B, OD_X>();
            pool.AcquireOrderedDeletePoolInst<OD_A, OD_B>();
        }

        {
            // Before:
            //    OD_B --> OD_X
            // Let:
            //    OD_A --> OD_X
            // After:
            //   OD_B --> OD_A -->OD_X
            LLBC_UnsafetyObjectPool pool;
            pool.AcquireOrderedDeletePoolInst<OD_B, OD_X>();
            pool.AcquireOrderedDeletePoolInst<OD_A, OD_X>();
        }
    }

    LLBC_PrintLn("Test 4: <FrontNode != nullptr && BackNode != nullptr>:");
    {
        // Before:
        //    A --> X --> Y1 --> Z1
        //    B --> I --> J1 --> K1
        // Let:
        //    X --> I
        // After:
        //    A --> X --> B -- >I --> Y1 --> Z1
        //                       |--> J1 --> K1

        LLBC_UnsafetyObjectPool pool;
        // Build: A --> X --> Y1 --> Z1
        pool.AcquireOrderedDeletePoolInst<OD_Y1, OD_Z1>();
        pool.AcquireOrderedDeletePoolInst<OD_X, OD_Y1>();
        pool.AcquireOrderedDeletePoolInst<OD_A, OD_X>();
        // Build: B --> I --> J1 --> K1
        pool.AcquireOrderedDeletePoolInst<OD_J1, OD_K1>();
        pool.AcquireOrderedDeletePoolInst<OD_I, OD_J1>();
        pool.AcquireOrderedDeletePoolInst<OD_B, OD_I>();
        // Let: X --> I
        pool.AcquireOrderedDeletePoolInst<OD_X, OD_I>();
    }

    LLBC_PrintLn("Test 5: <FrontNode != nullptr && BackNode != nullptr>(has same parent node):");
    {
        // Before:
        //    A --> X --> Y1 --> Z1
        //     |--> I --> J1 --> K1
        // Let:
        //    X --> I
        // After:
        //    A --> X -- >I --> Y1 --> Z1
        //                 |--> J1 --> K1

        LLBC_UnsafetyObjectPool pool;
        // Build A --> X
        pool.AcquireOrderedDeletePoolInst<OD_A, OD_X>();
        // Build A --> I
        pool.AcquireOrderedDeletePoolInst<OD_A, OD_I>();

        // Build: X --> Y1 --> Z1
        pool.AcquireOrderedDeletePoolInst<OD_Y1, OD_Z1>();
        pool.AcquireOrderedDeletePoolInst<OD_X, OD_Y1>();
        // Build:  I --> J1 --> K1
        pool.AcquireOrderedDeletePoolInst<OD_J1, OD_K1>();
        pool.AcquireOrderedDeletePoolInst<OD_I, OD_J1>();

        // Let: X --> I
        pool.AcquireOrderedDeletePoolInst<OD_X, OD_I>();
    }

    LLBC_PrintLn("Test 6: Real ordered delete test:");
    {
        // X --> A

        LLBC_UnsafetyObjectPool pool;
        pool.AcquireOrderedDeletePoolInst<OD_X, OD_A>();
        OD_X *x = pool.Get<OD_X>();
        x->Init(&pool);
    }
}

void TestCase_Core_ObjectPool::DoConverienceMethodsTest()
{
    LLBC_PrintLn("Begin object pool converience methods test:");

    typedef std::map<int, std::string> _TestType;

    _TestType *obj1 = LLBC_GetObjectFromSafetyPool<_TestType>();
    LLBC_PrintLn("Get object from safety object-pool: 0x%p, do some operations...", obj1);
    obj1->insert(std::make_pair(1, "Hello world!"));
    obj1->insert(std::make_pair(2, "Hey, Judy!"));
    LLBC_ReleaseObjectToSafetyPool(obj1);
    LLBC_PrintLn("Release object to safety object-pool");

    _TestType *obj2 = LLBC_GetObjectFromUnsafetyPool<_TestType>();
    LLBC_PrintLn("Get object from unsafety object-pool 0x%p, do some operations...", obj2);
    obj2->insert(std::make_pair(3, "Hello world!"));
    obj2->insert(std::make_pair(4, "Hey, Judy!"));
    LLBC_ReleaseObjectToUnsafetyPool(obj2);
    LLBC_PrintLn("Release object to unafety object-pool");

    ReferencableTestObj *refObj = LLBC_GetReferencableObjectFromPool<ReferencableTestObj>();
    LLBC_PrintLn("Get referencable-object from object-pool(unsafety) 0x%p, do some operations...", refObj);
    refObj->Retain();
    refObj->Release();
    refObj->Release();

    LLBC_SafetyObjectPool objPool1;
    LLBC_UnsafetyObjectPool objPool2;
    #if LLBC_DEBUG
    int perfTestTimes = 1000;
    int perTestPerTimeLoopTimes = 100;
    #else
    int perfTestTimes = 100000;
    int perTestPerTimeLoopTimes = 100;
    #endif
    LLBC_PrintLn("Exec performance compare test(Converience methods performance <<>> local object pools), "
                   "test times:%d, per-time loop times:%d", perfTestTimes, perTestPerTimeLoopTimes);
    LLBC_Time begTestTime = LLBC_Time::Now();
    for (int i = 0; i < perfTestTimes; ++i)
    {
        _TestType *obj1 = LLBC_GetObjectFromSafetyPool<_TestType>();
        _TestType *obj2 = LLBC_GetObjectFromUnsafetyPool<_TestType>();
        for (int j = 0; j < perTestPerTimeLoopTimes; ++j)
        {
            obj1->insert(std::make_pair(j, "Hello, World"));
            obj2->insert(std::make_pair(j + perTestPerTimeLoopTimes, "Hello, World"));
        }

        LLBC_ReleaseObjectToSafetyPool(obj1);
        LLBC_ReleaseObjectToUnsafetyPool(obj2);
    }
    LLBC_PrintLn("Converience methods test used time(ms): %lld", (LLBC_Time::Now() - begTestTime).GetTotalMilliSeconds());

    begTestTime = LLBC_Time::Now();
    objPool1.Release(objPool1.Get<_TestType>());
    objPool2.Release(objPool2.Get<_TestType>());
    for (int i = 0; i < perfTestTimes; ++i)
    {
        _TestType *obj1 = objPool1.Get<_TestType>();
        _TestType *obj2 = objPool2.Get<_TestType>();
        for (int j = 0; j < perTestPerTimeLoopTimes; ++j)
        {
            obj1->insert(std::make_pair(j, "Hello, World"));
            obj2->insert(std::make_pair(j + perTestPerTimeLoopTimes, "Hello, World"));
        }

        objPool1.Release(obj1);
        objPool2.Release(obj2);
    }
    LLBC_PrintLn("Local object pools test used time(ms): %lld", (LLBC_Time::Now() - begTestTime).GetTotalMilliSeconds());

    LLBC_PrintLn("Object pool converience methods test finished");
}

void TestCase_Core_ObjectPool::DoPerfTest()
{
    LLBC_PrintLn("Begin object pool performance test:");

    LLBC_UnsafetyObjectPool pool;
    std::vector<double> *poolObjs[ListSize];
    std::vector<double> *mallocObjs[ListSize];

    LLBC_Random rand;
    rand.Seed(static_cast<int>(LLBC_Time::NowTimestampInSecs()));

    int randTimes[TestTimes];
    int pushElems[TestTimes];
    for (int i = 0; i < TestTimes; ++i)
    {
        randTimes[i] = LLBC_Abs(rand.Rand(ListSize));
        pushElems[i] = LLBC_Abs(rand.Rand(ListSize));
    }

    // New/Delete test.
    LLBC_PrintLn("Test new/delete ...");
    LLBC_Time begTime = LLBC_Time::Now();
    for (int i = 0; i < TestTimes; ++i)
    {
        const int &newDelTimes = randTimes[i];
        for (int j = 0; j < newDelTimes; ++j)
        {
            mallocObjs[j] = new std::vector<double>();
            for (int k = 0; k < pushElems[i]; ++k)
                mallocObjs[j]->push_back(k);
        }

        for (int j = 0; j < newDelTimes; ++j)
            delete mallocObjs[j];
    }

    LLBC_TimeSpan usedTime = LLBC_Time::Now() - begTime;
    LLBC_PrintLn("New/delete test finished, used time: %lld", usedTime.GetTotalMicroSeconds());

    // Thread pool test.
    LLBC_PrintLn("Test pool Get/Release ...");
    begTime = LLBC_Time::Now();
    LLBC_ObjectPoolInst<std::vector<double> > *poolInst = pool.GetPoolInst<std::vector<double> >();
    for (int i = 0; i < TestTimes; ++i)
    {
        const int &newDelTimes = randTimes[i];
        for (int j = 0; j < newDelTimes; ++j)
        {
            // poolObjs[j] = pool.Get<std::vector<double> >();
            poolObjs[j] = poolInst->GetObject();
            for (int k = 0; k < pushElems[i]; ++k)
                poolObjs[j]->push_back(k);
        }
        for (int j = 0; j < newDelTimes; ++j)
            // pool.Release(poolObjs[j]);
            poolInst->ReleaseObject(poolObjs[j]);
    }
    usedTime = LLBC_Time::Now() - begTime;
    LLBC_PrintLn("Pool Get/Release test finished, used time: %lld", usedTime.GetTotalMicroSeconds());

    // Multithread test.
    LLBC_PrintLn("Test multiThread pool Get/Release ...");
    begTime = LLBC_Time::Now();

    const int threadNums = 4;
    ObjectPoolTestTask *task = new ObjectPoolTestTask;
    task->Activate(threadNums);
    task->Wait();
    delete task;

    usedTime = LLBC_Time::Now() - begTime;
    LLBC_PrintLn("MultiThread pool Get/Release test finished, used time: %lld, thread nums: %d", usedTime.GetTotalMicroSeconds(), threadNums);

    // Guarded object test.
    LLBC_PrintLn("Guarded object test(please debug it)...");
    {
        LLBC_ObjectGuard<std::vector<int> > obj = pool.GetGuarded<std::vector<int> >();

        obj->push_back(3);
        obj->push_back(4);
        std::vector<int> &derefObj = *obj;
        derefObj.clear();
        derefObj.push_back(5);

        LLBC_ObjectGuard<std::vector<double> > obj2 = poolInst->GetGuarded();
        obj2->push_back(30.0);
        obj2->push_back(40.0);
    }

    LLBC_PrintLn("Guarded object test finished");

    LLBC_PrintLn("Object pool performance test finished");
}

void TestCase_Core_ObjectPool::DoComplexObjPerfTest()
{
    LLBC_PrintLn("Begin complex object<ComplexObj> pool performance test:");

    LLBC_UnsafetyObjectPool pool;
    ComplexObj *poolObjs[ListSize];
    ComplexObj *mallocObjs[ListSize];

    LLBC_Random rand;
    rand.Seed(static_cast<int>(LLBC_Time::NowTimestampInSecs()));

    int randTimes[ComplexTestTimes];
    int pushElems[ComplexTestTimes];
    for (int i = 0; i < ComplexTestTimes; ++i)
    {
        randTimes[i] = LLBC_Abs(rand.Rand(ListSize));
        pushElems[i] = LLBC_Abs(rand.Rand(ListSize));
    }

    // New/Delete test.
    LLBC_PrintLn("Test new/delete ...");
    LLBC_Time begTime = LLBC_Time::Now();
    for (int i = 0; i < ComplexTestTimes; ++i)
    {
        const int &newDelTimes = randTimes[i];
        for (int j = 0; j < newDelTimes; ++j)
        {
            ComplexObj *co = new ComplexObj();
            co->strdictptr = new std::map<uint64, LLBC_String>;
            co->strdict2ptr = new std::map<LLBC_String, LLBC_String>;
            co->buf1 = LLBC_Malloc(char, newDelTimes);
            co->buf2 = LLBC_Malloc(char, newDelTimes * 2);
            co->buf3 = LLBC_Malloc(char, newDelTimes * 3);
            co->buf4 = LLBC_Malloc(char, newDelTimes * 4);
            co->buf5 = LLBC_Malloc(char, newDelTimes * 5);
            mallocObjs[j] = co;

            for (int k = 0; k < pushElems[i]; ++k)
            {
                co->u64vec.push_back(k);
            }
        }

        for (int j = 0; j < newDelTimes; ++j)
            delete mallocObjs[j];
    }

    LLBC_TimeSpan usedTime = LLBC_Time::Now() - begTime;
    LLBC_PrintLn("New/delete test finished, used time: %lld", usedTime.GetTotalMicroSeconds());

    // Thread pool test.
    LLBC_PrintLn("Test pool Get/Release ...");
    begTime = LLBC_Time::Now();
    LLBC_ObjectPoolInst<ComplexObj> *poolInst = pool.GetPoolInst<ComplexObj>();
    for (int i = 0; i < ComplexTestTimes; ++i)
    {
        const int &newDelTimes = randTimes[i];
        for (int j = 0; j < newDelTimes; ++j)
        {
            // ComplexObj *co = pool.Get<ComplexObj>();
            ComplexObj *co = poolInst->GetObject();
            if (!co->strdictptr)
                co->strdictptr = new std::map<uint64, LLBC_String>;
            if (!co->strdict2ptr)
                co->strdict2ptr = new std::map<LLBC_String, LLBC_String>;
            if (!co->buf1)
                co->buf1 = LLBC_Malloc(char, newDelTimes);
            if (!co->buf2)
                co->buf2 = LLBC_Malloc(char, newDelTimes * 2);
            if (!co->buf3)
            co->buf3 = LLBC_Malloc(char, newDelTimes * 3);
            if (!co->buf4)
            co->buf4 = LLBC_Malloc(char, newDelTimes * 4);
            if (!co->buf5)
            co->buf5 = LLBC_Malloc(char, newDelTimes * 5);
            poolObjs[j] = co;

            for (int k = 0; k < pushElems[i]; ++k)
            {
                co->u64vec.push_back(k);
            }
        }
        for (int j = 0; j < newDelTimes; ++j)
            // pool.Release(poolObjs[j]);
            poolInst->Release(poolObjs[j]);
    }
    usedTime = LLBC_Time::Now() - begTime;
    LLBC_PrintLn("Pool Get/Release test finished, used time: %lld", usedTime.GetTotalMicroSeconds());

    LLBC_PrintLn("Object pool performance test finished");

}

void TestCase_Core_ObjectPool::DoPoolDebugAssetTest()
{
#if !LLBC_CFG_CORE_OBJECT_POOL_DEBUG
    LLBC_PrintLn("Not enabled pool debug option<LLBC_CFG_CORE_OBJECT_POOL_DEBUG>, please enable it!");
    return;
#endif

    LLBC_SafetyObjectPool pool;
    pool.Release(pool.Get<LLBC_Packet>());

    // Test release null pointer.
    {
        // pool.Release<LLBC_Packet>(nullptr);
    }

    // Test repeat release.
    {
        // LLBC_Packet *pkt = pool.Get<LLBC_Packet>();
        // pool.Release(pkt);
        // pool.Release(pkt);
    }
}

void TestCase_Core_ObjectPool::DoStringSpecificPoolInstTest()
{
    LLBC_PrintLn("\nDoStringSpecificPoolInstTest begin...");
    LLBC_UnsafetyObjectPool pool;

    LLBC_ObjectPoolInst<std::string> *stdStringPoolInst = pool.GetPoolInst<std::string>();
    //LLBC_PrintLn("std::string pool inst ptr %p  %p", stdStringPoolInst, pool.GetStdStringPoolInst());
    std::string *stdString = stdStringPoolInst->GetObject();
    stdString->assign("std::string specific test...");
    LLBC_PrintLn("%s", stdString->c_str());
    stdStringPoolInst->Release(stdString);

    LLBC_ObjectPoolInst<LLBC_String> *llbcStringPoolInst = pool.GetPoolInst<LLBC_String>();
    //LLBC_PrintLn("std::string pool inst ptr %p  %p", llbcStringPoolInst, pool.GetLLBCStringPoolInst());
    LLBC_String *llbcString = llbcStringPoolInst->GetObject();
    llbcString->assign("LLBC_String specific test...");
    LLBC_PrintLn("%s", llbcString->c_str());
    llbcStringPoolInst->Release(llbcString);

    LLBC_PrintLn("DoStringSpecificPoolInstTest end...\n");
}
