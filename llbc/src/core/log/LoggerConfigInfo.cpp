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

/*
 * Internal macros define.
 */
#define __LLBC_GetLogCfg(key, dft, rootMeth, asMeth)  __LLBC_GetLogCfg2(key, LLBC_CFG_LOG_DEFAULT_##dft, rootMeth, asMeth)

#define __LLBC_GetLogCfg2(key, dft, rootMeth, asMeth) (cfg[key] ? \
                                                           cfg[key].asMeth() : \
                                                               (_notConfigUseRoot ? rootCfg->rootMeth() : (dft))) \

__LLBC_NS_BEGIN

// LogControls CIR parsing helpers, file-scope, only used by
// LLBC_LoggerConfigInfo::ParseLogControls. All are total on bad input:
// return false or empty; caller silently skips the offending item.
//
// CIR (Config Intermediate Representation) is a plain map/seq/scalar Variant
// tree produced by LoggerConfigurator regardless of the source format. This
// module never touches LLBC_XMLKeys or any XML-shape metadata -- the schema
// dictionary below is the ONLY contract with LoggerConfigurator.
//
// LogControls CIR schema (see doc in LoggerConfigInfo.h GetLogControls()):
//   Seq<
//     Dict {
//       "match":    Dict { "file": Dict{"name":Str, "lines"?:Str},
//                          "func"|"threadId"|"level": Str | Seq<Str> },
//       "appenders": Str | Seq<Str>,           # optional
//       "action":    Dict { "type":Str, "newLevel"?:Str }
//     }
//   >
//
// Repeated same-name children in XML are always emitted as Seq by the
// Configurator layer; a single occurrence stays as a scalar. Helpers below
// tolerate both shapes so any future loader (yaml/json/...) that already
// emits Seq for singletons continues to work.

// Parse a level: name ("Debug"/"Info"/..., case-insensitive) or decimal int.
static bool ParseLevelStr(const LLBC_String &s, int &outLevel)
{
    if (s.empty())
        return false;

    // Try int form first (LLBC_LogLevel::GetLevelEnum is strict on names).
    if (s.isdigit() || (s.size() > 1 && (s[0] == '-' || s[0] == '+') &&
                        s.substr(1).isdigit()))
    {
        const long n = std::strtol(s.c_str(), nullptr, 10);
        if (n < INT_MIN || n > INT_MAX)
            return false;
        const int lvl = static_cast<int>(n);
        if (!LLBC_LogLevel::IsValid(lvl))
            return false;
        outLevel = lvl;
        return true;
    }

    // Name form.
    const int lvl = LLBC_LogLevel::GetLevelEnum(s);
    if (!LLBC_LogLevel::IsValid(lvl))
        return false;
    outLevel = lvl;
    return true;
}

// Parse an appender type name ("console"/"file"/"network", case-insensitive).
static bool ParseAppenderTypeStr(const LLBC_String &s, int &outType)
{
    if (s.empty())
        return false;
    const int t = LLBC_LogAppenderType::GetTypeEnum(s.c_str());
    if (!LLBC_LogAppenderType::IsValid(t))
        return false;
    outType = t;
    return true;
}

// Parse a thread-id string ("12345"; leading '+'/'-' allowed).
static bool ParseThreadIdStr(const LLBC_String &s, LLBC_ThreadId &outTid)
{
    if (s.empty())
        return false;
    const char *p = s.c_str();
    if (*p == '-' || *p == '+')
        ++p;
    if (*p == '\0')
        return false;
    for (const char *q = p; *q; ++q)
        if (!isdigit(static_cast<unsigned char>(*q)))
            return false;
    char *endp = nullptr;
    const long long n = std::strtoll(s.c_str(), &endp, 10);
    if (!endp || *endp != '\0')
        return false;
    outTid = static_cast<LLBC_ThreadId>(n);
    return true;
}

// Parse the `lines` attr of <file>: comma-separated segments, each "N" (=>
// [N, N+1)) or "N-M" (=> [N, M), M > N). Whitespace tolerated.
// e.g. "100, 102-112" -> [(100,101), (102,112)]. Returns false on any bad
// segment (caller skips the whole item).
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

