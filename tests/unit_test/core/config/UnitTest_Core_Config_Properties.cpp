// The MIT License (MIT)
//
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

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/config/Properties.cpp

namespace
{

class ScopedPropertiesFile
{
public:
    ScopedPropertiesFile()
    : _path(std::filesystem::temp_directory_path() /
            ("llbc_unit_properties_" +
             std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) +
             ".properties"))
    {
    }

    ~ScopedPropertiesFile()
    {
        std::error_code ignored;
        std::filesystem::remove(_path, ignored);
    }

    LLBC_String Path() const
    {
        return LLBC_String(_path.string().c_str());
    }

private:
    std::filesystem::path _path;
};

class ScopedPropertiesDirectory
{
public:
    ScopedPropertiesDirectory()
    : _path(std::filesystem::temp_directory_path() /
            ("llbc_unit_properties_directory_" +
             std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())))
    {
        std::filesystem::create_directories(_path);
    }

    ~ScopedPropertiesDirectory()
    {
        std::error_code ignored;
        std::filesystem::remove_all(_path, ignored);
    }

    LLBC_String Path() const
    {
        return LLBC_String(_path.string().c_str());
    }

private:
    std::filesystem::path _path;
};

} // namespace

// Properties are a flat text representation of a nested Variant dictionary.
// Verify comments, dotted keys, escaped spaces/#/slashes, and a file round-trip.
TEST(PropertiesTest, LoadsNestedEscapedValuesAndRoundTripsThroughFile)
{
    const LLBC_String content(
        "# runtime configuration\n"
        "\n"
        "server.host = 127.0.0.1 # ignored comment\n"
        "server.port = 7788\n"
        "server.display_name = llbc\\ core\\#1\\\\\n"
        "feature.enabled = true\n");
    LLBC_Variant properties;
    LLBC_String error;

    ASSERT_EQ(LLBC_Properties::LoadFromString(content, properties, &error), LLBC_OK);
    EXPECT_EQ(error, "Success");
    EXPECT_TRUE(properties.Is<LLBC_Variant::Dict>());
    EXPECT_EQ(properties["server"]["host"].As<LLBC_String>(), "127.0.0.1");
    EXPECT_EQ(properties["server"]["port"].As<int>(), 7788);
    EXPECT_EQ(properties["server"]["display_name"].As<LLBC_String>(), "llbc core#1\\");
    EXPECT_EQ(properties["feature"]["enabled"].As<LLBC_String>(), "true");

    LLBC_String saved;
    ASSERT_EQ(LLBC_Properties::SaveToString(properties, saved, &error), LLBC_OK);
    EXPECT_EQ(error, "Success");
    EXPECT_NE(saved.find("server.display_name"), static_cast<LLBC_String::size_type>(-1));
    EXPECT_NE(saved.find("llbc\\ core\\#1\\\\"), static_cast<LLBC_String::size_type>(-1));

    LLBC_Variant reparsed;
    ASSERT_EQ(LLBC_Properties::LoadFromString(saved, reparsed, &error), LLBC_OK);
    EXPECT_EQ(reparsed["server"]["display_name"].As<LLBC_String>(), "llbc core#1\\");

    ScopedPropertiesFile file;
    ASSERT_EQ(LLBC_Properties::SaveToFile(reparsed, file.Path(), &error), LLBC_OK);
    EXPECT_EQ(error, "Success");
    LLBC_Variant fromFile;
    ASSERT_EQ(LLBC_Properties::LoadFromFile(file.Path(), fromFile, &error), LLBC_OK);
    EXPECT_EQ(error, "Success");
    EXPECT_EQ(fromFile["server"]["host"].As<LLBC_String>(), "127.0.0.1");
    EXPECT_EQ(fromFile["feature"]["enabled"].As<LLBC_String>(), "true");
}

