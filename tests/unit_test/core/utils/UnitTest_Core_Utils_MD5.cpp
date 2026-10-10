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

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/utils/Util_MD5.cpp
// @coverage-target: llbc/include/llbc/core/utils/Util_MD5Inl.h

namespace
{

std::string DigestToHex(const LLBC_String &digest)
{
    static constexpr char HexDigits[] = "0123456789abcdef";

    std::string result;
    result.reserve(digest.size() * 2);
    for (const unsigned char byte : digest)
    {
        result.push_back(HexDigits[byte >> 4]);
        result.push_back(HexDigits[byte & 0x0f]);
    }

    return result;
}

std::vector<uint8> MakePayload(size_t len)
{
    std::vector<uint8> payload;
    payload.reserve(len);
    for (size_t i = 0; i < len; ++i)
        payload.push_back(static_cast<uint8>((i * 37 + 11) % 256));

    return payload;
}

class ScopedTempFile
{
public:
    ScopedTempFile()
    : _path(std::filesystem::temp_directory_path() /
            ("llbc_unit_test_md5_" +
             std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) +
             ".bin"))
    {
    }

    ~ScopedTempFile()
    {
        std::error_code ignored;
        std::filesystem::remove(_path, ignored);
    }

    const std::filesystem::path &Path() const
    {
        return _path;
    }

private:
    std::filesystem::path _path;
};

} // namespace

// The public API must produce the canonical RFC 1321 vectors.
TEST(MD5Test, RFC1321HexDigestVectors)
{
    const std::pair<const char *, const char *> vectors[] = {
        {"", "d41d8cd98f00b204e9800998ecf8427e"},
        {"a", "0cc175b9c0f1b6a831c399e269772661"},
        {"abc", "900150983cd24fb0d6963f7d28e17f72"},
        {"message digest", "f96b697d7cb7938d525a2f31aaf161d0"},
        {"abcdefghijklmnopqrstuvwxyz", "c3fcd3d76192e4007dfb496cca67e13b"},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
         "d174ab98d277d9f5a5611c2c9f419d9f"},
        {"1234567890123456789012345678901234567890"
         "1234567890123456789012345678901234567890",
         "57edf4a22be3c955ac49da2e2107b67a"},
    };

    for (const auto &[input, expected] : vectors)
    {
        const LLBC_String bytes(input);
        EXPECT_EQ(LLBC_MD5::HexDigest(bytes), expected) << "input: " << input;
        EXPECT_EQ(DigestToHex(LLBC_MD5::Digest(bytes)), expected) << "input: " << input;
    }
}

// Exercise all MD5 padding boundaries, full message blocks, and binary input.
TEST(MD5Test, RawBufferDigestHandlesBlockAndPaddingBoundaries)
{
    struct TestCase
    {
        size_t length;
        const char *expectedHex;
    };

    const TestCase cases[] = {
        {1, "13c8ffd977013703a701cf8e11deac65"},
        {55, "d872aa0473a24da995ce4ac518ade767"},
        {56, "e23567645846677c205de80f9779081b"},
        {63, "4775b66278a8fc132ff80923378216cd"},
        {64, "71e123b70c7aa64826fcfe472694cd1c"},
        {65, "5949948f26e35203661075214faa3966"},
        {127, "520bdc2dbab7c25d64263ffb242d9e98"},
        {128, "3e93b378458b77da96b2357c3bda8cc2"},
        {129, "d1ae06bbf9128955a34bedb6231eee62"},
    };

    for (const auto &testCase : cases)
    {
        const auto payload = MakePayload(testCase.length);
        const auto digest = LLBC_MD5::Digest(payload.data(), payload.size());

        ASSERT_EQ(digest.size(), 16lu) << "length: " << testCase.length;
        EXPECT_EQ(DigestToHex(digest), testCase.expectedHex) << "length: " << testCase.length;
        EXPECT_EQ(LLBC_MD5::HexDigest(payload.data(), payload.size()), testCase.expectedHex)
            << "length: " << testCase.length;
    }
}

// The LLBC_String inline overloads must preserve binary bytes and agree with raw buffers.
TEST(MD5Test, OverloadsAndNullEmptyInputAreConsistent)
{
    const auto payload = MakePayload(65);
    const LLBC_String bytes(reinterpret_cast<const char *>(payload.data()), payload.size());

    const auto rawDigest = LLBC_MD5::Digest(payload.data(), payload.size());
    EXPECT_EQ(LLBC_MD5::Digest(bytes), rawDigest);
    EXPECT_EQ(LLBC_MD5::HexDigest(bytes), LLBC_MD5::HexDigest(payload.data(), payload.size()));

    const auto emptyDigest = LLBC_MD5::Digest(nullptr, 0);
    EXPECT_EQ(emptyDigest.size(), 16lu);
    EXPECT_EQ(DigestToHex(emptyDigest), "d41d8cd98f00b204e9800998ecf8427e");
    EXPECT_EQ(LLBC_MD5::HexDigest(nullptr, 0), "d41d8cd98f00b204e9800998ecf8427e");
}

// File hashing is intended for binary files; it must match the in-memory digest and
// report an empty result when the file cannot be read.
TEST(MD5Test, FileDigestMatchesMemoryAndReportsReadFailure)
{
    const auto payload = MakePayload(129);
    const LLBC_String bytes(reinterpret_cast<const char *>(payload.data()), payload.size());
    const auto expectedDigest = LLBC_MD5::Digest(bytes);
    const auto expectedHexDigest = LLBC_MD5::HexDigest(bytes);

    ScopedTempFile file;
    {
        std::ofstream output(file.Path(), std::ios::binary);
        ASSERT_TRUE(output.is_open());
        output.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
        ASSERT_TRUE(output.good());
    }

    const LLBC_String fileName(file.Path().string());
    EXPECT_EQ(LLBC_MD5::FileDigest(fileName), expectedDigest);
    EXPECT_EQ(LLBC_MD5::FileHexDigest(fileName), expectedHexDigest);

    const LLBC_String missingFileName((file.Path().string() + ".missing").c_str());
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_TRUE(LLBC_MD5::FileDigest(missingFileName).empty());
    EXPECT_NE(LLBC_GetLastError(), LLBC_ERROR_SUCCESS);

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_TRUE(LLBC_MD5::FileHexDigest(missingFileName).empty());
    EXPECT_NE(LLBC_GetLastError(), LLBC_ERROR_SUCCESS);
}
