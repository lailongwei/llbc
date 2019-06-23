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
    const int TestTimes = 100000;
    const int ListSize = 100;

    /**
    * \brief Object pool test task encapsulation.
    */
    class ObjectPoolTestTask : public LLBC_BaseTask
    {
    public:
        ObjectPoolTestTask()
        : m_repeatCount(TestTimes)
        , _pool(new LLBC_ObjectPool<LLBC_SpinLock>())
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
                if (m_repeatCount == 0)
                {
                    _lock.Unlock();
                    break;
                }

                runTimes = --m_repeatCount;
                _lock.Unlock();

                const int &newDelTimes = _randTimes[runTimes];
                for (int j = 0; j < newDelTimes; ++j)
                {
                    poolObjs[j] = _poolInst->GetObject();
                    //poolObjs[j] = _pool->Get<std::vector<double> >();
                    for (int k = 0; k < _pushElems[runTimes]; ++k)
                        poolObjs[j]->push_back(k);
                }

                for (int j = 0; j < newDelTimes; ++j)
                    //_pool->Release(poolObjs[j]);
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

        int m_repeatCount;

        LLBC_SpinLock _lock;
        LLBC_ObjectPool<LLBC_SpinLock> *_pool;
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

    LLBC_ThreadObjectPool pool;
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

    LLBC_PrintLine("Test pool Get/Release ...");
    begTime = LLBC_Time::Now();
    LLBC_ObjectPoolInst<std::vector<double> > *poolInst = pool.GetPoolInst<std::vector<double> >();
    for (int i = 0; i < TestTimes; ++i)
    {
        const int &newDelTimes = randTimes[i];
        for (int j = 0; j < newDelTimes; ++j)
        {
            poolObjs[j] = poolInst->GetObject();
            for (int k = 0; k < pushElems[i]; ++k)
                poolObjs[j]->push_back(k);
        }
        for (int j = 0; j < newDelTimes; ++j)
            poolInst->ReleaseObject(poolObjs[j]);
    }
    usedTime = LLBC_Time::Now() - begTime;
    LLBC_PrintLine("Pool Get/Release test finished, used time: %lld", usedTime.GetTotalMicroSeconds());

    LLBC_PrintLine("Test multiThread pool Get/Release ...");
    begTime = LLBC_Time::Now();

    const int threadNums = 4;
    ObjectPoolTestTask *task = LLBC_New0(ObjectPoolTestTask);
    task->Activate(threadNums);
    task->Wait();
    LLBC_Delete(task);

    usedTime = LLBC_Time::Now() - begTime;
    LLBC_PrintLine("MultiThread pool Get/Release test finished, used time: %lld, thread nums: %d", usedTime.GetTotalMicroSeconds(), threadNums);

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return 0;
}