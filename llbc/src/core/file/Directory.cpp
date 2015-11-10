/**
 * @file    Directory.cpp
 * @author  Longwei Lai
 * @date    2011/12/14
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_Directory.h"
#include "llbc/core/utils/Util_Text.h"

#include "llbc/core/file/File.h"
#include "llbc/core/file/Directory.h"

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(disable:4290)
#endif

__LLBC_NS_BEGIN

LLBC_Directory::LLBC_Directory()
{
}

LLBC_Directory::~LLBC_Directory()
{
}

LLBC_DirHandle LLBC_Directory::Handle() const
{
	return (LLBC_DirHandle)NULL;
}

int LLBC_Directory::SetDir(const LLBC_String &dir)
{
    if (dir.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }

    _directoryName = LLBC_ExpandDirectory(dir, true);
    if (_directoryName.empty())
        return LLBC_RTN_FAILED;

    return LLBC_RTN_OK;
}

bool LLBC_Directory::Exist() const
{
    if (_directoryName.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return false;
    }

    LLBC_String dirName = _directoryName;
    dirName.erase(dirName.length() - 1);

    return LLBC_DirectoryExist(dirName);
}

LLBC_String LLBC_Directory::GetDir() const
{
    if (_directoryName.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_String();
    }

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    LLBC_String temp = 
        _directoryName.substr(0, _directoryName.length() - 1);
    return LLBC_BaseName(temp);
}

LLBC_String LLBC_Directory::GetFullDir() const
{
    return _directoryName;
}

int LLBC_Directory::GetFiles(std::vector<LLBC_String> &files,
                             const LLBC_String &filterStr, 
                             bool recur) const
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    struct dirent **direntList = NULL;
    struct stat fileStat;
    int fileCount = 0;

    fileCount = scandir(_directoryName.c_str(), &direntList, NULL, alphasort);
    if (fileCount < 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_RTN_FAILED;
    }

    for (int i = 0; i < fileCount; i++)
    {
        LLBC_String fileName = _directoryName + direntList[i]->d_name;
        if (stat(fileName.c_str(), &fileStat) != -1)
        {
            if ((fileStat.st_mode & S_IFDIR) && recur && 
                (strcmp(".", direntList[i]->d_name) != 0 && 
                    strcmp("..", direntList[i]->d_name) != 0))
            {
                LLBC_Directory subDir;
                subDir.SetDir(fileName);
                subDir.GetFiles(files, filterStr, recur);
            }
            else if (!(fileStat.st_mode & S_IFDIR))
                files.push_back(fileName);
        }

        free(direntList[i]);
    }

    free(direntList);

    return LLBC_RTN_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    LLBC_String dirName = _directoryName;
    dirName += "*.*";

    WIN32_FIND_DATAA findData;
    LLBC_DirHandle handle = ::FindFirstFileA(dirName.c_str(), &findData);
    if (handle == INVALID_HANDLE_VALUE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_RTN_FAILED;
    }

    while (::FindNextFileA(handle, &findData))
    {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && recur)
        {
            if (!strcmp(findData.cFileName, ".."))
                continue;

            LLBC_Directory subDir;
            subDir.SetDir(_directoryName + findData.cFileName);
            subDir.GetFiles(files, filterStr, recur);
        }
        else if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            files.push_back(_directoryName + findData.cFileName);
    }

    ::FindClose(handle);

    return LLBC_RTN_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

LLBC_File *LLBC_Directory::GetFiles(size_t &fileCount, 
                                    const LLBC_String &filterStr, 
                                    const LLBC_String &mode, 
                                    bool recur) const
{
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    std::vector<LLBC_String> names;
    if (this->GetFiles(names, filterStr, recur) != LLBC_RTN_OK)
        return NULL;

    if (names.empty())
    {
        fileCount = 0;
        return NULL;
    }

    LLBC_File *fileList = new LLBC_File[names.size()];
    for (size_t i = 0; i < names.size(); i++)
    {
        if (fileList[i].Open(names[i], mode) != LLBC_RTN_OK)
        {
            delete []fileList;
            return NULL;
        }
    }

    fileCount = names.size();

    return fileList;
}

int LLBC_Directory::GetDirectories(std::vector<LLBC_String> &directories,
                                   const LLBC_String &filterStr, 
                                   bool recur) const
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    struct dirent **direntList = NULL;
    struct stat fileStat;
    int fileCount = 0;

    fileCount = scandir(_directoryName.c_str(), &direntList, NULL, alphasort);
    if (fileCount < 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_RTN_FAILED;
    }

    for(int i = 0; i < fileCount; i++)
    {
        LLBC_String fileName = _directoryName + direntList[i]->d_name;
        if (stat(fileName.c_str(), &fileStat) == -1)
        {
            free(direntList[i]);
            continue;
        }

        if ((fileStat.st_mode & S_IFDIR) &&
            (strcmp(".", direntList[i]->d_name) != 0 && 
                strcmp("..", direntList[i]->d_name) != 0))
        {
            directories.push_back(fileName);
            if (!recur)
            {
                free(direntList[i]);
                continue;
            }

            LLBC_Directory subDir;
            subDir.SetDir(fileName);
            subDir.GetDirectories(directories, filterStr, recur);
        }

        free(direntList[i]);
    }

    free(direntList);

    return LLBC_RTN_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    LLBC_String dirName = _directoryName;
    dirName += "*.*";

    WIN32_FIND_DATAA findData;
    LLBC_DirHandle handle = ::FindFirstFileA(dirName.c_str(), &findData);
    if (handle == INVALID_HANDLE_VALUE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_RTN_FAILED;
    }

    while (::FindNextFileA(handle, &findData))
    {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (strcmp(findData.cFileName, "..") == 0)
                continue;

            directories.push_back(_directoryName + findData.cFileName);
            if (recur)
            {
                LLBC_Directory subDir;
                subDir.SetDir(_directoryName + findData.cFileName);
                subDir.GetDirectories(directories, filterStr, recur);
            }
        }
    }

    ::FindClose(handle);

    return LLBC_RTN_OK;
#endif // LLBC_TARGET_PLATFORM_WIN32
}

LLBC_Directory *LLBC_Directory::GetDirectories(size_t &directoryCount, 
                                               const LLBC_String &filterStr, 
                                               bool recur) const
{
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    std::vector<LLBC_String> directories;
    if (this->GetDirectories(directories, filterStr, recur) != LLBC_RTN_OK)
    {
        return NULL;
    }
    if (directories.empty())
    {
        directoryCount = 0;
        return NULL;
    }

    LLBC_Directory *directoryList = new LLBC_Directory[directories.size()];
    for (size_t i = 0; i < directories.size(); i++)
    {
        directoryList[i].SetDir(directories[i]);
    }

    directoryCount = directories.size();

    return directoryList;
}

int LLBC_Directory::CreateSelfDir()
{
    if (_directoryName.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_RTN_FAILED;
    }

    return LLBC_Directory::CreateDir(_directoryName);
}

int LLBC_Directory::CreateDir(const LLBC_String &dirName)
{
    if (dirName.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }

    LLBC_String fullDirName = LLBC_ExpandDirectory(dirName, true);
    if (fullDirName.empty())
        return LLBC_RTN_FAILED;

#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (mkdir(fullDirName.c_str(), 0755) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    if (!::CreateDirectoryA(fullDirName.c_str(), NULL))
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK; 
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_Directory::CreateSubDir(const LLBC_String &subDirName)
{
    if (subDirName.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (strcmp(subDirName.c_str(), ".") == 0 ||
            strcmp(subDirName.c_str(), "..") == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_PERM);
        return LLBC_RTN_FAILED;
    }

    LLBC_String newDirectory = _directoryName + subDirName;

    if (mkdir(newDirectory.c_str(), 0755) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    LLBC_String newDirectory = _directoryName;
    newDirectory += subDirName;

    if (!::CreateDirectoryA(newDirectory.c_str(), NULL))
        LLBC_SetLastError(LLBC_ERROR_OSAPI);

    return LLBC_RTN_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_Directory::DeleteDir(const LLBC_String &dirName)
{
    LLBC_Directory dir;
    if (dir.SetDir(dirName) != LLBC_RTN_OK)
        return LLBC_RTN_FAILED;

    return dir.DeleteSelfDir();	
}

int LLBC_Directory::DeleteSelfDir()
{
    if (!LLBC_Directory::Exist())
        return LLBC_RTN_FAILED;

    std::vector<LLBC_String> delFileList;
    std::vector<LLBC_String> delSubDirList;

    // get all files and subdirectorys
    this->GetFiles(delFileList, "", true);
    this->GetDirectories(delSubDirList, "", true);

    // delete files
    for (size_t i = 0; i < delFileList.size(); i++)
    {
        if (LLBC_File::Delete(delFileList[i]) != LLBC_RTN_OK)
            return LLBC_RTN_FAILED;
    }
#if LLBC_TARGET_PLATFORM_NON_WIN32
    // delete empty directorys
    for(int i = (int)(delSubDirList.size() - 1); i >= 0; i--)
    {
        if (rmdir(delSubDirList[i].c_str()) != 0)
        {
            LLBC_SetLastError(LLBC_ERROR_CLIB);
            return LLBC_RTN_FAILED;
        }
    }

    // the last, delete self dir
    if (rmdir(_directoryName.c_str()) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    // delete empty directorys
    for(int i = static_cast<int>(delSubDirList.size()) - 1; i >= 0; i--)
    {
        if (!::RemoveDirectoryA(delSubDirList[i].c_str()))
        {
            LLBC_SetLastError(LLBC_ERROR_OSAPI);
            return LLBC_RTN_FAILED;
        }
    }

    // the last, delete self dir
    if (!::RemoveDirectoryA(_directoryName.c_str()))
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

LLBC_String LLBC_Directory::GetCurrentDirectory()
{
    return LLBC_GetCurrentDirectory();
}

int LLBC_Directory::SetCurrentDirectory(const LLBC_String &curDir)
{
	return LLBC_SetCurrentDirectory(curDir);
}

void LLBC_Directory::SetDirT(const LLBC_String &dir) throw (LLBC_IOException)
{
    if (this->SetDir(dir) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

LLBC_String LLBC_Directory::GetDirT() const throw (LLBC_IOException)
{
    LLBC_String dir = this->GetDir();
    if (dir.empty() && LLBC_GetLastError() != LLBC_ERROR_SUCCESS)
        throw LLBC_IOException(LLBC_FormatLastError());

    return dir;
}

LLBC_String LLBC_Directory::GetFullDirT() const throw (LLBC_IOException)
{
    return this->GetFullDir();
}

void LLBC_Directory::GetFilesT(std::vector<LLBC_String> &files, 
                               const LLBC_String &filterStr, 
                               bool recur) const throw (LLBC_IOException)
{
    if (this->GetFiles(files, filterStr, recur) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

LLBC_File *LLBC_Directory::GetFilesT(size_t &fileCount,
                                     const LLBC_String &filterStr,
                                     const LLBC_String &mode,
                                     bool recur) const throw (LLBC_IOException)
{
    LLBC_File *files = this->GetFiles(fileCount, filterStr, mode, recur);
    if (!files && LLBC_GetLastError() != LLBC_ERROR_SUCCESS)
        throw LLBC_IOException(LLBC_FormatLastError());

    return files;
}

void LLBC_Directory::GetDirectoriesT(std::vector<LLBC_String> &directories,
                                     const LLBC_String &filterStr,
                                     bool recur) const throw (LLBC_IOException)
{
    if (this->GetDirectories(directories, filterStr, recur) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

LLBC_Directory *LLBC_Directory::GetDirectoriesT(size_t &directoryCount, 
                                                const LLBC_String &filterStr, 
                                                bool recur) const throw (LLBC_IOException)
{
    LLBC_Directory *dirs = this->GetDirectories(directoryCount, filterStr, recur);
    if (!dirs && LLBC_GetLastError() != LLBC_ERROR_SUCCESS)
        throw LLBC_IOException(LLBC_FormatLastError());

    return dirs;
}

void LLBC_Directory::CreateSelfDirT() throw (LLBC_IOException)
{
    if (this->CreateSelfDir() != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

void LLBC_Directory::CreateDirT(const LLBC_String &dirName) throw (LLBC_IOException)
{
    if (LLBC_Directory::CreateDir(dirName) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

void LLBC_Directory::CreateSubDirT(const LLBC_String &subDirName) throw (LLBC_IOException)
{
    if (this->CreateSubDir(subDirName) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

void LLBC_Directory::DeleteSelfDirT() throw (LLBC_IOException)
{
    if (this->DeleteSelfDir() != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

void LLBC_Directory::DeleteDirT(const LLBC_String &dirName) throw (LLBC_IOException)
{
    if (LLBC_Directory::DeleteDir(dirName) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

LLBC_String LLBC_Directory::GetCurrentDirectoryT() throw (LLBC_IOException)
{
    LLBC_String cwd = LLBC_Directory::GetCurrentDirectory();
    if (cwd.empty() && LLBC_GetLastError() != LLBC_ERROR_SUCCESS)
        throw LLBC_IOException(LLBC_FormatLastError());

    return cwd;
}

void LLBC_Directory::SetCurrentDirectoryT(const LLBC_String &curDir) throw (LLBC_IOException)
{
    if (LLBC_Directory::SetCurrentDirectory(curDir) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

__LLBC_NS_END

#if LLBC_TARGET_PLATFORM_WIN32
 #pragma warning(default:4290)
#endif

#include "llbc/common/AfterIncl.h"
