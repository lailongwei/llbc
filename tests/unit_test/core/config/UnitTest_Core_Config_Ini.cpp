// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons
// to whom the Software is furnished to do so, subject to the following conditions:
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
// @coverage-target: llbc/src/core/config/Ini.cpp
// @coverage-target: llbc/include/llbc/core/config/IniInl.h

namespace
{

struct UnsupportedIniValue
{
    int value = 7;
};

class ScopedIniFile
{
public:
    ScopedIniFile()
    : _path(std::filesystem::temp_directory_path() /
            ("llbc_unit_ini_" +
             std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) +
             ".ini"))
    {
    }

    ~ScopedIniFile()
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

} // namespace

// INI parsing supports comments, typed lookup, and escaped separator characters
// in keys and values so that configuration text round-trips safely.
TEST(IniTest, LoadsQueriesCommentsAndEscapedCharacters)
{
    const LLBC_String content =
        "; file header\n"
        "[server] ; primary server\n"
        "host=127.0.0.1\n"
        "port=8080 ; listen port\n"
        "escaped\\=key=value\\;with\\=chars\n"
        "\n"
        "[feature]\n"
        "enabled=true\n";

    LLBC_Ini ini;
    ASSERT_EQ(ini.LoadFromContent(content), LLBC_OK) << ini.GetLoadError();
    EXPECT_TRUE(ini.IsHasSection("server"));
    EXPECT_TRUE(ini.IsHasKey("server", "host"));
    EXPECT_FALSE(ini.IsHasKey("server", "missing"));
    EXPECT_EQ(ini.GetValue<LLBC_String>("server", "host"), "127.0.0.1");
    EXPECT_EQ(ini.GetValue<int>("server", "port"), 8080);
    EXPECT_TRUE(ini.GetValue<bool>("feature", "enabled"));
    EXPECT_EQ(ini.GetValue<LLBC_String>("server", "escaped=key"), "value;with=chars");

    const LLBC_IniSection *server = ini.GetSection("server");
    ASSERT_NE(server, nullptr);
    EXPECT_EQ(server->GetSectionComment(), "primary server");
    EXPECT_EQ(server->GetComment("port"), "listen port");
    EXPECT_EQ(server->GetComment("host"), "");
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_SUCCESS);

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_EQ(server->GetComment("missing"), "");
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_EQ(ini.GetValue<int>("server", "missing", -1), -1);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    EXPECT_EQ(ini.GetSection("not-exist"), nullptr);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
}

