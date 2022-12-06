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


#include "llbc/common/Export.h"

#include "llbc/core/objectpool/IObjectPoolInst.h"

#include "llbc/core/log/LogData.h"

__LLBC_NS_BEGIN
LLBC_LogData::LLBC_LogData()
: logger(nullptr)
, loggerName(nullptr)

, msg(nullptr)
, msgLen(0)
, msgCap(0)

, level(-1)
, logTime(0)

, staticFile(nullptr)
, staticFunc(nullptr)
, logCodeFilePath(false)

, fileLen(0)
, funcLen(0)
, tagLen(0)

, line(0)

, threadId(LLBC_INVALID_NATIVE_THREAD_ID)

, _poolInst(nullptr)
{
}

LLBC_LogData::~LLBC_LogData()
{
    if (msg)
        free(msg);
}

void LLBC_LogData::Prepare()
{
    if (staticFile && !logCodeFilePath)
    {
        #if LLBC_TARGET_PLATFORM_WIN32
        const char *ps = strrchr(staticFile, '\\');
        if (ps == nullptr)
            ps = strrchr(staticFile, '/');
        #else // Non-Win32
        const char *ps = strrchr(file, '/');
        #endif // Win32

        if (ps != nullptr)
            staticFile = ps + 1;
    }
}

void LLBC_LogData::Clear()
{
    logger = nullptr;
    loggerName = nullptr;

    msgLen = 0;

    level = -1;
    logTime = 0;

    staticFile = nullptr;
    staticFunc = nullptr;
    logCodeFilePath = false;

    fileLen = 0;
    funcLen = 0;
    tagLen = 0;

    line = 0;

    threadId = LLBC_INVALID_NATIVE_THREAD_ID;
}

void LLBC_LogData::MarkPoolObject(LLBC_IObjectPoolInst &poolInst)
{
    _poolInst = &poolInst;
}

LLBC_IObjectPoolInst *LLBC_LogData::GetPoolInst()
{
    return _poolInst;
}

__LLBC_NS_END
