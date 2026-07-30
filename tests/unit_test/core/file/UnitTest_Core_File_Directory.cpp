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

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>

#if LLBC_TARGET_PLATFORM_NON_WIN32
#include <unistd.h>
#endif

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/file/Directory_*

namespace
{

class ScopedDirectoryTree
{
public:
    ScopedDirectoryTree()
    : _root(std::filesystem::temp_directory_path() /
            ("llbc_unit_directory_" +
             std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())))
    {
    }

    ~ScopedDirectoryTree()
    {
        std::error_code ignored;
        std::filesystem::remove_all(_root, ignored);
    }

    LLBC_String Root() const
    {
        return LLBC_String(_root.string().c_str());
    }

    LLBC_String Path(const LLBC_String &child) const
    {
        return LLBC_Directory::Join(Root(), child);
    }

private:
    std::filesystem::path _root;
};

class ScopedPermissions
{
public:
    explicit ScopedPermissions(const std::filesystem::path &path)
    : _path(path)
    {
        std::error_code error;
        _original = std::filesystem::status(_path, error).permissions();
        _valid = !error;
    }

    bool Set(std::filesystem::perms permissions, std::error_code &error)
    {
        if (!_valid)
        {
            error = std::make_error_code(std::errc::invalid_argument);
            return false;
        }

        std::filesystem::permissions(
            _path, permissions, std::filesystem::perm_options::replace, error);
        _restore = !error;
        return !error;
    }

    ~ScopedPermissions()
    {
        if (!_restore)
            return;

        std::error_code ignored;
        std::filesystem::permissions(
            _path, _original, std::filesystem::perm_options::replace, ignored);
    }

private:
    std::filesystem::path _path;
    std::filesystem::perms _original {};
    bool _valid = false;
    bool _restore = false;
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

#if LLBC_TARGET_PLATFORM_NON_WIN32
class ScopedEnvironmentVariable
{
public:
    explicit ScopedEnvironmentVariable(const char *name)
    : _name(name)
    {
        if (const char *value = std::getenv(_name.c_str()))
        {
            _hadValue = true;
            _value = value;
        }
    }

    ~ScopedEnvironmentVariable()
    {
        if (_hadValue)
            ::setenv(_name.c_str(), _value.c_str(), 1);
        else
            ::unsetenv(_name.c_str());
    }

    int Set(const char *value) const
    {
        return ::setenv(_name.c_str(), value, 1);
    }

    int Unset() const
    {
        return ::unsetenv(_name.c_str());
    }

private:
    std::string _name;
    std::string _value;
    bool _hadValue = false;
};
#endif

void SortStrings(LLBC_Strings &values)
{
    std::sort(values.begin(), values.end());
}

std::vector<std::string> ToStdStrings(const LLBC_Strings &values)
{
    std::vector<std::string> converted;
    converted.reserve(values.size());
    for (const auto &value : values)
        converted.emplace_back(value.c_str(), value.size());

    return converted;
}

} // namespace

