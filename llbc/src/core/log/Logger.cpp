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

#include "llbc/core/utils/Util_Text.h"

#include "llbc/core/os/OS_Time.h"

#include "llbc/core/thread/Guard.h"
#include "llbc/core/thread/MessageBlock.h"

#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/LogData.h"
#include "llbc/core/log/LoggerConfigInfo.h"
#include "llbc/core/log/ILogAppender.h"
#include "llbc/core/log/LogAppenderBuilder.h"
#include "llbc/core/log/LogRunnable.h"

#include "llbc/core/log/Logger.h"

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(disable:4996)
#endif

__LLBC_INTERNAL_NS_BEGIN

static const LLBC_NS LLBC_String __g_invalidLoggerName;

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_Logger::LLBC_Logger()
: _logLevel(LLBC_LogLevel::Debug)
, _config(NULL)

, _logRunnable(NULL)

, _lastFlushTime(0)
, _flushInterval(LLBC_CFG_LOG_DEFAULT_LOG_FLUSH_INTERVAL)
, _appenders(NULL)

, _msgBlockPoolInst(*_objPool.GetPoolInst<LLBC_MessageBlock>())
, _logDataPoolInst(*_objPool.GetPoolInst<LLBC_LogData>())
, _hookDelegs()
{
}

LLBC_Logger::~LLBC_Logger()
{
    Finalize();
}

int LLBC_Logger::Initialize(const LLBC_LoggerConfigInfo *config, LLBC_LogRunnable *sharedLogRunnable)
{
    // Parameters check.
    if (!config)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    // Reentry check.
    if (IsInit())
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    // Lock & Reentry check(again).
    LLBC_LockGuard guard(_lock);
    if (IsInit())
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    // Init basic data members.
    _name.append(config->GetLoggerName());
    _config = config;

    _logLevel = MIN(_config->GetConsoleLogLevel(), _config->GetFileLogLevel());
    _flushInterval = _config->GetFlushInterval();

    // Create console appender, if acquire.
    if (_config->IsLogToConsole())
    {
        LLBC_LogAppenderInitInfo appenderInitInfo;
        appenderInitInfo.level = _config->GetConsoleLogLevel();
        appenderInitInfo.pattern = _config->GetConsolePattern();
        appenderInitInfo.colourfulOutput = _config->IsColourfulOutput();

        LLBC_ILogAppender *appender = 
            LLBC_LogAppenderBuilderSingleton->BuildAppender(LLBC_LogAppenderType::Console);
        if (appender->Initialize(appenderInitInfo) != LLBC_OK)
        {
            LLBC_XDelete(appender);
            return LLBC_FAILED;
        }

        this->AddAppender(appender);
    }

    // Create file appender, if acquire.
    if (_config->IsLogToFile())
    {
        LLBC_LogAppenderInitInfo appenderInitInfo;
        appenderInitInfo.level = _config->GetFileLogLevel();
        appenderInitInfo.pattern = _config->GetFilePattern();
        appenderInitInfo.file = _config->GetLogFile();
        appenderInitInfo.fileSuffix = _config->GetLogFileSuffix();
        appenderInitInfo.dailyRolling = _config->IsDailyRollingMode();
        appenderInitInfo.maxFileSize = _config->GetMaxFileSize();
        appenderInitInfo.maxBackupIndex = _config->GetMaxBackupIndex();
        appenderInitInfo.lazyCreateLogFile = _config->IsLazyCreateLogFile();

        if (!_config->IsAsyncMode())
            appenderInitInfo.fileBufferSize = 0;
        else
            appenderInitInfo.fileBufferSize = _config->GetFileBufferSize();

        LLBC_ILogAppender *appender =
            LLBC_LogAppenderBuilderSingleton->BuildAppender(LLBC_LogAppenderType::File);
        if (appender->Initialize(appenderInitInfo) != LLBC_OK)
        {
            LLBC_XDelete(appender);
            return LLBC_FAILED;
        }

        this->AddAppender(appender);
    }

    // Set/Create log runnable.
    if (_config->IsAsyncMode())
    {
        _logRunnable = _config->IsIndependentThread() ? LLBC_New(LLBC_LogRunnable) : sharedLogRunnable;
        _logRunnable->AddLogger(this);

        if (_config->IsIndependentThread())
            _logRunnable->Activate(1, LLBC_ThreadFlag::Joinable, LLBC_ThreadPriority::BelowNormal);
    }

    return LLBC_OK;
}

bool LLBC_Logger::IsInit() const
{
    LLBC_Logger *ncThis = const_cast<LLBC_Logger *>(this);
    LLBC_LockGuard guard(ncThis->_lock);

    return _config != NULL;
}

