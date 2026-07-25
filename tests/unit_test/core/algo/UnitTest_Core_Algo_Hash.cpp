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

#include <array>
#include <string>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/algo/Hash.cpp
// @coverage-target: llbc/include/llbc/core/algo/HashInl.h

namespace
{

struct HashVector
{
    LLBC_HashAlgo::ENUM algorithm;
    const char *name;
    uint32 helloHash;
    uint32 binaryHash;
};

constexpr HashVector HashVectors[] = {
    {LLBC_HashAlgo::BKDR, "BKDR", 0x2f372e8eu, 0x37025568u},
    {LLBC_HashAlgo::DJB, "DJB", 0x0f923099u, 0x3f87048du},
    {LLBC_HashAlgo::SDBM, "SDBM", 0x28d19932u, 0x74364208u},
    {LLBC_HashAlgo::RS, "RS", 0x3ad49e92u, 0xc4aee960u},
    {LLBC_HashAlgo::JS, "JS", 0x6718efcbu, 0xb64fd387u},
    {LLBC_HashAlgo::PJW, "PJW", 0x006ec32fu, 0x0eeab5a0u},
    {LLBC_HashAlgo::ELF, "ELF", 0x006ec32fu, 0x0eeab5a0u},
    {LLBC_HashAlgo::AP, "AP", 0xec44f026u, 0xa89f16aeu},
    {LLBC_HashAlgo::MurmurHash3, "MurmurHash3", 0xb21f6e00u, 0xda7e3067u},
};

} // namespace

// The enum-to-string API is used by logging/configuration. It must identify every
// selectable algorithm and safely map out-of-range values to "Unknown".
TEST(HashTest, AlgorithmNamesAndInvalidValues)
{
    for (const auto &vector : HashVectors)
    {
        EXPECT_EQ(std::string(LLBC_HashAlgo::GetEnumStr(vector.algorithm).c_str()), vector.name);
    }

    EXPECT_EQ(std::string(LLBC_HashAlgo::GetEnumStr(LLBC_HashAlgo::End).c_str()), "Unknown");
    EXPECT_EQ(std::string(LLBC_HashAlgo::GetEnumStr(-1).c_str()), "Unknown");
    EXPECT_EQ(std::string(LLBC_HashAlgo::GetEnumStr(999).c_str()), "Unknown");
    EXPECT_EQ(LLBC_Hasher::Hash(LLBC_HashAlgo::End, "hello", 5), 0u);
}

// Runtime selection is the normal use case when a configured algorithm is
// selected. These independent, fixed vectors exercise every dispatcher branch and
// both the short and multi-word paths in the implementations.
TEST(HashTest, RuntimeDispatchMatchesKnownVectors)
{
    static constexpr char hello[] = "hello";
    static constexpr std::array<uint8, 16> binary = {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
    };

    for (const auto &vector : HashVectors)
    {
        EXPECT_EQ(LLBC_Hasher::Hash(vector.algorithm, hello, sizeof(hello) - 1),
                  vector.helloHash)
            << vector.name;
        EXPECT_EQ(LLBC_Hasher::Hash(vector.algorithm, binary.data(), binary.size()),
                  vector.binaryHash)
            << vector.name;
    }
}

// Template dispatch is the fast compile-time path. Cover each specialization and
// the global LLBC_Hash helper so changes to the selected branch cannot silently
// route to a different algorithm.
TEST(HashTest, CompileTimeAndGlobalDispatchMatchRuntime)
{
    static constexpr char hello[] = "hello";
    const size_t helloLen = sizeof(hello) - 1;

    EXPECT_EQ(LLBC_Hasher::Hash<LLBC_HashAlgo::BKDR>(hello, helloLen),
              LLBC_Hasher::Hash(LLBC_HashAlgo::BKDR, hello, helloLen));
    EXPECT_EQ(LLBC_Hasher::Hash<LLBC_HashAlgo::DJB>(hello, helloLen),
              LLBC_Hasher::Hash(LLBC_HashAlgo::DJB, hello, helloLen));
    EXPECT_EQ(LLBC_Hasher::Hash<LLBC_HashAlgo::SDBM>(hello, helloLen),
              LLBC_Hasher::Hash(LLBC_HashAlgo::SDBM, hello, helloLen));
    EXPECT_EQ(LLBC_Hasher::Hash<LLBC_HashAlgo::RS>(hello, helloLen),
              LLBC_Hasher::Hash(LLBC_HashAlgo::RS, hello, helloLen));
    EXPECT_EQ(LLBC_Hasher::Hash<LLBC_HashAlgo::JS>(hello, helloLen),
              LLBC_Hasher::Hash(LLBC_HashAlgo::JS, hello, helloLen));
    EXPECT_EQ(LLBC_Hasher::Hash<LLBC_HashAlgo::PJW>(hello, helloLen),
              LLBC_Hasher::Hash(LLBC_HashAlgo::PJW, hello, helloLen));
    EXPECT_EQ(LLBC_Hasher::Hash<LLBC_HashAlgo::ELF>(hello, helloLen),
              LLBC_Hasher::Hash(LLBC_HashAlgo::ELF, hello, helloLen));
    EXPECT_EQ(LLBC_Hasher::Hash<LLBC_HashAlgo::AP>(hello, helloLen),
              LLBC_Hasher::Hash(LLBC_HashAlgo::AP, hello, helloLen));
    EXPECT_EQ(LLBC_Hasher::Hash<LLBC_HashAlgo::MurmurHash3>(hello, helloLen),
              LLBC_Hasher::Hash(LLBC_HashAlgo::MurmurHash3, hello, helloLen));

    EXPECT_EQ(LLBC_Hash<>(hello, helloLen),
              LLBC_Hasher::Hash<LLBC_HashAlgo::Default>(hello, helloLen));
}

// Empty input must not dereference the pointer. A five-byte Murmur input covers
// its partial final block, where only one of four bytes is available.
TEST(HashTest, HandlesEmptyInputAndMurmurTailBlocks)
{
    EXPECT_EQ(LLBC_Hasher::Hash(LLBC_HashAlgo::BKDR, nullptr, 0), 0u);
    EXPECT_EQ(LLBC_Hasher::Hash(LLBC_HashAlgo::DJB, nullptr, 0), 5381u);
    EXPECT_EQ(LLBC_Hasher::Hash(LLBC_HashAlgo::SDBM, nullptr, 0), 0u);
    EXPECT_EQ(LLBC_Hasher::Hash(LLBC_HashAlgo::RS, nullptr, 0), 0u);
    EXPECT_EQ(LLBC_Hasher::Hash(LLBC_HashAlgo::JS, nullptr, 0), 1315423911u);
    EXPECT_EQ(LLBC_Hasher::Hash(LLBC_HashAlgo::PJW, nullptr, 0), 0u);
    EXPECT_EQ(LLBC_Hasher::Hash(LLBC_HashAlgo::ELF, nullptr, 0), 0u);
    EXPECT_EQ(LLBC_Hasher::Hash(LLBC_HashAlgo::AP, nullptr, 0), 0u);
    EXPECT_EQ(LLBC_Hasher::Hash(LLBC_HashAlgo::MurmurHash3, nullptr, 0), 0u);

    static constexpr std::array<uint8, 5> tail = {0, 255, 1, 254, 2};
    EXPECT_EQ(LLBC_Hasher::Hash(LLBC_HashAlgo::MurmurHash3, tail.data(), tail.size()),
              0x93073545u);
}
