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
// @coverage-target: llbc/src/core/utils/Util_Variant.cpp

// Ini2Variant builds a nested dictionary mirroring sections and keys, which is
// used by configuration consumers that prefer a uniform Variant representation.
TEST(VariantUtilTest, ConvertsIniSectionsAndReplacesExistingDestination)
{
    LLBC_Ini ini;
    const LLBC_String content =
        "[server]\n"
        "host=127.0.0.1\n"
        "port=8080\n"
        "\n"
        "[feature]\n"
        "enabled=yes\n";
    ASSERT_EQ(ini.LoadFromContent(content), LLBC_OK) << ini.GetLoadError();

    LLBC_Variant destination;
    LLBC_VariantUtil::Ini2Variant(ini, destination);
    ASSERT_TRUE(destination.Is<LLBC_Variant::Dict>());
    EXPECT_EQ(destination["server"]["host"].As<LLBC_String>(), "127.0.0.1");
    EXPECT_EQ(destination["server"]["port"].As<LLBC_String>(), "8080");
    EXPECT_EQ(destination["feature"]["enabled"].As<LLBC_String>(), "yes");

    // Reuse an already-dictionary destination: conversion must clear stale keys.
    destination["stale"] = 1;
    LLBC_VariantUtil::Ini2Variant(ini, destination);
    EXPECT_TRUE(static_cast<const LLBC_Variant &>(destination)["stale"].Is<void>());
    EXPECT_EQ(destination.Size(), ini.GetAllSections().size());
}

// Xml2Variant exposes document roots as a children sequence and each element as
// a dictionary containing name, text value, attributes, and a children sequence.
TEST(VariantUtilTest, ConvertsXmlDocumentAndLeafElementsToStableShape)
{
    LLBC_TINYXML2_NS XMLDocument document;
    ASSERT_EQ(document.Parse(
                  "<root id='1'><child kind='first'>text</child><child/></root><solo/>"),
              LLBC_TINYXML2_NS XML_SUCCESS);

    LLBC_Variant converted;
    LLBC_VariantUtil::Xml2Variant(document, converted);
    ASSERT_TRUE(converted.Is<LLBC_Variant::Dict>());

    const auto &documentChildren = converted[LLBC_XMLKeys::Children];
    ASSERT_TRUE(documentChildren.Is<LLBC_Variant::Seq>());
    ASSERT_EQ(documentChildren.Size(), 2lu);
    EXPECT_EQ(documentChildren[0][LLBC_XMLKeys::Name].As<LLBC_String>(), "root");
    EXPECT_EQ(documentChildren[1][LLBC_XMLKeys::Name].As<LLBC_String>(), "solo");

    const auto &root = converted["root"];
    ASSERT_TRUE(root.Is<LLBC_Variant::Dict>());
    EXPECT_EQ(root[LLBC_XMLKeys::Name].As<LLBC_String>(), "root");
    EXPECT_EQ(root[LLBC_XMLKeys::Value].As<LLBC_String>(), "");
    EXPECT_EQ(root[LLBC_XMLKeys::Attrs]["id"].As<LLBC_String>(), "1");
    ASSERT_TRUE(root[LLBC_XMLKeys::Children].Is<LLBC_Variant::Seq>());
    ASSERT_EQ(root[LLBC_XMLKeys::Children].Size(), 2lu);

    const auto &firstChild = root[LLBC_XMLKeys::Children][0];
    EXPECT_EQ(firstChild[LLBC_XMLKeys::Name].As<LLBC_String>(), "child");
    EXPECT_EQ(firstChild[LLBC_XMLKeys::Value].As<LLBC_String>(), "text");
    EXPECT_EQ(firstChild[LLBC_XMLKeys::Attrs]["kind"].As<LLBC_String>(), "first");
    EXPECT_TRUE(firstChild[LLBC_XMLKeys::Children].Is<LLBC_Variant::Seq>());
    EXPECT_TRUE(firstChild[LLBC_XMLKeys::Children].IsEmpty());

    // A leaf element must also retain the same children-sequence shape.
    const auto *solo = document.FirstChildElement("solo");
    ASSERT_NE(solo, nullptr);
    LLBC_Variant leaf;
    LLBC_VariantUtil::Xml2Variant(*solo, leaf);
    EXPECT_EQ(leaf[LLBC_XMLKeys::Name].As<LLBC_String>(), "solo");
    EXPECT_TRUE(leaf[LLBC_XMLKeys::Attrs].Is<LLBC_Variant::Dict>());
    EXPECT_TRUE(leaf[LLBC_XMLKeys::Attrs].IsEmpty());
    EXPECT_TRUE(leaf[LLBC_XMLKeys::Children].Is<LLBC_Variant::Seq>());
    EXPECT_TRUE(leaf[LLBC_XMLKeys::Children].IsEmpty());

    LLBC_VariantUtil::Xml2Variant(*solo, leaf);
    EXPECT_EQ(leaf[LLBC_XMLKeys::Name].As<LLBC_String>(), "solo");

    // Re-running on a dictionary clears prior content before rebuilding it.
    converted["stale"] = 1;
    LLBC_VariantUtil::Xml2Variant(document, converted);
    EXPECT_TRUE(static_cast<const LLBC_Variant &>(converted)["stale"].Is<void>());
}
