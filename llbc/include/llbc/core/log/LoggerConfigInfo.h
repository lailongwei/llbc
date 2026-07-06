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

#include "llbc/common/Common.h"
#include "llbc/core/log/LogTrace.h"
#include "llbc/core/log/LogControl.h"

__LLBC_NS_BEGIN

/**
 * Previous declare some classes.
 */
class LLBC_Variant;
class LLBC_LogTimeAccessor;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The logger config info class encapsulation.
 */
class LLBC_HIDDEN LLBC_LoggerConfigInfo
{
public:
    LLBC_LoggerConfigInfo();
    ~LLBC_LoggerConfigInfo();

public:
    /**
     * Initialize logger config.
     * @param[in] loggerName      - the logger name.
     * @param[in] cfg             - config property.
     * @param[in] rootCfg         - the root logger config.
     * @param[in] logTimeAccessor - the log time accessor.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Initialize(const LLBC_String &loggerName,
                   const LLBC_Variant &cfg,
                   const LLBC_LoggerConfigInfo *rootCfg,
                   const LLBC_LogTimeAccessor &logTimeAccessor);

public:
    /**
     * Get logger name.
     * @return const LLBC_String & - the logger name.
     */
    const LLBC_String &GetLoggerName() const;

    /**
     * Get not config use option from root or not(use library default).
     * @return bool - the use root option.
     */
    bool IsNotConfigUseRoot() const;

    /**
     * Get log time accessor.
     * @return const LLBC_LogTimeAccessor & - the log time accessor.
     */
    const LLBC_LogTimeAccessor &GetLogTimeAccessor() const;

public:
    /**
     * Get logger log level.
     * @return int - the logger log level.
     */
    int GetLogLevel() const;

    /**
     * Get require color log traces list. 
     * @return {traceKey, [traceContent]} - _requireColorLogTraces.
     */
    const LLBC_LogTraces &GetRequireColorLogTraces() const;

    /**
     * Get appender log level.
     * 
     * @param[in] appenderType - log appender type.
     * @return int - the appender log level, if return End and LLBC_GetLastError() != LLBC_ERROR_SUCCESS, 
     *               means error occurred.
     */
    int GetAppenderLogLevel(int appenderType) const;

    /**
     * Get asynchronous mode switch.
     * @return bool - asynchronous mode switch.
     */
    bool IsAsyncMode() const;

    /**
     * Independent logger thread switch, only available in Async-Mode.
     * @return bool - independent logger thread switch.
     */
    bool IsIndependentThread() const;

    /**
     * Get file refresh interval.
     * @return int - the file refresh interval.
     */
    int GetFlushInterval() const;

public:
    /**
     * Add timestamp in json log.
     * @return bool - add timestamp in json log flag.
     */
    bool IsAddTimestampInJsonLog() const;

public:
    /**
     * Get log to console switch.
     * @return bool - log to console switch.
     */
    bool IsLogToConsole() const;

    /**
     * Get console log level.
     * @return int - console log level
     */
    int GetConsoleLogLevel() const;

    /**
     * Get console log pattern.
     * @return const LLBC_String & - console log pattern.
     */
    const LLBC_String &GetConsolePattern() const;

    /**
     * Get colorful output flag(only available in WIN platform).
     * @return bool - the colorful output flag.
     */
    bool IsColourfulOutput() const;

public:
    /**
     * Get log to file switch.
     * @return bool - log to file switch.
     */
    bool IsLogToFile() const;

    /**
     * Get file log level.
     * @return int - file log level.
     */
    int GetFileLogLevel() const;

    /**
     * Get file log pattern.
     * @return const LLBC_String & - file log pattern.
     */
    const LLBC_String &GetFilePattern() const;

    /**
     * Get log directory.
     * @return const LLBC_String & - log directory.
     */
    const LLBC_String &GetLogDir() const;

    /**
     * Get log file name.
     * @return const LLBC_String & - log file name.
     */
    const LLBC_String &GetLogFile() const;

    /**
     * Get original log file name.
     * @return const LLBC_String & - original log file name.
     */
    const LLBC_String &GetOriginalLogFile() const;

    /**
     * Get log file suffix.
     * @return const LLBC_String & - log file suffix.
     */
    const LLBC_String &GetLogFileSuffix() const;

    /**
     * Get log file path or not, when log code file name(%f).
     * @return bool - log file path flag, if true, log file path, otherwise only log file name.
     */
    bool IsLogCodeFilePath() const;

    /**
     * Get force application log path flag
     * @return const bool - log path flag
     */
    bool IsForceAppLogPath() const;

    /**
     * Get file rolling mode.
     * @return bool - file rolling mode.
     */
    int GetFileRollingMode() const;

    /**
     * Get max log file size.
     * @return sint64 - max log file size.
     */
    sint64 GetMaxFileSize() const;

    /**
     * Get max backup log file index.
     * @return int - max backup log file index.
     */
    int GetMaxBackupIndex() const;

    /**
     * Get file buffer size.
     * @return int - the file buffer size.
     */
    int GetFileBufferSize() const;

    /**
     * Is fadvise discard file page cache enabled.
     * @return bool - is fadvise discard file page cache enabled.
     */
    bool IsFadviseDiscardEnabled() const;

