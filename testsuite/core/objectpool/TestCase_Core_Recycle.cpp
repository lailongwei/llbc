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

#include "core/objectpool/TestCase_Core_Recycle.h"

namespace
{
#if LLBC_DEBUG
    static const int TEST_TIMES = 1000;
    static const int BATCH_TEST_TIMES = 100;
#else
    static const int TEST_TIMES = 10000;
    static const int BATCH_TEST_TIMES = 1000;
#endif

    class NormalObj
    {
    public:
        NormalObj()
        {
            std::cout << this << "<NormalObj>: Constructor called!" << std::endl;
        }

        ~NormalObj()
        {
            std::cout << this << "<NormalObj>: Destructor called!" << std::endl;
        }
    };

    class PoolObj
    {
    public:
        PoolObj()
        : _poolInst(nullptr)
        {
            std::cout << "<PoolObj>: Constructor call!" << std::endl;
        }

        ~PoolObj()
        {
            std::cout << "<PoolObj>: Destructor called!" << std::endl;
        }

        void Clear()
        {
            std::cout << "PoolObj::Clear() called!" << std::endl;
        }

        void MarkPoolObject(LLBC_IObjectPoolInst &poolInst)
        {
            _poolInst = &poolInst;
            std::cout << "PoolObj::MarkPoolObject() called!" << std::endl;
        }

        LLBC_IObjectPoolInst *GetPoolInst()
        {
            std::cout << "PoolObj::GetPoolInst() called!" << std::endl;
            return _poolInst;
        }

    private:
        LLBC_IObjectPoolInst *_poolInst;
    };
}

int TestCase_Core_Recycle::Run(int argc, char *argv[])
{
    std::cout << "Recycle test:" << std::endl;

    DoBasicTest();
    DoPerfTest();

    std::cout << "Press any key to continue..." << std::endl;
    getchar();

    return 0;
}

void TestCase_Core_Recycle::DoBasicTest()
{
    std::cout << "Basic test:" << std::endl;

    std::cout << "- Test NormalObj(non-pool-object) new release:" << std::endl;
    std::cout << ">> Begin new NormalObject..." << std::endl;
    NormalObj *nmlObj = new NormalObj;
    std::cout << ">> Call LLBC_Recycle(nmlObj) to release object..." << std::endl;
    LLBC_Recycle(nmlObj);

    std::cout << "- Test PoolObj get/release/delete: " << std::endl;
    {
        LLBC_SafetyObjectPool pool;
        std::cout << ">> Sizeof(LLBC_SafetyObjectPool): " << sizeof(pool) << std::endl;
        std::cout << ">> Begin get pool object from pool:" << std::endl;
        PoolObj *po = pool.Get<PoolObj>();

        std::cout << ">> Call LLBC_Recycle(po) to release object..." << std::endl;
        LLBC_Recycle(po);

        std::cout << ">> Destroy object pool..." << std::endl;
    }

    std::cout << "Basic test finished!" << std::endl;
}

void TestCase_Core_Recycle::DoPerfTest()
{
    std::cout << "Performance test(test times:" << TEST_TIMES * BATCH_TEST_TIMES <<"):" << std::endl;

    // Normal new/delete test.
    LLBC_Packet **pkts = LLBC_Malloc(LLBC_Packet *, sizeof(LLBC_Packet *) * BATCH_TEST_TIMES);
    std::cout << "Do LLBC_Packet new/delete test:" << std::endl;
    sint64 beginTestTime = LLBC_GetMicroSeconds();
    for (int i = 0; i != TEST_TIMES; ++i)
    {
        for (int j = 0; j != BATCH_TEST_TIMES; ++j)
            pkts[j] = new LLBC_Packet;

        for (int j = BATCH_TEST_TIMES - 1; j != -1; --j)
            delete pkts[j];
    }
    sint64 usedTime = LLBC_GetMicroSeconds() - beginTestTime;
    std::cout << "Normal LLBC_Packet new/delete test used time:" << usedTime << std::endl;

    // Normal new/recycle test.
    std::cout << "Do LLBC_Packet new/recycle test:" << std::endl;
    beginTestTime = LLBC_GetMicroSeconds();
    for (int i = 0; i != TEST_TIMES; ++i)
    {
        for (int j = 0; j != BATCH_TEST_TIMES; ++j)
            pkts[j] = new LLBC_Packet;

        for (int j = BATCH_TEST_TIMES - 1; j != -1; --j)
            LLBC_Recycle(pkts[j]);
    }
    usedTime = LLBC_GetMicroSeconds() - beginTestTime;
    std::cout << "Normal LLBC_Packet new/recycle test used time:" << usedTime << std::endl;

    // Object pool new/release test.
    std::cout << "Do LLBC_Packet object pool get/release test:" << std::endl;
    {
        LLBC_UnsafetyObjectPool pool;
        LLBC_ObjectPoolInst<LLBC_Packet> *poolInst = pool.GetPoolInst<LLBC_Packet>();

        beginTestTime = LLBC_GetMicroSeconds();
        for (int i = 0; i != TEST_TIMES; ++i)
        {
            for (int j = 0; j != BATCH_TEST_TIMES; ++j)
                pkts[j] = poolInst->GetObject();

            for (int j = BATCH_TEST_TIMES - 1; j != -1; --j)
                poolInst->Release(pkts[j]);
        }
        usedTime = LLBC_GetMicroSeconds() - beginTestTime;
        std::cout << "LLBC_Packet object pool new/release test used time:" << usedTime << std::endl;
    }

    // Object pool new/release test.
    std::cout << "Do LLBC_Packet object pool get/recycle test:" << std::endl;
    {
        LLBC_UnsafetyObjectPool pool;
        LLBC_ObjectPoolInst<LLBC_Packet> *poolInst = pool.GetPoolInst<LLBC_Packet>();

        beginTestTime = LLBC_GetMicroSeconds();
        for (int i = 0; i != TEST_TIMES; ++i)
        {
            for (int j = 0; j != BATCH_TEST_TIMES; ++j)
                pkts[j] = poolInst->GetObject();

            for (int j = BATCH_TEST_TIMES - 1; j != -1; --j)
                LLBC_Recycle(pkts[j]);
        }
        usedTime = LLBC_GetMicroSeconds() - beginTestTime;
        std::cout << "LLBC_Packet object pool new/recycle test used time:" << usedTime << std::endl;
    }

    std::cout << "Preformance test finished!" << std::endl;
}
