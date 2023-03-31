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

#include "llbc/core/utils/Util_Text.h"
#include "llbc/core/utils/Util_Debug.h"

#include "llbc/core/os/OS_Time.h"

#include "llbc/core/thread/Guard.h"

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

__LLBC_NS_BEGIN

LLBC_Logger::LLBC_Logger()
: _logLevel(LLBC_LogLevel::Begin)
, _addTimestampInJsonLog(false)
, _config(nullptr)

, _logRunnable(nullptr)

, _lastFlushTime(0)
, _flushInterval(LLBC_CFG_LOG_DEFAULT_LOG_FLUSH_INTERVAL)
, _appenders(nullptr)

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

    _logLevel = config->GetLogLevel();
    _addTimestampInJsonLog = config->IsAddTimestampInJsonLog();
    _config = config;

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
            ClearNonRunnableMembers();

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
        appenderInitInfo.filePath = _config->GetLogFile();
        appenderInitInfo.fileSuffix = _config->GetLogFileSuffix();
        appenderInitInfo.fileRollingMode = _config->GetFileRollingMode();
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
            ClearNonRunnableMembers();

            return LLBC_FAILED;
        }

        this->AddAppender(appender);
    }

    // Set/Create log runnable.
    if (_config->IsAsyncMode())
    {
        _logRunnable = _config->IsIndependentThread() ? new LLBC_LogRunnable : sharedLogRunnable;
        _logRunnable->AddLogger(this);

        if (_config->IsIndependentThread())
            _logRunnable->Activate(1, LLBC_ThreadPriority::BelowNormal);
    }

    return LLBC_OK;
}

bool LLBC_Logger::IsInit() const
{
    return _config != nullptr;
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
        {
            _logRunnable->Stop();
            delete _logRunnable;
        }
        else
        {
            Flush(true, false);
        }

        _logRunnable = nullptr;
    }

    // Clear non-runnable data members.
    ClearNonRunnableMembers(false);
}

void LLBC_Logger::SetLogLevel(int level)
{
    LLBC_LockGuard guard(_lock);
    _logLevel = MIN(MAX(LLBC_LogLevel::Begin, level), LLBC_LogLevel::End - 1);
}

bool LLBC_Logger::IsTakeOver() const
{
    LLBC_LockGuard guard(_lock);
    return _config->IsTakeOver();
}

bool LLBC_Logger::IsAsyncMode() const
{
    LLBC_LockGuard guard(_lock);
    return _config->IsAsyncMode();
}

int LLBC_Logger::InstallHook(int level, const LLBC_Delegate<void(const LLBC_LogData *)> &hookDeleg)
{
    if (UNLIKELY(!LLBC_LogLevel::IsLegal(level) ||
        !hookDeleg))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);
    _hookDelegs[level] = hookDeleg;

    return LLBC_OK;
}

void LLBC_Logger::UninstallHook(int level)
{
    if (UNLIKELY(!LLBC_LogLevel::IsLegal(level)))
        return;

    LLBC_LockGuard guard(_lock);
    _hookDelegs[level] = nullptr;
}

int LLBC_Logger::VOutput(int level,
                         const char *tag,
                         const char *file,
                         int line,
                         const char *func,
                         const char *fmt,
                         va_list va) 
{
    if (_logLevel > level)
        return LLBC_OK;

    LLBC_LogData *data = BuildLogData(level,
                                      tag,
                                      file,
                                      line,
                                      func,
                                      fmt,
                                      va);
    if (UNLIKELY(!data))
        return LLBC_FAILED;

    if (_hookDelegs[level])
        _hookDelegs[level](data);

    if (!_config->IsAsyncMode())
    {
        const int ret = this->OutputLogData(*data);
        LLBC_Recycle(data);

        return ret;
    }

    _logRunnable->PushLogData(data);

    return LLBC_OK;
}

