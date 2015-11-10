/**
 * @file    Directory.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2011/12/14
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_FILE_DIRECTORY_H__
#define __LLBC_CORE_FILE_DIRECTORY_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN
class LLBC_File;
__LLBC_NS_END

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(disable:4290)
#endif

__LLBC_NS_BEGIN

/**
 * \brief Directory class encapsulation.
 */
class LLBC_EXPORT LLBC_Directory
{
public:
	LLBC_Directory();
	virtual ~LLBC_Directory();

public:
    /**
     * Get directory handle, unsafe method.
     */
	LLBC_DirHandle Handle() const;

public:
    /**
     * Set directory.
     * @param[in] dir - directory name.
     */
	int SetDir(const LLBC_String &dir);

    /**
     * Check directory exist or not.
     * @return bool - return true if successed, otherwise return false.
     */
	bool Exist() const;

    /**
     * Get directory name.
     * @return LLBC_String - directory name.
     */
	LLBC_String GetDir() const;
	LLBC_String GetFullDir() const;

    /**
     * Get current directory's file list, not include directory types file.
     * @param[out] files    - files.
     * @param[in] filterStr - filter string, regular expression format, now not support.
     * @param[in] recur     - recursion flag, if set to true, this function will recur to scan directory,
     *                        default is false.
     * @return std::deque<LLBC_String> - file name list.
     */
    int GetFiles(std::vector<LLBC_String> &files,
                 const LLBC_String &filterStr, 
                 bool recur = false) const;

    /**
     * Get files, like another GetFiles() method, return must use LLBC_XDeletes(ptr) to delete.
     */
	LLBC_File *GetFiles(size_t &fileCount,
                        const LLBC_String &filterStr,
                        const LLBC_String &mode = "r",
                        bool recur = false) const;

    /**
     * Get current directory's sub directory list.
     * @param[out] directories - directories array.
     * @param[in] filterStr    - filter string, reqular expression format, now not support.
     * @param[in] recur        - recursion flag.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int GetDirectories(std::vector<LLBC_String> &directories,
                       const LLBC_String &filterStr, 
                       bool recur = false) const;

    /**
     * Get specified directory's sub directory list.
     */
	LLBC_Directory *GetDirectories(size_t &directoryCount,
                                   const LLBC_String &filterStr,
                                   bool recur = false) const;

    /**
     * Create directory.
     */
	int CreateSelfDir();
	static int CreateDir(const LLBC_String &dirName);

    /**
     * Create sub directory.
     */
	int CreateSubDir(const LLBC_String &subDirName);

    /**
     * Delete directory.
     */
	int DeleteSelfDir();
	static int DeleteDir(const LLBC_String &dirName);

    /**
     * Get/Set current directory.
     */
	static LLBC_String GetCurrentDirectory();
    static int SetCurrentDirectory(const LLBC_String &curDir);

    /* Exception throw APIs. */
public:
    void SetDirT(const LLBC_String &dir) throw (LLBC_IOException);

	LLBC_String GetDirT() const throw (LLBC_IOException);
	LLBC_String GetFullDirT() const throw (LLBC_IOException);

    void GetFilesT(std::vector<LLBC_String> &files,
                   const LLBC_String &filterStr, 
                   bool recur = false) const throw (LLBC_IOException);

	LLBC_File *GetFilesT(size_t &fileCount,
                         const LLBC_String &filterStr,
                         const LLBC_String &mode = "r",
                         bool recur = false) const throw (LLBC_IOException);

    void GetDirectoriesT(std::vector<LLBC_String> &directories,
                         const LLBC_String &filterStr, 
                         bool recur = false) const throw (LLBC_IOException);

	LLBC_Directory *GetDirectoriesT(size_t &directoryCount,
                                    const LLBC_String &filterStr,
                                    bool recur = false) const throw (LLBC_IOException);

	void CreateSelfDirT() throw (LLBC_IOException);
	static void CreateDirT(const LLBC_String &dirName) throw (LLBC_IOException);

	void CreateSubDirT(const LLBC_String &subDirName) throw (LLBC_IOException);

	void DeleteSelfDirT() throw (LLBC_IOException);
	static void DeleteDirT(const LLBC_String &dirName) throw (LLBC_IOException);

	static LLBC_String GetCurrentDirectoryT() throw (LLBC_IOException);
    static void SetCurrentDirectoryT(const LLBC_String &curDir) throw (LLBC_IOException);

private:
	LLBC_String _directoryName;
};

__LLBC_NS_END

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(default:4290)
#endif

#endif // !__LLBC_CORE_FILE_DIRECTORY_H__
