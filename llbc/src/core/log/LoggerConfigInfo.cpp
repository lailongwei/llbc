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

#include "llbc/core/os/OS_SysConf.h"
#include "llbc/core/os/OS_Process.h"
#include "llbc/core/utils/Util_Text.h"
#include "llbc/core/file/Directory.h"
#include "llbc/core/variant/Variant.h"

#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/LogRollingMode.h"
#include "llbc/core/log/BaseLogAppender.h"
#include "llbc/core/log/LoggerConfigInfo.h"

#include "llbc/core/rapidjson/json.h"

/*
 * Internal macros define.
 */
#define __LLBC_GetLogCfg(key, dft, rootMeth, asMeth)  __LLBC_GetLogCfg2(key, LLBC_CFG_LOG_DEFAULT_##dft, rootMeth, asMeth)

#define __LLBC_GetLogCfg2(key, dft, rootMeth, asMeth) (cfg[key] ? \
                                                           cfg[key].asMeth() : \
                                                               (_notConfigUseRoot ? rootCfg->rootMeth() : (dft))) \

__LLBC_NS_BEGIN

// =============================================================================
// LogControls JSON parsing helpers (file-scope static).
//
// These helpers are dedicated to LLBC_LoggerConfigInfo::ParseLogControls and
// MUST NOT be reused elsewhere (they are file-scope on purpose).
//
// All helpers are total functions on bad input: they either return false or
// produce an empty result; callers silently skip the offending control item.
//
// For lenient JSON value access (HasMember + Is<T>() + Get<T>() folded into
// one call), see LLBC_JsonGetMember / LLBC_JsonGetStr / LLBC_JsonGetInt in
// "llbc/core/rapidjson/json.h".
// =============================================================================

// Parse a level value from JSON. Accepts string ("Debug"/"Info"/...) or int.
// Returns false if value is invalid.
//
// Note: GetLevelEnum() does strict (uppercased) equality only, so we strip
// surrounding whitespace here for friendlier user input (e.g. " Warn ", "INFO\n").
static bool ParseLevel(const LLBC_Json::Value &v, int &outLevel)
{
    if (v.IsString())
    {
        const LLBC_String s = LLBC_String(v.GetString()).strip();
        if (s.empty())
            return false;
        const int lvl = LLBC_LogLevel::GetLevelEnum(s);
        if (!LLBC_LogLevel::IsValid(lvl))
            return false;
        outLevel = lvl;
        return true;
    }
    if (v.IsInt())
    {
        const int lvl = v.GetInt();
        if (!LLBC_LogLevel::IsValid(lvl))
            return false;
        outLevel = lvl;
        return true;
    }
    return false;
}

// Parse an appender type name ("console"/"file"/"network", case-insensitive).
// Returns false on unknown name or non-string value.
static bool ParseAppenderType(const LLBC_Json::Value &v, int &outType)
{
    if (!v.IsString())
        return false;
    const LLBC_String s = LLBC_String(v.GetString()).strip();
    const int t = LLBC_LogAppenderType::GetTypeEnum(s.c_str());
    if (!LLBC_LogAppenderType::IsValid(t))
        return false;
    outType = t;
    return true;
}

// Read an optional int field from a JSON object.
// - field absent (or explicit JSON null) -> outVal unchanged, return true
// - field present and is int / int64     -> outVal = <the int>, return true
// - field present but not an int
//   (string / float / bool / array / object / etc.) -> return false
//
// "Present but explicitly 0" is indistinguishable from "absent" by design --
// both are treated as "not specified" (whole-file wildcard at the call site).
// Callers should pre-initialize outVal to the "absent" default (typically 0).
static bool ParseOptionalInt(const LLBC_Json::Value &parent,
                             const char *key,
                             int &outVal)
{
    const auto &v = LLBC_JsonGetMember(parent, key);
    if (v.IsNull())
        return true;
    if (!v.IsInt() && !v.IsInt64())
        return false;
    outVal = v.IsInt() ? v.GetInt() : static_cast<int>(v.GetInt64());
    return true;
}

