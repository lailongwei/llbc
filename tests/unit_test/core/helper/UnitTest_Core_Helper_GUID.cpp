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

#include <cctype>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/helper/GUIDHelper.cpp

namespace
{

bool IsCanonicalGuid(const LLBC_String &guid)
{
    if (guid.size() != 36)
        return false;

    for (size_t i = 0; i < guid.size(); ++i)
    {
        if (i == 8 || i == 13 || i == 18 || i == 23)
        {
            if (guid[i] != '-')
                return false;
        }
        else if (!std::isxdigit(static_cast<unsigned char>(guid[i])) ||
                 std::islower(static_cast<unsigned char>(guid[i])))
        {
            return false;
        }
    }

    return true;
}

} // namespace

// Format uses the standard 8-4-4-4-12 uppercase GUID layout and must not depend
// on platform-specific integer widths.
TEST(GUIDHelperTest, FormatsKnownGuidWithCanonicalFieldOrder)
{
    EXPECT_EQ(sizeof(LLBC_GUID), 16lu);

    LLBC_GUID guid {};
    guid.Data1 = 0x01234567u;
    guid.Data2 = 0x89abu;
    guid.Data3 = 0xcdefu;
    guid.Data4[0] = 0x01u;
    guid.Data4[1] = 0x23u;
    guid.Data4[2] = 0x45u;
    guid.Data4[3] = 0x67u;
    guid.Data4[4] = 0x89u;
    guid.Data4[5] = 0xabu;
    guid.Data4[6] = 0xcdu;
    guid.Data4[7] = 0xefu;

    EXPECT_EQ(LLBC_GUIDHelper::Format(guid), "01234567-89AB-CDEF-0123-456789ABCDEF");
}

// Generated GUIDs must have a stable printable representation, and GenStr() is
// equivalent to formatting the generated structure.
TEST(GUIDHelperTest, GeneratesCanonicalNonEmptyGuidStrings)
{
    const LLBC_GUID first = LLBC_GUIDHelper::Gen();
    const LLBC_GUID second = LLBC_GUIDHelper::Gen();
    const LLBC_String firstString = LLBC_GUIDHelper::Format(first);
    const LLBC_String secondString = LLBC_GUIDHelper::Format(second);

    EXPECT_TRUE(IsCanonicalGuid(firstString));
    EXPECT_TRUE(IsCanonicalGuid(secondString));
    EXPECT_NE(firstString, "00000000-0000-0000-0000-000000000000");
    EXPECT_NE(firstString, secondString);
    EXPECT_TRUE(IsCanonicalGuid(LLBC_GUIDHelper::GenStr()));
}
