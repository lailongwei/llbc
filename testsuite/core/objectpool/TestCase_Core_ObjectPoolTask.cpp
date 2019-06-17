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


#include "core/objectpool/TestCase_Core_ObjectPoolTask.h"

ObjectPoolTestTask::ObjectPoolTestTask()
: m_repeatCount(TestTimes)
, _pool(new LLBC_ObjectPool<LLBC_SpinLock>())
, _poolInst(_pool->GetPoolInst<std::vector<double> >())
{
    LLBC_Random rand;
    rand.Seed(LLBC_Time::Now().GetTimeTick());

    for (int i = 0; i < TestTimes; ++i)
    {
        _randTimes[i] = LLBC_Abs(rand.Rand(ListSize));
        _pushElems[i] = LLBC_Abs(rand.Rand(ListSize));
    }
}

ObjectPoolTestTask::~ObjectPoolTestTask()
{
    
}

void ObjectPoolTestTask::Svc()
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

void ObjectPoolTestTask::Cleanup()
{
    LLBC_XDelete(_pool);
}
