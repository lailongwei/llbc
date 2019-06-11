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

    LLBC_ObjectPool<> pool;
    std::vector<int*>* vec = pool.Get<std::vector<int*> >();
    
    for (int i = 0; i < 70; ++i)
    {
        int* data = pool.Get<int>();
        *data = i;
        vec->push_back(data);
    }

    std::vector<int*>::const_iterator it;
    for (it = vec->begin(); it != vec->end(); ++it)
        pool.Release(*it);

    pool.Release(vec);


    pool.GetGuarded<double>();

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return 0;
}