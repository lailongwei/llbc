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

#include "llbc/core/os/OS_Console.h"

#include "llbc/core/log/LogData.h"
#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/LogTokenChain.h"
#include "llbc/core/log/LogConsoleAppender.h"

#if LLBC_TARGET_PLATFORM_WIN32
#include "llbc/core/thread/Guard.h"
#endif  // LLBC_TARGET_PLATFORM_WIN32

namespace
{
    typedef LLBC_NS LLBC_LogLevel _LogLevel;
}

__LLBC_NS_BEGIN

#if LLBC_TARGET_PLATFORM_WIN32
LLBC_FastLock LLBC_LogConsoleAppender::_colorLock;
#endif  // LLBC_TARGET_PLATFORM_WIN32

LLBC_LogConsoleAppender::LLBC_LogConsoleAppender()
: _colourfulOutput(LLBC_CFG_LOG_DEFAULT_COLOURFUL_OUTPUT)
{
}

LLBC_LogConsoleAppender::~LLBC_LogConsoleAppender()
{
    Finalize();
}

int LLBC_LogConsoleAppender::GetType() const
{
    return LLBC_LogAppenderType::Console;
}

int LLBC_LogConsoleAppender::Initialize(const LLBC_LogAppenderInitInfo &initInfo)
{
    if (_Base::Initialize(initInfo) != LLBC_OK)
        return LLBC_FAILED;

    _colourfulOutput = initInfo.colourfulOutput;

    return LLBC_OK;
}

void LLBC_LogConsoleAppender::Finalize()
{
    _Base::Finalize();
}

int LLBC_LogConsoleAppender::Output(const LLBC_LogData &data)
{
    LLBC_LogTokenChain *chain = GetTokenChain();
    if (UNLIKELY(!chain))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    const int logLevel = data.level;
    if (logLevel < GetLogLevel())
        return LLBC_OK;

    FILE *out = nullptr;
    if (logLevel >= _LogLevel::Warn)
    {
        out = stderr;
        #if !LLBC_CFG_LOG_DIRECT_FLUSH_TO_CONSOLE
        LLBC_FlushFile(stdout);
        #endif
    }
    else
    {
        out = stdout;
    }

    LLBC_String &logFmtBuf = GetLogFormatBuf();
    logFmtBuf.clear();
    chain->Format(data, logFmtBuf);

#if LLBC_TARGET_PLATFORM_WIN32
    LLBC_LockGuard colorLock(_colorLock);
#endif //LLBC_TARGET_PLATFORM_WIN32

    int oldOutputColor = 0;
    if (_colourfulOutput)
    {
        oldOutputColor = LLBC_GetConsoleColor(out);
        LLBC_SetConsoleColor(out, DetermineLogTextColor(logLevel));
    }

    LLBC_FilePrint(out, "%s", logFmtBuf.c_str());

#if LLBC_CFG_LOG_DIRECT_FLUSH_TO_CONSOLE
    if (logLevel < _LogLevel::Warn) 
        LLBC_FlushFile(stdout);
#endif

    if (_colourfulOutput)
        LLBC_SetConsoleColor(out, oldOutputColor);

    return LLBC_OK;
}

int LLBC_LogConsoleAppender::DetermineLogTextColor(int logLv)
{
    typedef LLBC_ConsoleColor _CC;

    if (logLv == _LogLevel::Warn)
        return _CC::Bg_Default | _CC::Fg_Yellow | _CC::Highlight_Fg;
    else if (logLv == _LogLevel::Error || logLv == _LogLevel::Fatal)
        return _CC::Bg_Default | _CC::Fg_Red | _CC::Highlight_Fg;
    else
        return _CC::Bg_Default | _CC::Fg_Default;
}

__LLBC_NS_END