// Parse a "line spec" string (the right-hand side of "name:..."
// or the value of object-form "lines"). Multi-segment, comma-
// separated; each segment is either a single line "N" or a
// half-open range "N-M" ([N, M), with M > N).
// Examples:
//   "42"               -> [(42, 43)]
//   "10-12"            -> [(10, 12)]
//   "100, 102-112"     -> [(100, 101), (102, 112)]
// Whitespace around tokens is stripped. Returns false on any
// malformed segment (caller should silently skip the offending
// control item).
static bool ParseLineSpec(const LLBC_String &spec,
                          std::vector<std::pair<int, int> > &outRanges)
{
    outRanges.clear();
    LLBC_String s = LLBC_String(spec).strip();
    if (s.empty())
        return false;

    auto segs = s.split(',', -1, true);
    if (segs.empty())
        return false;
    for (auto &raw : segs)
    {
        LLBC_String seg = raw.strip();
        if (seg.empty())
            return false;

        const auto dashPos = seg.find('-');
        if (dashPos == LLBC_String::npos)
        {
            // Single line "N" -> [N, N+1).
            if (!seg.isdigit())
                return false;
            const long ln = std::strtol(seg.c_str(), nullptr, 10);
            if (ln < 0 || ln >= INT_MAX)
                return false;
            outRanges.emplace_back(static_cast<int>(ln), static_cast<int>(ln) + 1);
        }
        else
        {
            // Range "N-M" -> [N, M).
            LLBC_String beg = seg.substr(0, dashPos).strip();
            LLBC_String end = seg.substr(dashPos + 1).strip();
            if (!beg.isdigit() || !end.isdigit())
                return false;
            const long lb = std::strtol(beg.c_str(), nullptr, 10);
            const long le = std::strtol(end.c_str(), nullptr, 10);
            if (lb < 0 || le <= lb || le > INT_MAX)
                return false;
            outRanges.emplace_back(static_cast<int>(lb), static_cast<int>(le));
        }
    }
    return !outRanges.empty();
}

// Parse a string-form "file" value with optional multi-segment line spec.
// Accepted forms (whitespace around tokens is stripped):
//   "Foo.cpp"              -> name="Foo.cpp", ranges=[]   (whole-file wildcard)
//   "Foo.cpp:42"           -> name="Foo.cpp", ranges=[(42,43)]
//   "Foo.cpp:10-12"        -> name="Foo.cpp", ranges=[(10,12)]
//   "Foo.cpp:100, 102-112" -> name="Foo.cpp", ranges=[(100,101),(102,112)]
// Returns false if the string is malformed.
static bool ParseFileStr(const char *raw,
                         LLBC_String &name,
                         std::vector<std::pair<int, int> > &ranges)
{
    LLBC_String s = LLBC_String(raw).strip();
    if (s.empty())
        return false;

    ranges.clear();

    // Split file name from line spec at ':'.
    // The line spec part (e.g. "42", "10-12", "100, 102-112") never
    // contains ':', and we only accept basenames here (no Windows drive
    // letters like "C:\Foo.cpp"), so any well-formed input contains at
    // most one ':'. We pick find() (the first ':') for clarity; rfind()
    // would yield the same result on every well-formed input.
    const auto colonPos = s.find(':');
    if (colonPos == LLBC_String::npos)
    {
        name = s;
        return !name.empty();
    }

    LLBC_String left  = s.substr(0, colonPos).strip();
    LLBC_String right = s.substr(colonPos + 1).strip();
    if (left.empty() || right.empty())
        return false;

    if (!ParseLineSpec(right, ranges))
        return false;
    name = left;
    return true;
}

