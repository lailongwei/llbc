local Directory = llbc.newclass('llbc.Directory')

-- Check given path exist or not.
-- @param path - the given path.
-- @return bool - return bool if given path exist, otherwise return false.
Directory.exists = _llbc.Dir_Exists

-- Check given path is directory or not.
-- @param path - the given path.
-- @return bool - return true if is given path is directory, otherwise return false.
Directory.is_dir = _llbc.Dir_IsDir

-- Create directory.
-- @param path - the directory path.
Directory.create = _llbc.Dir_Create

-- Remove directory.
-- @param path - the directory path.
Directory.remove = _llbc.Dir_Remove

-- Check given path is absolute path or not.
-- @param path - the given path.
-- @return bool - return true if given path is absolute path, otherwise return false.
Directory.is_abspath = _llbc.Dir_IsAbsPath

-- Convert path to absolute path.
-- @param path - the given path.
-- @return string - the converted absolute path.
Directory.abspath = _llbc.Dir_AbsPath

-- Join directory.
-- @param ... - the directories.
-- @return string - the joined directory.
Directory.join = _llbc.Dir_Join

-- Split extension.
-- @param path - the path.
-- @return path[string], extension[string] - the splited parts.
Directory.split_ext = _llbc.Dir_SplitExt

-- Get directory files.
-- @param path - the path.
-- @param recursive - recursive flag, default is false.
-- @return {...} - the files.
Directory.get_files = _llbc.Dir_GetFiles

-- Get directory sub-directories.
-- @param path - the path.
-- @param recursive - recursive flag, default is false.
-- @return {...} - the sub-directories.
Directory.get_directories = _llbc.Dir_GetDirectories

-- Get basename.
-- @param path - the path.
-- @return string - the basename.
Directory.basename = _llbc.Dir_BaseName

-- Get dirname.
-- @param string - the path.
-- @return string - the dirname.
Directory.dirname = _llbc.Dir_DirName

-- Get current directory.
-- @return string - the current directory.
Directory.curdir = _llbc.Dir_CurDir

-- Set current directory.
-- @param curdir - the new current directory.
Directory.set_curdir = _llbc.Dir_SetCurDir

llbc.Directory = Directory