void LLBC_Logger::Finalize()
{
    // Lock logger.
    LLBC_LockGuard guard(_lock);

    // Check inited or not.
    if (!IsInit())
        return;

    // Stop runnable, if is async mode and use independent logger thread.
    if (_config->IsAsyncMode())
    {
        if (_config->IsIndependentThread())
            _logRunnable->Stop();
        else
            Flush(true, false);

        _logRunnable = nullptr;
    }

    // Uninstall all hooks.
    for (int level = LLBC_LogLevel::Begin; level != LLBC_LogLevel::End; ++level)
        UninstallHook(level);

    // Delete all appenders.
    while (_appenders)
    {
        LLBC_ILogAppender *appender = _appenders;
        _appenders = _appenders->GetAppenderNext();

        LLBC_Delete(appender);
    }

    // Reset basic members.
    _name.clear();
    _config = NULL;
    _logLevel = LLBC_LogLevel::Debug;
}

const LLBC_String &LLBC_Logger::GetLoggerName() const
{
    LLBC_Logger *nonConstThis = const_cast<LLBC_Logger *>(this);
    LLBC_LockGuard guard(nonConstThis->_lock);

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    return _name;
}

int LLBC_Logger::GetLogLevel() const
{
    return _logLevel;
}

void LLBC_Logger::SetLogLevel(int level)
{
    LLBC_LockGuard guard(_lock);
    _logLevel = MIN(MAX(LLBC_LogLevel::Begin, level), LLBC_LogLevel::End - 1);
}

bool LLBC_Logger::IsTakeOver() const
{
    LLBC_Logger *ncThis = const_cast<LLBC_Logger *>(this);
    LLBC_LockGuard guard(ncThis->_lock);

    return _config->IsTakeOver();
}

bool LLBC_Logger::IsAsyncMode() const
{
    LLBC_Logger *ncThis = const_cast<LLBC_Logger *>(this);
    LLBC_LockGuard guard(ncThis->_lock);

    return _config->IsAsyncMode();
}

int LLBC_Logger::InstallHook(int level, const std::function<void(const LLBC_LogData *)> &hookDeleg)
{
    if (UNLIKELY(!LLBC_LogLevel::IsLegal(level) ||
        !hookDeleg))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);

    UninstallHook(level);
    _hookDelegs[level] = hookDeleg;

    return LLBC_OK;
}

void LLBC_Logger::UninstallHook(int level)
{
    LLBC_LockGuard guard(_lock);

    if (LIKELY(LLBC_LogLevel::IsLegal(level)))
        _hookDelegs[level] = nullptr;
}

int LLBC_Logger::Debug(const char *tag, const char *file, int line, const char *fmt, ...)
{
    if (LLBC_LogLevel::Debug < _logLevel)
        return LLBC_OK;

    char *fmttedMsg; int msgLen;
    LLBC_FormatArg(fmt, fmttedMsg, msgLen);

    return DirectOutput(LLBC_LogLevel::Debug, tag, file, line, fmttedMsg, msgLen);
}

int LLBC_Logger::Info(const char *tag, const char *file, int line, const char *fmt, ...)
{
    if (LLBC_LogLevel::Info < _logLevel)
        return LLBC_OK;

    char *fmttedMsg; int msgLen;
    LLBC_FormatArg(fmt, fmttedMsg, msgLen);

    return DirectOutput(LLBC_LogLevel::Info, tag, file, line, fmttedMsg, msgLen);
}

int LLBC_Logger::Warn(const char *tag, const char *file, int line, const char *fmt, ...)
{
    if (LLBC_LogLevel::Warn < _logLevel)
        return LLBC_OK;

    char *fmttedMsg; int msgLen;
    LLBC_FormatArg(fmt, fmttedMsg, msgLen);

    return DirectOutput(LLBC_LogLevel::Warn, tag, file, line, fmttedMsg, msgLen);
}

int LLBC_Logger::Error(const char *tag, const char *file, int line, const char *fmt, ...)
{
    if (LLBC_LogLevel::Error < _logLevel)
        return LLBC_OK;

    char *fmttedMsg; int msgLen;
    LLBC_FormatArg(fmt, fmttedMsg, msgLen);

    return DirectOutput(LLBC_LogLevel::Error, tag, file, line, fmttedMsg, msgLen);
}

int LLBC_Logger::Fatal(const char *tag, const char *file, int line, const char *fmt, ...)
{
    if (LLBC_LogLevel::Fatal < _logLevel)
        return LLBC_OK;

    char *fmttedMsg; int msgLen;
    LLBC_FormatArg(fmt, fmttedMsg, msgLen);

    return DirectOutput(LLBC_LogLevel::Fatal, tag, file, line, fmttedMsg, msgLen);
}

int LLBC_Logger::Output(int level, const char *tag, const char *file, int line, const char *fmt, ...) 
{
    if (level < _logLevel)
        return LLBC_OK;

    char *fmttedMsg; int msgLen;
    LLBC_FormatArg(fmt, fmttedMsg, msgLen);

    return DirectOutput(level, tag, file, line, fmttedMsg, msgLen);
}

