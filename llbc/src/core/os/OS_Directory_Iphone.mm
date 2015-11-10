/**
 * @file    OS_Directory_Iphone.mm
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/08/24
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/ExportHeader.h"

#include "llbc/core/os/OS_Directory.h"

#if LLBC_TARGET_PLATFORM_IPHONE

__LLBC_NS_BEGIN

LLBC_String LLBC_GetHomeDirectory(bool appendSlash)
{
    NSString *homePath = NSHomeDirectory();
    LLBC_String path = [homePath UTF8String];
    if(appendSlash)
    {
        path.append(1, LLBC_SLASH_A);
    }
    
    return path;
}

LLBC_String LLBC_GetDocumentDirectory(bool appendSlash)
{
    NSArray *docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *docPath = [docPaths objectAtIndex:0];
    LLBC_String path = [docPath UTF8String];
    if(appendSlash)
    {
        path.append(1, LLBC_SLASH_A);
    }
    
    return path;
}

LLBC_String LLBC_GetTemporaryDirectory(bool appendSlash)
{
    NSString *tempPath = NSTemporaryDirectory();
    LLBC_String path = [tempPath UTF8String];
    if(!appendSlash)
    {
        path = path.substr(0, path.size() - 1);
    }
    
    return path;
}

LLBC_String LLBC_GetCacheDirectory(bool appendSlash)
{
    NSArray *cachePaths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *cachePath = [cachePaths objectAtIndex:0];
    LLBC_String path = [cachePath UTF8String];
    if(appendSlash)
    {
        path.append(1, LLBC_SLASH_A);
    }
    
    return path;
}

__LLBC_NS_END 

#endif // LLBC_TARGET_PLATFORM_IPHONE

