/**
 * @file    LogConsoleAppender.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/10
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_Console.h"

#include "llbc/core/log/LogData.h"
#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/LogTokenChain.h"
#include "llbc/core/log/LogConsoleAppender.h"

namespace
{
    typedef LLBC_NS LLBC_LogLevel _LogLevel;
}

//https://en.wikipedia.org/wiki/ANSI_escape_code
#if LLBC_TARGET_PLATFORM_NON_WIN32
namespace __AnsiColor
{
    const char* Fg_Hightlight_Yellow = "\x1B[93m";
    const char* Fg_Hightlight_Red    = "\x1B[91m";
    const char* Reset                = "\x1B[0m";
}
#endif

__LLBC_NS_BEGIN

LLBC_LogConsoleAppender::LLBC_LogConsoleAppender()
: _colourfulOutput(LLBC_CFG_LOG_DEFAULT_ENABLED_COLOURFUL_OUTPUT)
{
}

LLBC_LogConsoleAppender::~LLBC_LogConsoleAppender()
{
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
    LLBC_LogTokenChain *chain = NULL;
    if (UNLIKELY(!(chain = GetTokenChain())))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    const int logLevel = data.level;
    if (logLevel < GetLogLevel())
        return LLBC_OK;

    FILE * const out = logLevel >= _LogLevel::Warn ? stderr : stdout;

    LLBC_String formattedData;
    chain->Format(data, formattedData);

#if LLBC_TARGET_PLATFORM_WIN32
    int oldOutputColor = 0;
    if (_colourfulOutput)
    {
        oldOutputColor = LLBC_GetConsoleColor(out);

        const int outputColor = DetermineLogTextColor(logLevel);
        LLBC_SetConsoleColor(out, outputColor);
    }

    LLBC_FilePrint(out, "%s", formattedData.c_str());
#else
    const char *outputColor = NULL;
    if (_colourfulOutput && (outputColor = DetermineAnsiTextColor(logLevel)) != NULL)
        LLBC_FilePrint(out, "%s%s%s", outputColor, formattedData.c_str(), __AnsiColor::Reset);
    else
        LLBC_FilePrint(out, "%s", formattedData.c_str());
#endif

#if LLBC_CFG_LOG_DIRECT_FLUSH_TO_CONSOLE
    if (logLevel < _LogLevel::Warn) 
        LLBC_FlushFile(stdout);
#endif

#if LLBC_TARGET_PLATFORM_WIN32
    if (_colourfulOutput)
        LLBC_SetConsoleColor(out, oldOutputColor);
#endif

    return LLBC_OK;
}

#if LLBC_TARGET_PLATFORM_WIN32
int LLBC_LogConsoleAppender::DetermineLogTextColor(int logLv)
{
    typedef LLBC_ConsoleColor _CC;

    if (logLv == _LogLevel::Warn)
        return _CC::Bg_Black | _CC::Fg_Yellow | _CC::Highlight_Fg;
    else if (logLv == _LogLevel::Error ||
        logLv == _LogLevel::Fatal)
        return _CC::Bg_Black | _CC::Fg_Red | _CC::Highlight_Fg;
    else
        return _CC::Bg_Black | _CC::Fg_White;
}
#else
const char *LLBC_LogConsoleAppender::DetermineAnsiTextColor(int logLv)
{
    switch (logLv)
    {
    case _LogLevel::Warn:
        return __AnsiColor::Fg_Hightlight_Yellow;
    case _LogLevel::Error:
    case _LogLevel::Fatal:
        return __AnsiColor::Fg_Hightlight_Red;
    default:
        return NULL;
    }
}
#endif

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
