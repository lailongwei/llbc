/**
 * @file	Directory_Iphone.mm
 * @author	Longwei Lai<lailongwei@126.com>
 * @date	2016/02/23
 * @version	1.0
 *
 * @brief
 */

 #include "llbc/common/Export.h"

 #include "llbc/core/file/Directory.h"

// iPhone/Macosx implementation:
#if LLBC_TARGET_PLATFORM_IPHONE || LLBC_TARGET_PLATFORM_MAC
 #include <mach-o/dyld.h>
 __LLBC_NS_BEGIN
LLBC_String LLBC_Directory::ModuleFilePath(bool readLink)
{
    // Get executable path.
    auto &commonTls = __LLBC_GetLibTls()->commonTls;
    auto &pathBuf = commonTls.pathBuf;
    uint32 size = sizeof(pathBuf);
    if (UNLIKELY(_NSGetExecutablePath(pathBuf, &size) != 0))
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return "";
    }

    // If dont't need read link, return.
    if (!readLink)
        return pathBuf;

    // Get real path && return.
    // Note: Use libTls.commonTls.rtti buf to call realpath() for improve performance.
    auto &rttiBuf = commonTls.rtti;
    static_assert(sizeof(rttiBuf) >= sizeof(pathBuf),
                  "llbc framework internal error, rtti buf size must be greater then or equal to path buf size");
    if (UNLIKELY(realpath(pathBuf, rttiBuf) == nullptr))
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return "";
    }

    return rttiBuf;
}
__LLBC_NS_END
#endif // iPhone/Macosx

// iPhone only implementation:
 #if LLBC_TARGET_PLATFORM_IPHONE
 __LLBC_NS_BEGIN
LLBC_String LLBC_Directory::DocDir()
{
    NSArray *docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *docPath = [docPaths objectAtIndex:0];
    return [docPath UTF8String];
}

LLBC_String LLBC_Directory::HomeDir()
{
	NSString *homePath = NSHomeDirectory();
	return [homePath UTF8String];
}

LLBC_String LLBC_Directory::TempDir()
{
	NSString *tempPath = NSTemporaryDirectory();
	return [tempPath UTF8String];
}

LLBC_String LLBC_Directory::CacheDir()
{
    NSArray *cachePaths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *cachePath = [cachePaths objectAtIndex:0];
    return [cachePath UTF8String];
}
 __LLBC_NS_END
 #endif // iPhone
