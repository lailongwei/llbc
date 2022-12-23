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


#include "llbc/common/Export.h"

#if LLBC_TARGET_PLATFORM_NON_WIN32
 #include <dirent.h>
#endif // Non-Win32

#include "llbc/core/file/Directory.h"
#include "llbc/core/file/File.h"

__LLBC_NS_BEGIN

bool LLBC_Directory::Exists(const LLBC_String &path)
{
#if LLBC_TARGET_PLATFORM_WIN32
    DWORD attrs = ::GetFileAttributesA(path.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return false;
    }

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    return (attrs & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
#else // Non-WIN32
    struct stat st;
    if (stat(path.c_str(), &st) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return false;
    }

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    return (st.st_mode & S_IFDIR) == S_IFDIR;
#endif // WIN32
}

int LLBC_Directory::Create(const LLBC_String &path)
{
    if (path.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    size_t prePos = LLBC_String::npos;
    std::vector<size_t> sepPoses;
    for (size_t pos = 0; pos < path.size(); ++pos)
    {
        if (path[pos] != LLBC_SLASH_A &&
                path[pos] != LLBC_BACKLASH_A)
            continue;

        if ((prePos != LLBC_String::npos) && (prePos + 1 == pos))
        {
            LLBC_SetLastError(LLBC_ERROR_INVALID);
            return LLBC_FAILED;
        }

        prePos = pos;
        sepPoses.push_back(pos);
    }

    if (sepPoses.empty() ||
        *sepPoses.rbegin() != path.size() - 1)
        sepPoses.push_back(path.size());

    for (size_t i = 0; i < sepPoses.size(); ++i)
    {
        const LLBC_String toPath = path.substr(0, sepPoses[i] + 1);
        if (Exists(toPath))
            continue;

#if LLBC_TARGET_PLATFORM_WIN32
        if (!::CreateDirectoryA(toPath.c_str(), nullptr))
        {
            LLBC_SetLastError(LLBC_ERROR_OSAPI);
            return LLBC_FAILED;
        }
#else
        if (mkdir(toPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) // permission: rwxr-xr-x
        {
            LLBC_SetLastError(LLBC_ERROR_CLIB);
            return LLBC_FAILED;
        }
#endif
    }

    return LLBC_OK;
}

int LLBC_Directory::Remove(const LLBC_String &path)
{
    if (!Exists(path))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    std::vector<LLBC_String> delFileList;
    std::vector<LLBC_String> delSubDirList;

    // Get all files and sub-directories.
    if (GetFiles(path, delFileList, true) != LLBC_OK)
        return LLBC_FAILED;
    if (GetDirectories(path, delSubDirList, true) != LLBC_OK)
        return LLBC_FAILED;

    // Delete files.
    for (size_t i = 0; i < delFileList.size(); ++i)
    {
        if (LLBC_File::DeleteFile(delFileList[i]) != LLBC_OK)
            return LLBC_FAILED;
    }
#if LLBC_TARGET_PLATFORM_NON_WIN32
    // Delete empty directories.
    for(int i = (int)(delSubDirList.size() - 1); i >= 0; --i)
    {
        if (rmdir(delSubDirList[i].c_str()) != 0)
        {
            LLBC_SetLastError(LLBC_ERROR_CLIB);
            return LLBC_FAILED;
        }
    }

    // The last, delete self dir.
    if (rmdir(path.c_str()) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    // Delete empty directories.
    for(int i = static_cast<int>(delSubDirList.size()) - 1; i >= 0; --i)
    {
        if (!::RemoveDirectoryA(delSubDirList[i].c_str()))
        {
            LLBC_SetLastError(LLBC_ERROR_OSAPI);
            return LLBC_FAILED;
        }
    }

    // The last, delete self directory.
    if (!::RemoveDirectoryA(path.c_str()))
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

bool LLBC_Directory::IsAbsPath(const LLBC_String& path)
{
    if (UNLIKELY(path.empty()))
        return false;

#if LLBC_TARGET_PLATFORM_WIN32
    return (path.size() >= 3 &&
            LLBC_String::isalpha(path[0]) &&
            path[1] == ':' &&
            path[2] == LLBC_BACKLASH_A);
#else
    return path[0] == LLBC_SLASH_A;
#endif
}

LLBC_String LLBC_Directory::AbsPath(const LLBC_String &path)
{
    if (path.empty())
        return CurDir();

    LLBC_Strings parts;
    LLBC_String absPath = path;
#if LLBC_TARGET_PLATFORM_WIN32
    // Replace all '/' character to '\\'
    absPath.findreplace(LLBC_SLASH_A, LLBC_BACKLASH_A);

    // Merge all '\\\\\\...' prefix characters to 'X:\a\'
    size_t replaceTo = 0;
    while (replaceTo < absPath.size() &&
        absPath[replaceTo] == LLBC_BACKLASH_A)
        replaceTo++;

    if (replaceTo > 0)
        absPath.replace(0, replaceTo, CurDir().substr(0, 3));

    if (absPath.length() >= 3)
    {
        // If not start with 'X:\\', join the current directory.
        if (!(LLBC_String::isalpha(absPath[0]) &&
              absPath[1] == ':' &&
              absPath[2] == LLBC_BACKLASH_A))
        {
            absPath = Join(CurDir(), absPath);
        }
        else if (absPath.length() == 3) // If start with 'X:\\' and length == 3, direct return.
        {
            return absPath;
        }
    }
    else
    {
        // Path length < 3, Join current directory and return.
        absPath = Join(CurDir(), absPath);
    }

    // Trim the tail '\\' characters.
    while (absPath.length() > 3 &&
           absPath[absPath.length() - 1] == LLBC_BACKLASH_A)
        absPath = absPath.substr(0, absPath.length() - 1);

    // Fetch folder and non-folder path parts(split with '\\').
    const LLBC_String root = absPath.substr(0, 3);
    parts = absPath.substr(3).split(LLBC_BACKLASH_A);
#else
    if (absPath[0] != LLBC_SLASH_A) // Not start with '/', join current directory.
        absPath = Join(CurDir(), absPath);
    else if (absPath.length() == 1) // root directory, direct return.
        return absPath;

    // Merge all '/////...' prefix characters to '/'
    size_t replaceTo = 0;
    while (replaceTo < absPath.size() &&
           absPath[replaceTo] == LLBC_SLASH_A)
        replaceTo++;

    if (replaceTo > 0)
        absPath.replace(0, replaceTo, 1, '/');

    // Trim the tail '/' characters.
    while(absPath.size() > 1 && 
        absPath[absPath.length() - 1] == LLBC_SLASH_A)
       absPath = absPath.substr(0, absPath.length() - 1);

    // Fetch root and non-root path parts(split with '/').
    const LLBC_String root = absPath.substr(0, 1);
    parts = absPath.substr(1).split(LLBC_SLASH_A);
#endif
    // Merge path parts list(process "." and ".." parts).
    LLBC_Strings stack;
    for (size_t i = 0; i < parts.size(); ++i)
    {
        if (parts[i] == ".")
        {
            continue;
        }
        else if (parts[i] == "..")
        {
            if (!stack.empty())
                stack.erase(stack.begin() + (stack.size() - 1));
        }
        else
        {
            stack.push_back(parts[i]);
        }
    }

    // Join all processed path parts.
    return Join(root, stack);
}

LLBC_String LLBC_Directory::Join(const LLBC_String &path1,
                                  const LLBC_String &path2)
{
    LLBC_String joined(path1);

    if (path1.empty())
        return path2;

    bool path1HasSlash = false;
    const char &lastCh = *path1.rbegin();
#if LLBC_TARGET_PLATFORM_WIN32
    if (lastCh == LLBC_SLASH_A ||
        lastCh == LLBC_BACKLASH_A)
#else
    if (lastCh == LLBC_SLASH_A)
#endif
        path1HasSlash = true;

    bool path2HasSlash = false;
    if (!path2.empty())
    {
        const char &firstCh = *path2.begin();
#if LLBC_TARGET_PLATFORM_WIN32
        if (firstCh == LLBC_SLASH_A ||
            firstCh == LLBC_BACKLASH_A)
#else
        if (firstCh == LLBC_SLASH_A)
#endif
            path2HasSlash = true;
    }

    if (path1HasSlash)
        joined += path2HasSlash ? path2.substr(1) : path2;
    else
#if LLBC_TARGET_PLATFORM_NON_WIN32
        joined += path2HasSlash ? path2 : LLBC_String().format("%c%s", LLBC_SLASH_A, path2.c_str());
#else // WIN32
        joined += path2HasSlash ? path2 : LLBC_String().format("%c%s", LLBC_BACKLASH_A, path2.c_str());
#endif // Non-WIN32

    return joined;
}

LLBC_String LLBC_Directory::Join(const LLBC_Strings &paths)
{
    LLBC_String finalPath;
    for (size_t i = 0; i < paths.size(); ++i)
        finalPath = Join(finalPath, paths[i]);

    return finalPath;
}

LLBC_String LLBC_Directory::Join(const LLBC_String &path1,
                                 const LLBC_Strings &paths)
{
    LLBC_String finalPath = path1;
    for (size_t i = 0; i < paths.size(); ++i)
        finalPath = Join(finalPath, paths[i]);

    return finalPath;
}

LLBC_Strings LLBC_Directory::SplitExt(const LLBC_String &path)
{
    LLBC_Strings parts;

    LLBC_String baseName = BaseName(path);
    size_t dotIndex = baseName.rfind('.');
    if (dotIndex == LLBC_String::npos)
    {
        parts.push_back(path);
        parts.push_back("");
    }
    else
    {
        dotIndex = path.rfind('.');
        parts.push_back(path.substr(0, dotIndex));
        parts.push_back(path.substr(dotIndex));
    }

    return parts;
}

int LLBC_Directory::GetFiles(const LLBC_String &path, LLBC_Strings &files, bool recursive)
{
    static LLBC_String __emptyFileExt;
    return GetFiles(path, files, __emptyFileExt, recursive);
}

int LLBC_Directory::GetFiles(const LLBC_String &path, LLBC_Strings &files, const LLBC_String &ext, bool recursive)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    struct stat fileStat;
    struct dirent **direntList = nullptr;

    int fileCount = scandir(path.c_str(), &direntList, nullptr, alphasort);
    if (fileCount < 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    bool allProcessed = true;

    int procIdx = 0;
    for (; procIdx < fileCount; ++procIdx)
    {
        LLBC_String fileName = Join(path, direntList[procIdx]->d_name);
        if (stat(fileName.c_str(), &fileStat) != 0)
        {
            allProcessed = false;
            free(direntList[procIdx]);

            break;
        }

        if (fileStat.st_mode & S_IFDIR)
        {
            if (!recursive ||
                (strcmp(".", direntList[procIdx]->d_name) == 0 ||
                strcmp("..", direntList[procIdx]->d_name) == 0))
                continue;

            if (GetFiles(fileName, files, recursive) != LLBC_OK)
            {
                allProcessed = false;
                free(direntList[procIdx]);

                break;
            }
        }
        else
        {
            files.push_back(fileName);
        }

        free(direntList[procIdx]);
    }

    for (procIdx = procIdx + 1;
         procIdx < fileCount;
         procIdx++)
        free(direntList[procIdx]);

    free(direntList);

    if (!allProcessed)
    {
        files.clear();
        return LLBC_FAILED;
    }
#else // Win32
    LLBC_String findPath = Join(path, "*.*");

    WIN32_FIND_DATAA findData;
    HANDLE handle = ::FindFirstFileA(findPath.c_str(), &findData);
    if (handle == INVALID_HANDLE_VALUE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    bool allProcessed = true;
    while (::FindNextFileA(handle, &findData))
    {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (!recursive ||
                strcmp(findData.cFileName, "..") == 0)
                continue;

            if (GetFiles(Join(path, findData.cFileName), files, recursive) != LLBC_OK)
            {
                allProcessed = false;
                break;
            }
        }
        else
        {
            files.push_back(Join(path, findData.cFileName));
        }
    }

    ::FindClose(handle);
    if (!allProcessed)
    {
        files.clear();
        return LLBC_FAILED;
    }
#endif // Non-Win32

    if (files.empty() || 
            ext.empty())
        return LLBC_OK;

    for (int i = static_cast<int>(files.size() - 1); i >= 0; --i)
    {
        const LLBC_String &file = files[i];
        const LLBC_Strings splited = SplitExt(file);
        if (splited.size() == 1 || splited[1] != ext)
            files.erase(files.begin() + i);
    }

    return LLBC_OK;
}

int LLBC_Directory::GetDirectories(const LLBC_String &path, LLBC_Strings &directories, bool recursive)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    struct dirent **direntList = nullptr;
    struct stat fileStat;
    int fileCount = 0;

    fileCount = scandir(path.c_str(), &direntList, nullptr, alphasort);
    if (fileCount < 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    bool allProcessed = true;

    int procIdx = 0;
    for(; procIdx < fileCount; ++procIdx)
    {
        LLBC_String fileName = Join(path, direntList[procIdx]->d_name);
        if (stat(fileName.c_str(), &fileStat) == -1)
        {
            allProcessed = false;
            free(direntList[procIdx]);

            break;
        }

        if ((fileStat.st_mode & S_IFDIR) &&
            (strcmp(".", direntList[procIdx]->d_name) != 0 && 
                strcmp("..", direntList[procIdx]->d_name) != 0))
        {
            directories.push_back(fileName);
            if (!recursive)
            {
                free(direntList[procIdx]);
                continue;
            }

            if (GetDirectories(fileName, directories, recursive) != LLBC_OK)
            {
                allProcessed = false;
                free(direntList[procIdx]);

                break;
            }
        }

        free(direntList[procIdx]);
    }

    for (procIdx = procIdx + 1; procIdx < fileCount; ++procIdx)
        free(direntList[procIdx]);

    free(direntList);

    if (!allProcessed)
    {
        directories.clear();
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    LLBC_String dirName = Join(path, "*.*");

    WIN32_FIND_DATAA findData;
    HANDLE handle = ::FindFirstFileA(dirName.c_str(), &findData);
    if (handle == INVALID_HANDLE_VALUE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    bool allProcessed = true;
    while (::FindNextFileA(handle, &findData))
    {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (strcmp(findData.cFileName, "..") == 0)
                continue;

            const LLBC_String dirPath = Join(path, findData.cFileName);
            directories.push_back(dirPath);
            if (!recursive)
                continue;

            if (GetDirectories(dirPath, directories, recursive) != LLBC_OK)
            {
                allProcessed = false;
                break;
            }
        }
    }

    ::FindClose(handle);
    if (!allProcessed)
    {
        directories.clear();
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_WIN32
}

LLBC_String LLBC_Directory::ModuleFileDir()
{
    return DirName(ModuleFilePath());
}

LLBC_String LLBC_Directory::ModuleFileName()
{
    return BaseName(ModuleFilePath());
}

LLBC_String LLBC_Directory::ModuleFilePath()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    ssize_t ret = -1;
    char buf[PATH_MAX + 1];
    if ((ret = readlink("/proc/self/exe", buf, PATH_MAX)) == -1)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_String();
    }

    buf[ret] = '\0';

    return buf;
#else // LLBC_TARGET_PLATFORM_WIN32
    DWORD ret = 0;
    int bufLen = MAX_PATH + 1;
    char *buf = LLBC_Malloc(char, bufLen);
    while ((ret = ::GetModuleFileNameA(nullptr, buf, bufLen)) == bufLen)
    {
        buf = LLBC_Realloc(char, buf, bufLen * 2);
    }

    if (ret == 0)
    {
        free(buf);
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_String();
    }

    buf[ret] = '\0';

    const LLBC_String modFileName(buf);
    free(buf);

    return modFileName;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

LLBC_String LLBC_Directory::DirName(const LLBC_String &path)
{
#if LLBC_TARGET_PLATFORM_WIN32
    size_t found = path.rfind(LLBC_BACKLASH_A);
    if (found == LLBC_String::npos)
    {
        found = path.rfind(LLBC_SLASH_A);
        if (found == LLBC_String::npos)
            return LLBC_String();
    }

    if (path.length() == 3 &&
        (LLBC_String::isalpha(path[0]) &&
         path[1] == ':' &&
         (path[2] == LLBC_BACKLASH_A || path[2] == LLBC_SLASH_A)))
        return path;

    return path.substr(0, found);
#else
    const size_t found = path.rfind(LLBC_SLASH_A);
    if (found == LLBC_String::npos)
        return LLBC_String();

    if (path.length() == 1)
        return path;
    else 
        return path.substr(0, found);
#endif
}

LLBC_String LLBC_Directory::BaseName(const LLBC_String &path)
{
#if LLBC_TARGET_PLATFORM_WIN32
    size_t found = path.rfind(LLBC_BACKLASH_A);
    if (found == LLBC_String::npos)
    {
        found = path.rfind(LLBC_SLASH_A);
        if (found == LLBC_String::npos)
            return path;
    }

    return path.substr(found + 1);
#else
    const size_t found = path.rfind(LLBC_SLASH_A);
    if (found == LLBC_String::npos)
        return path;
    else
        return path.substr(found + 1);
#endif
}

LLBC_String LLBC_Directory::CurDir()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    uint32 bufLen = 0;
#if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID
    bufLen = PATH_MAX;
#else
    bufLen = MAXPATHLEN;
#endif
    
    char cwd[bufLen];
    if (!getcwd(cwd, bufLen))
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_String();
    }
 
    return cwd;
#else
    LPSTR cwd = nullptr;
    DWORD cwdSize = 0;
    cwdSize = ::GetCurrentDirectoryA(0, nullptr);
    cwd = reinterpret_cast<LPSTR>(malloc(cwdSize * sizeof(CHAR)));
    if (::GetCurrentDirectoryA(cwdSize, cwd) == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        free(cwd);
        return "";
    }
    
    const LLBC_String path(cwd);
    free(cwd);

    return path;
#endif
}

int LLBC_Directory::SetCurDir(const LLBC_String &path)
{
    if (path.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (chdir(path.c_str()) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // Win32
    if (!::SetCurrentDirectoryA(path.c_str()))
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif // Non-Win32
}

__LLBC_NS_END
