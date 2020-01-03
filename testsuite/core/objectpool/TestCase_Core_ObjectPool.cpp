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
    const int ListSize = 100;
    #else
    const int TestTimes = 100000;
    const int ListSize = 100;
    #endif

    // Ordered delete test classes.
    class OD_A {};
    class OD_X
    {
    public:
        OD_X()
        : _a(NULL)
        , _pool(NULL)
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
            LLBC_PrintLine("  ->[ptr:0x%08p]%s: Called!", this, __FUNCTION__);
        }

        ~TestObj()
        {
            LLBC_PrintLine("  ->[ptr:0x%08p]%s: Called!", this, __FUNCTION__);
        }
    };

    class MarkableTestObj
    {
    public:
        MarkableTestObj()
        {
            LLBC_PrintLine("  ->[ptr:0x%08p]%s: Called!", this, __FUNCTION__);
        }

        ~MarkableTestObj()
        {
            LLBC_PrintLine("  ->[ptr:0x%08p]%s: Called!", this, __FUNCTION__);
        }

    public:
        void MarkPoolObject()
        {
            LLBC_PrintLine("  ->[ptr:0x%08p]%s: Called!", this, __FUNCTION__);
        }
    };

    class ClearableTestObj
    {
    public:
        ClearableTestObj()
        {
            LLBC_PrintLine("  ->[ptr:0x%08p]%s: Called!", this, __FUNCTION__);
        }

        ~ClearableTestObj()
        {
            LLBC_PrintLine("  ->[ptr:0x%08p]%s: Called!", this, __FUNCTION__);
        }

    public:
        void Clear()
        {
            LLBC_PrintLine("  ->[ptr:0x%08p]%s: Called!", this, __FUNCTION__);
        }
    };

    class ReferencableTestObj : public LLBC_ReferencablePoolObj
    {
    public:
        ReferencableTestObj()
        {
            LLBC_PrintLine("  ->[ptr:0x%08p]%s: Called!", this, __FUNCTION__);
        }

        virtual ~ReferencableTestObj()
        {
            LLBC_PrintLine("  ->[ptr:0x%08p]%s: Called!", this, __FUNCTION__);
        }

    public:
        void Clear()
        {
            LLBC_PrintLine("  ->[ptr:0x%08p]%s: Called!", this, __FUNCTION__);
        }
    };

    /**
    * \brief Object pool test task encapsulation.
    */
    class ObjectPoolTestTask : public LLBC_BaseTask
    {
    public:
        ObjectPoolTestTask()
        : _repeatCount(TestTimes)
        , _pool(new LLBC_SafetyObjectPool())
        , _poolInst(_pool->GetPoolInst<std::vector<double> >())
        {
            LLBC_Random rand;
            rand.Seed(static_cast<int>(LLBC_Time::NowTimeStamp()));

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
        LLBC_ObjectPoolInst<std::vector<double>, LLBC_SpinLock> *_poolInst;
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
    LLBC_PrintLine("core/objectpool test:");

    DoBasicTest();
    DoOrderedDeleteTest();
    DoConverienceMethodsTest();
    DoPrefTest();

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return 0;
}

void TestCase_Core_ObjectPool::DoBasicTest()
{
    LLBC_PrintLine("Begin object pool basic test:");

    // Test Get/Release
    {
        LLBC_ObjectPool<> pool;

        const int testTimes = 10;
        LLBC_PrintLine("LLBC_ObjectPool<>:Get/Release test(test times:%d):", testTimes);
        for (int i = 0; i < testTimes; ++i)
        {
            LLBC_PrintLine("  -> Get TestObj...");
            TestObj *obj = pool.Get<TestObj>();

            LLBC_PrintLine("  -> Release TestObj...");
            pool.Release(obj);
        }

        LLBC_PrintLine("LLBC_ObjectPool<>:Get/Release test(clearable obj)(test times:%d):", testTimes);
        for (int i = 0; i < testTimes; ++i)
        {
            LLBC_PrintLine("  -> Get ClearableTestObj...");
            ClearableTestObj *obj = pool.Get<ClearableTestObj>();

            LLBC_PrintLine("  -> Release ClearableTestObj...");
            pool.Release(obj);
        }

        LLBC_PrintLine("LLBC_ObjectPool<>:GetGuarded test(clearable obj)(test times:%d):", testTimes);
        for (int i = 0; i < testTimes; ++i)
        {
            LLBC_PrintLine("  -> Get(Guarded) ClearableTestObj...");
            LLBC_ObjectGuard<ClearableTestObj> obj = pool.GetGuarded<ClearableTestObj>();
        }
    }

    // Test Markable pool object.
    {
        LLBC_ObjectPool<> pool;

        const int testTimes = 10;
        LLBC_PrintLine("Markable object test(times:%d)", testTimes);
        for (int i = 0; i < testTimes; ++i)
        {
            MarkableTestObj *obj = pool.Get<MarkableTestObj>();
            pool.Release(obj);
        }
    }

    // Test Referencable object
    {
        LLBC_ObjectPool<> pool;

        const int testTimes = 10;
        LLBC_PrintLine("Referencable object test(times: %d)", testTimes);
        for (int i = 0; i < testTimes; ++i)
        {
            ReferencableTestObj *obj = pool.GetReferencable<ReferencableTestObj>();
            LLBC_Release(obj);
        }
    }

    LLBC_PrintLine("Object pool basic test finished");
}

void TestCase_Core_ObjectPool::DoOrderedDeleteTest()
{
    LLBC_PrintLine("Do ordered delete test(Please use debug tools too check test result):");

    LLBC_PrintLine("Test case 1: <FrontNode == NULL && BackNode == NULL>:");
    {
        LLBC_UnsafetyObjectPool pool;
        pool.AcquireOrderedDeletePoolInst<OD_A, OD_B>();
    }

    LLBC_PrintLine("Test case 2: <FrontNode != NULL && BackNode == NULL>:");
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

    LLBC_PrintLine("Test case 3: <FrontNode == NULL && BackNode != NULL>:");
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

    LLBC_PrintLine("Test 4: <FrontNode != NULL && BackNode != NULL>:");
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

    LLBC_PrintLine("Test 5: Real ordered delete test:");
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
    LLBC_PrintLine("Begin object pool converience methods test:");

    typedef std::map<int, std::string> _TestType;

    _TestType *obj1 = LLBC_GetObjectFromSafetyPool<_TestType>();
    LLBC_PrintLine("Get object from safety object-pool: 0x%08x, do some operations...", obj1);
    obj1->insert(std::make_pair(1, "Hello world!"));
    obj1->insert(std::make_pair(2, "Hey, Judy!"));
    LLBC_ReleaseObjectToSafetyPool(obj1);
    LLBC_PrintLine("Release object to safety object-pool");

    _TestType *obj2 = LLBC_GetObjectFromUnsafetyPool<_TestType>();
    LLBC_PrintLine("Get object from unsafety object-pool 0x%08x, do some operations...", obj2);
    obj2->insert(std::make_pair(3, "Hello world!"));
    obj2->insert(std::make_pair(4, "Hey, Judy!"));
    LLBC_ReleaseObjectToUnsafetyPool(obj2);
    LLBC_PrintLine("Release object to unafety object-pool");

    ReferencableTestObj *refObj = LLBC_GetReferencableObjectFromPool<ReferencableTestObj>();
    LLBC_PrintLine("Get referencable-object from object-pool(unsafety) 0x%08x, do some operations...", refObj);
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
    LLBC_PrintLine("Exec performance compare test(Converience methods performance <<>> local object pools), "
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
    LLBC_PrintLine("Converience methods test used time(ms): %lld", (LLBC_Time::Now() - begTestTime).GetTotalMilliSeconds());

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
    LLBC_PrintLine("Local object pools test used time(ms): %lld", (LLBC_Time::Now() - begTestTime).GetTotalMilliSeconds());

    LLBC_PrintLine("Object pool converience methods test finished");
}

void TestCase_Core_ObjectPool::DoPrefTest()
{
    LLBC_PrintLine("Begin object pool performance test:");

    LLBC_UnsafetyObjectPool pool;
    std::vector<double> *poolObjs[ListSize];
    std::vector<double> *mallocObjs[ListSize];

    LLBC_Random rand;
    rand.Seed(static_cast<int>(LLBC_Time::Now().GetTimeStamp()));

    int randTimes[TestTimes];
    int pushElems[TestTimes];
    for (int i = 0; i < TestTimes; ++i)
    {
        randTimes[i] = LLBC_Abs(rand.Rand(ListSize));
        pushElems[i] = LLBC_Abs(rand.Rand(ListSize));
    }

    // New/Delete test.
    LLBC_PrintLine("Test new/delete ...");
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
    LLBC_PrintLine("New/delete test finished, used time: %lld", usedTime.GetTotalMicroSeconds());

    // Thread pool test.
    LLBC_PrintLine("Test pool Get/Release ...");
    begTime = LLBC_Time::Now();
    LLBC_ObjectPoolInst<std::vector<double> > *poolInst = pool.GetPoolInst<std::vector<double> >();
    for (int i = 0; i < TestTimes; ++i)
    {
        const int &newDelTimes = randTimes[i];
        for (int j = 0; j < newDelTimes; ++j)
        {
            poolObjs[j] = pool.Get<std::vector<double> >();
            // poolObjs[j] = poolInst->GetObject();
            for (int k = 0; k < pushElems[i]; ++k)
                poolObjs[j]->push_back(k);
        }
        for (int j = 0; j < newDelTimes; ++j)
            pool.Release(poolObjs[j]);
            // poolInst->ReleaseObject(poolObjs[j]);
    }
    usedTime = LLBC_Time::Now() - begTime;
    LLBC_PrintLine("Pool Get/Release test finished, used time: %lld", usedTime.GetTotalMicroSeconds());

    // Multithread test.
    LLBC_PrintLine("Test multiThread pool Get/Release ...");
    begTime = LLBC_Time::Now();

    const int threadNums = 4;
    ObjectPoolTestTask *task = LLBC_New0(ObjectPoolTestTask);
    task->Activate(threadNums);
    task->Wait();
    LLBC_Delete(task);

    usedTime = LLBC_Time::Now() - begTime;
    LLBC_PrintLine("MultiThread pool Get/Release test finished, used time: %lld, thread nums: %d", usedTime.GetTotalMicroSeconds(), threadNums);

    // Guarded object test.
    LLBC_PrintLine("Guarded object test(please debug it)...");
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

    LLBC_PrintLine("Guarded object test finished");

    LLBC_PrintLine("Object pool performance test finished");
}
