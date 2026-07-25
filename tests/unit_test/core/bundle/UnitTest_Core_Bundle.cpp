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

#include <llbc/core/bundle/Bundle.h>
#include <llbc/core/file/Directory.h>
#include <llbc/core/file/File.h>

#include <chrono>
#include <filesystem>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/bundle/Bundle.cpp

namespace
{

class ScopedBundleDirectory
{
public:
    ScopedBundleDirectory()
    : _path(std::filesystem::temp_directory_path() /
            ("llbc_unit_bundle_object_" +
             std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())))
    {
        std::filesystem::create_directories(_path);
    }

    ~ScopedBundleDirectory()
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

class ScopedCurrentDirectory
{
public:
    ScopedCurrentDirectory()
    : _original(LLBC_Directory::CurDir())
    {
    }

    bool Enter(const LLBC_String &path)
    {
        if (_original.empty() || LLBC_Directory::SetCurDir(path) != LLBC_OK)
            return false;

        _restore = true;
        return true;
    }

    ~ScopedCurrentDirectory()
    {
        if (_restore)
            LLBC_Directory::SetCurDir(_original);
    }

private:
    LLBC_String _original;
    bool _restore = false;
};

} // namespace

// Framework startup creates one main bundle. Its lifetime belongs to the
// framework, so verify its public state and duplicate-creation guard only.
TEST(BundleTest, ExposesMainBundleAndRejectsDuplicateCreation)
{
    const LLBC_Bundle *mainBundle = LLBC_Bundle::GetMainBundle();
    ASSERT_NE(mainBundle, nullptr);
    EXPECT_NE(mainBundle->GetHandle(), LLBC_INVALID_BUNDLE_HANDLE);
    EXPECT_FALSE(mainBundle->GetBundlePath().empty());
    EXPECT_EQ(mainBundle->GetBundleName(), LLBC_Directory::BaseName(mainBundle->GetBundlePath()));

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_EQ(LLBC_Bundle::CreateMainBundle(), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_REENTRY);
}

// An independent bundle must keep its handle and cached metadata synchronized
// through initialization, resource lookup, finalization, and later reuse.
TEST(BundleTest, ManagesIndependentBundleStateAndResources)
{
#if LLBC_TARGET_PLATFORM_NON_IPHONE
    ScopedBundleDirectory bundleDirectory;
    ScopedCurrentDirectory currentDirectory;
    ASSERT_TRUE(currentDirectory.Enter(bundleDirectory.Path()));
    ASSERT_EQ(LLBC_Directory::Create("nested"), LLBC_OK);
    ASSERT_EQ(LLBC_File::TouchFile("top.txt"), LLBC_OK);
    ASSERT_EQ(LLBC_File::TouchFile("nested/item.dat"), LLBC_OK);

    LLBC_Bundle bundle;
    EXPECT_EQ(bundle.GetHandle(), LLBC_INVALID_BUNDLE_HANDLE);
    EXPECT_TRUE(bundle.GetBundleName().empty());
    EXPECT_TRUE(bundle.GetBundlePath().empty());

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_TRUE(bundle.GetResPath("top.txt").empty());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_INIT);
    EXPECT_TRUE(bundle.GetResPath("top", "txt").empty());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_INIT);
    EXPECT_TRUE(bundle.GetResPath("item", "dat", "nested").empty());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_INIT);

    ASSERT_EQ(bundle.Initialize(""), LLBC_OK);
    EXPECT_NE(bundle.GetHandle(), LLBC_INVALID_BUNDLE_HANDLE);
    EXPECT_EQ(bundle.GetBundlePath(), LLBC_Directory::CurDir());
    EXPECT_EQ(bundle.GetBundleName(), LLBC_Directory::BaseName(bundle.GetBundlePath()));
    EXPECT_EQ(bundle.GetResPath("top.txt"), LLBC_Directory::Join(bundle.GetBundlePath(), "top.txt"));
    EXPECT_EQ(bundle.GetResPath("top", "txt"),
              LLBC_Directory::Join(bundle.GetBundlePath(), "top.txt"));
    EXPECT_EQ(bundle.GetResPath("item", "dat", "nested"),
              LLBC_Directory::Join(bundle.GetBundlePath(), "nested/item.dat"));

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_EQ(bundle.Initialize("nested"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_REENTRY);

    bundle.Finalize();
    EXPECT_EQ(bundle.GetHandle(), LLBC_INVALID_BUNDLE_HANDLE);
    EXPECT_TRUE(bundle.GetBundleName().empty());
    EXPECT_TRUE(bundle.GetBundlePath().empty());

    ASSERT_EQ(bundle.Initialize("nested"), LLBC_OK);
    EXPECT_EQ(bundle.GetBundleName(), "nested");
    EXPECT_EQ(bundle.GetResPath("item", "dat"),
              LLBC_Directory::Join(bundle.GetBundlePath(), "item.dat"));
    bundle.Finalize();
    bundle.Finalize();

    LLBC_Bundle missingBundle;
    EXPECT_EQ(missingBundle.Initialize("missing"), LLBC_FAILED);
    EXPECT_EQ(missingBundle.GetHandle(), LLBC_INVALID_BUNDLE_HANDLE);
#else
    GTEST_SKIP() << "non-iPhone bundle behavior is platform-specific";
#endif
}
