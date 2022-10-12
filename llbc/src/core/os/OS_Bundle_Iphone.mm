/**
 * @file    OS_Bundle_Iphone.mm
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/08/25
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"

#include "llbc/core/os/OS_Bundle.h"

#if LLBC_TARGET_PLATFORM_IPHONE
#import <Foundation/Foundation.h>

__LLBC_NS_BEGIN

LLBC_String LLBC_GetMainBundlePath()
{
    NSBundle *mainBundle = [NSBundle mainBundle];
    return [mainBundle.bundlePath UTF8String];
}

LLBC_BundleHandle LLBC_CreateBundle(const LLBC_String &path)
{
    if(path.empty())
    {
        return [[NSBundle mainBundle] retain];
    }
    
    NSString *nsPath = [NSString stringWithUTF8String:path.c_str()];
    NSBundle *bundle = [[NSBundle alloc] initWithPath:nsPath];

    if(bundle == nil)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_INVALID_BUNDLE_HANDLE;
    }

    return bundle;
}

void LLBC_ReleaseBundle(LLBC_BundleHandle bundle)
{
    if(bundle != LLBC_INVALID_BUNDLE_HANDLE)
    {
        [(NSBundle *)bundle release];
    }
}

LLBC_String LLBC_GetBundlePath(LLBC_BundleHandle bundle)
{
    if(bundle == LLBC_INVALID_BUNDLE_HANDLE)
    {
        bundle = [NSBundle mainBundle];
    }
    
    return [((NSBundle *)bundle).bundlePath UTF8String];
}

LLBC_String LLBC_GetBundleResPath(LLBC_BundleHandle bundle, const LLBC_String &name)
{
    if(UNLIKELY(name.empty()))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return "";
    }

    if(bundle == LLBC_INVALID_BUNDLE_HANDLE)
    {
        bundle = [NSBundle mainBundle];
    }

    NSString *nsName = [NSString stringWithUTF8String:name.c_str()];
    NSString *nsPath = [(NSBundle *)bundle pathForResource:nsName ofType:@""];
    if(nsPath == nil)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return "";
    }

    return [nsPath UTF8String];
}

LLBC_String LLBC_GetBundleResPath(LLBC_BundleHandle bundle, const LLBC_String &name, const LLBC_String &ext)
{
    if(UNLIKELY(name.empty() && ext.empty()))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return "";
    }

    if(bundle == LLBC_INVALID_BUNDLE_HANDLE)
    {
        bundle = [NSBundle mainBundle];
    }
    
    NSString *nsName = [NSString stringWithUTF8String:name.c_str()];
    NSString *nsExt = [NSString stringWithUTF8String:ext.c_str()];
    NSString *nsPath = [(NSBundle *)bundle pathForResource:nsName ofType:nsExt];
    if(nsPath == nil)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return "";
    }

    return [nsPath UTF8String];
}

LLBC_String LLBC_GetBundleResPath(LLBC_BundleHandle bundle, const LLBC_String &name, const LLBC_String &ext, const LLBC_String &inDir)
{
    if(UNLIKELY(name.empty() && ext.empty()))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return "";
    }
    
    if(bundle == LLBC_INVALID_BUNDLE_HANDLE)
    {
        bundle = [NSBundle mainBundle];
    }
    
    NSString *nsName = [NSString stringWithUTF8String:name.c_str()];
    NSString *nsExt = [NSString stringWithUTF8String:ext.c_str()];
    NSString *nsInDir = [NSString stringWithUTF8String:inDir.c_str()];
    NSString *nsPath = [(NSBundle *)bundle pathForResource:nsName ofType:nsExt inDirectory:nsInDir];
    if(nsPath == nil)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return "";
    }
    
    return [nsPath UTF8String];
}

__LLBC_NS_END

#endif // LLBC_TARGET_PLATFORM_IPHONE