// Path helpers are used across file/configuration APIs. Verify their lexical
// behavior without depending on the contents of the host working directory.
TEST(DirectoryTest, NormalizesJoinsSplitsAndReportsRuntimeDirectories)
{
    const LLBC_String current = LLBC_Directory::CurDir();
    ASSERT_FALSE(current.empty());
    EXPECT_TRUE(LLBC_Directory::Exists(current));
    EXPECT_FALSE(LLBC_Directory::IsAbsPath(""));
    EXPECT_TRUE(LLBC_Directory::IsAbsPath("/"));
    EXPECT_EQ(LLBC_Directory::AbsPath(""), current);
    EXPECT_EQ(LLBC_Directory::AbsPath("/"), "/");
    EXPECT_EQ(LLBC_Directory::AbsPath("alpha/./beta/../gamma"),
              LLBC_Directory::Join(current, "alpha/gamma"));
    EXPECT_EQ(LLBC_Directory::AbsPath("/tmp///"), "/tmp");

    EXPECT_EQ(LLBC_Directory::Join("", "beta"), "beta");
    EXPECT_EQ(LLBC_Directory::Join("alpha", "beta"), "alpha/beta");
    EXPECT_EQ(LLBC_Directory::Join("alpha/", "/beta"), "alpha/beta");
    EXPECT_EQ(LLBC_Directory::Join(LLBC_Strings {"alpha", "beta", "gamma"}),
              "alpha/beta/gamma");
    EXPECT_EQ(LLBC_Directory::Join("alpha", "beta", "gamma", "delta"),
              "alpha/beta/gamma/delta");

    const auto split = LLBC_Directory::SplitExt("/tmp/archive.tar.gz");
    ASSERT_EQ(split.size(), 2lu);
    EXPECT_EQ(split[0], "/tmp/archive.tar");
    EXPECT_EQ(split[1], ".gz");
    const auto noExtension = LLBC_Directory::SplitExt("README");
    ASSERT_EQ(noExtension.size(), 2lu);
    EXPECT_EQ(noExtension[0], "README");
    EXPECT_TRUE(noExtension[1].empty());
    EXPECT_EQ(LLBC_Directory::DirName("/"), "/");
    EXPECT_TRUE(LLBC_Directory::BaseName("/").empty());
    EXPECT_EQ(LLBC_Directory::DirName("/usr/local/bin/tool"), "/usr/local/bin");
    EXPECT_EQ(LLBC_Directory::BaseName("/usr/local/bin/tool"), "tool");
    EXPECT_EQ(LLBC_Directory::DirName("plain"), "");
    EXPECT_EQ(LLBC_Directory::BaseName("plain"), "plain");

    const LLBC_String rawModulePath = LLBC_Directory::ModuleFilePath(false);
    const LLBC_String modulePath = LLBC_Directory::ModuleFilePath();
    const LLBC_String moduleDir = LLBC_Directory::ModuleFileDir();
    const LLBC_String moduleName = LLBC_Directory::ModuleFileName();
    EXPECT_FALSE(rawModulePath.empty());
    EXPECT_FALSE(modulePath.empty());
    EXPECT_FALSE(moduleDir.empty());
    EXPECT_FALSE(moduleName.empty());
    EXPECT_EQ(LLBC_Directory::Join(moduleDir, moduleName), modulePath);

    EXPECT_FALSE(LLBC_Directory::HomeDir().empty());
    EXPECT_FALSE(LLBC_Directory::TempDir().empty());
    EXPECT_FALSE(LLBC_Directory::CacheDir().empty());
    EXPECT_FALSE(LLBC_Directory::DocDir().empty());
    EXPECT_EQ(LLBC_Directory::SetCurDir(current), LLBC_OK);
    EXPECT_EQ(LLBC_Directory::SetCurDir(""), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_INVALID);
    EXPECT_EQ(LLBC_Directory::SetCurDir("/definitely/not/a/real/llbc-directory"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
}

// Recursive create/list/remove is the common deployment and cache-cleanup
// workflow. Directory::Create must reject a pre-existing regular file rather
// than reporting a nonexistent directory as successfully created.
TEST(DirectoryTest, CreatesEnumeratesFiltersAndRecursivelyRemovesTrees)
{
    ScopedDirectoryTree tree;
    const LLBC_String root = tree.Root();
    const LLBC_String nested = tree.Path("nested/inner");

    EXPECT_EQ(LLBC_Directory::Create(""), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_INVALID);
    EXPECT_EQ(LLBC_Directory::Create(root + "//invalid"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_INVALID);
    ASSERT_EQ(LLBC_Directory::Create(nested), LLBC_OK);
    EXPECT_TRUE(LLBC_Directory::Exists(root));
    EXPECT_TRUE(LLBC_Directory::Exists(nested));

    LLBC_Strings files;
    ASSERT_EQ(LLBC_Directory::GetFiles(root, files, ".txt"), LLBC_OK);
    EXPECT_TRUE(files.empty());
    LLBC_Strings directories;
    ASSERT_EQ(LLBC_Directory::GetDirectories(root, directories), LLBC_OK);
    ASSERT_EQ(directories.size(), 1lu);
    EXPECT_EQ(directories[0], tree.Path("nested"));

    const LLBC_String filePath = tree.Path("regular-file");
    ASSERT_EQ(LLBC_File::TouchFile(filePath), LLBC_OK);
    EXPECT_EQ(LLBC_Directory::Create(filePath), LLBC_FAILED);
    EXPECT_FALSE(LLBC_Directory::Exists(filePath));

    const LLBC_String rootText = tree.Path("root.txt");
    const LLBC_String rootLog = tree.Path("root.log");
    const LLBC_String nestedText = tree.Path("nested/child.txt");
    const LLBC_String nestedLog = tree.Path("nested/inner/child.log");
    ASSERT_EQ(LLBC_File::TouchFile(rootText), LLBC_OK);
    ASSERT_EQ(LLBC_File::TouchFile(rootLog), LLBC_OK);
    ASSERT_EQ(LLBC_File::TouchFile(nestedText), LLBC_OK);
    ASSERT_EQ(LLBC_File::TouchFile(nestedLog), LLBC_OK);

    files.clear();
    ASSERT_EQ(LLBC_Directory::GetFiles(root, files), LLBC_OK);
    SortStrings(files);
    EXPECT_EQ(ToStdStrings(files),
              std::vector<std::string>({filePath.c_str(), rootLog.c_str(), rootText.c_str()}));

    files.clear();
    ASSERT_EQ(LLBC_Directory::GetFiles(root, files, true), LLBC_OK);
    SortStrings(files);
    EXPECT_EQ(ToStdStrings(files),
              std::vector<std::string>({
                  nestedText.c_str(), nestedLog.c_str(), filePath.c_str(), rootLog.c_str(), rootText.c_str()}));

    files.clear();
    ASSERT_EQ(LLBC_Directory::GetFiles(root, files, ".txt", true), LLBC_OK);
    SortStrings(files);
    EXPECT_EQ(ToStdStrings(files), std::vector<std::string>({nestedText.c_str(), rootText.c_str()}));

    directories.clear();
    ASSERT_EQ(LLBC_Directory::GetDirectories(root, directories), LLBC_OK);
    ASSERT_EQ(directories.size(), 1lu);
    EXPECT_EQ(directories[0], tree.Path("nested"));

    directories.clear();
    ASSERT_EQ(LLBC_Directory::GetDirectories(root, directories, true), LLBC_OK);
    SortStrings(directories);
    EXPECT_EQ(ToStdStrings(directories),
              std::vector<std::string>({tree.Path("nested").c_str(), nested.c_str()}));

    files.clear();
    EXPECT_EQ(LLBC_Directory::GetFiles(tree.Path("missing"), files, true), LLBC_FAILED);
    EXPECT_TRUE(files.empty());
    EXPECT_EQ(LLBC_Directory::Remove(tree.Path("missing")), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);

    ASSERT_EQ(LLBC_Directory::Remove(root), LLBC_OK);
    EXPECT_FALSE(LLBC_Directory::Exists(root));
}

// Directory enumeration must fail transactionally if an entry disappears or
// cannot be stat'ed during traversal. A dangling symlink models this common
// deployment race without mutating paths outside the test-owned tree.
TEST(DirectoryTest, ClearsEnumerationResultsForDanglingEntriesAndInvalidRoots)
{
    ScopedDirectoryTree tree;
    const LLBC_String root = tree.Root();
    ASSERT_EQ(LLBC_Directory::Create(root), LLBC_OK);

    const auto danglingPath = std::filesystem::path(root.c_str()) / "a-dangling";
    const LLBC_String directSurvivor = tree.Path("z-survivor");
    std::error_code symlinkError;
    std::filesystem::create_symlink("missing-target", danglingPath, symlinkError);
    if (symlinkError)
        GTEST_SKIP() << "unable to create dangling symlink: " << symlinkError.message();
    ASSERT_EQ(LLBC_File::TouchFile(directSurvivor), LLBC_OK);

    LLBC_Strings files {"stale"};
    EXPECT_EQ(LLBC_Directory::GetFiles(root, files, true), LLBC_FAILED);
    EXPECT_TRUE(files.empty());

    LLBC_Strings directories {"stale"};
    EXPECT_EQ(LLBC_Directory::GetDirectories(root, directories, true), LLBC_FAILED);
    EXPECT_TRUE(directories.empty());

    EXPECT_EQ(LLBC_Directory::Remove(root), LLBC_FAILED);
    EXPECT_TRUE(LLBC_Directory::Exists(root));

    std::filesystem::remove(danglingPath, symlinkError);
    ASSERT_FALSE(symlinkError);
    ASSERT_EQ(LLBC_File::DeleteFile(directSurvivor), LLBC_OK);

    const LLBC_String nested = tree.Path("nested");
    ASSERT_EQ(LLBC_Directory::Create(nested), LLBC_OK);
    const auto nestedDanglingPath = std::filesystem::path(nested.c_str()) / "a-dangling";
    const LLBC_String nestedSurvivor = LLBC_Directory::Join(nested, "z-survivor");
    std::filesystem::create_symlink("missing-target", nestedDanglingPath, symlinkError);
    ASSERT_FALSE(symlinkError);
    ASSERT_EQ(LLBC_File::TouchFile(nestedSurvivor), LLBC_OK);

    files = {"stale"};
    EXPECT_EQ(LLBC_Directory::GetFiles(root, files, true), LLBC_FAILED);
    EXPECT_TRUE(files.empty());
    directories = {"stale"};
    EXPECT_EQ(LLBC_Directory::GetDirectories(root, directories, true), LLBC_FAILED);
    EXPECT_TRUE(directories.empty());

    std::filesystem::remove(nestedDanglingPath, symlinkError);
    ASSERT_FALSE(symlinkError);
    ASSERT_EQ(LLBC_File::DeleteFile(nestedSurvivor), LLBC_OK);

    const LLBC_String regularFile = tree.Path("not-a-directory");
    ASSERT_EQ(LLBC_File::TouchFile(regularFile), LLBC_OK);
    LLBC_Strings invalidRootDirectories;
    EXPECT_EQ(LLBC_Directory::GetDirectories(regularFile, invalidRootDirectories), LLBC_FAILED);
    EXPECT_TRUE(invalidRootDirectories.empty());

    ASSERT_EQ(LLBC_Directory::Remove(root), LLBC_OK);
}

// Recursive cleanup must surface the operation that cannot mutate the
// test-owned tree. Permission scopes are restored automatically so the test
// leaves no inaccessible temporary paths behind.
TEST(DirectoryTest, ReportsPermissionFailuresDuringRecursiveRemoval)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (::geteuid() == 0)
        GTEST_SKIP() << "root bypasses directory permission checks";

    ScopedDirectoryTree tree;
    const LLBC_String root = tree.Root();
    ASSERT_EQ(LLBC_Directory::Create(root), LLBC_OK);

    const LLBC_String nestedTarget = tree.Path("nested-target");
    ASSERT_EQ(LLBC_Directory::Create(LLBC_Directory::Join(nestedTarget, "child")), LLBC_OK);
    {
        ScopedPermissions permissions(std::filesystem::path(nestedTarget.c_str()));
        std::error_code error;
        ASSERT_TRUE(permissions.Set(std::filesystem::perms::owner_read |
                                        std::filesystem::perms::owner_exec,
                                    error))
            << error.message();
        EXPECT_EQ(LLBC_Directory::Remove(nestedTarget), LLBC_FAILED);
        EXPECT_TRUE(LLBC_Directory::Exists(nestedTarget));
    }
    ASSERT_EQ(LLBC_Directory::Remove(nestedTarget), LLBC_OK);

    const LLBC_String emptyTarget = tree.Path("empty-target");
    ASSERT_EQ(LLBC_Directory::Create(emptyTarget), LLBC_OK);
    {
        ScopedPermissions permissions(std::filesystem::path(root.c_str()));
        std::error_code error;
        ASSERT_TRUE(permissions.Set(std::filesystem::perms::owner_read |
                                        std::filesystem::perms::owner_exec,
                                    error))
            << error.message();
        EXPECT_EQ(LLBC_Directory::Remove(emptyTarget), LLBC_FAILED);
        EXPECT_TRUE(LLBC_Directory::Exists(emptyTarget));
    }
    ASSERT_EQ(LLBC_Directory::Remove(emptyTarget), LLBC_OK);

    const LLBC_String fileTarget = tree.Path("file-target");
    ASSERT_EQ(LLBC_Directory::Create(fileTarget), LLBC_OK);
    ASSERT_EQ(LLBC_File::TouchFile(LLBC_Directory::Join(fileTarget, "locked-file")), LLBC_OK);
    {
        ScopedPermissions permissions(std::filesystem::path(fileTarget.c_str()));
        std::error_code error;
        ASSERT_TRUE(permissions.Set(std::filesystem::perms::owner_read |
                                        std::filesystem::perms::owner_exec,
                                    error))
            << error.message();
        EXPECT_EQ(LLBC_Directory::Remove(fileTarget), LLBC_FAILED);
        EXPECT_TRUE(LLBC_Directory::Exists(fileTarget));
    }
    ASSERT_EQ(LLBC_Directory::Remove(fileTarget), LLBC_OK);
#else
    GTEST_SKIP() << "permission semantics are platform-specific";
#endif
}

// getcwd() can fail when a process remains in a directory that has been
// removed by a deployment cleanup. Restore the original process directory via
// RAII so this host-global condition is contained to the test.
TEST(DirectoryTest, ReportsDeletedCurrentDirectory)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    ScopedDirectoryTree tree;
    const LLBC_String removedCwd = tree.Path("removed-cwd");
    ASSERT_EQ(LLBC_Directory::Create(removedCwd), LLBC_OK);

    ScopedCurrentDirectory currentDirectory;
    ASSERT_TRUE(currentDirectory.Enter(removedCwd));

    std::error_code error;
    std::filesystem::remove(std::filesystem::path(removedCwd.c_str()), error);
    ASSERT_FALSE(error) << error.message();
    EXPECT_TRUE(LLBC_Directory::CurDir().empty());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
#else
    GTEST_SKIP() << "deleted-current-directory behavior is POSIX-specific";
#endif
}

// HOME may be absent in services, containers, and sandboxed processes. Empty
// values must be handled as absent before normalizing a trailing separator.
TEST(DirectoryTest, HandlesMissingEmptyAndTrailingSlashHomeEnvironment)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    ScopedEnvironmentVariable home("HOME");

    ASSERT_EQ(home.Unset(), 0);
    EXPECT_TRUE(LLBC_Directory::HomeDir().empty());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);

    ASSERT_EQ(home.Set(""), 0);
    EXPECT_TRUE(LLBC_Directory::HomeDir().empty());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);

    ASSERT_EQ(home.Set("/tmp/llbc-home/"), 0);
    EXPECT_EQ(LLBC_Directory::HomeDir(), "/tmp/llbc-home");
    ASSERT_EQ(home.Set("/"), 0);
    EXPECT_EQ(LLBC_Directory::HomeDir(), "/");
#else
    GTEST_SKIP() << "HOME environment semantics are POSIX-specific";
#endif
}
