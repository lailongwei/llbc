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


#include "core/file/TestCase_Core_File_Directory.h"

int TestCase_Core_File_Directory::Run(int argc, char *argv[])
{
    std::cout <<"core/file/Directory test:" <<std::endl;

    bool allTested = false;
    do
    {
        if (CurDirTest() != LLBC_OK)
            break;
        if (ExistsTest() != LLBC_OK)
            break;
        if (CreateRemoveTest() != LLBC_OK)
            break;
        if (AbsPathTest() != LLBC_OK)
            break;
        if (JoinTest() != LLBC_OK)
            break;
        if (SplitExtTest() != LLBC_OK)
            break;
        if (GetFilesTest() != LLBC_OK)
            break;
        if (GetDirectoriesTest() != LLBC_OK)
            break;
        if (ModuleFileTest() != LLBC_OK)
            break;
        if (DirNameBaseNameTest() != LLBC_OK)
            break;
        if (MiscTest() != LLBC_OK)
            break;

        allTested = true;
    } while(0);

    std::cout <<"All test done, press any key to exit..." <<std::endl;
    getchar();

    return allTested ? LLBC_OK : LLBC_FAILED;
}

int TestCase_Core_File_Directory::CurDirTest()
{
    LLBC_PrintLine("CurDir/SetCurDir test:");

#if LLBC_TARGET_PLATFORM_WIN32
    const LLBC_String toPath = "C:\\";
#else
    const LLBC_String toPath = "/";
#endif

    const LLBC_String curDir = LLBC_Directory::CurDir();
    LLBC_PrintLine("Current directory: %s", curDir.c_str());
    LLBC_PrintLine("Set current directory To <%s>: ", toPath.c_str());

    if (LLBC_Directory::SetCurDir(toPath) != LLBC_OK)
    {
        LLBC_PrintLine("Set current directory to <%s> failed, error: %s", toPath.c_str(), LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    LLBC_PrintLine("After set, current directory: %s", LLBC_Directory::CurDir().c_str());
    LLBC_Directory::SetCurDir(curDir);

    LLBC_PrintLine("");

    return LLBC_OK;
}

int TestCase_Core_File_Directory::ExistsTest()
{
    LLBC_PrintLine("Exists test:");

    LLBC_String path = ".";
    LLBC_PrintLine("%s exists? %d", path.c_str(), LLBC_Directory::Exists(path));

    path = "~~~~~~~~";
    LLBC_PrintLine("%s exists? %d", path.c_str(), LLBC_Directory::Exists(path));

    path = LLBC_Directory::ModuleFileName();
    LLBC_PrintLine("%s exists? %d", path.c_str(), LLBC_Directory::Exists(path));

    LLBC_PrintLine("");

    return LLBC_OK;
}

int TestCase_Core_File_Directory::CreateRemoveTest()
{
    LLBC_PrintLine("Create/Remove test:");
    LLBC_PrintLine("Create directory: a");
    if (LLBC_Directory::Create("a") != LLBC_OK)
    {
        LLBC_PrintLine("Create directory a failed, error: %s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    for (int i = 0; i < 10; i++)
    {
        const LLBC_String path = LLBC_String().format("a/b_%d", i);
    LLBC_PrintLine("Create directory: %s", path.c_str());
        if (LLBC_Directory::Create(path) != LLBC_OK)
        {
            LLBC_PrintLine("Create directory %s failed, error: %s", path.c_str(), LLBC_FormatLastError());
            return LLBC_FAILED;
        }
    }

    LLBC_PrintLine("Remove directory: a");
    if (LLBC_Directory::Remove("a") != LLBC_OK)
    {
        LLBC_PrintLine("Remove directory a failed, error: %s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    // Complex path create/remove test.
    const LLBC_String complexPath = "a/b/c__d/d/add";
    LLBC_PrintLine("Create directory: %s", complexPath.c_str());
    if (LLBC_Directory::Create(complexPath) != LLBC_OK)
    {
        LLBC_PrintLine("Create directory %s faiiled, error: %s", complexPath.c_str(), LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    LLBC_PrintLine("Remove directory: %s", complexPath.c_str());
    if (LLBC_Directory::Remove(complexPath) != LLBC_OK)
    {
        LLBC_PrintLine("Remove directory %s failed, error: %s", complexPath.c_str(), LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    LLBC_PrintLine("");

    return LLBC_OK;
}

int TestCase_Core_File_Directory::AbsPathTest()
{
    LLBC_PrintLine("AbsPath test:");
    
    LLBC_String path = "";
    LLBC_PrintLine("%s abspath: %s", path.c_str(), LLBC_Directory::AbsPath(path).c_str());
    path = ".";
    LLBC_PrintLine("%s abspath: %s", path.c_str(), LLBC_Directory::AbsPath(path).c_str());
    path = "/";
    LLBC_PrintLine("%s abspath: %s", path.c_str(), LLBC_Directory::AbsPath(path).c_str());
#if LLBC_TARGET_PLATFORM_WIN32
    path = "a/b\\c/..\\";
#else
    path = "a/b/c/../";
#endif
    LLBC_PrintLine("%s abspath: %s", path.c_str(), LLBC_Directory::AbsPath(path).c_str());
#if LLBC_TARGET_PLATFORM_WIN32
    path = "c:\\";
    LLBC_PrintLine("%s abspath: %s", path.c_str(), LLBC_Directory::AbsPath(path).c_str());
    path = "c:\\windows\\.\\.\\..\\windows/././";
    LLBC_PrintLine("%s abspath: %s", path.c_str(), LLBC_Directory::AbsPath(path).c_str());
#endif

    LLBC_PrintLine("");

    return LLBC_OK;
}

int TestCase_Core_File_Directory::JoinTest()
{
    LLBC_PrintLine("Join test:");

    LLBC_Strings parts;
    parts.push_back("a"), parts.push_back("b"), parts.push_back("c"), JoinTest(parts);
    parts.clear(), parts.push_back("a/"), parts.push_back("b"), JoinTest(parts);
    parts.clear(), parts.push_back("a"), parts.push_back("/b"), JoinTest(parts);
    parts.clear(), parts.push_back("a/"), parts.push_back("/b/"), JoinTest(parts);
#if LLBC_TARGET_PLATFORM_WIN32
    parts.clear(), parts.push_back("c:\\"), parts.push_back("/b"), JoinTest(parts);
#endif

    return LLBC_OK;
}

int TestCase_Core_File_Directory::SplitExtTest()
{
    LLBC_PrintLine("SplitExt test:");

    LLBC_String path("a/b");
    LLBC_Strings parts = LLBC_Directory::SplitExt(path);
    LLBC_PrintLine("Split %s: part1: [%s], part2: [%s]", path.c_str(), parts[0].c_str(), parts[1].c_str());

    path = ("a.exe/b");
    parts = LLBC_Directory::SplitExt(path);
    LLBC_PrintLine("Split %s: part1: [%s], part2: [%s]", path.c_str(), parts[0].c_str(), parts[1].c_str());

    path = ("a.exe/b.exe");
    parts = LLBC_Directory::SplitExt(path);
    LLBC_PrintLine("Split %s: part1: [%s], part2: [%s]", path.c_str(), parts[0].c_str(), parts[1].c_str());

    path = ("a.exe/b.exe.bk");
    parts = LLBC_Directory::SplitExt(path);
    LLBC_PrintLine("Split %s: part1: [%s], part2: [%s]", path.c_str(), parts[0].c_str(), parts[1].c_str());

    LLBC_PrintLine("");

    return LLBC_OK;
}

int TestCase_Core_File_Directory::GetFilesTest()
{
    LLBC_PrintLine("GetFiles test:");

    LLBC_PrintLine("Create some directories and files for test:");
    LLBC_Directory::Create("a/b");
    LLBC_Directory::Create("a/c");
    LLBC_Directory::Create("a/d");
    LLBC_File::TouchFile("a/z");

    LLBC_File::TouchFile("a/b/bb");
    LLBC_File::TouchFile("a/c/cc");
    LLBC_File::TouchFile("a/d/dd");
    LLBC_Directory::Create("a/b/zz");

    LLBC_PrintLine("Get files from directory a(recursive = false)");
    if (GetFilesTest("a", false) != LLBC_OK)
    {
        LLBC_Directory::Remove("a");
        return LLBC_FAILED;
    }

    LLBC_PrintLine("Get files from directory a(recursive = true)");
    if (GetFilesTest("a", true) != LLBC_OK)
    {
        LLBC_Directory::Remove("a");
        return LLBC_FAILED;
    }

    LLBC_Directory::Remove("a");
    LLBC_PrintLine("");

    return LLBC_OK;
}

int TestCase_Core_File_Directory::GetDirectoriesTest()
{
    LLBC_PrintLine("GetDirectories test:");

    LLBC_PrintLine("Create some directories and files for test:");
    LLBC_Directory::Create("a/b");
    LLBC_Directory::Create("a/c");
    LLBC_Directory::Create("a/d");
    LLBC_File::TouchFile("a/z");

    LLBC_File::TouchFile("a/b/bb");
    LLBC_File::TouchFile("a/c/cc");
    LLBC_File::TouchFile("a/d/dd");
    LLBC_Directory::Create("a/b/zz");

    LLBC_PrintLine("Get directories from directory a(recursive = false)");
    if (GetDirectoriesTest("a", false) != LLBC_OK)
    {
        LLBC_Directory::Remove("a");
        return LLBC_FAILED;
    }

    LLBC_PrintLine("Get directories from directory a(recursive = true)");
    if (GetDirectoriesTest("a", true) != LLBC_OK)
    {
        LLBC_Directory::Remove("a");
        return LLBC_FAILED;
    }

    LLBC_Directory::Remove("a");
    LLBC_PrintLine("");

    return LLBC_OK;
    return LLBC_OK;
}

int TestCase_Core_File_Directory::ModuleFileTest()
{
    LLBC_PrintLine("ModuleFile test:");
    LLBC_PrintLine("Module file name: %s", LLBC_Directory::ModuleFileName().c_str());
    LLBC_PrintLine("Module file directory: %s", LLBC_Directory::ModuleFileDir().c_str());

    LLBC_PrintLine("");

    return LLBC_OK;
}

int TestCase_Core_File_Directory::DirNameBaseNameTest()
{
    LLBC_PrintLine("DirName/BaseName test:");

    LLBC_String path = "a/b";
    LLBC_PrintLine("path %s dirName: %s, baseName: %s", 
        path.c_str(), LLBC_Directory::DirName(path).c_str(), LLBC_Directory::BaseName(path).c_str());

    path = "/a/b.exe";
    LLBC_PrintLine("path %s dirName: %s, baseName: %s", 
        path.c_str(), LLBC_Directory::DirName(path).c_str(), LLBC_Directory::BaseName(path).c_str());

    path = "a/b.exe/";
    LLBC_PrintLine("path %s dirName: %s, baseName: %s", 
        path.c_str(), LLBC_Directory::DirName(path).c_str(), LLBC_Directory::BaseName(path).c_str());

    LLBC_PrintLine("");

    return LLBC_OK;
}

int TestCase_Core_File_Directory::MiscTest()
{
    LLBC_PrintLine("Misc test:");

    LLBC_PrintLine("Document directory: %s", LLBC_Directory::DocDir().c_str());
    LLBC_PrintLine("Home directory: %s", LLBC_Directory::HomeDir().c_str());
    LLBC_PrintLine("Temporary directory: %s", LLBC_Directory::TempDir().c_str());
    LLBC_PrintLine("Cache directory: %s", LLBC_Directory::CacheDir().c_str());

    LLBC_PrintLine("");

    return LLBC_OK;
}

int TestCase_Core_File_Directory::JoinTest(const LLBC_Strings &pathParts)
{
    LLBC_Print("Join [");
    for (size_t i = 0; i < pathParts.size(); i++)
    {
        LLBC_Print("%s", pathParts[i].c_str());
        if (i != pathParts.size() - 1)
            LLBC_Print(", ");
    }

    LLBC_Print("], ");
    const LLBC_String &joined = LLBC_Directory::Join(pathParts);
    LLBC_PrintLine("Done, Result: %s", joined.c_str());

    return LLBC_OK;
}

int TestCase_Core_File_Directory::GetFilesTest(const LLBC_String &path, bool recursive)
{
    LLBC_PrintLine("Get files from directory: %s, recursive: %d", path.c_str(), recursive);
    
    LLBC_Strings files;
    if (LLBC_Directory::GetFiles(path, files, recursive) != LLBC_OK)
    {
        LLBC_PrintLine("Failed, error: %s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    LLBC_Print("Directory %s files(count: %ld): ", path.c_str(), files.size());
    for (size_t i = 0; i < files.size(); i++)
    {
        LLBC_Print("%s", files[i].c_str());
        if (i != files.size() - 1)
            LLBC_Print(", ");
    }

    LLBC_PrintLine("");

    return LLBC_OK;
}

int TestCase_Core_File_Directory::GetDirectoriesTest(const LLBC_String &path, bool recursive)
{
    LLBC_PrintLine("Get directories from directory: %s, recursive: %d", path.c_str(), recursive);
    
    LLBC_Strings directories;
    if (LLBC_Directory::GetDirectories(path, directories, recursive) != LLBC_OK)
    {
        LLBC_PrintLine("Failed, error: %s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    LLBC_Print("Directory %s subdirectories(count: %ld): ", path.c_str(), directories.size());
    for (size_t i = 0; i < directories.size(); i++)
    {
        LLBC_Print("%s", directories[i].c_str());
        if (i != directories.size() - 1)
            LLBC_Print(", ");
    }

    LLBC_PrintLine("");

    return LLBC_OK;
}
