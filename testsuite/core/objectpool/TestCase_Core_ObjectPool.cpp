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
    const int listSize = 100;
    std::vector<double> *poolObjs[listSize];
    std::vector<double> *mallocObjs[listSize];

    LLBC_Random rand;
    rand.Seed(LLBC_Time::Now().GetTimeTick());

    const int testTimes = 100000;
    int randTimes[testTimes];
    int pushElems[testTimes];
    for (int i = 0; i < testTimes; ++i)
    {
        randTimes[i] = LLBC_Abs(rand.Rand(listSize));
        pushElems[i] = LLBC_Abs(rand.Rand(listSize));
    }

    LLBC_PrintLine("Test new/delete ...");
    LLBC_Time begTime = LLBC_Time::Now();
    for (int i = 0; i < testTimes; ++i)
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
    for (int i = 0; i < testTimes; ++i)
    {
        const int &newDelTimes = randTimes[i];
        for (int j = 0; j < newDelTimes; ++j)
        {
            poolObjs[j] = pool.Get<std::vector<double> >();
            for (int k = 0; k < pushElems[i]; ++k)
                poolObjs[j]->push_back(k);
        }
        for (int j = 0; j < newDelTimes; ++j)
            pool.Release(poolObjs[j]);
    }
    usedTime = LLBC_Time::Now() - begTime;
    LLBC_PrintLine("Pool Get/Release test finished, used time: %lld", usedTime.GetTotalMicroSeconds());

    //LLBC_ObjectPool<EmptyLock, EmptyLock> pool;
    //const int LstCnt = 100000;
    //std::vector<int*>* lst[LstCnt];
    //std::vector<int*>* lst1[LstCnt];

    //const sint64 beginT = LLBC_Time::Now().GetTimeTick();
    //for (int i = 0; i < LstCnt; i++)
    //    lst[i] = pool.Get<std::vector<int*> >();

    //const sint64 endT = LLBC_Time::Now().GetTimeTick();
    //LLBC_PrintLine("Get %d objects from object pool cost time [%llu].", LstCnt, endT-beginT);

    //const sint64 beginT1 = LLBC_Time::Now().GetTimeTick();
    //for (int i = 0; i < LstCnt; i++)
    //    lst1[i] = new std::vector<int*>();
    //
    //const sint64 endT1 = LLBC_Time::Now().GetTimeTick();
    //LLBC_PrintLine("New %d objects cost time [%llu].", LstCnt, endT1 - beginT1);

    //const sint64 beginT4 = LLBC_Time::Now().GetTimeTick();
    //for (int i = 0; i < LstCnt; i++)
    //    delete lst1[i];

    //const sint64 endT4 = LLBC_Time::Now().GetTimeTick();
    //LLBC_PrintLine("Delete %d objects cost time [%llu].", LstCnt, endT4 - beginT4);

    //const sint64 beginT2 = LLBC_Time::Now().GetTimeTick();
    //for (int i = 0; i < LstCnt; i++)
    //    pool.Release(lst[i]);
    //const sint64 endT2 = LLBC_Time::Now().GetTimeTick();
    //LLBC_PrintLine("Release %d object-pool objects cost time [%llu].", LstCnt, endT2 - beginT2);

    //const sint64 beginT3 = LLBC_Time::Now().GetTimeTick();
    //for (int i = 0; i < LstCnt; i++)
    //    lst[i] = pool.Get<std::vector<int*> >();

    //const sint64 endT3 = LLBC_Time::Now().GetTimeTick();
    //LLBC_PrintLine("Get %d objects from object pool cost time [%llu].", LstCnt, endT3 - beginT3);


    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return 0;
}