int LLBC_Logger::OutputNonFormat(int level, const char *tag, const char *file, int line, const char *message, size_t messageLen)
{
    if (level < _logLevel)
        return LLBC_OK;

    if (UNLIKELY(message == NULL))
        return DirectOutput(level, tag, file, line, NULL, 0);

    char *copyMessage = LLBC_Malloc(char, messageLen + 1);
    LLBC_MemCpy(copyMessage, message, messageLen);
    copyMessage[messageLen] = '\0';

    return DirectOutput(level, tag, file, line, copyMessage, static_cast<int>(messageLen));
}

int LLBC_Logger::OutputNonFormat2(int level, const char *tag, const char *file, int line, char *message, size_t messageLen)
{
    if (level < _logLevel)
    {
        if (message)
            LLBC_Free(message);

        return LLBC_OK;
    }

    return DirectOutput(level, tag, file, line, message, static_cast<int>(messageLen));
}

int LLBC_Logger::DirectOutput(int level, const char *tag, const char *file, int line, char *message, int len) 
{
    LLBC_LogData *data = BuildLogData(level, tag, file, line, message, len);
    if (_hookDelegs[level])
        _hookDelegs[level](data);

    if (!_config->IsAsyncMode())
    {
        const int ret = this->OutputLogData(*data);
        LLBC_Recycle(data);

        return ret;
    }

    LLBC_MessageBlock *block = _msgBlockPoolInst.GetObject();
    block->Write(&data, sizeof(LLBC_LogData *));
    _logRunnable->Push(block);

    return LLBC_OK;
}

LLBC_LogData *LLBC_Logger::BuildLogData(int level,
                                        const char *tag,
                                        const char *file,
                                        int line,
                                        char *message,
                                        int len)
{
    LLBC_LogData *data = _logDataPoolInst.GetObject();

    data->logger = this;
    data->level = level;
    data->loggerName = _name.c_str();

    data->tagLen = tag ? LLBC_StrLenA(tag) : 0;

    if (file)
    {
        data->fileLen = LLBC_StrLenA(file);
        if (!_config->IsLogCodeFilePath())
        {
#if LLBC_TARGET_PLATFORM_WIN32
            const char *ps = strrchr(file, '\\');
#else // Non-Win32
            const char *ps = strrchr(file, '/');
#endif // Win32
            if (ps != NULL)
            {
                data->fileLen -= (static_cast<uint32>(ps - file) + 1);
                file = ps + 1;
            }
#if LLBC_TARGET_PLATFORM_WIN32 // In Win32 platform, search '/' again
            else
            {
                if ((ps = strrchr(file, '/')) != NULL)
                {
                    data->fileLen -= (static_cast<uint32>(ps - file) + 1);
                    file = ps + 1;
                }
            }
#endif // Win32
        }
    }

    data->fileBeg = data->tagLen;

    const uint32 othersSize = data->tagLen + data->fileLen;
    if (othersSize != 0)
    {
        if (data->othersSize < othersSize)
        {
            data->othersSize = othersSize;
            data->others = LLBC_Realloc(char, data->others, othersSize);
        }

        if (tag)
            ::memcpy(data->others + data->tagBeg, tag, data->tagLen);
        if (file)
            ::memcpy(data->others + data->fileBeg, file, data->fileLen);
    }

    data->logTime = LLBC_GetMilliSeconds();

    data->line = line;

    data->msg = message;
    data->msgLen = len;

    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    data->threadId = tls->coreTls.threadId;

    return data;
}

void LLBC_Logger::AddAppender(LLBC_ILogAppender *appender)
{
    appender->SetAppenderNext(NULL);
    if (!_appenders)
    {
        _appenders = appender;
        return;
    }

    LLBC_ILogAppender *tmpAppender = _appenders;
    while (tmpAppender->GetAppenderNext())
    {
        tmpAppender = tmpAppender->GetAppenderNext();
    }

    tmpAppender->SetAppenderNext(appender);
}

int LLBC_Logger::OutputLogData(const LLBC_LogData &data)
{
    LLBC_ILogAppender *appender = _appenders;
    if (!appender)
    {
        return LLBC_OK;
    }

    while (appender)
    {
        if (appender->Output(data) != LLBC_OK)
        {
            return LLBC_FAILED;
        }

        appender = appender->GetAppenderNext();
    }

    return LLBC_OK;
}

void LLBC_Logger::Flush(bool force, sint64 now) 
{
    // If not force flush appenders and the flush time not reached, no flush.
    if (!force)
    {
        if (now == 0)
            now = LLBC_GetMilliSeconds();
        sint64 diff = now - _lastFlushTime;
        if (diff >= 0 && diff < _flushInterval)
            return;
    }

    // Flush appenders.
    FlushAppenders(force);

    // Update last flush time(use flushed time to avoid logger performance problem).
    _lastFlushTime = now != 0 ? now : LLBC_GetMilliSeconds();
}

void LLBC_Logger::FlushAppenders(bool force)
{  
    // Foreach appenders to flush.
    LLBC_ILogAppender *appender = _appenders;
    while (appender)
    {
        appender->Flush();
        appender = appender->GetAppenderNext();
    }
}

__LLBC_NS_END

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(default:4996)
#endif

#include "llbc/common/AfterIncl.h"
