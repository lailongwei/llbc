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

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/utils/Util_Base64.cpp
// @coverage-target: llbc/include/llbc/core/utils/Util_Base64Inl.h

// Test the RFC 4648 standard encode test vectors:
TEST(Base64Test, RFC4648EncodeVectors)
{
    // The canonical test vectors from RFC 4648 section 10.
    EXPECT_EQ(LLBC_Base64::Encode(std::string("")), "");
    EXPECT_EQ(LLBC_Base64::Encode(std::string("f")), "Zg==");
    EXPECT_EQ(LLBC_Base64::Encode(std::string("fo")), "Zm8=");
    EXPECT_EQ(LLBC_Base64::Encode(std::string("foo")), "Zm9v");
    EXPECT_EQ(LLBC_Base64::Encode(std::string("foob")), "Zm9vYg==");
    EXPECT_EQ(LLBC_Base64::Encode(std::string("fooba")), "Zm9vYmE=");
    EXPECT_EQ(LLBC_Base64::Encode(std::string("foobar")), "Zm9vYmFy");
}

// Test the RFC 4648 standard decode test vectors:
TEST(Base64Test, RFC4648DecodeVectors)
{
    // Decode the canonical vectors back to the original bytes.
    EXPECT_EQ(LLBC_Base64::Decode(std::string("Zg==")), "f");
    EXPECT_EQ(LLBC_Base64::Decode(std::string("Zm8=")), "fo");
    EXPECT_EQ(LLBC_Base64::Decode(std::string("Zm9v")), "foo");
    EXPECT_EQ(LLBC_Base64::Decode(std::string("Zm9vYg==")), "foob");
    EXPECT_EQ(LLBC_Base64::Decode(std::string("Zm9vYmE=")), "fooba");
    EXPECT_EQ(LLBC_Base64::Decode(std::string("Zm9vYmFy")), "foobar");
}

// Test encode/decode round-trip over every input length modulo 3:
TEST(Base64Test, RoundTripAllLengths)
{
    // Encoding then decoding must reproduce the original text, regardless of
    // whether the length aligns to a 3-byte group.
    const std::string base = "The quick brown fox jumps over the lazy dog.";
    for (size_t len = 0; len <= base.size(); ++len)
    {
        const std::string input = base.substr(0, len);
        const std::string encoded = LLBC_Base64::Encode(input);
        EXPECT_EQ(LLBC_Base64::Decode(encoded), input) << "roundtrip failed at len:" << len;
    }
}

// Test encode/decode round-trip over binary data (full 0..255 byte range):
TEST(Base64Test, RoundTripBinaryBytes)
{
    // Base64 must be byte-transparent: all 256 byte values (including embedded
    // '\0' and 0xff) survive an encode/decode cycle.
    std::string input;
    input.reserve(256);
    for (int b = 0; b < 256; ++b)
        input.push_back(static_cast<char>(b));

    const std::string encoded = LLBC_Base64::Encode(input);
    // Encoded output only contains alphabet chars and padding.
    for (const char c : encoded)
    {
        const bool inAlphabet = (c >= 'A' && c <= 'Z') ||
                                (c >= 'a' && c <= 'z') ||
                                (c >= '0' && c <= '9') ||
                                c == '+' || c == '/' || c == '=';
        EXPECT_TRUE(inAlphabet) << "unexpected char in encoded output:" << static_cast<int>(c);
    }

    const std::string decoded = LLBC_Base64::Decode(encoded);
    ASSERT_EQ(decoded.size(), input.size());
    EXPECT_EQ(decoded, input);
}

// Test CalcEncodeLen():
TEST(Base64Test, CalcEncodeLen)
{
    // Encoded length is ceil(n / 3) * 4, and 0 for empty input.
    EXPECT_EQ(LLBC_Base64::CalcEncodeLen(0), 0lu);
    EXPECT_EQ(LLBC_Base64::CalcEncodeLen(1), 4lu);
    EXPECT_EQ(LLBC_Base64::CalcEncodeLen(2), 4lu);
    EXPECT_EQ(LLBC_Base64::CalcEncodeLen(3), 4lu);
    EXPECT_EQ(LLBC_Base64::CalcEncodeLen(4), 8lu);
    EXPECT_EQ(LLBC_Base64::CalcEncodeLen(6), 8lu);
    EXPECT_EQ(LLBC_Base64::CalcEncodeLen(7), 12lu);

    // The reported length must match the actual encoded string length.
    for (size_t n = 1; n <= 32; ++n)
    {
        const std::string input(n, 'x');
        EXPECT_EQ(LLBC_Base64::Encode(input).size(), LLBC_Base64::CalcEncodeLen(n))
            << "mismatch at n:" << n;
    }
}