// Parse a "match.file" JSON value (string-form or object-form) into a name +
// optional line ranges. Returns false on any malformed input; caller skips.
static bool ParseFileMatch(const LLBC_Json::Value &fv,
                           LLBC_String &outName,
                           std::vector<std::pair<int, int> > &outRanges)
{
    outRanges.clear();

    // 1. String form  : "Foo.cpp" / "Foo.cpp:42" / "Foo.cpp:10-12" / "Foo.cpp:100, 102-112".
    if (fv.IsString())
        return ParseFileStr(fv.GetString(), outName, outRanges);

    // 2. Object form  : { "name":"Foo.cpp",
    //                  "line":N,                // optional single line
    //                  "lineEnd":M,             // optional, with "line", range [N,M)
    //                  "lines":"100, 102-112" } // optional multi-segment;
    if (!fv.IsObject())
        return false;

    const char *rawName = LLBC_JsonGetStr(LLBC_JsonGetMember(fv, "name"));
    if (!rawName)
        return false;
    LLBC_String name = LLBC_String(rawName).strip();
    if (name.empty())
        return false;

    // "lines" (string, multi-segment) wins if present.
    const auto &linesVal = LLBC_JsonGetMember(fv, "lines");
    if (!linesVal.IsNull())
    {
        const char *linesStr = LLBC_JsonGetStr(linesVal);
        if (!linesStr)
            return false;
        const LLBC_String linesSpec = LLBC_String(linesStr).strip();
        if (linesSpec.empty())
            return false;
        if (!ParseLineSpec(linesSpec, outRanges))
            return false;
        outName = std::move(name);
        return true;
    }

    // "line" / "lineEnd" are optional ints; if either is present with a
    // non-int type the whole control item is rejected (caller skips).
    // Both absent (or both explicit 0) means "not specified" -> whole-file
    // wildcard via the empty-ranges branch below.
    int line = 0;
    int lineEnd = 0;
    if (!ParseOptionalInt(fv, "line", line) ||
        !ParseOptionalInt(fv, "lineEnd", lineEnd))
        return false;
    if (line < 0 || lineEnd < 0 || (lineEnd != 0 && lineEnd <= line))
        return false;

    if (lineEnd > 0)
        outRanges.emplace_back(line, lineEnd);
    else if (line > 0)
        outRanges.emplace_back(line, line + 1);
    // line==0 && lineEnd==0 -> whole-file wildcard (empty ranges).

    outName = std::move(name);
    return true;
}

// Collect a JSON value into `out`. The value may be either a single scalar
// (treated as a 1-element input) or an array of scalars. `extract(v, t)`
// returns true and fills `t` if `v` is a valid scalar; otherwise the entry
// is silently dropped (so the resulting vector may be shorter than the input
// array, or even empty).
//
// Used by "match.func" (string / array-of-strings), "match.threadId"
// (int / array-of-ints) and "match.level" (level-string-or-int / array of same).
template <typename T, typename Fn>
static void CollectAsVector(const LLBC_Json::Value &v,
                            std::vector<T> &out,
                            Fn extract)
{
    T tmp;
    if (v.IsArray())
    {
        for (auto it = v.Begin(); it != v.End(); ++it)
        {
            if (extract(*it, tmp))
                out.push_back(std::move(tmp));
        }
    }
    else
    {
        if (extract(v, tmp))
            out.push_back(std::move(tmp));
    }
}

LLBC_LoggerConfigInfo::LLBC_LoggerConfigInfo()
: _notConfigUseRoot(false)

, _logTimeAccessor(nullptr)

, _asyncMode(false)
, _independentThread(false)
, _flushInterval(0)

, _addTimestampInJsonLog(0)

, _logToConsole(true)
, _consoleLogLevel(LLBC_LogLevel::End)
, _colourfulOutput(true)

, _logToFile(false)
, _fileLogLevel(LLBC_LogLevel::End)
, _logCodeFilePath(true)
, _forceAppLogPath(false)
, _fileRollingMode(LLBC_LogRollingMode::End)
, _maxFileSize(INT_MAX)
, _maxBackupIndex(0)
, _fileBufferSize(0)
, _fadviseDiscardEnabled(false)
, _fadviseDiscardSize(0)
, _fadviseDiscardRetainSize(0)
, _lazyCreateLogFile(false)

, _takeOver(false)
{
}

LLBC_LoggerConfigInfo::~LLBC_LoggerConfigInfo()
{
}

