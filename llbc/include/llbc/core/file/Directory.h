/**
 * @file    Directory.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/02/17
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_FILE_DIRECTORY_H__
#define __LLBC_CORE_FILE_DIRECTORY_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief The directory operations encapsulation.
 */
class LLBC_EXPORT LLBC_Directory
{
public:
    /**
     * Check given path's directory exists or not.
     * @param[in] path - the given path.
     * @return bool - return true if given path exists and is directory.
     */
    static bool Exists(const LLBC_String &path);

    /**
     * Recursive create directory.
     * @param[in] path - the will create directory path.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int Create(const LLBC_String &path);

    /**
     * Recursive remove directory.
     * @param[in] path - the will remove path.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int Remove(const LLBC_String &path);

public:
    /**
     * Convert path to normalized absolutize path.
     * @return LLBC_String - the normalized absolutize path.
     */
    static LLBC_String AbsPath(const LLBC_String &path);

    /**
     * Join one or more path components intelligently.
     * @param[in] path1~7 - the path components.
     * @param[in] paths   - the paths list.
     * @return LLBC_String - the joined path.
     */
    static LLBC_String Join(const LLBC_String &path1,
                            const LLBC_String &path2);
    static LLBC_String Join(const LLBC_String &path1,
                            const LLBC_String &path2,
                            const LLBC_String &path3);
    static LLBC_String Join(const LLBC_String &path1,
                            const LLBC_String &path2,
                            const LLBC_String &path3,
                            const LLBC_String &path4);
    static LLBC_String Join(const LLBC_String &path1,
                            const LLBC_String &path2,
                            const LLBC_String &path3,
                            const LLBC_String &path4,
                            const LLBC_String &path5);
    static LLBC_String Join(const LLBC_String &path1,
                            const LLBC_String &path2,
                            const LLBC_String &path3,
                            const LLBC_String &path4,
                            const LLBC_String &path5,
                            const LLBC_String &path6);
    static LLBC_String Join(const LLBC_String &path1,
                            const LLBC_String &path2,
                            const LLBC_String &path3,
                            const LLBC_String &path4,
                            const LLBC_String &path5,
                            const LLBC_String &path6,
                            const LLBC_String &path7);
    static LLBC_String Join(const LLBC_Strings &paths);
    static LLBC_String Join(const LLBC_String &path1,
                            const LLBC_Strings &paths);

    /**
     * Split file extension, always success.
     * @param[in] path - the file path.
     * @return LLBC_Strings - the splited parts, patrts[0] is not has extension path, parts[1] is the file extension.
     */
    static LLBC_Strings SplitExt(const LLBC_String &path);

    /**
     * Get specific path's files, not include directory type files.
     * @param[in]  path      - the path.
     * @param[out] files     - all given path's files.
     * @param[in] recursive  - recursive flag, if set to true, function will recursive scan path, default is false.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int GetFiles(const LLBC_String &path, std::vector<LLBC_String> &files, bool recursive = false);

    /**
     * Recursive specific path's directories.
     * @param[in] path         - the path.
     * @param[out] directories - the given path's directories.
     * @param[in] recursive    - recursive flag, default is false.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int GetDirectories(const LLBC_String &path, LLBC_Strings &directories, bool recursive = false);

public:
    /**
     * Get current executable module file name.
     * @return LLBC_String - the module file name.
     */
    static LLBC_String ModuleFileName();

    /**
     * Get current executable module file directory.
     * @return LLBC_String - the module file directory.
     */
    static LLBC_String ModuleFileDir();

    /**
     * Get given path directory name.
     * @param[in] path - the path.
     * @return LLBC_String - the directory name for the path.
     */
    static LLBC_String DirName(const LLBC_String &path);

    /**
     * Get given path base name.
     * @param[in] path - the path.
     * @return LLBC_String - the base name for the path.
     */
    static LLBC_String BaseName(const LLBC_String &path);

public:
    /**
     * Get current directory.
     * @return LLBC_String - current directory.
     */
    static LLBC_String CurDir();

    /**
     * Set current directory.
     * @param[in] path - the new current directory.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int SetCurDir(const LLBC_String &path);

    /**
     * Get document directory.
     * @return LLBC_String - document directory.
     */
    static LLBC_String DocDir();

    /**
     * Get home directory.
     * @return LLBC_String - the home directory.
     */
    static LLBC_String HomeDir();

    /**
     * Get temporary directory.
     * @return LLBC_String - the temporary directory.
     */
    static LLBC_String TempDir();

    /**
     * Get cache directory.
     * @return LLBC_String - the cache directory.
     */
    static LLBC_String CacheDir();
};

__LLBC_NS_END

#endif // !__LLBC_CORE_FILE_DIRECTORY_H__