// Test that the LLBC_String / raw-buffer overloads agree with the std::string overload:
TEST(Base64Test, OverloadConsistency)
{
    const char *raw = "llbc-base64-overloads";
    const size_t rawLen = ::strlen(raw);

    const std::string stdEncoded = LLBC_Base64::Encode(std::string(raw, rawLen));

    // LLBC_String value-return overload.
    const LLBC_String llbcEncoded = LLBC_Base64::Encode(LLBC_String(raw, rawLen));
    EXPECT_EQ(std::string(llbcEncoded.data(), llbcEncoded.size()), stdEncoded);

    // char*/len value-return overload.
    const LLBC_String bufEncoded = LLBC_Base64::Encode(raw, rawLen);
    EXPECT_EQ(std::string(bufEncoded.data(), bufEncoded.size()), stdEncoded);

    // Output-parameter overloads return LLBC_OK.
    std::string stdOut;
    EXPECT_EQ(LLBC_Base64::Encode(std::string(raw, rawLen), stdOut), LLBC_OK);
    EXPECT_EQ(stdOut, stdEncoded);

    LLBC_String llbcOut;
    EXPECT_EQ(LLBC_Base64::Encode(LLBC_String(raw, rawLen), llbcOut), LLBC_OK);
    EXPECT_EQ(std::string(llbcOut.data(), llbcOut.size()), stdEncoded);

    // Decode back through the raw-buffer overload.
    EXPECT_EQ(std::string(LLBC_Base64::Decode(stdEncoded.data(), stdEncoded.size()).c_str()),
              std::string(raw, rawLen));
}

// Test that the LLBC_String decode overloads agree with the std::string overload:
TEST(Base64Test, DecodeOverloadConsistency)
{
    const std::string encoded = LLBC_Base64::Encode(std::string("llbc-base64-decode"));

    // LLBC_String value-return overload (const LLBC_String &).
    const LLBC_String decodedVal = LLBC_Base64::Decode(LLBC_String(encoded.data(), encoded.size()));
    EXPECT_EQ(std::string(decodedVal.data(), decodedVal.size()), "llbc-base64-decode");

    // LLBC_String output-parameter overload returns LLBC_OK.
    LLBC_String decodedOut;
    EXPECT_EQ(LLBC_Base64::Decode(LLBC_String(encoded.data(), encoded.size()), decodedOut), LLBC_OK);
    EXPECT_EQ(std::string(decodedOut.data(), decodedOut.size()), "llbc-base64-decode");
}

// Test the empty-input fast paths of the LLBC_String encode/decode overloads:
TEST(Base64Test, EmptyInputLLBCStringOverloads)
{
    // Encode empty via the char*/len -> LLBC_String path yields an empty string.
    const LLBC_String encodedEmpty = LLBC_Base64::Encode("", 0);
    EXPECT_TRUE(encodedEmpty.empty());

    // "A=" is a degenerate input whose decoded length is 0, exercising the
    // empty-output fast path of the char*/len -> LLBC_String decode overload.
    LLBC_String decodedEmpty;
    EXPECT_EQ(LLBC_Base64::Decode("A=", 2, decodedEmpty), LLBC_OK);
    EXPECT_TRUE(decodedEmpty.empty());
}

// Test that decoding out-of-alphabet input is handled defensively (no crash):
TEST(Base64Test, DecodeInvalidCharsIsRobust)
{
    // '{' is not part of the base64 alphabet (it sits just past 'z'/'9' in the
    // internal lookup). Decoding a group containing it must not crash; the
    // decoded bytes are unspecified, so only the API contract is asserted.
    LLBC_String output;
    EXPECT_EQ(LLBC_Base64::Decode("ab{c", 4, output), LLBC_OK);
}

// Test the fixed-buffer decode overload's buffer-size guard:
TEST(Base64Test, DecodeIntoTooSmallBuffer)
{
    // "Zm9vYmFy" decodes to "foobar" (6 bytes).
    const char *encoded = "Zm9vYmFy";
    const size_t encodedLen = ::strlen(encoded);
    ASSERT_EQ(LLBC_Base64::CalcDecodedLen(encoded, encodedLen), 6lu);

    // A buffer smaller than the decoded length triggers the guard: it fails with
    // LLBC_ERROR_LIMIT and returns LLBC_FAILED (aligned with Encode()).
    char tooSmall[4];
    size_t outputLen = sizeof(tooSmall);
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_EQ(LLBC_Base64::Decode(encoded, encodedLen, tooSmall, outputLen), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_LIMIT);

    // A sufficiently large buffer decodes correctly.
    char large[16];
    size_t largeLen = sizeof(large);
    EXPECT_EQ(LLBC_Base64::Decode(encoded, encodedLen, large, largeLen), LLBC_OK);
    EXPECT_EQ(largeLen, 6lu);
    EXPECT_EQ(std::string(large, largeLen), "foobar");
}

// Test the fixed-buffer encode overload's buffer-size guard:
TEST(Base64Test, EncodeIntoTooSmallBuffer)
{
    const char *input = "hello";
    const size_t inputLen = ::strlen(input);

    // A buffer smaller than the required encoded length must fail with LLBC_ERROR_LIMIT.
    char tooSmall[4];
    size_t outputLen = sizeof(tooSmall);
    ASSERT_LT(outputLen, LLBC_Base64::CalcEncodeLen(inputLen));
    EXPECT_EQ(LLBC_Base64::Encode(input, inputLen, tooSmall, outputLen), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_LIMIT);

    // A sufficiently large buffer succeeds and reports the encoded length.
    char large[64];
    size_t largeLen = sizeof(large);
    EXPECT_EQ(LLBC_Base64::Encode(input, inputLen, large, largeLen), LLBC_OK);
    EXPECT_EQ(largeLen, LLBC_Base64::CalcEncodeLen(inputLen));
    EXPECT_EQ(std::string(large, largeLen), LLBC_Base64::Encode(std::string(input, inputLen)));
}