// Load failures are transactional: callers must not be left with stale or
// partially parsed dictionaries. Save validates that every nested key segment
// is suitable for dotted properties syntax.
TEST(PropertiesTest, RejectsMalformedInputAndResetsDestinationOnFailure)
{
    LLBC_Variant properties(LLBC_String("stale"));
    LLBC_String error;
    EXPECT_EQ(LLBC_Properties::LoadFromString("valid = value\nmissing_separator",
                                               properties,
                                               &error),
              LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_FORMAT);
    EXPECT_TRUE(properties.Is<LLBC_Variant::Dict>());
    EXPECT_TRUE(properties.IsEmpty());
    EXPECT_NE(error.find("#2"), static_cast<LLBC_String::size_type>(-1));

    EXPECT_EQ(LLBC_Properties::LoadFromString("bad-key = value", properties, &error), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_FORMAT);
    EXPECT_TRUE(properties.Is<LLBC_Variant::Dict>());
    EXPECT_TRUE(properties.IsEmpty());

    EXPECT_EQ(LLBC_Properties::LoadFromString("value = trailing\\", properties, &error), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_FORMAT);
    EXPECT_TRUE(properties.Is<LLBC_Variant::Dict>());
    EXPECT_TRUE(properties.IsEmpty());

    properties["stale"] = "value";
    ScopedPropertiesFile file;
    EXPECT_EQ(LLBC_Properties::LoadFromFile(file.Path(), properties, &error), LLBC_FAILED);
    EXPECT_TRUE(properties.Is<LLBC_Variant::Dict>());
    EXPECT_TRUE(properties.IsEmpty());
    EXPECT_NE(error.find("Open properties file"), static_cast<LLBC_String::size_type>(-1));

    LLBC_String content("unchanged");
    EXPECT_EQ(LLBC_Properties::SaveToString(LLBC_Variant(42), content, &error), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_FORMAT);
    EXPECT_TRUE(content.empty());
    EXPECT_NE(error.find("dictionary"), static_cast<LLBC_String::size_type>(-1));

    LLBC_Variant invalidKeys;
    invalidKeys["bad-key"] = 1;
    content = "unchanged";
    EXPECT_EQ(LLBC_Properties::SaveToString(invalidKeys, content, &error), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_FORMAT);
    EXPECT_TRUE(content.empty());
    EXPECT_NE(error.find("bad-key"), static_cast<LLBC_String::size_type>(-1));
}

// Escapes distinguish meaningful trailing spaces and # characters from
// formatting whitespace/comments. Empty key components and unsupported escape
// sequences must fail transactionally with a useful format error.
TEST(PropertiesTest, PreservesEscapedTrailingWhitespaceAndRejectsInvalidEscapes)
{
    LLBC_Variant properties;
    LLBC_String error;
    ASSERT_EQ(LLBC_Properties::LoadFromString(
                  "title = value\\ \n"
                  "hash = visible\\#comment # ignored\n",
                  properties,
                  &error),
              LLBC_OK);
    EXPECT_EQ(properties["title"].As<LLBC_String>(), "value ");
    EXPECT_EQ(properties["hash"].As<LLBC_String>(), "visible#comment");

    EXPECT_EQ(LLBC_Properties::LoadFromString(" = missing-key", properties, &error), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_FORMAT);
    EXPECT_TRUE(properties.Is<LLBC_Variant::Dict>());
    EXPECT_TRUE(properties.IsEmpty());
    EXPECT_NE(error.find("Property key invalid"), static_cast<LLBC_String::size_type>(-1));

    EXPECT_EQ(LLBC_Properties::LoadFromString("... = no-key-items", properties, &error),
              LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_FORMAT);
    EXPECT_TRUE(properties.Is<LLBC_Variant::Dict>());
    EXPECT_TRUE(properties.IsEmpty());
    EXPECT_NE(error.find("Property key invalid"), static_cast<LLBC_String::size_type>(-1));

    EXPECT_EQ(LLBC_Properties::LoadFromString("key = invalid\\q", properties, &error), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_FORMAT);
    EXPECT_TRUE(properties.Is<LLBC_Variant::Dict>());
    EXPECT_TRUE(properties.IsEmpty());
    EXPECT_NE(error.find("invalid escape"), static_cast<LLBC_String::size_type>(-1));
}