// Programmatic section/value edits, copy operations, and serialized output are
// the normal workflow for applications that generate or update configuration.
TEST(IniTest, EditsCopiesSavesAndReloadsConfiguration)
{
    LLBC_IniSection runtime;
    runtime.SetSectionComment("runtime settings");
    ASSERT_EQ(runtime.SetValue("threads", 4, "worker count"), LLBC_OK);
    ASSERT_EQ(runtime.SetValue("path", LLBC_String("a=b;c"), "escaped value"), LLBC_OK);

    LLBC_Ini ini;
    EXPECT_EQ(ini.SetSection("", runtime), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    ASSERT_EQ(ini.SetSection("runtime", runtime), LLBC_OK);
    ASSERT_EQ(ini.SetValue("runtime", "verbose", true), LLBC_OK);
    EXPECT_EQ(ini.GetValue<int>("runtime", "threads"), 4);
    EXPECT_EQ(ini.GetValue<LLBC_String>("runtime", "path"), "a=b;c");

    LLBC_IniSection additional;
    ASSERT_EQ(additional.SetValue("threads", 8), LLBC_OK);
    ASSERT_EQ(additional.SetValue("queue", 256), LLBC_OK);
    ASSERT_EQ(ini.SetSection("runtime", additional, true), LLBC_OK);
    EXPECT_EQ(ini.GetValue<int>("runtime", "threads"), 8);
    EXPECT_EQ(ini.GetValue<int>("runtime", "queue"), 256);

    LLBC_Ini copied(ini);
    ASSERT_EQ(ini.SetValue("runtime", "threads", 16), LLBC_OK);
    EXPECT_EQ(copied.GetValue<int>("runtime", "threads"), 8);

    LLBC_String saved;
    ASSERT_EQ(copied.SaveToContent(saved, true, true), LLBC_OK);
    EXPECT_NE(saved.find("[runtime]"), static_cast<LLBC_String::size_type>(-1));
    EXPECT_NE(saved.find("path=a\\=b\\;c"), static_cast<LLBC_String::size_type>(-1));

    LLBC_Ini reloaded;
    ASSERT_EQ(reloaded.LoadFromContent(saved), LLBC_OK) << reloaded.GetLoadError();
    EXPECT_EQ(reloaded.GetValue<int>("runtime", "threads"), 8);
    EXPECT_EQ(reloaded.GetValue<int>("runtime", "queue"), 256);
    EXPECT_EQ(reloaded.GetValue<LLBC_String>("runtime", "path"), "a=b;c");

    LLBC_IniSection *reloadedSection = reloaded.GetSection("runtime");
    ASSERT_NE(reloadedSection, nullptr);
    EXPECT_EQ(reloadedSection->RemoveComment("threads"), LLBC_OK);
    EXPECT_EQ(reloadedSection->GetComment("threads"), "");
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_SUCCESS);
    EXPECT_EQ(reloadedSection->RemoveValue("queue"), LLBC_OK);
    EXPECT_FALSE(reloadedSection->IsHasKey("queue"));
    EXPECT_EQ(reloadedSection->RemoveValue("queue"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
}

// Parser failures should leave a precise load error and return a framework
// failure instead of accepting malformed or ambiguous configuration lines.
TEST(IniTest, RejectsInvalidSectionsKeysDuplicatesAndEscapes)
{
    LLBC_Ini ini;

    EXPECT_EQ(ini.LoadFromContent("key=value\n"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_FORMAT);
    EXPECT_NE(ini.GetLoadError().find("unspecific section"), static_cast<LLBC_String::size_type>(-1));

    EXPECT_EQ(ini.LoadFromContent("[section]\n=value\n"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_FORMAT);
    EXPECT_NE(ini.GetLoadError().find("key empty"), static_cast<LLBC_String::size_type>(-1));

    EXPECT_EQ(ini.LoadFromContent("[section]\nkey=1\nkey=2\n"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_FORMAT);
    EXPECT_NE(ini.GetLoadError().find("key repeat"), static_cast<LLBC_String::size_type>(-1));

    EXPECT_EQ(ini.LoadFromContent("[section]\nkey=bad\\x\n"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_FORMAT);
    EXPECT_NE(ini.GetLoadError().find("invalid escape format"), static_cast<LLBC_String::size_type>(-1));
}

// The inline facade provides typed conversions for all supported primitive
// values, a Variant pass-through, and a defined NOT_IMPL fallback for unsupported
// caller types.
TEST(IniTest, TypedInlineGettersAndSettersCoverSupportedValues)
{
    LLBC_IniSection section;
    ASSERT_EQ(section.SetValue("truth", LLBC_String("true")), LLBC_OK);
    ASSERT_EQ(section.SetValue("number", LLBC_String("42")), LLBC_OK);
    ASSERT_EQ(section.SetValue("negative", LLBC_String("-42")), LLBC_OK);
    ASSERT_EQ(section.SetValue("floating", LLBC_String("3.5")), LLBC_OK);
    ASSERT_EQ(section.SetValue("text", LLBC_String("hello")), LLBC_OK);

    EXPECT_TRUE(section.GetValue<bool>("truth"));
    EXPECT_EQ(section.GetValue<sint8>("negative"), -42);
    EXPECT_EQ(section.GetValue<uint8>("number"), 42u);
    EXPECT_EQ(section.GetValue<sint16>("negative"), -42);
    EXPECT_EQ(section.GetValue<uint16>("number"), 42u);
    EXPECT_EQ(section.GetValue<sint32>("negative"), -42);
    EXPECT_EQ(section.GetValue<uint32>("number"), 42u);
    EXPECT_EQ(section.GetValue<long>("negative"), -42L);
    EXPECT_EQ(section.GetValue<ulong>("number"), 42UL);
    EXPECT_EQ(section.GetValue<sint64>("negative"), -42LL);
    EXPECT_EQ(section.GetValue<uint64>("number"), 42ULL);
    EXPECT_FLOAT_EQ(section.GetValue<float>("floating"), 3.5f);
    EXPECT_DOUBLE_EQ(section.GetValue<double>("floating"), 3.5);
    EXPECT_EQ(section.GetValue<LLBC_String>("text"), "hello");
    EXPECT_EQ(section.GetValue<std::string>("text"), "hello");
    EXPECT_EQ(section.GetValue<LLBC_Variant>("number").As<int>(), 42);

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_EQ(section.GetValue<UnsupportedIniValue>("text").value, 7);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_IMPL);

    LLBC_Ini ini;
    ASSERT_EQ(ini.SetSection("typed", section), LLBC_OK);
    EXPECT_EQ(ini.GetValue<int>("typed", "number"), 42);
    EXPECT_EQ(ini.SetValue("typed", "number", 99), LLBC_OK);
    EXPECT_EQ(ini.GetValue<int>("typed", "number"), 99);
    EXPECT_EQ(ini.SetValue("missing", "value", 1), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
}

// Saving is used for generated configuration. Verify ordered serialization,
// header emission, file round-trips, replacement semantics, and copy assignment.
TEST(IniTest, SavesToFilesOrdersContentAndReplacesSections)
{
    LLBC_IniSection first;
    ASSERT_EQ(first.SetValue("zeta", 1), LLBC_OK);
    ASSERT_EQ(first.SetValue("alpha", 2), LLBC_OK);
    LLBC_IniSection second;
    second.SetSectionComment("second section");
    ASSERT_EQ(second.SetValue("plain", LLBC_String("value")), LLBC_OK);

    LLBC_Ini ini;
    ASSERT_EQ(ini.SetSection("z-section", first), LLBC_OK);
    ASSERT_EQ(ini.SetSection("a-section", second), LLBC_OK);

    LLBC_String insertionOrdered;
    ASSERT_EQ(ini.SaveToContent(insertionOrdered, false, false), LLBC_OK);
    EXPECT_LT(insertionOrdered.find("[z-section]"), insertionOrdered.find("[a-section]"));
    EXPECT_LT(insertionOrdered.find("zeta=1"), insertionOrdered.find("alpha=2"));

    LLBC_String sorted;
    ASSERT_EQ(ini.SaveToContent(sorted, true, true), LLBC_OK);
    EXPECT_LT(sorted.find("[a-section]"), sorted.find("[z-section]"));
    EXPECT_LT(sorted.find("alpha=2"), sorted.find("zeta=1"));

    ScopedIniFile file;
    ASSERT_EQ(ini.SaveToFile(file.Path(), LLBC_Strings {"generated", "do not edit"}, true, true),
              LLBC_OK);
    const LLBC_String fileContent = LLBC_File::ReadToEnd(file.Path());
    EXPECT_NE(fileContent.find("; generated"), static_cast<LLBC_String::size_type>(-1));
    EXPECT_NE(fileContent.find("; do not edit"), static_cast<LLBC_String::size_type>(-1));

    LLBC_Ini loaded;
    ASSERT_EQ(loaded.LoadFromFile(file.Path()), LLBC_OK) << loaded.GetLoadError();
    EXPECT_EQ(loaded.GetValue<int>("z-section", "alpha"), 2);
    EXPECT_EQ(loaded.GetSection("a-section")->GetSectionComment(), "second section");
    EXPECT_EQ(loaded.GetAllSections().size(), 2lu);
    EXPECT_FALSE(loaded.IsHasKey("missing", "key"));
    EXPECT_EQ(loaded.GetComment("missing", "key"), "");
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);

    LLBC_IniSection replacement;
    ASSERT_EQ(replacement.SetValue("fresh", 9), LLBC_OK);
    ASSERT_EQ(loaded.SetSection("z-section", replacement, false), LLBC_OK);
    EXPECT_FALSE(loaded.IsHasKey("z-section", "alpha"));
    EXPECT_EQ(loaded.GetValue<int>("z-section", "fresh"), 9);

    LLBC_Ini assigned;
    assigned = loaded;
    LLBC_Ini *assignedAlias = &assigned;
    assigned = *assignedAlias;
    EXPECT_EQ(assigned.GetValue<int>("z-section", "fresh"), 9);
}

// Section parsing also accepts a later comment for an initially uncommented
// duplicate section. Distinct malformed forms must report the precise parser
// error instead of being silently reinterpreted as key/value data.
TEST(IniTest, HandlesDuplicateSectionCommentsAndSeparatorFailurePaths)
{
    LLBC_Ini ini;
    ASSERT_EQ(ini.LoadFromContent("[section]\n[section] ; documented\nkey=value\n"), LLBC_OK);
    ASSERT_NE(ini.GetSection("section"), nullptr);
    EXPECT_EQ(ini.GetSection("section")->GetSectionComment(), "documented");

    LLBC_IniSection *section = ini.GetSection("section");
    ASSERT_NE(section, nullptr);
    EXPECT_EQ(section->GetAllValues().size(), 1lu);
    EXPECT_EQ(section->GetAllComments().size(), 1lu);
    EXPECT_EQ(section->RemoveComment("missing"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);

    EXPECT_EQ(ini.LoadFromContent("[section]\nkey\n"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_FORMAT);
    EXPECT_NE(ini.GetLoadError().find("separator[=] not found"), static_cast<LLBC_String::size_type>(-1));

    EXPECT_EQ(ini.LoadFromContent("[bad\\x]\nkey=value\n"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_FORMAT);
    EXPECT_NE(ini.GetLoadError().find("invalid escape format"), static_cast<LLBC_String::size_type>(-1));

    ScopedIniFile missingFile;
    EXPECT_EQ(ini.LoadFromFile(missingFile.Path()), LLBC_FAILED);
    EXPECT_NE(ini.GetLoadError().find("open ini file"), static_cast<LLBC_String::size_type>(-1));
}

// Empty values are valid configuration data. File-backed loaders must also
// distinguish a path that opens but cannot be read (such as a directory) from
// a path whose parent cannot be created for save output.
TEST(IniTest, HandlesEmptyValuesAndReportsFileReadWriteFailures)
{
    LLBC_Ini ini;
    ASSERT_EQ(ini.LoadFromContent("[section]\nempty=\n"), LLBC_OK) << ini.GetLoadError();
    EXPECT_EQ(ini.GetValue<LLBC_String>("section", "empty"), "");
    EXPECT_EQ(ini.GetComment("section", "empty"), "");
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_SUCCESS);

    const LLBC_String tempDirectory(
        std::filesystem::temp_directory_path().string().c_str());
    EXPECT_EQ(ini.LoadFromFile(tempDirectory), LLBC_FAILED);
#if LLBC_TARGET_PLATFORM_NON_WIN32
    EXPECT_NE(ini.GetLoadError().find("open ini file"), static_cast<LLBC_String::size_type>(-1));
#else
    EXPECT_NE(ini.GetLoadError().find("ini file"), static_cast<LLBC_String::size_type>(-1));
#endif

    EXPECT_EQ(ini.SaveToFile("/definitely/not/a/real/llbc-ini/output.ini",
                             LLBC_Strings()),
              LLBC_FAILED);
}

// Merging follows SetValue's existing invariant that every key/value has a comment
// entry, using an empty comment when the incoming section has none. Also exercise
// the successful empty-file loading path used by optional configs.
TEST(IniTest, RestoresEmptyCommentWhenMergingAndLoadsEmptyFiles)
{
    LLBC_IniSection incoming;
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_EQ(incoming.SetValue("", 1), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);

    ASSERT_EQ(incoming.SetValue("commentless", 7), LLBC_OK);
    ASSERT_EQ(incoming.RemoveComment("commentless"), LLBC_OK);
    EXPECT_TRUE(incoming.GetAllComments().empty());

    LLBC_IniSection existing;
    ASSERT_EQ(existing.SetValue("retained", 1, "keep"), LLBC_OK);

    LLBC_Ini ini;
    ASSERT_EQ(ini.SetSection("merged", existing), LLBC_OK);
    ASSERT_EQ(ini.SetSection("merged", incoming, true), LLBC_OK);
    EXPECT_EQ(ini.GetValue<int>("missing", "key", -1), -1);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);

    const LLBC_IniSection *merged = ini.GetSection("merged");
    ASSERT_NE(merged, nullptr);
    EXPECT_NE(merged->GetAllComments().find("commentless"),
              merged->GetAllComments().end());
    EXPECT_EQ(merged->GetComment("commentless"), "");
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_SUCCESS);

    LLBC_String saved;
    ASSERT_EQ(ini.SaveToContent(saved), LLBC_OK);
    EXPECT_NE(saved.find("commentless=7 ; \n"), static_cast<LLBC_String::size_type>(-1));

    ScopedIniFile emptyFile;
    {
        LLBC_File writer(emptyFile.Path(), LLBC_FileMode::TextWrite);
        ASSERT_TRUE(writer.IsOpened());
    }

    LLBC_Ini loaded;
    ASSERT_EQ(loaded.LoadFromFile(emptyFile.Path()), LLBC_OK) << loaded.GetLoadError();
    EXPECT_TRUE(loaded.GetAllSections().empty());
}

// SaveToContent writes a complete INI document into its output parameter rather
// than appending to a previous document from an unrelated configuration object.
TEST(IniTest, ReplacesExistingOutputContentOnRepeatedSave)
{
    LLBC_IniSection firstSection;
    ASSERT_EQ(firstSection.SetValue("first", 1), LLBC_OK);
    LLBC_Ini first;
    ASSERT_EQ(first.SetSection("first", firstSection), LLBC_OK);

    LLBC_String content("stale");
    ASSERT_EQ(first.SaveToContent(content), LLBC_OK);
    EXPECT_NE(content.find("[first]"), static_cast<LLBC_String::size_type>(-1));
    EXPECT_EQ(content.find("stale"), static_cast<LLBC_String::size_type>(-1));

    LLBC_IniSection secondSection;
    ASSERT_EQ(secondSection.SetValue("second", 2), LLBC_OK);
    LLBC_Ini second;
    ASSERT_EQ(second.SetSection("second", secondSection), LLBC_OK);
    ASSERT_EQ(second.SaveToContent(content), LLBC_OK);
    EXPECT_NE(content.find("[second]"), static_cast<LLBC_String::size_type>(-1));
    EXPECT_EQ(content.find("[first]"), static_cast<LLBC_String::size_type>(-1));
}