// Parse a CIR `file` node (Dict{"name":Str, "lines"?:Str}) into (name, ranges).
// Absent / empty `lines` => empty ranges (whole-file wildcard).
static bool ParseFileNode(const LLBC_Variant &fileNode,
                          LLBC_String &outName,
                          std::vector<std::pair<int, int> > &outRanges)
{
    outRanges.clear();
    if (!fileNode.Is<LLBC_Variant::Dict>())
        return false;

    LLBC_String name = fileNode["name"].As<LLBC_Variant::Str>().strip();
    if (name.empty())
        return false;

    const LLBC_String linesSpec = fileNode["lines"].As<LLBC_Variant::Str>().strip();
    if (!linesSpec.empty() && !ParseLineSpec(linesSpec, outRanges))
        return false;

    outName = std::move(name);
    return true;
}

// Normalize a CIR "one-or-many" node into a Seq for uniform iteration.
//
// Accepted input shapes (all sources: xml-CIR, yaml, json, .cfg):
//   - Nil                        -> empty Seq
//   - Seq<T>                     -> copy of the input Seq
//   - Dict with `wrapperKey`     -> unwrap once, then re-normalize
//                                   (handles XML container element wrapping
//                                   repeated children, e.g. <items><item>...
//                                   </item><item>...</item></items>)
//   - Dict (no wrapper) / Str /
//     other scalar               -> 1-element Seq holding the node as-is
//
// The result is a value-typed Seq; callers just iterate `.As<LLBC_Variant::Seq>()` without
// having to branch on scalar/Seq/Dict-wrapper. `wrapperKey` is optional:
// pass empty (default) to skip wrapper unwrapping when a caller knows the
// node is already at leaf level (e.g. Str|Seq<Str>).
static LLBC_Variant NormalizeToSeq(const LLBC_Variant &node,
                                   const LLBC_String &wrapperKey = "")
{
    // Unwrap XML-style single-child container: Dict{ wrapperKey: X } -> X.
    if (!wrapperKey.empty() && node.Is<LLBC_Variant::Dict>() && !node[wrapperKey].Is<void>())
        return NormalizeToSeq(node[wrapperKey], "");

    LLBC_Variant out;
    out.Become<LLBC_Variant::Seq>();
    if (node.Is<void>())
        return out;
    if (node.Is<LLBC_Variant::Seq>())
    {
        for (auto &elem : node.As<LLBC_Variant::Seq>())
            out.SeqPushBack(elem);
        return out;
    }
    // Scalar (Str/Int/...) or Dict-single -> wrap as 1-element Seq.
    out.SeqPushBack(node);
    return out;
}

