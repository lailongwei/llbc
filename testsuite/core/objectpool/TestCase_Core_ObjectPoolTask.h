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

#ifndef __LLBC_TEST_CASE_CORE_OBJECT_POOL_TASK_H__
#define __LLBC_TEST_CASE_CORE_OBJECT_POOL_TASK_H__

#include "llbc.h"
using namespace llbc;

namespace {
    const int TestTimes = 100000;
    const int ListSize = 100;
}

/**
* \brief Object pool test task encapsulation.
*/
class ObjectPoolTestTask : public LLBC_BaseTask
{
public:
    ObjectPoolTestTask();
    virtual ~ObjectPoolTestTask();

public:
    virtual void Svc();

    virtual void Cleanup();

private:
    int _randTimes[TestTimes];
    int _pushElems[TestTimes];

    int m_repeatCount;

    LLBC_SpinLock _lock;
    LLBC_ObjectPool<LLBC_SpinLock> *_pool;
    LLBC_ObjectPoolInst<std::vector<double>, LLBC_SpinLock> *_poolInst;
};


#endif // !__LLBC_TEST_CASE_CORE_OBJECT_POOL_TASK_H__
