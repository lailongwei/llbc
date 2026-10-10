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

#include <llbc.h>
using namespace llbc;

#include <algorithm>
#include <array>
#include <list>
#include <limits>
#include <vector>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/random/Random.cpp
// @coverage-target: llbc/include/llbc/core/random/RandomInl.h

// Explicit seeds are the core repeatability contract: two generators with the
// same seed must emit the same complete sequence, and Seed() must reset it.
TEST(RandomTest, ExplicitSeedProducesRepeatableSequences)
{
    LLBC_Random first(123456u);
    LLBC_Random second(123456u);
    for (int i = 0; i < 32; ++i)
        EXPECT_EQ(first.Rand(), second.Rand()) << "index: " << i;

    first.Seed(77);
    second.Seed(77);
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_EQ(first.Rand(97), second.Rand(97));
        EXPECT_EQ(first.Rand(-97), second.Rand(-97));
        EXPECT_EQ(first.Rand(-17, 29), second.Rand(-17, 29));
        EXPECT_EQ(first.RandReal(), second.RandReal());
        EXPECT_EQ(first.BoolJudge(), second.BoolJudge());
    }

    // A zero constructor seed intentionally delegates entropy selection to
    // std::random_device. It must still produce a usable generator.
    LLBC_Random entropySeeded;
    static_cast<void>(entropySeeded.Rand());
}

// Integer and real range APIs are used for indexes, offsets, and probabilistic
// choices. Verify every documented orientation and the degenerate boundaries.
TEST(RandomTest, RangeMethodsRespectDocumentedBounds)
{
    LLBC_Random random(9u);

    EXPECT_EQ(random.Rand(0), 0);
    EXPECT_EQ(random.Rand(5, 5), 5);
    const int minInt = std::numeric_limits<int>::min();
    const int maxInt = std::numeric_limits<int>::max();
    for (int i = 0; i < 128; ++i)
    {
        const int positiveEnd = random.Rand(7);
        EXPECT_GE(positiveEnd, 0);
        EXPECT_LT(positiveEnd, 7);

        const int negativeEnd = random.Rand(-7);
        EXPECT_GE(negativeEnd, -7);
        EXPECT_LT(negativeEnd, 0);

        const int ascending = random.Rand(-11, 19);
        EXPECT_GE(ascending, -11);
        EXPECT_LT(ascending, 19);

        const int descending = random.Rand(19, -11);
        EXPECT_GE(descending, -11);
        EXPECT_LT(descending, 19);

        const int minBounded = random.Rand(minInt);
        EXPECT_GE(minBounded, minInt);
        EXPECT_LT(minBounded, 0);

        const int fullAscending = random.Rand(minInt, maxInt);
        EXPECT_GE(fullAscending, minInt);
        EXPECT_LT(fullAscending, maxInt);

        const int fullDescending = random.Rand(maxInt, minInt);
        EXPECT_GE(fullDescending, minInt);
        EXPECT_LT(fullDescending, maxInt);

        const double real = random.RandReal();
        EXPECT_GE(real, 0.0);
        EXPECT_LT(real, 1.0);
        static_cast<void>(random.BoolJudge());
    }
}

// Weighted selection accepts all supported container types. A single positive
// bucket makes the intended selection deterministic without relying on a random
// distribution, while Choice/Shuffle validate iterator-oriented utilities.
TEST(RandomTest, WeightedChoiceAndShuffleOperateOnSupportedContainers)
{
    LLBC_Random random(101u);
    const std::vector<int> vectorWeights = {0, 0, 9, 0};
    const std::list<int> listWeights = {0, 0, 9, 0};
    const std::array<int, 4> arrayWeights = {0, 0, 9, 0};
    const int cArrayWeights[] = {0, 0, 9, 0};

    EXPECT_EQ(random.Rand(vectorWeights), 2);
    EXPECT_EQ(random.Rand(listWeights), 2);
    EXPECT_EQ(random.Rand(arrayWeights), 2);
    EXPECT_EQ(random.Rand(cArrayWeights), 2);

    const std::vector<int> emptyVectorWeights;
    const std::list<int> emptyListWeights;
    const std::array<int, 3> noSelectableWeights = {0, 0, 0};
    const std::array<int, 3> negativeWeights = {-5, 0, 9};
    const std::array<int, 2> largeWeights = {
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::max(),
    };
    EXPECT_EQ(random.Rand(emptyVectorWeights), 0);
    EXPECT_EQ(random.Rand(emptyListWeights), 0);
    EXPECT_EQ(random.Rand(noSelectableWeights), 0);
    EXPECT_EQ(random.Rand(negativeWeights), 2);
    const int largeWeightChoice = random.Rand(largeWeights);
    EXPECT_GE(largeWeightChoice, 0);
    EXPECT_LT(largeWeightChoice, 2);

    std::array<int, 5> values = {10, 20, 30, 40, 50};
    const auto chosen = random.Choice(values.begin(), values.end());
    ASSERT_NE(chosen, values.end());
    EXPECT_NE(std::find(values.begin(), values.end(), *chosen), values.end());
    EXPECT_EQ(random.Choice(values.end(), values.end()), values.end());

    const std::array<int, 5> original = values;
    std::array<int, 5> shuffledAgain = values;
    LLBC_Random firstShuffle(42u);
    LLBC_Random secondShuffle(42u);
    firstShuffle.Shuffle(values.begin(), values.end());
    secondShuffle.Shuffle(shuffledAgain.begin(), shuffledAgain.end());

    EXPECT_EQ(values, shuffledAgain);
    std::sort(values.begin(), values.end());
    EXPECT_EQ(values, original);
}

// Global helpers are synchronized wrappers around one generator. Reseeding must
// make a mixed sequence reproducible, just as for a local LLBC_Random instance.
TEST(RandomTest, GlobalFunctionsAreRepeatableAfterReseeding)
{
    LLBC_SeedRand(20260722u);
    const int raw = LLBC_Rand();
    const int end = LLBC_Rand(31);
    const int range = LLBC_Rand(-7, 11);
    const double real = LLBC_RandReal();
    const bool judge = LLBC_BoolJudge();

    LLBC_SeedRand(20260722u);
    EXPECT_EQ(LLBC_Rand(), raw);
    EXPECT_EQ(LLBC_Rand(31), end);
    EXPECT_EQ(LLBC_Rand(-7, 11), range);
    EXPECT_EQ(LLBC_RandReal(), real);
    EXPECT_EQ(LLBC_BoolJudge(), judge);
}