int LLBC_LoggerConfigInfo::Initialize(const LLBC_String &loggerName,
                                      const LLBC_Variant &cfg,
                                      const LLBC_LoggerConfigInfo *rootCfg,
                                      const LLBC_LogTimeAccessor &logTimeAccessor)
{
    // LoggerName.
    _loggerName = loggerName;

    // Not config use default/root option.
    LLBC_String notCfgUseOpt;
    if (cfg["notConfigUse"])
        notCfgUseOpt = cfg["notConfigUse"].AsStr();
    else if (rootCfg)
        notCfgUseOpt = rootCfg->IsNotConfigUseRoot() ? "root" : "default";
    else
        notCfgUseOpt = LLBC_CFG_LOG_DEFAULT_NOT_CONFIG_OPTION_USE;
    _notConfigUseRoot = notCfgUseOpt.strip().tolower() == "root" && rootCfg;

    // Log time accessor.
    _logTimeAccessor = &logTimeAccessor;

    // Require color log traces.
    auto keyContentsGroups =
        cfg["requireColorLogTraces"].AsStr().strip().split(LLBC_CFG_CORE_LOG_TRACE_SEPARATORS[0], -1, true);
    for (auto &group : keyContentsGroups)
    {
        auto keyContentsPair = group.strip().split(LLBC_CFG_CORE_LOG_TRACE_SEPARATORS[1], 1, true);
        if (keyContentsPair.size() != 2)
            continue;

        LLBC_String key = keyContentsPair[0].strip();
        LLBC_String contentListStr = keyContentsPair[1].strip();
        if (key.empty() || contentListStr.empty())
            continue;

        std::vector<LLBC_LogTrace::TraceContent> contentVec;
        auto contents = contentListStr.split(LLBC_CFG_CORE_LOG_TRACE_SEPARATORS[2], -1, true);
        for (auto &content : contents)
        {
            auto normalizedContent = content.strip();
            if (!normalizedContent.empty())
                contentVec.emplace_back(normalizedContent);
        }

        if (!contentVec.empty())
            _requireColorLogTraces.emplace(LLBC_LogTrace::TraceKey(key), contentVec);
    }

    // Log mute rules: file+line, func, file+level.
    // logControls: a JSON array string. Each element describes a single
    // log output control item; details see LoggerConfigInfo.h.
    ParseLogControls(cfg["logControls"].AsStr().strip(), _logControls);

    _asyncMode = __LLBC_GetLogCfg(
        "asynchronous", ASYNC_MODE, IsAsyncMode, AsLooseBool);
    if (_asyncMode)
        _independentThread = __LLBC_GetLogCfg(
            "independentThread", INDEPENDENT_THREAD, IsIndependentThread, AsLooseBool);
    _flushInterval = __LLBC_GetLogCfg(
        "flushInterval", LOG_FLUSH_INTERVAL, GetFlushInterval, AsInt32);

    // Json styled log configs.
    _addTimestampInJsonLog = __LLBC_GetLogCfg(
        "addTimestampInJsonLog", ADD_TIMESTAMP_IN_JSON_LOG, IsAddTimestampInJsonLog, AsLooseBool);

    // Console log configs.
    _logToConsole = __LLBC_GetLogCfg("logToConsole", LOG_TO_CONSOLE, IsLogToConsole, AsLooseBool);
    if (_logToConsole)
    {
        if (cfg["consoleLogLevel"])
            _consoleLogLevel = LLBC_LogLevel::GetLevelEnum(cfg["consoleLogLevel"].AsStr().c_str());
        else
            _consoleLogLevel = _notConfigUseRoot ? rootCfg->GetConsoleLogLevel() : LLBC_CFG_LOG_DEFAULT_LEVEL;

        if (cfg["consolePattern"])
            _consolePattern = cfg["consolePattern"].AsStr();
        else
            _consolePattern = _notConfigUseRoot ?
                rootCfg->GetConsolePattern().c_str() : LLBC_CFG_LOG_DEFAULT_CONSOLE_LOG_PATTERN;
        _colourfulOutput = __LLBC_GetLogCfg(
            "colourfulOutput", COLOURFUL_OUTPUT, IsColourfulOutput, AsLooseBool);
    }

    // File log configs.
    _logToFile = __LLBC_GetLogCfg("logToFile", LOG_TO_FILE, IsLogToFile, AsLooseBool);
    if (_logToFile)
    {
        // File log level.
        if (cfg["fileLogLevel"])
            _fileLogLevel = LLBC_LogLevel::GetLevelEnum(cfg["fileLogLevel"].AsStr().c_str());
        else
            _fileLogLevel = _notConfigUseRoot ? rootCfg->GetFileLogLevel() : LLBC_CFG_LOG_DEFAULT_LEVEL;

        // Log file dir.
        if (!(_logDir = __LLBC_GetLogCfg2("logDir", "", GetLogDir, AsStr).strip()).empty())
        {
            #if LLBC_TARGET_PLATFORM_WIN32
            _logDir = _logDir.findreplace('/', '\\');
            #else
            _logDir = _logDir.findreplace('\\', '/');
            #endif
        }

        // Force application log path(log file place into application module file dir).
        _forceAppLogPath = __LLBC_GetLogCfg(
            "forceAppLogPath", FORCE_APP_LOG_PATH, IsForceAppLogPath, AsLooseBool);

        // Log file name.
        if (cfg["logFile"])
            _logFile = cfg["logFile"].AsStr().strip();

        if (_logFile.empty())
        {
            if (_loggerName != LLBC_CFG_LOG_ROOT_LOGGER_NAME &&
                _notConfigUseRoot)
                _logFile = rootCfg->GetOriginalLogFile();
            else
                _logFile = "%L";
        }

        _originalLogFile = _logFile;

        if (!_logDir.empty())
            _logFile = LLBC_Directory::Join(_logDir, _logFile);
        if (!LLBC_Directory::IsAbsPath(_logFile))
        {
            if (_forceAppLogPath)
                _logFile = LLBC_Directory::Join(LLBC_Directory::ModuleFileDir(), _logFile);
            else
                _logFile = LLBC_Directory::AbsPath(_logFile);
        }

        // Log file suffix.
        _logFileSuffix = __LLBC_GetLogCfg(
            "logFileSuffix", LOG_FILE_SUFFIX, GetLogFileSuffix, AsStr);
        // Log code file path.
        _logCodeFilePath = __LLBC_GetLogCfg(
            "logCodeFilePath", LOG_CODE_FILE_PATH, IsLogCodeFilePath, AsLooseBool);
        // Log file pattern.
        if (cfg["filePattern"])
            _filePattern = cfg["filePattern"].AsStr();
        else
            _filePattern = _notConfigUseRoot ?
                rootCfg->GetFilePattern().c_str() : LLBC_CFG_LOG_DEFAULT_FILE_LOG_PATTERN;

        // File rolling mode.
        if (cfg["fileRollingMode"])
            _fileRollingMode = LLBC_LogRollingMode::Str2Mode(cfg["fileRollingMode"].AsStr());
        else
            _fileRollingMode = _notConfigUseRoot ?
                rootCfg->GetFileRollingMode() : LLBC_CFG_LOG_DEFAULT_FILE_ROLLING_MODE;
        if (!LLBC_LogRollingMode::IsValid(_fileRollingMode))
            _fileRollingMode = LLBC_CFG_LOG_DEFAULT_FILE_ROLLING_MODE;

        // Max file size.
        if (cfg["maxFileSize"])
            _maxFileSize = NormalizeSizeStr(cfg["maxFileSize"],
                                            LLBC_CFG_LOG_DEFAULT_MAX_FILE_SIZE,
                                            LLBC_CFG_LOG_MIN_FILE_SIZE_LIMIT,
                                            LLBC_CFG_LOG_MAX_FILE_SIZE_LIMIT);
        else if (_notConfigUseRoot)
            _maxFileSize = rootCfg->GetMaxFileSize();
        else
            _maxFileSize = LLBC_CFG_LOG_DEFAULT_MAX_FILE_SIZE;
        // Max backup index.
        _maxBackupIndex = __LLBC_GetLogCfg2(
            "maxBackupIndex", LLBC_CFG_LOG_MAX_BACKUP_INDEX, GetMaxBackupIndex, AsInt32);
        // Lazy create log file.
        _lazyCreateLogFile = __LLBC_GetLogCfg2(
            "lazyCreateLogFile", LLBC_CFG_LOG_LAZY_CREATE_LOG_FILE, IsLazyCreateLogFile, AsLooseBool);

        // File buffer size.
        if (_asyncMode)
            _fileBufferSize = __LLBC_GetLogCfg(
                "fileBufferSize", LOG_FILE_BUFFER_SIZE, GetFileBufferSize, AsInt32);

        // Advise discard file page cache.
        _fadviseDiscardEnabled = LLBC_CFG_LOG_DEFAULT_ENABLE_FADVISE_DISCARD;
        if (!cfg["enableFadviseDiscard"].AsStr().strip().empty())
            _fadviseDiscardEnabled = cfg["enableFadviseDiscard"].AsLooseBool();

        if (_fadviseDiscardEnabled)
        {
            // Fadvise discard size.
            _fadviseDiscardSize = NormalizeSizeStr(cfg["fadviseDiscardSize"],
                                                   LLBC_CFG_LOG_DEFAULT_FADVISE_DISCARD_SIZE,
                                                   LLBC_CFG_LOG_FADVISE_DISCARD_SIZE_MIN,
                                                   _maxFileSize);
            // Align fadvise discard size.
            _fadviseDiscardSize = _fadviseDiscardSize & ~(LLBC_pageSize - 1);

            // File page cache retain size.
            int fadviseDiscardRetainPercent = LLBC_CFG_LOG_DEFAULT_FADVISE_DISCARD_RETAIN_PERCENT;
            if (!cfg["fadviseDiscardRetainPercent"].AsStr().strip().empty())
                fadviseDiscardRetainPercent = std::clamp(cfg["fadviseDiscardRetainPercent"].AsInt32(),
                                                         LLBC_CFG_LOG_FADVISE_DISCARD_RETAIN_PERCENT_MIN,
                                                         LLBC_CFG_LOG_FADVISE_DISCARD_RETAIN_PERCENT_MAX);

            // Align page cache retain size.
            _fadviseDiscardRetainSize = (_fadviseDiscardSize * fadviseDiscardRetainPercent / 100) & ~(LLBC_pageSize - 1);
        }
    }

    // Misc configs.
    if (!rootCfg)
        _takeOver = __LLBC_GetLogCfg2(
            "takeOver", LLBC_CFG_LOG_ROOT_LOGGER_TAKE_OVER_UNCONFIGED, IsTakeOver, AsLooseBool);

    // Check configs.
    _maxBackupIndex = MAX(0, _maxBackupIndex);
    _flushInterval = MIN(MAX(0, _flushInterval), LLBC_CFG_LOG_MAX_LOG_FLUSH_INTERVAL);

    // Normalize log file name.
    NormalizeLogFileName();

    // Rejudge root logger 'notConfigUse' option, if is root logger config.
    if (loggerName == LLBC_CFG_LOG_ROOT_LOGGER_NAME)
        _notConfigUseRoot = notCfgUseOpt.strip().tolower() == "root";

    return LLBC_OK;
}