// Collect a CIR "scalar-or-Seq" leaf (typical for repeated leaf children
// like <func>a</func><func>b</func>) into a typed vector via `extract`.
// Entries where `extract` returns false are dropped silently.
template <typename T, typename Fn>
static void CollectValues(const LLBC_Variant &node,
                          std::vector<T> &out,
                          Fn extract)
{
    T tmp;
    const LLBC_Variant elems = NormalizeToSeq(node);
    for (auto &elem : elems.As<LLBC_Variant::Seq>())
    {
        if (extract(elem, tmp))
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
        notCfgUseOpt = cfg["notConfigUse"].As<LLBC_Variant::Str>();
    else if (rootCfg)
        notCfgUseOpt = rootCfg->IsNotConfigUseRoot() ? "root" : "default";
    else
        notCfgUseOpt = LLBC_CFG_LOG_DEFAULT_NOT_CONFIG_OPTION_USE;
    _notConfigUseRoot = notCfgUseOpt.strip().tolower() == "root" && rootCfg;

    // Log time accessor.
    _logTimeAccessor = &logTimeAccessor;

    // Require color log traces.
    auto keyContentsGroups =
        cfg["requireColorLogTraces"].As<LLBC_Variant::Str>().strip().split(LLBC_CFG_CORE_LOG_TRACE_SEPARATORS[0], -1, true);
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

    ParseLogControls(cfg["logControls"], _logControls);

    _asyncMode = __LLBC_GetLogCfg(
        "asynchronous", ASYNC_MODE, IsAsyncMode, AsLooseBool);
    if (_asyncMode)
        _independentThread = __LLBC_GetLogCfg(
            "independentThread", INDEPENDENT_THREAD, IsIndependentThread, AsLooseBool);
    _flushInterval = __LLBC_GetLogCfg(
        "flushInterval", LOG_FLUSH_INTERVAL, GetFlushInterval, As<int>);

    // Json styled log configs.
    _addTimestampInJsonLog = __LLBC_GetLogCfg(
        "addTimestampInJsonLog", ADD_TIMESTAMP_IN_JSON_LOG, IsAddTimestampInJsonLog, AsLooseBool);

    // Console log configs.
    _logToConsole = __LLBC_GetLogCfg("logToConsole", LOG_TO_CONSOLE, IsLogToConsole, AsLooseBool);
    if (_logToConsole)
    {
        if (cfg["consoleLogLevel"])
            _consoleLogLevel = LLBC_LogLevel::GetLevelEnum(cfg["consoleLogLevel"].As<LLBC_Variant::Str>().c_str());
        else
            _consoleLogLevel = _notConfigUseRoot ? rootCfg->GetConsoleLogLevel() : LLBC_CFG_LOG_DEFAULT_LEVEL;

        if (cfg["consolePattern"])
            _consolePattern = cfg["consolePattern"].As<LLBC_Variant::Str>();
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
            _fileLogLevel = LLBC_LogLevel::GetLevelEnum(cfg["fileLogLevel"].As<LLBC_Variant::Str>().c_str());
        else
            _fileLogLevel = _notConfigUseRoot ? rootCfg->GetFileLogLevel() : LLBC_CFG_LOG_DEFAULT_LEVEL;

        // Log file dir.
        if (!(_logDir = __LLBC_GetLogCfg2("logDir", "", GetLogDir, As<LLBC_String>).strip()).empty())
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
            _logFile = cfg["logFile"].As<LLBC_Variant::Str>().strip();

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
            "logFileSuffix", LOG_FILE_SUFFIX, GetLogFileSuffix, As<LLBC_String>);
        // Log code file path.
        _logCodeFilePath = __LLBC_GetLogCfg(
            "logCodeFilePath", LOG_CODE_FILE_PATH, IsLogCodeFilePath, AsLooseBool);
        // Log file pattern.
        if (cfg["filePattern"])
            _filePattern = cfg["filePattern"].As<LLBC_Variant::Str>();
        else
            _filePattern = _notConfigUseRoot ?
                rootCfg->GetFilePattern().c_str() : LLBC_CFG_LOG_DEFAULT_FILE_LOG_PATTERN;

        // File rolling mode.
        if (cfg["fileRollingMode"])
            _fileRollingMode = LLBC_LogRollingMode::Str2Mode(cfg["fileRollingMode"].As<LLBC_Variant::Str>());
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
            "maxBackupIndex", LLBC_CFG_LOG_MAX_BACKUP_INDEX, GetMaxBackupIndex, As<int>);
        // Lazy create log file.
        _lazyCreateLogFile = __LLBC_GetLogCfg2(
            "lazyCreateLogFile", LLBC_CFG_LOG_LAZY_CREATE_LOG_FILE, IsLazyCreateLogFile, AsLooseBool);

        // File buffer size.
        if (_asyncMode)
            _fileBufferSize = __LLBC_GetLogCfg(
                "fileBufferSize", LOG_FILE_BUFFER_SIZE, GetFileBufferSize, As<int>);

        // Advise discard file page cache.
        _fadviseDiscardEnabled = LLBC_CFG_LOG_DEFAULT_ENABLE_FADVISE_DISCARD;
        if (!cfg["enableFadviseDiscard"].As<LLBC_Variant::Str>().strip().empty())
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
            if (!cfg["fadviseDiscardRetainPercent"].As<LLBC_Variant::Str>().strip().empty())
                fadviseDiscardRetainPercent = std::clamp(cfg["fadviseDiscardRetainPercent"].As<int>(),
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

void LLBC_LoggerConfigInfo::ParseLogControls(const LLBC_Variant &cfg,
                                             std::vector<LLBC_LogControlItem> &out)
{
    // `cfg` == CIR node for the `logControls` key. Expected shapes:
    //   - Dict{"item": Str|Dict|Seq<Dict>} : XML-style container element
    //     wrapping repeated <item> children (a single <item> collapses to a
    //     scalar/Dict in CIR; multiple <item>s produce a Seq of Dicts).
    //   - Seq<Dict>                        : loader-native list (yaml/json).
    //   - Str|Dict{...}                    : single item, no wrapper.
    // Absent (Nil) or malformed shapes are silent no-ops -- keep bootstrap
    // robust; consistent with per-item skip-on-error policy below.
    // NormalizeToSeq handles all shapes uniformly: it unwraps the "item"
    // container when present, and lifts a lone scalar/Dict into a
    // 1-element Seq -- so downstream just iterates.
    const LLBC_Variant items = NormalizeToSeq(cfg, "item");
    for (auto &itemNode : items.As<LLBC_Variant::Seq>())
    {
        if (!itemNode.Is<LLBC_Variant::Dict>())
            continue;

        LLBC_LogControlItem item;

        // 1) match: enabled rules AND-combined; each rule's values OR-combined.
        const LLBC_Variant &matchNode = itemNode["match"];
        if (matchNode.Is<LLBC_Variant::Dict>())
        {
            // 1.1) file: at most one per item (data model carries a single
            //      basename + ranges). A present-but-malformed `file` rejects
            //      the whole item.
            const LLBC_Variant &fileNode = matchNode["file"];
            if (!fileNode.Is<void>())
            {
                LLBC_String name;
                std::vector<std::pair<int, int> > ranges;
                if (!ParseFileNode(fileNode, name, ranges))
                    continue;
                item.file.enabled = true;
                item.file.file = std::move(name);
                item.file.lineRanges = std::move(ranges);
            }

            // 1.2) func: Str | Seq<Str>; empty entries dropped.
            {
                std::vector<LLBC_String> funcs;
                CollectValues<LLBC_String>(
                    matchNode["func"], funcs,
                    [](const LLBC_Variant &elem, LLBC_String &o) {
                        o = elem.As<LLBC_Variant::Str>().strip();
                        return !o.empty();
                    });
                if (!funcs.empty())
                {
                    item.func.enabled = true;
                    item.func.values = std::move(funcs);
                }
            }

            // 1.3) threadId: Str | Seq<Str>.
            {
                std::vector<LLBC_ThreadId> tids;
                CollectValues<LLBC_ThreadId>(
                    matchNode["threadId"], tids,
                    [](const LLBC_Variant &elem, LLBC_ThreadId &o) {
                        return ParseThreadIdStr(elem.As<LLBC_Variant::Str>().strip(), o);
                    });
                if (!tids.empty())
                {
                    item.threadId.enabled = true;
                    item.threadId.values = std::move(tids);
                }
            }

            // 1.4) level: Str | Seq<Str>; name or int accepted.
            {
                std::vector<int> lvls;
                CollectValues<int>(
                    matchNode["level"], lvls,
                    [](const LLBC_Variant &elem, int &o) {
                        return ParseLevelStr(elem.As<LLBC_Variant::Str>().strip(), o);
                    });
                if (!lvls.empty())
                {
                    item.level.enabled = true;
                    item.level.values = std::move(lvls);
                }
            }
        }

        if (!item.HasAnyMatch())
            continue;

        // 2) appenders (optional): Dict{"appender": Str|Seq<Str>}; empty/
        //    absent == all appenders. Unknown names dropped silently.
        //    The dict shape mirrors the XML container element <appenders>
        //    holding repeated <appender> children; a scalar/Seq shortcut
        //    (loader emits appenders directly as Str|Seq<Str>) is also
        //    accepted for source formats that don't need the container.
        //    NormalizeToSeq handles both via the "appender" wrapper key.
        const LLBC_Variant appenders =
            NormalizeToSeq(itemNode["appenders"], "appender");
        for (auto &elem : appenders.As<LLBC_Variant::Seq>())
        {
            int t = 0;
            if (ParseAppenderTypeStr(elem.As<LLBC_Variant::Str>().strip(), t))
                item.appenderTypes.push_back(t);
        }

        // 3) action: Dict{"type":Str, "newLevel"?:Str}. Tagged-union.
        const LLBC_Variant &actionNode = itemNode["action"];
        if (!actionNode.Is<LLBC_Variant::Dict>())
            continue;
        const LLBC_String actionType = actionNode["type"].As<LLBC_Variant::Str>().strip().tolower();
        if (actionType == "mute")
        {
            item.action = LLBC_LogControlAction::Mute;
        }
        else if (actionType == "setlevel")
        {
            item.action = LLBC_LogControlAction::SetLevel;
            // Operand: newLevel. Missing/malformed rejects the whole item.
            int nlvl = 0;
            if (!ParseLevelStr(actionNode["newLevel"].As<LLBC_Variant::Str>().strip(), nlvl))
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