    /**
     * Get fadvise discard file page cache size.
     * @return sint64 - the fadvise discard file page cache size.
     */
    sint64 GetFadviseDiscardSize() const;

    /**
     * Get fadvise discard retain size.
     * @return sint64 - the fadvise discard retain size.
     */
    sint64 GetFadviseDiacardRetainSize() const;

public:
    /**
     * Get take over option.
     * @return bool - take over option.
     */
    bool IsTakeOver() const;

public:
    /**
     * Get logfile create option.
     * @return bool - logfile create option.
     */
    bool IsLazyCreateLogFile() const;

public:
    /**
     * Get parsed log control items.
     * Parsed from cfg key `logControls`, which the LoggerConfigurator layer
     * hands over in a domain-neutral CIR (Config Intermediate Representation)
     * shape regardless of the source format (XML today; YAML/JSON later).
     *
     * CIR schema for `logControls` (plain map/seq/scalar Variant tree; no
     * XML metadata such as [Name]/[Attrs]/[Children] is involved):
     *
     *   logControls: Dict { "item": Dict | Seq<Dict> }   // XML container
     *              | Seq<Dict>                           // loader-native
     *              | Dict { ... }                        // single-item shortcut
     *
     * Each item is a Dict:
     *   Dict {
     *     "match": Dict {                          // optional; empty == no rule
     *       "file": Dict { "name": Str,
     *                      "lines"?: Str },        // "N" | "N-M" | "N,M-K,..."
     *       "func":     Str | Seq<Str>,            // OR-combined
     *       "threadId": Str | Seq<Str>,            // OR-combined
     *       "level":    Str | Seq<Str>             // OR; name or decimal
     *     },
     *     "appenders": Dict { "appender": Str | Seq<Str> }
     *                  | Str | Seq<Str>,           // optional; empty == all
     *     "action":    Dict {
     *       "type":     Str,                       // "mute" | "setLevel"
     *       "newLevel"?: Str                       // required iff type=setLevel
     *     }
     *   }
     *
     * Within one item, enabled rules are AND-combined; each rule's value-set
     * (file.lineRanges / func.values / threadId.values / level.values) is
     * OR-combined. Declaration order is preserved across items.
     *
     * XML source binding is documented separately (see doc/log_control.md);
     * this module is not aware of any XML tag/attribute name.
     * @return const std::vector<LLBC_LogControlItem> & - the parsed items.
     */
    const std::vector<LLBC_LogControlItem> &GetLogControls() const;

private:
    /**
     * Normalize the log file name.
     */
    void NormalizeLogFileName();

    /**
     * Normalize str of size.
     * @param sizeStr       - the un-normalized size str.
     * @param defaultSize   - default size, if sizeStr invalid.
     * @param low           - the min value.
     * @param high          - the max value.
     * @return sint64 - the normalized size str.
     */
    sint64 NormalizeSizeStr(const LLBC_String &sizeStr, sint64 defaultSize, sint64 low, sint64 high);

    /**
     * Parse the CIR node for `logControls` (see schema in GetLogControls()
     * doc above) into LLBC_LogControlItem list. Malformed input is ignored
     * silently (consistent with other log cfg keys -- keep bootstrap robust).
     * Per-item failures are also skipped silently; only well-formed items
     * with at least one match rule, a known action and (when action==SetLevel)
     * a valid newLevel are appended to `out`. Declaration order is preserved.
     *
     * `cfg` is a Seq<Dict> per the CIR schema. If `cfg` is not a Seq (i.e.
     * the key was absent or the loader emitted a non-list shape), this is a
     * silent no-op.
     * @param[in]  cfg - the CIR node of `logControls`.
     * @param[out] out - the parsed control items, appended.
     */
    static void ParseLogControls(const LLBC_Variant &cfg,
                                 std::vector<LLBC_LogControlItem> &out);

private:
    LLBC_String _loggerName;
    bool _notConfigUseRoot;

    const LLBC_LogTimeAccessor *_logTimeAccessor;

    bool _asyncMode;
    bool _independentThread;
    int _flushInterval;

    bool _addTimestampInJsonLog;

    bool _logToConsole;
    int _consoleLogLevel;
    LLBC_String _consolePattern;
    bool _colourfulOutput;

    bool _logToFile;
    int _fileLogLevel;
    LLBC_String _logDir;
    LLBC_String _logFile;
    LLBC_String _originalLogFile;
    LLBC_String _logFileSuffix;
    bool _logCodeFilePath;
    bool _forceAppLogPath;
    LLBC_String _filePattern;
    int _fileRollingMode;
    sint64 _maxFileSize;
    int _maxBackupIndex;
    int _fileBufferSize;
    bool _fadviseDiscardEnabled;
    sint64 _fadviseDiscardSize;
    sint64 _fadviseDiscardRetainSize;
    bool _lazyCreateLogFile;
    LLBC_LogTraces _requireColorLogTraces;
    bool _takeOver;

    std::vector<LLBC_LogControlItem> _logControls;
};

__LLBC_NS_END

#include "llbc/core/log/LoggerConfigInfoInl.h"