int LLBC_LoggerConfigInfo::GetAppenderLogLevel(int appenderType) const
{
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    if (appenderType == LLBC_LogAppenderType::Console)
    {
        return GetConsoleLogLevel();
    }
    else if (appenderType == LLBC_LogAppenderType::File)
    {
        return GetFileLogLevel();
    }
    else
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_LogLevel::End;
    }
}

void LLBC_LoggerConfigInfo::NormalizeLogFileName()
{
    // Replace process id: %p.
    const LLBC_String curProcId =
        LLBC_Num2Str(LLBC_GetCurrentProcessId());
    _logFile.findreplace("%p", curProcId);

    // Replace exec name: %e/%m.
    // Note: %m pattern is deprecated.
    // TODO: Temporary support pattern additional params.
    while (true)
    {
        auto modFileNamePatternBeg = _logFile.find("%e");
        LLBC_DoIf(modFileNamePatternBeg == LLBC_String::npos, modFileNamePatternBeg = _logFile.find("%m"));
        LLBC_BreakIf(modFileNamePatternBeg == LLBC_String::npos);

        bool needPreserveExecNameLink = false;
        size_t modFileNamePatternEnd = LLBC_String::npos;

        modFileNamePatternEnd = modFileNamePatternBeg + 2;
        if (modFileNamePatternEnd != _logFile.size() && _logFile[modFileNamePatternEnd] == '{')
        {
            ++modFileNamePatternEnd;
            while (modFileNamePatternEnd < _logFile.size() &&
                   _logFile[modFileNamePatternEnd] != '}')
                ++modFileNamePatternEnd;

            if (modFileNamePatternEnd < _logFile.size())
            {
                if (_logFile.substr(
                        modFileNamePatternBeg + 3,
                        modFileNamePatternEnd - modFileNamePatternBeg - 3).strip().tolower() == "preservelink")
                    needPreserveExecNameLink = true;
                ++modFileNamePatternEnd;
            }
        }

        const LLBC_String modFileName =
            LLBC_Directory::SplitExt(LLBC_Directory::ModuleFileName(!needPreserveExecNameLink))[0];
        _logFile.erase(modFileNamePatternBeg, modFileNamePatternEnd - modFileNamePatternBeg);
        _logFile.insert(modFileNamePatternBeg, modFileName);
    };

    // Replace logger name: %l.
    _logFile.findreplace("%l", _loggerName);

    // Replace smart logger name: %L.
    // for root logger:
    //   => %L => root
    //   => xxx_%L => xxx
    //   => xxx__%L => xxx
    //   => xxx-%L => xxx
    //   => xxx--%L => xxx
    // for non-root logger: same as %l.
    if (_loggerName != LLBC_CFG_LOG_ROOT_LOGGER_NAME ||
        LLBC_Directory::BaseName(_logFile) == "%L")
    {
        _logFile.findreplace("%L", _loggerName);
    }
    else
    {
        LLBC_String::size_type smartLoggerNamePos = 0;
        while ((smartLoggerNamePos = _logFile.find("%L", smartLoggerNamePos)) != LLBC_String::npos)
        {
            while (smartLoggerNamePos > 0 &&
                (_logFile[smartLoggerNamePos - 1] == '_' ||
                    _logFile[smartLoggerNamePos - 1] == '-'))
                _logFile.erase(--smartLoggerNamePos);
            _logFile.erase(smartLoggerNamePos, 2);
        }
    }

#if LLBC_TARGET_PLATFORM_IPHONE
    if (_logToFile &&
        !_logFile.empty() &&
        _logFile[0] != '/')
        _logFile = LLBC_Directory::Join(LLBC_Directory::TempDir(), _logFile);
#endif // LLBC_TARGET_PLATFORM_IPHONE
}

