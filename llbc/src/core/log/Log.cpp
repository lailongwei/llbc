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
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_Console.h"
#include "llbc/core/thread/Guard.h"
#include "llbc/core/utils/Util_Debug.h"

#include "llbc/core/log/Logger.h"
#include "llbc/core/log/LoggerManager.h"

#include "llbc/core/log/Log.h"

__LLBC_INTERNAL_NS_BEGIN

static LLBC_NS LLBC_LogHelper __llbc_logHelper;

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_LogHelper &__LLBC_GetLogHelper()
{
    return LLBC_INTERNAL_NS __llbc_logHelper;
}

LLBC_Logger *LLBC_LogHelper::_rootLogger = nullptr;
LLBC_LoggerManager *LLBC_LogHelper::_loggerManager = nullptr;

int LLBC_LogHelper::init(const LLBC_String &cfgFile)
{
    return LLBC_LoggerManagerSingleton->Initialize(cfgFile);
}

void LLBC_LogHelper::destroy()
{
    LLBC_LoggerManagerSingleton->Finalize();
}

void LLBC_LogHelper::Initialize(LLBC_LoggerManager *loggerManager)
{
    if (UNLIKELY(_rootLogger))
    {
        trace("llbc library internal error, repeat to init Log");
        return;
    }

    _loggerManager = loggerManager;
    _rootLogger = _loggerManager->GetRootLogger();
}

void LLBC_LogHelper::Finalize()
{
    if (UNLIKELY(!_rootLogger))
    {
        trace("llbc library internal error, try to destroy uninit Log");
        return;
    }

    _loggerManager = nullptr;
    _rootLogger = nullptr;
}

void LLBC_LogHelper::UnInitOutput(FILE *to, const char *msg)
{
    LLBC_FilePrint(to, "[Log] %s\n", msg);
    if (to != stderr)
        LLBC_FlushFile(to);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