int LLBC_Logger::NonFormatOutput(int level,
                                 const char *tag,
                                 const char *file,
                                 int line,
                                 const char *func,
                                 sint64 time,
                                 const char *msg,
                                 size_t msgLen)
{
    if (level < _logLevel)
        return LLBC_OK;

    LLBC_LogData *data = BuildLogData(level,
                                      tag,
                                      file,
                                      line,
                                      func,
                                      time != 0 ? time : LLBC_GetMicroSeconds(),
                                      msg,
                                      msgLen);
    if (_hookDelegs[level])
        _hookDelegs[level](data);

    if (!_config->IsAsyncMode())
    {
        const int ret = this->OutputLogData(*data);
        LLBC_Recycle(data);

        return ret;
    }

    _logRunnable->PushLogData(data);

    return LLBC_OK;
}

LLBC_FORCE_INLINE LLBC_LogData *LLBC_Logger::BuildLogData(int level,
                                                          const char *tag,
                                                          const char *file,
                                                          int line,
                                                          const char *func,
                                                          const char *fmt,
                                                          va_list va)
{
    // Format message.
    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    int len = vsnprintf(libTls->coreTls.loggerFmtBuf,
                        sizeof(libTls->coreTls.loggerFmtBuf),
                        fmt,
                        va);
    if (UNLIKELY(len < 0))
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return nullptr;
    }

    // Normalize length.
    if (UNLIKELY(len > static_cast<int>(sizeof(libTls->coreTls.loggerFmtBuf) - 1)))
        len = static_cast<int>(sizeof(libTls->coreTls.loggerFmtBuf) - 1);

    LLBC_LogData *data = _logDataPoolInst.GetObject();
    if (data->msgCap < len + 1)
    {
        data->msgCap = MAX(len + 1, 192);
        data->msg = LLBC_Realloc(char, data->msg, data->msgCap);
    }

    // Copy formatted message to LogData.
    data->msgLen = len;
    if (len > 0)
        memcpy(data->msg, libTls->coreTls.loggerFmtBuf, len);
    data->msg[len] = '\0';

    // Fill other LogData members.
    FillLogDataNonMsgMembers(level,
                             tag,
                             file,
                             line,
                             func,
                             LLBC_GetMicroSeconds(),
                             data,
                             libTls);

    return data;
}

LLBC_FORCE_INLINE LLBC_LogData *LLBC_Logger::BuildLogData(int level,
                                                          const char *tag,
                                                          const char *file,
                                                          int line,
                                                          const char *func,
                                                          sint64 time,
                                                          const char *msg,
                                                          size_t msgLen)
{
    // Calculate message length, if required.
    if (msgLen == static_cast<size_t>(-1) && msg)
        msgLen = strlen(msg);

    // Set message length to 0, if msg is null.
    if (UNLIKELY(!msg))
        msgLen = 0;

    // Normalize message length(<= LLBC_CFG_LOG_FORMAT_BUF_SIZE - 1).
    if (UNLIKELY(msgLen >= LLBC_CFG_LOG_FORMAT_BUF_SIZE))
        msgLen = LLBC_CFG_LOG_FORMAT_BUF_SIZE - 1;

    // Alloc new LogData, and adjust message capacity.
    LLBC_LogData *data = _logDataPoolInst.GetObject();
    const int msgCap = MAX(static_cast<int>(msgLen) + 1, 192);
    if (data->msgCap < msgCap)
    {
        data->msgCap = msgCap;
        data->msg = LLBC_Realloc(char, data->msg, msgCap);
    }

    // Copy message to LogData.
    data->msgLen = static_cast<int>(msgLen);
    if (msgLen > 0)
        memcpy(data->msg, msg, msgLen);
    data->msg[msgLen] = '\0';

    // Fill LogData other members.
    FillLogDataNonMsgMembers(level,
                             tag,
                             file,
                             line,
                             func,
                             time,
                             data,
                             __LLBC_GetLibTls());

    return data;
}

