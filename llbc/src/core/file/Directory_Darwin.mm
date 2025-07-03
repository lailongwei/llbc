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
#import <Foundation/Foundation.h>
 __LLBC_NS_BEGIN
LLBC_String LLBC_Directory::ModuleFilePath(bool readLink)
{
    if (readLink)
    {
        char buf[PATH_MAX + 1];
        uint32 size = sizeof(buf);
        if (_NSGetExecutablePath(buf, &size) != 0)
        {
            LLBC_SetLastError(LLBC_ERROR_CLIB);
            return "";
        }

        return LLBC_String(buf, size);
    }
    else
    {
        NSProcessInfo *processInfo = [NSProcessInfo processInfo];
        NSString *executablePath = [[processInfo arguments] objectAtIndex:0];
        return AbsPath([executablePath UTF8String]);
    }
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
