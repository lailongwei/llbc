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

#include <llbc/core/os/OS_Bundle.h>

#include <chrono>
#include <filesystem>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/os/OS_Bundle_NonIphone.cpp

namespace
{

class ScopedBundleDirectory
{
public:
    ScopedBundleDirectory()
    : _path(std::filesystem::temp_directory_path() /
            ("llbc_unit_bundle_" +
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

// Non-iPhone bundles use the current directory as their main bundle. Validate
// default-handle resolution, relative sub-bundles, resource extension handling,
// and missing/invalid resource errors without relying on repository assets.
TEST(BundleOsTest, ResolvesMainAndRelativeBundleResources)
{
#if LLBC_TARGET_PLATFORM_NON_IPHONE
    ScopedBundleDirectory bundleDirectory;
    ScopedCurrentDirectory currentDirectory;
    ASSERT_TRUE(currentDirectory.Enter(bundleDirectory.Path()));

    ASSERT_EQ(LLBC_Directory::Create("nested"), LLBC_OK);
    ASSERT_EQ(LLBC_File::TouchFile("top.txt"), LLBC_OK);
    ASSERT_EQ(LLBC_File::TouchFile("nested/item.dat"), LLBC_OK);

    const LLBC_String mainPath = LLBC_GetMainBundlePath();
    EXPECT_EQ(mainPath, LLBC_Directory::CurDir());

    LLBC_BundleHandle mainBundle = LLBC_CreateBundle("");
    ASSERT_NE(mainBundle, LLBC_INVALID_BUNDLE_HANDLE);
    EXPECT_EQ(LLBC_GetBundlePath(mainBundle), mainPath);
    EXPECT_EQ(LLBC_GetBundlePath(LLBC_INVALID_BUNDLE_HANDLE), mainPath);
    EXPECT_EQ(LLBC_GetBundleResPath(mainBundle, "top", "txt"),
              LLBC_Directory::Join(mainPath, "top.txt"));
    EXPECT_EQ(LLBC_GetBundleResPath(LLBC_INVALID_BUNDLE_HANDLE, "top.txt"),
              LLBC_Directory::Join(mainPath, "top.txt"));
    EXPECT_EQ(LLBC_GetBundleResPath(mainBundle, "item", "dat", "nested"),
              LLBC_Directory::Join(mainPath, "nested/item.dat"));
    EXPECT_EQ(LLBC_GetBundleResPath(mainBundle, "missing", "txt"), "");
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    EXPECT_EQ(LLBC_GetBundleResPath(mainBundle, ""), "");
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    LLBC_ReleaseBundle(mainBundle);
    LLBC_ReleaseBundle(LLBC_INVALID_BUNDLE_HANDLE);

    LLBC_BundleHandle nestedBundle = LLBC_CreateBundle("nested/");
    ASSERT_NE(nestedBundle, LLBC_INVALID_BUNDLE_HANDLE);
    EXPECT_EQ(LLBC_GetBundlePath(nestedBundle), LLBC_Directory::Join(mainPath, "nested"));
    EXPECT_EQ(LLBC_GetBundleResPath(nestedBundle, "item", "dat"),
              LLBC_Directory::Join(mainPath, "nested/item.dat"));
    LLBC_ReleaseBundle(nestedBundle);

    EXPECT_EQ(LLBC_CreateBundle("missing"), LLBC_INVALID_BUNDLE_HANDLE);
#else
    GTEST_SKIP() << "non-iPhone bundle behavior is platform-specific";
#endif
}

// A deployment cleanup can remove the current directory after startup. Main
// bundle lookup must then fail gracefully instead of constructing an invalid
// relative path from a stale working-directory string.
TEST(BundleOsTest, HandlesDeletedCurrentDirectoryForDefaultBundleLookup)
{
#if LLBC_TARGET_PLATFORM_NON_IPHONE && LLBC_TARGET_PLATFORM_NON_WIN32
    ScopedBundleDirectory bundleDirectory;
    ScopedCurrentDirectory currentDirectory;
    ASSERT_TRUE(currentDirectory.Enter(bundleDirectory.Path()));

    std::error_code removeError;
    std::filesystem::remove(std::filesystem::path(bundleDirectory.Path().c_str()), removeError);
    ASSERT_FALSE(removeError) << removeError.message();

    EXPECT_TRUE(LLBC_GetMainBundlePath().empty());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(LLBC_CreateBundle(""), LLBC_INVALID_BUNDLE_HANDLE);
    EXPECT_TRUE(LLBC_GetBundlePath(LLBC_INVALID_BUNDLE_HANDLE).empty());
    EXPECT_TRUE(LLBC_GetBundleResPath(LLBC_INVALID_BUNDLE_HANDLE, "resource").empty());
#else
    GTEST_SKIP() << "deleted-current-directory behavior is POSIX-specific";
#endif
}
