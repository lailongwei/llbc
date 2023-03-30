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


#include "core/bundle/TestCase_Core_Bundle.h"

TestCase_Core_Bundle::TestCase_Core_Bundle()
{
}

TestCase_Core_Bundle::~TestCase_Core_Bundle()
{
}

int TestCase_Core_Bundle::Run(int argc, char *argv[])
{
    LLBC_PrintLn("core/bundle test:");
    
    // Test main bundle.
    const LLBC_Bundle *mainBundle = LLBC_Bundle::GetMainBundle();
    LLBC_PrintLn("Main bundle handle: %p", mainBundle);
    LLBC_PrintLn("Main bundle path: %s", mainBundle->GetBundlePath().c_str());
    LLBC_PrintLn("Main bundle file name: %s", mainBundle->GetBundleName().c_str());
    
    // Try to access resource: Default.png.
#if LLBC_TARGET_PLATFORM_NON_IPHONE
    LLBC_File::TouchFile(mainBundle->GetBundlePath() + "/" + "Default.png");
#endif // LLBC_TARGET_PLATFORM_NON_IPHONE
    LLBC_PrintLn("Default.png path: %s", mainBundle->GetResPath("Default", "png").c_str());
    
    // Test create bundle.
    LLBC_Bundle myBundle;
    LLBC_String myBundlePath = "adfasdkfasfas";
    if(myBundle.Initialize(myBundlePath) != LLBC_OK)
    {
        LLBC_PrintLn("Create bundle with path [%s] failed, reason: %s, right!!!!", myBundlePath.c_str(), LLBC_FormatLastError());
    }
    else
    {
        LLBC_PrintLn("Try to create not exist's bundle[%s], but return successed, please check it!!!", myBundlePath.c_str());
    }
    
    if(myBundle.Initialize("") != LLBC_OK)
    {
        LLBC_PrintLn("Create main bundle failed, reason: %s, please check it!!!", LLBC_FormatLastError());
    }
    else
    {
        LLBC_PrintLn("Create main bundle successed!");
        LLBC_PrintLn("New main bundle path: %s", myBundle.GetBundlePath().c_str());
        LLBC_PrintLn("New main bundle name: %s", myBundle.GetBundleName().c_str());
        
        LLBC_PrintLn("Access Default.png with new main bundle, return path: %s", myBundle.GetResPath("Default.png").c_str());
    }
    
#if LLBC_TARGET_PLATFORM_NON_IPHONE
    LLBC_File::DeleteFile(mainBundle->GetBundlePath() + "/" + "Default.png");
#endif // LLBC_TARGET_PLATFORM_NON_IPHONE
    
    LLBC_PrintLn("Press any key to continue ...");
    getchar();

    return 0;
}
