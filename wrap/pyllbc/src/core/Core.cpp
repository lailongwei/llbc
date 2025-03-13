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


#include "pyllbc/common/Export.h"

#include "pyllbc/core/Core.h"

#include "src/core/_CoreMethods.h"

void pyllbc_InitCore()
{
}

void pyllbc_DestroyCore()
{
}

void pyllbc_AddCoreMethods()
{
    pyllbc_Module *inl = pyllbc_InlModule;

    // core/guid
    pyllbc_CoreMethods &methods = *pyllbc_s_CoreMethods;
    inl->AddMethod(methods.GenGUID);

    // core/file
    pyllbc_Module *top = pyllbc_TopModule;
    top->AddMethod(methods.GetModuleFilePath);

    // core/bundle
    inl->AddMethod(methods.NewBundle);
    inl->AddMethod(methods.DelBundle);
    inl->AddMethod(methods.GetMainBundle);
    inl->AddMethod(methods.GetBundlePath);
    inl->AddMethod(methods.GetBundleName);
    inl->AddMethod(methods.InitBundle);
    inl->AddMethod(methods.GetBundleResPath);

    // core/random
    inl->AddMethod(methods.NewRandom);
    inl->AddMethod(methods.DelRandom);
    inl->AddMethod(methods.SeedRand);
    inl->AddMethod(methods.Rand);
    inl->AddMethod(methods.RandReal);
    inl->AddMethod(methods.BoolJudge);

    // core/config
    inl->AddMethod(methods.Properties_LoadFromString);
    inl->AddMethod(methods.Properties_LoadFromFile);
    inl->AddMethod(methods.Properties_SaveToString);
    inl->AddMethod(methods.Properties_SaveToFile);

    // core/log
    inl->AddMethod(methods.InitLoggerMgr);
    inl->AddMethod(methods.UnInitLoggerMgr);
    inl->AddMethod(methods.LogMsg);

    // core/utils
    top->AddMethod(methods.refcnt);
    inl->AddMethod(methods.Hash);

    // core/event
    inl->AddMethod(methods.Event_New);
    inl->AddMethod(methods.Event_Del);
    inl->AddMethod(methods.Event_GetItem);
    inl->AddMethod(methods.Event_SetItem);
}

void pyllbc_AddCoreObjs()
{
}
