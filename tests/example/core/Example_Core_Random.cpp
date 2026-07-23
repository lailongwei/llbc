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

#include "core/Example_Core_Random.h"

int Example_Core_Random::Run(int argc, char *argv[])
{
    // LLBC_Random is implemented on the Mersenne Twister (mt19937) algorithm
    LLBC_Random rand;

    // ========================= Seeding =========================
    // Seed with a fixed seed to reproduce the same random sequence (production typically uses a timestamp as the seed)
    rand.Seed(2025);
    LLBC_PrintLn("Seeded with 2025.");
    LLBC_PrintLn("\n");

    // ========================= Integer random =========================
    LLBC_PrintLn("Integer random:");
    // Rand(): full-range random integer [-2^31, 2^31)
    LLBC_PrintLn("- Rand()          : %d", rand.Rand());
    // Rand(end): [0, end)
    LLBC_PrintLn("- Rand(100)       : %d (in [0, 100))", rand.Rand(100));
    // Rand(begin, end): [begin, end)
    LLBC_PrintLn("- Rand(10, 20)    : %d (in [10, 20))", rand.Rand(10, 20));
    LLBC_PrintLn("\n");

    // ========================= Real / bool random =========================
    LLBC_PrintLn("Real / bool random:");
    // RandReal(): [0.0, 1.0)
    LLBC_PrintLn("- RandReal()      : %f (in [0.0, 1.0))", rand.RandReal());
    // BoolJudge(): a single boolean judgement (about 50% probability)
    LLBC_PrintLn("- BoolJudge()     : %s", rand.BoolJudge() ? "true" : "false");
    LLBC_PrintLn("\n");

    // ========================= Weighted random =========================
    LLBC_PrintLn("Weighted random:");
    // Pass a weights container, return the hit index [0, size); a larger weight means a higher hit probability
    std::vector<int> weights = {1, 10, 100};
    int hit[3] = {0, 0, 0};
    for (int i = 0; i < 1000; ++i)
        ++hit[rand.Rand(weights)];
    LLBC_PrintLn("- weights {1,10,100} over 1000 draws => idx0:%d idx1:%d idx2:%d",
                 hit[0], hit[1], hit[2]);
    LLBC_PrintLn("\n");

    // ========================= Pick from a range / shuffle =========================
    LLBC_PrintLn("Choice / Shuffle:");
    std::vector<LLBC_String> fruits = {"apple", "banana", "cherry", "durian"};
    // Choice(begin, end): returns an iterator to the randomly hit element
    auto it = rand.Choice(fruits.begin(), fruits.end());
    LLBC_PrintLn("- Choice picked   : %s", it->c_str());

    // Shuffle(begin, end): shuffles the range elements with equal probability
    rand.Shuffle(fruits.begin(), fruits.end());
    LLBC_String shuffled;
    for (auto &f : fruits)
        shuffled.append(f).append(" ");
    LLBC_PrintLn("- Shuffle result  : %s", shuffled.c_str());

    return LLBC_OK;
}