// Empty dictionaries are representable at the root and as a scalar-looking
// nested line. Saving must also propagate recursive validation failures and
// report failures before opening/writing invalid destination paths.
TEST(PropertiesTest, SavesEmptyDictionariesAndReportsRecursiveAndFileFailures)
{
    LLBC_String error;
    LLBC_String content;

    LLBC_Variant emptyRoot;
    emptyRoot.Become<LLBC_Variant::Dict>();
    ASSERT_EQ(LLBC_Properties::SaveToString(emptyRoot, content, &error), LLBC_OK);
    EXPECT_TRUE(content.empty());
    EXPECT_EQ(error, "Success");

    LLBC_Variant emptyNested;
    emptyNested["section"].Become<LLBC_Variant::Dict>();
    ASSERT_EQ(LLBC_Properties::SaveToString(emptyNested, content, &error), LLBC_OK);
    EXPECT_NE(content.find("section ="), static_cast<LLBC_String::size_type>(-1));

    LLBC_Variant recursiveInvalidKey;
    recursiveInvalidKey["outer"]["bad-key"] = "value";
    EXPECT_EQ(LLBC_Properties::SaveToString(recursiveInvalidKey, content, &error), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_FORMAT);
    EXPECT_TRUE(content.empty());
    EXPECT_NE(error.find("outer.bad-key"), static_cast<LLBC_String::size_type>(-1));

    ScopedPropertiesFile file;
    EXPECT_EQ(LLBC_Properties::SaveToFile(LLBC_Variant(7), file.Path(), &error), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_FORMAT);

    LLBC_Variant valid;
    valid["key"] = "value";
    ScopedPropertiesDirectory directory;
    EXPECT_EQ(LLBC_Properties::SaveToFile(valid, directory.Path(), &error), LLBC_FAILED);
    EXPECT_NE(error.find("Open file"), static_cast<LLBC_String::size_type>(-1));

    LLBC_Variant unreadable(LLBC_String("stale"));
    EXPECT_EQ(LLBC_Properties::LoadFromFile(directory.Path(), unreadable, &error), LLBC_FAILED);
    EXPECT_TRUE(unreadable.Is<LLBC_Variant::Str>());
    EXPECT_TRUE(unreadable.IsEmpty());
#if LLBC_TARGET_PLATFORM_NON_WIN32
    EXPECT_NE(error.find("Open properties file"), static_cast<LLBC_String::size_type>(-1));
#else
    EXPECT_NE(error.find("properties file"), static_cast<LLBC_String::size_type>(-1));
#endif
}

// SaveToString owns its output parameter: repeated saves replace prior content,
// and a nested validation failure clears any old or partially serialized data.
TEST(PropertiesTest, ReplacesOutputAndClearsItOnSerializationFailure)
{
    LLBC_String error;
    LLBC_String content("stale");

    LLBC_Variant first;
    first["first"] = "one";
    ASSERT_EQ(LLBC_Properties::SaveToString(first, content, &error), LLBC_OK);
    EXPECT_EQ(content, "first = one\n");

    LLBC_Variant second;
    second["second"] = "two";
    ASSERT_EQ(LLBC_Properties::SaveToString(second, content, &error), LLBC_OK);
    EXPECT_EQ(content, "second = two\n");

    LLBC_Variant invalid;
    invalid["valid"] = "kept-out";
    invalid["zz-invalid"] = "must-fail";
    content = "sentinel";
    EXPECT_EQ(LLBC_Properties::SaveToString(invalid, content, &error), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_FORMAT);
    EXPECT_TRUE(content.empty());
    EXPECT_NE(error.find("zz-invalid"), static_cast<LLBC_String::size_type>(-1));
}
