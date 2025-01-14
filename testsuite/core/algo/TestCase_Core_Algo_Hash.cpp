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

#include "core/algo/TestCase_Core_Algo_Hash.h"

TestCase_Core_Algo_Hash::TestCase_Core_Algo_Hash()
{
}

TestCase_Core_Algo_Hash::~TestCase_Core_Algo_Hash()
{
}

int TestCase_Core_Algo_Hash::Run(int argc, char *argv[])
{
    std::cout << "core/algo/Hash test:" << std::endl;

    LLBC_ErrorAndReturnIf(SimpleTest() != LLBC_OK, LLBC_FAILED);

    std::cout << "Press any key to continue..." << std::endl;
    getchar();

    return 0;
}

int TestCase_Core_Algo_Hash::SimpleTest()
{
    std::cout << "Simple test:" << std::endl;

    for (int i = 0; i < 100; ++i)
    {
        auto key = LLBC_GUIDHelper::GenStr();
        LLBC_PrintLn("- %03d: key: %s", i, key.c_str());

        LLBC_PrintLn("  - Template hash method test:");
        LLBC_PrintLn("    - BKDR: %u", LLBC_Hash<LLBC_HashAlgo::BKDR>(key.c_str(), key.size()));
        LLBC_PrintLn("    - DJB: %u", LLBC_Hash<LLBC_HashAlgo::DJB>(key.c_str(), key.size()));
        LLBC_PrintLn("    - SDBM: %u", LLBC_Hash<LLBC_HashAlgo::SDBM>(key.c_str(), key.size()));
        LLBC_PrintLn("    - RS: %u", LLBC_Hash<LLBC_HashAlgo::RS>(key.c_str(), key.size()));
        LLBC_PrintLn("    - JS: %u", LLBC_Hash<LLBC_HashAlgo::JS>(key.c_str(), key.size()));
        LLBC_PrintLn("    - PJW: %u", LLBC_Hash<LLBC_HashAlgo::PJW>(key.c_str(), key.size()));
        LLBC_PrintLn("    - ELF: %u", LLBC_Hash<LLBC_HashAlgo::ELF>(key.c_str(), key.size()));
        LLBC_PrintLn("    - AP: %u", LLBC_Hash<LLBC_HashAlgo::AP>(key.c_str(), key.size()));
        LLBC_PrintLn("    - MurmurHash3: %u", LLBC_Hash<LLBC_HashAlgo::MurmurHash3>(key.c_str(), key.size()));

        LLBC_PrintLn("  - Dynamic hash method test:");
        for (int hashAlgo = LLBC_HashAlgo::Begin;
            hashAlgo != LLBC_HashAlgo::End;
            ++hashAlgo)
            LLBC_PrintLn("    - %s: %u",
                         LLBC_HashAlgo::GetEnumStr(hashAlgo).c_str(),
                         LLBC_Hasher::Hash(static_cast<LLBC_HashAlgo::ENUM>(hashAlgo),
                                           key.c_str(),
                                           key.size()));
    }

    return LLBC_OK;
}