sint64 LLBC_LoggerConfigInfo::NormalizeSizeStr(const LLBC_String &sizeStr, sint64 defaultSize, sint64 low, sint64 high)
{
    // strip.
    const LLBC_String nmlSizeStr = sizeStr.strip();

    // If is empty, use default.
    if (nmlSizeStr.empty())
        return defaultSize;

    // Find storage unit.
    LLBC_String::size_type unitPos = 0;
    for (; unitPos < nmlSizeStr.size(); ++unitPos)
    {
        if (!isdigit(nmlSizeStr[unitPos]) && nmlSizeStr[unitPos] != '.')
            break;
    }

    // normalize storage unit.
    const LLBC_String unit = nmlSizeStr.substr(unitPos).strip().tolower();
    double nmlLogFileSize = LLBC_Variant(nmlSizeStr.substr(0, unitPos));
    // - k/kb, kib
    if (unit == "k" || unit == "kb")
        nmlLogFileSize *= 1000.0;
    else if (unit == "kib")
        nmlLogFileSize *= 1024.0;
    // - m/mb, mib
    else if (unit == "m" || unit == "mb")
        nmlLogFileSize *= (1000.0 * 1000.0);
    else if (unit == "mib")
        nmlLogFileSize *= (1024.0 * 1024.0);
    // - g/gb, gib
    else if (unit == "g" || unit == "gb")
        nmlLogFileSize *= (1000.0 * 1000.0 * 1000.0);
    else if (unit == "gib")
        nmlLogFileSize *= (1024.0 * 1024.0 * 1024.0);
    // - unknown/unsupported storage unit, ignore.
    // else
    //     // ... ...

    // Clamp.
    return std::clamp(static_cast<sint64>(nmlLogFileSize), low, high);
}