LLBC_FORCE_INLINE void LLBC_Logger::FillLogDataNonMsgMembers(int level,
                                                             const char *tag,
                                                             const char *file,
                                                             int line,
                                                             const char *func,
                                                             sint64 time,
                                                             LLBC_LogData *logData,
                                                             __LLBC_LibTls *libTls)
{
    // fill: logger.
    logData->logger = this;

    // fill: log level.
    logData->level = level;

    // fill: log time.
    logData->logTime = time;

    // fill: file, func.
    if (file)
    {
        logData->fileLen = static_cast<int>(strlen(file));
        if (!_config->IsLogCodeFilePath())
        {
            const char *ps = file + logData->fileLen - 1;
            while (ps != file)
            {
                #if LLBC_TARGET_PLATFORM_WIN32
                if (*ps == '\\' ||
                    *ps == '/')
                #else // Non-Win32
                if (*ps == '/')
                #endif
                {
                    logData->fileLen -= static_cast<int>((ps + 1 - file));
                    file = ps + 1;
                    break;
                }

                --ps;
            }
        }

        const int exceedLen = logData->fileLen - static_cast<int>((sizeof(logData->file) - 1));
        if (UNLIKELY(exceedLen > 0))
        {
            file += exceedLen;
            logData->fileLen = sizeof(logData->file) - 1;
        }

        memcpy(logData->file, file, logData->fileLen);
        logData->file[logData->fileLen] = '\0';
    }

    if (func)
    {
        logData->funcLen = static_cast<int>(strlen(func));
        const int exceedLen = logData->funcLen - static_cast<int>((sizeof(logData->func) - 1));
        if (UNLIKELY(exceedLen > 0))
            logData->funcLen = sizeof(logData->func) - 1;

        memcpy(logData->func, func, logData->funcLen);
        logData->func[logData->funcLen] = '\0';
    }

    // fill: tag.
    if (tag)
    {
        logData->tagLen = static_cast<int>(strlen(tag));
        const int exceedLen = logData->tagLen - static_cast<int>((sizeof(logData->tag) - 1));
        if (UNLIKELY(exceedLen > 0))
            logData->tagLen = sizeof(logData->tag) - 1;

        memcpy(logData->tag, tag, logData->tagLen);
        logData->tag[logData->tagLen] = '\0';
    }

    // fill: line.
    logData->line = line;

    // fill: threadId.
    logData->threadId = libTls->coreTls.threadId;
}

void LLBC_Logger::AddAppender(LLBC_ILogAppender *appender)
{
    appender->SetAppenderNext(nullptr);
    if (!_appenders)
    {
        _appenders = appender;
        return;
    }

    LLBC_ILogAppender *tmpAppender = _appenders;
    while (tmpAppender->GetAppenderNext())
        tmpAppender = tmpAppender->GetAppenderNext();

    tmpAppender->SetAppenderNext(appender);
}

int LLBC_Logger::OutputLogData(const LLBC_LogData &data)
{
    LLBC_ILogAppender *appender = _appenders;
    if (!appender)
        return LLBC_OK;

    while (appender)
    {
        if (appender->Output(data) != LLBC_OK)
            return LLBC_FAILED;

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
    FlushAppenders();

    // Update last flush time(use flushed time to avoid logger performance problem).
    _lastFlushTime = now != 0 ? now : LLBC_GetMilliSeconds();
}

void LLBC_Logger::FlushAppenders()
{  
    // Foreach appenders to flush.
    LLBC_ILogAppender *appender = _appenders;
    while (appender)
    {
        appender->Flush();
        appender = appender->GetAppenderNext();
    }
}

void LLBC_Logger::ClearNonRunnableMembers(bool keepErrNo)
{
    // If require keep errno(include sub errno), cache it and define recover defer operation.
    int errNo, subErrNo;
    if (keepErrNo)
    {
        errNo = LLBC_GetLastError();
        subErrNo = LLBC_GetSubErrorNo();
    }

    LLBC_Defer(
        LLBC_DoIf(keepErrNo,
            LLBC_SetLastError(errNo); LLBC_SetSubErrorNo(subErrNo)));

    // Uninstall all hooks.
    for (int level = LLBC_LogLevel::Begin; level != LLBC_LogLevel::End; ++level)
        _hookDelegs[level] = nullptr;

    // Delete all appenders.
    while (_appenders)
    {
        LLBC_ILogAppender *appender = _appenders;
        _appenders = _appenders->GetAppenderNext();

        delete appender;
    }

    // Reset basic members.
    _flushInterval = 0;
    _lastFlushTime = 0;

    _config = nullptr;
    _addTimestampInJsonLog = false;
    _logLevel = LLBC_LogLevel::Begin;

    _name.clear();
}

__LLBC_NS_END

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(default:4996)
#endif
