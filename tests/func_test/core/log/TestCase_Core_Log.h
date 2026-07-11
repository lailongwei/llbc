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

#pragma once

#include "llbc.h"
using namespace llbc;

class TestCase_Core_Log final : public LLBC_BaseTestCase
{
public:
    TestCase_Core_Log();
    ~TestCase_Core_Log() override;

public:
    int Run(int argc, char *argv[]) override;

private:
    void DoLogLevelSetTest();
    void DoJsonLogTest();
    void DoUninitLogTest();
    void SyncLoggerMultiThreadTest();
    void DoConditionMacroLogTest();
    int DoLoggerMgrReloadTest();
    void DoLogColorFilterTest();
    void DoLogTraceEnableLevelTest();

    void DoLogTraceTest();
    template <typename _KeyTy, typename _ContentTy>
    void AddLogTrace(const _KeyTy &key, const _ContentTy &content);

    void OnLogHook(const LLBC_LogData *logData);

private:
    LLBC_String _logCfgFilePath;
};