void LLBC_LoggerConfigInfo::ParseLogControls(const LLBC_String &logControlsStr,
                                             std::vector<LLBC_LogControlItem> &out)
{
    if (logControlsStr.empty())
        return;

    LLBC_Json::Document doc;
    if (doc.Parse(logControlsStr.c_str()).HasParseError() || !doc.IsArray())
        return;

    for (auto it = doc.Begin(); it != doc.End(); ++it)
    {
        if (!it->IsObject())
            continue;
        LLBC_LogControlItem item;

        // 1) match rules. Within one item, enabled rules are AND-combined
        //    (all-of); each rule's own value-set (matchLineRanges /
        //    matchFuncs / matchThreadIds / matchLevels) is OR-combined.
        const auto &m = LLBC_JsonGetMember(*it, "match");
        if (m.IsObject())
        {
            // 1.1) file + line: string-form or object-form. See ParseFileMatch().
            const auto &fileVal = LLBC_JsonGetMember(m, "file");
            if (!fileVal.IsNull())
            {
                LLBC_String name;
                std::vector<std::pair<int, int> > ranges;
                if (ParseFileMatch(fileVal, name, ranges))
                {
                    item.haveFile = true;
                    item.matchFile = std::move(name);
                    item.matchLineRanges = std::move(ranges);
                }
            }

            // 1.2) func: string (single) OR array of strings (multi).
            // Empty / non-string entries silently dropped; the rule
            // is enabled only if at least one valid name remains.
            const auto &funcVal = LLBC_JsonGetMember(m, "func");
            if (!funcVal.IsNull())
            {
                std::vector<LLBC_String> funcs;
                CollectAsVector<LLBC_String>(
                    funcVal,
                    funcs,
                    [](const LLBC_Json::Value &v, LLBC_String &out) {
                        const char *s = LLBC_JsonGetStr(v);
                        if (!s)
                            return false;
                        out = LLBC_String(s).strip();
                        return !out.empty();
                    });
                if (!funcs.empty())
                {
                    item.haveFunc = true;
                    item.matchFuncs = std::move(funcs);
                }
            }

            // 1.3) threadId: integer (single) OR array of integers (multi).
            const auto &tidVal = LLBC_JsonGetMember(m, "threadId");
            if (!tidVal.IsNull())
            {
                std::vector<LLBC_ThreadId> tids;
                CollectAsVector<LLBC_ThreadId>(
                    tidVal,
                    tids,
                    [](const LLBC_Json::Value &v, LLBC_ThreadId &out) {
                        if (v.IsInt64())
                            out = static_cast<LLBC_ThreadId>(v.GetInt64());
                        else if (v.IsInt())
                            out = static_cast<LLBC_ThreadId>(v.GetInt());
                        else
                            return false;
                        return true;
                    });
                if (!tids.empty())
                {
                    item.haveThreadId = true;
                    item.matchThreadIds = std::move(tids);
                }
            }

            // 1.4) level: string / int (single) OR array of strings/ints (multi).
            // Empty / non-level entries silently dropped; the rule
            // is enabled only if at least one valid level remains.
            const auto &levelVal = LLBC_JsonGetMember(m, "level");
            if (!levelVal.IsNull())
            {
                std::vector<int> lvls;
                CollectAsVector<int>(
                    levelVal,
                    lvls,
                    [](const LLBC_Json::Value &v, int &out) {
                        return ParseLevel(v, out);
                    });
                if (!lvls.empty())
                {
                    item.haveLevel = true;
                    item.matchLevels = std::move(lvls);
                }
            }
        }

        if (!item.HasAnyMatch())
            continue;

        // 2) appender scope (optional). Empty / absent == all appenders.
        const auto &appendersVal = LLBC_JsonGetMember(*it, "appenders");
        if (appendersVal.IsArray())
        {
            for (auto ait = appendersVal.Begin(); ait != appendersVal.End(); ++ait)
            {
                int t = 0;
                if (ParseAppenderType(*ait, t))
                    item.appenderTypes.push_back(t);
            }
        }

        // 3) action.
        LLBC_String actionStr;
        if (const char *s = LLBC_JsonGetStr(LLBC_JsonGetMember(*it, "action")))
            actionStr = LLBC_String(s).strip().tolower();
        if (actionStr == "mute")
        {
            item.action = LLBC_LogControlAction::Mute;
        }
        else if (actionStr == "setlevel")
        {
            item.action = LLBC_LogControlAction::SetLevel;
            int nlvl = 0;
            if (!ParseLevel(LLBC_JsonGetMember(*it, "newLevel"), nlvl))
                continue;
            item.newLevel = nlvl;
        }
        else
        {
            continue;
        }

        out.push_back(std::move(item));
    }
}

__LLBC_NS_END

/**
 * Internal macros undef.
 */
#undef __LLBC_GetLogCfg
#undef __LLBC_GetLogCfg2
