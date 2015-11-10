/**
 * @file    TestCase_Core_Bundle.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/08/25
 * @version 1.0
 *
 * @brief
 */

#include "core/bundle/TestCase_Core_Bundle.h"

TestCase_Core_Bundle::TestCase_Core_Bundle()
{
}

TestCase_Core_Bundle::~TestCase_Core_Bundle()
{
}

int TestCase_Core_Bundle::Run(int argc, char *argv[])
{
    LLBC_PrintLine("core/bundle test:");
    
    // Test main bundle.
    const LLBC_Bundle *mainBundle = LLBC_Bundle::GetMainBundle();
    LLBC_PrintLine("Main bundle handle: %p", mainBundle);
    LLBC_PrintLine("Main bundle path: %s", mainBundle->GetBundlePath().c_str());
    LLBC_PrintLine("Main bundle file name: %s", mainBundle->GetBundleName().c_str());
    
    // Try to access resource: Default.png.
#if LLBC_TARGET_PLATFORM_NON_IPHONE
    LLBC_File::TouchT(mainBundle->GetBundlePath() + "/" + "Default.png");
#endif // LLBC_TARGET_PLATFORM_NON_IPHONE
    LLBC_PrintLine("Default.png path: %s", mainBundle->GetResPath("Default", "png").c_str());
    
    // Test create bundle.
    LLBC_Bundle myBundle;
    LLBC_String myBundlePath = "adfasdkfasfas";
    if(myBundle.Initialize(myBundlePath) != LLBC_RTN_OK)
    {
        LLBC_PrintLine("Create bundle with path [%s] failed, reason: %s, right!!!!", myBundlePath.c_str(), LLBC_FormatLastError());
    }
    else
    {
        LLBC_PrintLine("Try to create not exist's bundle[%s], but return successed, please check it!!!", myBundlePath.c_str());
    }
    
    if(myBundle.Initialize("") != LLBC_RTN_OK)
    {
        LLBC_PrintLine("Create main bundle failed, reason: %s, please check it!!!", LLBC_FormatLastError());
    }
    else
    {
        LLBC_PrintLine("Create main bundle successed!");
        LLBC_PrintLine("New main bundle path: %s", myBundle.GetBundlePath().c_str());
        LLBC_PrintLine("New main bundle name: %s", myBundle.GetBundleName().c_str());
        
        LLBC_PrintLine("Access Default.png with new main bundle, return path: %s", myBundle.GetResPath("Default.png").c_str());
    }
    
#if LLBC_TARGET_PLATFORM_NON_IPHONE
    LLBC_File::DeleteT(mainBundle->GetBundlePath() + "/" + "Default.png");
#endif // LLBC_TARGET_PLATFORM_NON_IPHONE
    
    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return 0;
}
