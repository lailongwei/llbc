// The MIT License (MIT)
//
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

#include <llbc.h>
using namespace llbc;

#include <llbc/core/log/LogData.h>
#include <llbc/core/log/LogFormattingInfo.h>
#include <llbc/core/log/LogLevel.h>
#include <llbc/core/log/LogMessageBuffer.h>
#include <llbc/core/log/LogRollingMode.h>
#include <llbc/core/log/LogTimeAccessor.h>
#include <llbc/core/log/LogTokenChain.h>
#include <llbc/core/log/LogTrace.h>

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <memory>
#include <string>
#include <string_view>
#include <thread>

#if LLBC_TARGET_PLATFORM_NON_WIN32
#include <unistd.h>
#endif

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/log/LogLevel.cpp
// @coverage-target: llbc/src/core/log/LogRollingMode.cpp
// @coverage-target: llbc/src/core/log/LogFormattingInfo.cpp
// @coverage-target: llbc/src/core/log/LogData.cpp
// @coverage-target: llbc/src/core/log/LogTrace.cpp
// @coverage-target: llbc/src/core/log/LogJsonMsg.cpp
// @coverage-target: llbc/src/core/log/LogMessageBuffer.cpp
// @coverage-target: llbc/src/core/log/LogTokenBuilder.cpp
// @coverage-target: llbc/src/core/log/LogTokenChain.cpp
// @coverage-target: llbc/src/core/log/LogStrToken.cpp
// @coverage-target: llbc/src/core/log/LogNameToken.cpp
// @coverage-target: llbc/src/core/log/LogExecNameToken.cpp
// @coverage-target: llbc/src/core/log/LogTagToken.cpp
// @coverage-target: llbc/src/core/log/LogFileToken.cpp
// @coverage-target: llbc/src/core/log/LogLineToken.cpp
// @coverage-target: llbc/src/core/log/LogFunctionToken.cpp
// @coverage-target: llbc/src/core/log/LogThreadIdToken.cpp
// @coverage-target: llbc/src/core/log/LogProcessIdToken.cpp
// @coverage-target: llbc/src/core/log/LogLevelToken.cpp
// @coverage-target: llbc/src/core/log/LogNewLineToken.cpp
// @coverage-target: llbc/src/core/log/LogMsgToken.cpp
// @coverage-target: llbc/src/core/log/LogTimeToken.cpp
// @coverage-target: llbc/src/core/log/LogRealTimeToken.cpp
// @coverage-target: llbc/src/core/log/LogEnvToken.cpp
// @coverage-target: llbc/src/core/log/LogTraceToken.cpp
// @coverage-target: llbc/src/core/log/LogNullToken.cpp
// @coverage-target: llbc/src/core/log/BaseLogAppender.cpp
// @coverage-target: llbc/src/core/log/LogFileAppender.cpp
// @coverage-target: llbc/include/llbc/core/log/LoggerInl.h
// @coverage-target: llbc/include/llbc/core/log/LogJsonMsgInl.h
// @coverage-target: llbc/include/llbc/core/log/LogMessageBufferInl.h
// @coverage-target: llbc/include/llbc/core/log/LogTimeAccessorInl.h
// @coverage-target: llbc/include/llbc/core/log/LogTraceInl.h

namespace
{

enum class TraceTestEnum : sint32
{
    Value = 7,
};

struct StreamedLogValue
{
    int value;
};

std::ostream &operator<<(std::ostream &stream, const StreamedLogValue &value)
{
    return stream << "custom:" << value.value;
}

class ScopedLoggerConfig
{
public:
    ScopedLoggerConfig()
    : _path(std::filesystem::temp_directory_path() /
            ("llbc_unit_logger_" +
             std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) +
             ".cfg"))
    {
    }

    ~ScopedLoggerConfig()
    {
        std::error_code ignored;
        std::filesystem::remove(_path, ignored);
    }

    LLBC_String Path() const
    {
        return LLBC_String(_path.string().c_str());
    }

    void Write(const char *content) const
    {
        LLBC_File file(Path(), LLBC_FileMode::TextWrite);
        ASSERT_TRUE(file.IsOpened());
        ASSERT_EQ(file.Write(content), LLBC_OK);
        ASSERT_EQ(file.Flush(), LLBC_OK);
    }

private:
    std::filesystem::path _path;
};

class ScopedTempDirectory
{
public:
    ScopedTempDirectory()
    : _path(std::filesystem::temp_directory_path() /
            ("llbc_unit_log_dir_" +
             std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())))
    {
    }

    ~ScopedTempDirectory()
    {
        std::error_code ignored;
        std::filesystem::remove_all(_path, ignored);
    }

    const std::filesystem::path &Path() const
    {
        return _path;
    }

private:
    std::filesystem::path _path;
};

std::string ReadFileContents(const std::filesystem::path &path)
{
    std::ifstream input(path, std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(input),
                       std::istreambuf_iterator<char>());
}

#if LLBC_TARGET_PLATFORM_NON_WIN32
class ScopedStdoutRedirect
{
public:
    bool Begin()
    {
        _capture = tmpfile();
        if (!_capture)
            return false;

        fflush(stdout);
        _savedStdout = dup(STDOUT_FILENO);
        if (_savedStdout == -1)
            return false;

        if (dup2(fileno(_capture), STDOUT_FILENO) == -1)
            return false;

        _active = true;
        return true;
    }

    std::string ReadCaptured() const
    {
        if (!_capture)
            return std::string();

        fflush(stdout);
        fflush(_capture);
        const long writePosition = ftell(_capture);
        if (writePosition < 0)
            return std::string();
        fseek(_capture, 0, SEEK_SET);

        std::string content;
        char buffer[256];
        size_t read = 0;
        while ((read = fread(buffer, 1, sizeof(buffer), _capture)) != 0)
            content.append(buffer, read);

        clearerr(_capture);
        fseek(_capture, writePosition, SEEK_SET);
        return content;
    }

    ~ScopedStdoutRedirect()
    {
        if (_active)
        {
            fflush(stdout);
            dup2(_savedStdout, STDOUT_FILENO);
        }
        if (_savedStdout != -1)
            close(_savedStdout);
        if (_capture)
            fclose(_capture);
    }

private:
    FILE *_capture = nullptr;
    int _savedStdout = -1;
    bool _active = false;
};
#endif

} // namespace

// Log levels and rolling modes are configuration-facing enum conversions. Test
// canonical and short names, aliases, case normalization, and invalid values.
TEST(LogBasicsTest, ConvertsLevelsAndRollingModes)
{
    struct LevelCase
    {
        int level;
        const char *full;
        const char *shortName;
    };
    const LevelCase levels[] = {
        {LLBC_LogLevel::Trace, "TRACE", "T"},
        {LLBC_LogLevel::Debug, "DEBUG", "D"},
        {LLBC_LogLevel::Info, "INFO", "I"},
        {LLBC_LogLevel::Warn, "WARN", "W"},
        {LLBC_LogLevel::Error, "ERROR", "E"},
        {LLBC_LogLevel::Fatal, "FATAL", "F"},
    };

    for (const auto &level : levels)
    {
        EXPECT_STREQ(LLBC_LogLevel::GetLevelStr(level.level).c_str(), level.full);
        EXPECT_STREQ(LLBC_LogLevel::GetLevelStr(level.level, true).c_str(), level.shortName);
        EXPECT_EQ(LLBC_LogLevel::GetLevelEnum(LLBC_String(level.full).tolower()), level.level);
        EXPECT_TRUE(LLBC_LogLevel::IsValid(level.level));
    }
    EXPECT_STREQ(LLBC_LogLevel::GetLevelStr(-1).c_str(), "UNKNOWN");
    EXPECT_STREQ(LLBC_LogLevel::GetLevelStr(LLBC_LogLevel::End, true).c_str(), "U");
    EXPECT_EQ(LLBC_LogLevel::GetLevelEnum(""), LLBC_LogLevel::End);
    EXPECT_EQ(LLBC_LogLevel::GetLevelEnum("not-a-level"), LLBC_LogLevel::End);
    EXPECT_FALSE(LLBC_LogLevel::IsValid(-1));
    EXPECT_FALSE(LLBC_LogLevel::IsValid(LLBC_LogLevel::End));

    EXPECT_STREQ(LLBC_LogRollingMode::GetModeStr(LLBC_LogRollingMode::NoRolling).c_str(),
                 "NOROLLING");
    EXPECT_STREQ(LLBC_LogRollingMode::GetModeStr(LLBC_LogRollingMode::HourlyRolling).c_str(),
                 "HOURLYROLLING");
    EXPECT_STREQ(LLBC_LogRollingMode::GetModeStr(LLBC_LogRollingMode::DailyRolling).c_str(),
                 "DAILYROLLING");
    EXPECT_STREQ(LLBC_LogRollingMode::GetModeStr(-1).c_str(), "UKNLOGROLLING");
    EXPECT_EQ(LLBC_LogRollingMode::Str2Mode("hourly"), LLBC_LogRollingMode::HourlyRolling);
    EXPECT_EQ(LLBC_LogRollingMode::Str2Mode("HOURLYROLLING"),
              LLBC_LogRollingMode::HourlyRolling);
    EXPECT_EQ(LLBC_LogRollingMode::Str2Mode("DAILYROLLING"), LLBC_LogRollingMode::DailyRolling);
    EXPECT_EQ(LLBC_LogRollingMode::Str2Mode("daily"), LLBC_LogRollingMode::DailyRolling);
    EXPECT_EQ(LLBC_LogRollingMode::Str2Mode("no"), LLBC_LogRollingMode::NoRolling);
    EXPECT_EQ(LLBC_LogRollingMode::Str2Mode("NOROLLING"), LLBC_LogRollingMode::NoRolling);
    EXPECT_EQ(LLBC_LogRollingMode::Str2Mode("invalid"), LLBC_LogRollingMode::End);
    EXPECT_TRUE(LLBC_LogRollingMode::IsValid(LLBC_LogRollingMode::NoRolling));
    EXPECT_FALSE(LLBC_LogRollingMode::IsValid(LLBC_LogRollingMode::End));
}

// Formatting metadata is applied by log tokens after they append a field. Its
// field clamp, left/right padding, and reset behavior must be deterministic.
TEST(LogBasicsTest, FormatsFieldsAndResetsMetadata)
{
    LLBC_LogFormattingInfo format;
    EXPECT_TRUE(format.leftAlign);
    EXPECT_EQ(format.minLen, 0);
    EXPECT_EQ(format.fillCharacter, ' ');

    format.minLen = 5;
    format.fillCharacter = '.';
    LLBC_String left("x");
    format.Format(left, 0);
    EXPECT_EQ(left, "x....");

    format.leftAlign = false;
    LLBC_String right("prex");
    format.Format(right, 3);
    EXPECT_EQ(right, "pre....x");

    format.leftAlign = true;
    LLBC_String clamped("abc");
    format.Format(clamped, -8);
    EXPECT_EQ(clamped, "abc..");

    LLBC_String endClamped("abc");
    format.Format(endClamped, 100);
    EXPECT_EQ(endClamped, "abc.....");

    format.minLen = 1;
    LLBC_String alreadyWide("abc");
    format.Format(alreadyWide, 0);
    EXPECT_EQ(alreadyWide, "abc");

    format.addiParam = "token-param";
    format.Reset();
    EXPECT_TRUE(format.leftAlign);
    EXPECT_EQ(format.minLen, 0);
    EXPECT_EQ(format.fillCharacter, ' ');
    EXPECT_TRUE(format.addiParam.empty());
}

// Log data objects are pooled message envelopes. Reuse must retain small
// allocations but release oversized ones, reset metadata, and preserve safe
// typed-pool access without requiring a live logger.
TEST(LogBasicsTest, ReusesLogDataBuffersAndMetadata)
{
    {
        LLBC_LogData smallData;
        smallData.msg = LLBC_Malloc(char, 32);
        ASSERT_NE(smallData.msg, nullptr);
        smallData.msgCap = 32;
        smallData.msgLen = 7;
        smallData.fileLen = 3;
        smallData.funcLen = 4;
        smallData.tagLen = 5;
        smallData.logTrace = std::make_shared<LLBC_String>("trace");
        char *smallBuffer = smallData.msg;
        smallData.Reuse();
        EXPECT_EQ(smallData.msg, smallBuffer);
        EXPECT_EQ(smallData.msgCap, 32);
        EXPECT_EQ(smallData.msgLen, 0);
        EXPECT_EQ(smallData.fileLen, 0);
        EXPECT_EQ(smallData.funcLen, 0);
        EXPECT_EQ(smallData.tagLen, 0);
        EXPECT_FALSE(smallData.logTrace);
    }

    LLBC_LogData largeData;
    EXPECT_EQ(largeData.GetTypedObjPool(), nullptr);
    largeData.SetTypedObjPool(nullptr);
    EXPECT_EQ(largeData.GetTypedObjPool(), nullptr);
    largeData.msg = LLBC_Malloc(char, 32);
    ASSERT_NE(largeData.msg, nullptr);
    largeData.msgLen = 1;
    largeData.msgCap = LLBC_CFG_LOG_FORMAT_BUF_SIZE;
    largeData.logTrace = std::make_shared<LLBC_String>("large");
    largeData.Reuse();
    EXPECT_EQ(largeData.msg, nullptr);
    EXPECT_EQ(largeData.msgCap, 0);
    EXPECT_EQ(largeData.msgLen, 0);
    EXPECT_FALSE(largeData.logTrace);
}

// Stream-style logging first appends lightweight text directly, then lazily
// creates an ostream when numeric/custom formatting is needed. Both paths must
// keep one coherent buffer and retain standard stream formatting state.
TEST(LogBasicsTest, BuildsMessagesThroughDirectAndStreamOperators)
{
    LLBC_LogMessageBuffer message;
    LLBC_String llbcText("llbc");
    std::string stdText("-std");
    char mutableText[] = "-mutable";
    char *nullText = nullptr;

    message << llbcText << stdText << mutableText << "-" << nullText << ':' << 'x';
    EXPECT_EQ(message.str(message), "llbc-std-mutable-null:x");

    std::ostream &stream = message << true;
    static_cast<void>(message << static_cast<uint8>('A'));
    static_cast<void>(message << static_cast<sint16>(-2));
    static_cast<void>(message << static_cast<uint16>(3));
    static_cast<void>(message << static_cast<sint32>(-4));
    static_cast<void>(message << static_cast<uint32>(5));
    static_cast<void>(message << static_cast<sint64>(-6));
    static_cast<void>(message << static_cast<uint64>(7));
    static_cast<void>(message << static_cast<long>(-8));
    static_cast<void>(message << static_cast<ulong>(9));
    static_cast<void>(message << std::hex);
    static_cast<void>(message << static_cast<uint32>(255));
    static_cast<void>(message << std::dec);
    static_cast<void>(message << std::fixed);
    static_cast<void>(message << std::setprecision(1));
    static_cast<void>(message << 1.25f);
    static_cast<void>(message << 2.5);
    static_cast<void>(message << static_cast<ldouble>(3.75));
    static_cast<void>(message << StreamedLogValue {10});

    const LLBC_String &streamed = message.str(stream);
    EXPECT_NE(streamed.find("llbc-std-mutable-null:x165-23-45-67-89ff1.22.53.8custom:10"),
              static_cast<LLBC_String::size_type>(-1));

    void *pointer = reinterpret_cast<void *>(static_cast<uintptr_t>(0x1234));
    std::ostream &pointerStream = message << pointer;
    const LLBC_String &withPointer = message.str(pointerStream);
    EXPECT_NE(withPointer.find("0x"), static_cast<LLBC_String::size_type>(-1));

    std::string streamedStdText("-stream-std");
    char streamedMutableText[] = "-stream-mutable";
    const char *streamedNullText = nullptr;
    message << streamedStdText << streamedMutableText << streamedNullText << '!';
    message << LLBC_String("-tail");
    std::ostream &tailStream = message << static_cast<sint32>(11);
    EXPECT_NE(message.str(tailStream).find("-stream-std-stream-mutablenull!-tail11"),
              static_cast<LLBC_String::size_type>(-1));
}

// Trace values originate from API parameters, configuration, and request
// metadata. Every supported string representation must be normalized into the
// fixed-size trace buffers without losing terminators or corrupting adjacent
// trace fields.
TEST(LogBasicsTest, NormalizesTraceDataAndPreservesFixedBufferInvariants)
{
    const char *requestKey = "request";
    const std::string_view viewContent("render");
    LLBC_LogTrace logTrace(requestKey, viewContent);
    EXPECT_EQ(std::string(logTrace.traceKey.str, logTrace.traceKey.strLen), "request");
    EXPECT_EQ(std::string(logTrace.traceContent.str, logTrace.traceContent.strLen), "render");

    LLBC_LogTrace fromVariant(LLBC_Variant(42), LLBC_String("value"));
    EXPECT_EQ(std::string(fromVariant.traceKey.str, fromVariant.traceKey.strLen), "42");
    EXPECT_EQ(std::string(fromVariant.traceContent.str, fromVariant.traceContent.strLen), "value");

    const LLBC_LogTrace::TraceKey nullKey(nullptr);
    EXPECT_FALSE(static_cast<bool>(nullKey));
    EXPECT_TRUE(!nullKey);
    const char *nullCharPtr = nullptr;
    const LLBC_LogTrace::TraceKey nullPointerKey(nullCharPtr);
    EXPECT_FALSE(static_cast<bool>(nullPointerKey));
    EXPECT_EQ(nullPointerKey.str[0], '\0');

    const char emptyArray[] = "";
    const LLBC_LogTrace::TraceKey emptyArrayKey(emptyArray);
    const LLBC_LogTrace::TraceKey emptyStringKey {std::string()};
    EXPECT_FALSE(static_cast<bool>(emptyArrayKey));
    EXPECT_FALSE(static_cast<bool>(emptyStringKey));

    const LLBC_CString borrowedContent("borrowed");
    const LLBC_LogTrace::TraceContent cstringContent(borrowedContent);
    EXPECT_EQ(std::string(cstringContent.str, cstringContent.strLen), "borrowed");
    const LLBC_LogTrace::TraceContent enumContent(TraceTestEnum::Value);
    const LLBC_LogTrace::TraceContent boolContent(true);
    EXPECT_EQ(std::string(enumContent.str, enumContent.strLen), "7");
    EXPECT_EQ(std::string(boolContent.str, boolContent.strLen), "1");

    const std::string longKey(LLBC_CFG_CORE_LOG_TRACE_KEY_LIMIT + 8, 'k');
    const LLBC_LogTrace::TraceKey truncatedKey(longKey);
    EXPECT_EQ(truncatedKey.strLen, static_cast<size_t>(LLBC_CFG_CORE_LOG_TRACE_KEY_LIMIT - 1));
    EXPECT_EQ(truncatedKey.str[truncatedKey.strLen], '\0');
    EXPECT_EQ(std::string(truncatedKey.str, truncatedKey.strLen),
              std::string(LLBC_CFG_CORE_LOG_TRACE_KEY_LIMIT - 1, 'k'));

    const LLBC_LogTrace::TraceKey numericKey(std::numeric_limits<uint64>::max());
    EXPECT_EQ(numericKey.strLen, static_cast<size_t>(LLBC_CFG_CORE_LOG_TRACE_KEY_LIMIT - 1));
    EXPECT_EQ(numericKey.str[numericKey.strLen], '\0');
    EXPECT_EQ(std::string(numericKey.str, numericKey.strLen), "184467440737095");

    LLBC_LogTrace::TraceKey alpha("abc");
    const LLBC_LogTrace::TraceKey beta("abd");
    EXPECT_LT(alpha, beta);
    EXPECT_NE(alpha, beta);
    const LLBC_LogTrace::TraceKey shortKey("a");
    const LLBC_LogTrace::TraceKey longerKey("bb");
    EXPECT_LT(shortKey, longerKey);
    EXPECT_FALSE(longerKey < shortKey);
    EXPECT_FALSE(emptyArrayKey < emptyStringKey);
    EXPECT_NE(alpha, LLBC_LogTrace::TraceKey("ab"));
    LLBC_LogTrace::TraceKey assignedKey;
    assignedKey = alpha;
    EXPECT_EQ(assignedKey, alpha);
    LLBC_LogTrace::TraceKey *alphaAlias = &alpha;
    alpha = *alphaAlias;
    EXPECT_EQ(alpha, LLBC_LogTrace::TraceKey("abc"));

    LLBC_LogTrace copied(logTrace);
    EXPECT_EQ(copied.traceKey, logTrace.traceKey);
    LLBC_LogTrace assignedTrace(0, "");
    assignedTrace = copied;
    EXPECT_EQ(assignedTrace.traceKey, copied.traceKey);
    EXPECT_EQ(assignedTrace.traceContent, copied.traceContent);
    LLBC_LogTrace *copiedAlias = &copied;
    copied = *copiedAlias;
    EXPECT_EQ(copied.traceContent, logTrace.traceContent);
}

// Log timestamps support deterministic offsets used by test environments and
// replay pipelines while still exposing unmodified wall-clock values.
TEST(LogBasicsTest, AppliesConfiguredLogTimeOffsets)
{
    LLBC_LogTimeAccessor accessor;
    EXPECT_EQ(accessor.GetLogTimeOffset(), 0);

    accessor.SetLogTimeOffset(LLBC_TimeSpan::FromMicros(123456));
    EXPECT_EQ(accessor.GetLogTimeOffset(), 123456);

    const sint64 realMicros = accessor.RealNowInMicroseconds();
    const sint64 adjustedMicros = accessor.NowInMicroseconds();
    EXPECT_GE(adjustedMicros - realMicros, 123456);

    const sint64 realMillis = accessor.RealNowInMilliseconds();
    const sint64 adjustedMillis = accessor.NowInMilliseconds();
    EXPECT_GE(adjustedMillis - realMillis, 123);
}

// The exported token-chain API accepts a default pattern once, can be rebuilt
// after cleanup, and preserves escaped or trailing percent characters exactly.
TEST(LogBasicsTest, BuildsAndRebuildsConsoleTokenChains)
{
    LLBC_LogTimeAccessor accessor;
    LLBC_LogTokenChain chain;
    EXPECT_EQ(chain.Build("", accessor), LLBC_OK);
    EXPECT_EQ(chain.Build("ignored", accessor), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_REENTRY);

    chain.Cleanup();
    ASSERT_EQ(chain.Build("literal %% tail", accessor), LLBC_OK);
    LLBC_LogData data;
    LLBC_String formatted;
    chain.Format(data, formatted);
    EXPECT_EQ(formatted, "literal % tail");

    chain.Cleanup();
    ASSERT_EQ(chain.Build("trailing%", accessor), LLBC_OK);
    formatted.clear();
    chain.Format(data, formatted);
    EXPECT_EQ(formatted, "trailing%");
}

// The public LoggerMgr API owns the hidden trace manager. Initialize a
// synchronous logger without appenders so trace counts, color matching, hooks,
// limits, removal, reload, and cleanup can be verified without writing logs to
// the terminal or filesystem.
TEST(LogBasicsTest, ManagesLogTracesThroughConfiguredLogger)
{
    ScopedLoggerConfig config;
    config.Write(
        "root.asynchronous=false\n"
        "root.takeOver=true\n"
        "root.notConfigUse=default\n"
        "root.addTimestampInJsonLog=false\n"
        "root.logToConsole=false\n"
        "root.logToFile=false\n"
        "root.consoleLogLevel=FATAL\n"
        "root.fileLogLevel=FATAL\n");

    LLBC_LoggerMgr manager;
    EXPECT_FALSE(manager.IsInited());
    EXPECT_EQ(manager.GetRootLogger(), nullptr);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_INIT);
    EXPECT_EQ(manager.Reload(), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_INIT);

    ASSERT_EQ(manager.Initialize(config.Path(), LLBC_TimeSpan::FromMicros(1000)), LLBC_OK);
    ASSERT_TRUE(manager.IsInited());
    EXPECT_EQ(manager.GetLogTimeAccessor().GetLogTimeOffset(), 1000);

    LLBC_Logger *root = manager.GetRootLogger();
    ASSERT_NE(root, nullptr);
    EXPECT_EQ(manager.GetLogger("unknown-logger"), root);
    EXPECT_EQ(root->GetLoggerName(), "root");
    EXPECT_EQ(root->GetLogTimeAccessor().GetLogTimeOffset(), 1000);
    static_cast<void>(root->GetLogLevel());
    static_cast<void>(root->GetLoggerObjPool());

    const LLBC_LogTrace activeLogTrace("req", "id1");
    ASSERT_EQ(root->AddLogTrace(activeLogTrace), LLBC_OK);
    ASSERT_EQ(root->AddLogTrace(activeLogTrace), LLBC_OK);
    EXPECT_EQ(root->GetLogTraceTimes(activeLogTrace), 2lu);
    ASSERT_EQ(root->RemoveLogTrace(activeLogTrace, false), LLBC_OK);
    EXPECT_EQ(root->GetLogTraceTimes(activeLogTrace), 1lu);

    ASSERT_EQ(root->AddColorLogTrace(activeLogTrace), LLBC_OK);
    EXPECT_TRUE(root->GetColorLogTag());
    EXPECT_EQ(root->AddColorLogTrace(activeLogTrace), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_REPEAT);

    int hookCalls = 0;
    std::string hookMessage;
    std::string hookTrace;
    ASSERT_EQ(root->SetLogHook(
                  {LLBC_LogLevel::Trace,
                   LLBC_LogLevel::Debug,
                   LLBC_LogLevel::Info,
                   LLBC_LogLevel::Warn,
                   LLBC_LogLevel::Error,
                   LLBC_LogLevel::Fatal},
                  [&](const LLBC_LogData *data) {
                      ++hookCalls;
                      if (data->level == LLBC_LogLevel::Info)
                      {
                          hookMessage.assign(data->msg, data->msgLen);
                          if (data->logTrace)
                              hookTrace.assign(data->logTrace->data(), data->logTrace->size());
                      }
                  }),
              LLBC_OK);
    EXPECT_EQ(root->Trace("trace-test", "/tmp/demo.cpp", 4, "TraceFn", "trace"), LLBC_OK);
    EXPECT_EQ(root->Debug("trace-test", "/tmp/demo.cpp", 5, "TraceFn", "debug"), LLBC_OK);
    EXPECT_EQ(root->Info("trace-test", "/tmp/demo.cpp", 7, "TraceFn", "payload:%d", 9), LLBC_OK);
    EXPECT_EQ(root->Warn("trace-test", "/tmp/demo.cpp", 9, "TraceFn", "warn"), LLBC_OK);
    EXPECT_EQ(root->Error("trace-test", "/tmp/demo.cpp", 10, "TraceFn", "error"), LLBC_OK);
    EXPECT_EQ(root->Fatal("trace-test", "/tmp/demo.cpp", 11, "TraceFn", "fatal"), LLBC_OK);
    EXPECT_EQ(root->Output(LLBC_LogLevel::Debug,
                           "trace-test",
                           "/tmp/demo.cpp",
                           12,
                           "TraceFn",
                           "output"),
              LLBC_OK);
    EXPECT_EQ(hookCalls, 7);
    EXPECT_EQ(hookMessage, "payload:9");
    EXPECT_EQ(hookTrace, "req:id1");
    ASSERT_EQ(root->SetLogHook(
                  {LLBC_LogLevel::Trace,
                   LLBC_LogLevel::Debug,
                   LLBC_LogLevel::Info,
                   LLBC_LogLevel::Warn,
                   LLBC_LogLevel::Error,
                   LLBC_LogLevel::Fatal},
                  nullptr),
              LLBC_OK);

    EXPECT_EQ(root->RemoveColorLogTrace("req", "missing-content"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    ASSERT_EQ(root->AddColorLogTrace("color-key", "color-content"), LLBC_OK);
    EXPECT_EQ(root->RemoveColorLogKey(LLBC_LogTrace::TraceKey("missing-color-key")),
              LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    ASSERT_EQ(root->RemoveColorLogKey(LLBC_LogTrace::TraceKey("color-key")), LLBC_OK);
    ASSERT_EQ(root->RemoveColorLogTrace(activeLogTrace), LLBC_OK);
    EXPECT_FALSE(root->GetColorLogTag());
    EXPECT_EQ(root->RemoveColorLogTrace(activeLogTrace), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    EXPECT_EQ(root->Info("trace-test", "/tmp/demo.cpp", 13, "TraceFn", "suppressed"), LLBC_OK);
    EXPECT_EQ(root->Output(LLBC_LogLevel::Info,
                           "trace-test",
                           "/tmp/demo.cpp",
                           14,
                           "TraceFn",
                           "suppressed"),
              LLBC_OK);
    ASSERT_EQ(root->AddLogTrace("clear-color", "content"), LLBC_OK);
    ASSERT_EQ(root->AddColorLogTrace("clear-color", "content"), LLBC_OK);
    EXPECT_TRUE(root->GetColorLogTag());
    root->ClearAllColorLogTraces();
    EXPECT_FALSE(root->GetColorLogTag());

    ASSERT_EQ(root->RemoveLogTrace(activeLogTrace, true), LLBC_OK);
    EXPECT_EQ(root->GetLogTraceTimes(activeLogTrace), 0lu);
    EXPECT_EQ(root->RemoveLogTrace(activeLogTrace, false), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    root->ClearLogTrace(LLBC_LogTrace::TraceKey("missing-log-key"));

    for (int index = 0; index < LLBC_CFG_CORE_LOG_TRACE_SAME_KEY_CONTENT_COUNT_LIMIT; ++index)
    {
        const LLBC_String content = LLBC_String().format("value-%d", index);
        EXPECT_EQ(root->AddLogTrace("limit", content), LLBC_OK);
    }
    EXPECT_EQ(root->AddLogTrace("limit", "one-too-many"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_LIMIT);
    EXPECT_EQ(root->AddLogTrace("invalid", ""), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_INVALID);

    root->ClearLogTrace(LLBC_LogTrace::TraceKey("limit"));
    root->ClearLogTrace("missing-log-key-template");
    EXPECT_EQ(root->GetLogTraceTimes("limit", "value-0"), 0lu);
    EXPECT_EQ(root->RemoveLogTrace("missing", "value", false), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);

    const std::string longTraceContent(LLBC_CFG_CORE_LOG_TRACE_KEY_LIMIT - 1, 'x');
    for (int index = 0; index < 20; ++index)
    {
        const LLBC_String key = LLBC_String().format("key-%02d", index);
        ASSERT_EQ(root->AddLogTrace(key, longTraceContent), LLBC_OK);
    }
    ASSERT_EQ(root->AddColorLogTrace("key-00", longTraceContent), LLBC_OK);
    size_t truncatedTraceSize = 0;
    ASSERT_EQ(root->SetLogHook(LLBC_LogLevel::Info, [&](const LLBC_LogData *data) {
        truncatedTraceSize = data->logTrace ? data->logTrace->size() : 0;
    }), LLBC_OK);
    EXPECT_EQ(root->Info("trace-limit", "/tmp/demo.cpp", 8, "TraceFn", "limit"), LLBC_OK);
    EXPECT_EQ(truncatedTraceSize,
              static_cast<size_t>(LLBC_CFG_CORE_LOG_TRACE_BUILT_CONTENT_SIZE_LIMIT - 1));
    ASSERT_EQ(root->SetLogHook(LLBC_LogLevel::Info, nullptr), LLBC_OK);
    root->ClearAllColorLogTraces();
    root->ClearAllLogTraces();
    EXPECT_EQ(root->GetLogTraceTimes(activeLogTrace), 0lu);

    EXPECT_EQ(manager.Reload(), LLBC_OK);
    manager.Finalize();
    EXPECT_FALSE(manager.IsInited());
}

// Console patterns are configured through the public logger API. Exercise the
// parser and every normally reachable token in one synchronous record, including
// formatting modifiers, escaped percent signs, environment values, and traces.
TEST(LogBasicsTest, FormatsConfiguredConsolePatternTokens)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    const char *path = std::getenv("PATH");
    ASSERT_NE(path, nullptr);
    ASSERT_NE(*path, '\0');

    ScopedStdoutRedirect redirect;
    ASSERT_TRUE(redirect.Begin());

    ScopedLoggerConfig config;
    config.Write(
        "root.asynchronous=false\n"
        "root.takeOver=true\n"
        "root.notConfigUse=default\n"
        "root.logToConsole=true\n"
        "root.consoleLogLevel=TRACE\n"
        "root.consolePattern=TOKEN|%N|%e|%g|%f|%l|%F|%t|%p|%L{short}|%m|%T|%R|%R{alwaysformat}|%E{PATH}|%%|%x{enablelevel:INFO}|%-12m|%q|END%n\n"
        "root.colourfulOutput=false\n"
        "root.logToFile=false\n"
        "root.fileLogLevel=FATAL\n");

    LLBC_LoggerMgr manager;
    ASSERT_EQ(manager.Initialize(config.Path()), LLBC_OK);
    LLBC_Logger *root = manager.GetRootLogger();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(root->AddLogTrace("token-key", "trace-content"), LLBC_OK);

    ASSERT_EQ(root->Info("token-tag", "/tmp/tokens.cpp", 37, "TokenFunction", "payload"),
              LLBC_OK);
    ASSERT_EQ(root->Info("token-tag", "/tmp/tokens.cpp", 2001, "TokenFunction", "payload"),
              LLBC_OK);

    const std::string captured = redirect.ReadCaptured();
    manager.Finalize();

    const size_t firstBegin = captured.find("TOKEN|");
    ASSERT_NE(firstBegin, std::string::npos);
    const size_t firstEnd = captured.find('\n', firstBegin);
    ASSERT_NE(firstEnd, std::string::npos);
    const std::string firstLine = captured.substr(firstBegin, firstEnd - firstBegin);

    const size_t secondBegin = captured.find("TOKEN|", firstEnd + 1);
    ASSERT_NE(secondBegin, std::string::npos);
    const size_t secondEnd = captured.find('\n', secondBegin);
    ASSERT_NE(secondEnd, std::string::npos);
    const std::string secondLine = captured.substr(secondBegin, secondEnd - secondBegin);

    EXPECT_NE(firstLine.find("TOKEN|root|"), std::string::npos);
    EXPECT_NE(firstLine.find("|token-tag|/tmp/tokens.cpp|37|TokenFunction|"),
              std::string::npos);
    EXPECT_NE(firstLine.find("|I|payload|"), std::string::npos);
    EXPECT_NE(firstLine.find(std::string("|") + path + "|%|token-key:trace-content|payload"),
              std::string::npos);
    EXPECT_NE(firstLine.find("payload     ||END"), std::string::npos);
    EXPECT_NE(secondLine.find("|/tmp/tokens.cpp|2001|TokenFunction|"),
              std::string::npos);
    EXPECT_NE(secondLine.find("|I|payload|"), std::string::npos);
#else
    GTEST_SKIP() << "stdout redirection is POSIX-specific";
#endif
}

// File appenders must create their parent directories on demand, defer file
// creation when configured, and retain a rotated file before reopening a fresh
// output file once the configured size threshold is crossed.
TEST(LogBasicsTest, WritesLazyFilesAndRotatesOversizedLogs)
{
    ScopedTempDirectory directory;
    const std::filesystem::path logDirectory = directory.Path() / "nested";
    const std::filesystem::path lazyBase = logDirectory / "lazy";
    const std::filesystem::path lazyFile = logDirectory / "lazy.log";

    ScopedLoggerConfig lazyConfig;
    const std::string lazyConfigText =
        "root.asynchronous=false\n"
        "root.takeOver=true\n"
        "root.notConfigUse=default\n"
        "root.logToConsole=false\n"
        "root.logToFile=true\n"
        "root.fileLogLevel=TRACE\n"
        "root.filePattern=%m%n\n"
        "root.logFile=" + lazyBase.generic_string() + "\n"
        "root.logFileSuffix=.log\n"
        "root.fileRollingMode=NoRolling\n"
        "root.lazyCreateLogFile=true\n";
    lazyConfig.Write(lazyConfigText.c_str());

    {
        LLBC_LoggerMgr manager;
        ASSERT_EQ(manager.Initialize(lazyConfig.Path()), LLBC_OK);
        EXPECT_TRUE(std::filesystem::is_directory(logDirectory));
        EXPECT_FALSE(std::filesystem::exists(lazyFile));

        LLBC_Logger *root = manager.GetRootLogger();
        ASSERT_NE(root, nullptr);
        ASSERT_EQ(root->Info("file-test", "/tmp/file.cpp", 1, "FileFn", "lazy-message"),
                  LLBC_OK);
        manager.Finalize();
    }
    EXPECT_TRUE(std::filesystem::exists(lazyFile));
    EXPECT_NE(ReadFileContents(lazyFile).find("lazy-message\n"), std::string::npos);

    const std::filesystem::path rollingBase = logDirectory / "rolling";
    const std::filesystem::path rollingFile = logDirectory / "rolling.log";
    const std::filesystem::path backupFile = logDirectory / "rolling.1.log";
    const std::filesystem::path secondBackupFile = logDirectory / "rolling.2.log";
    ScopedLoggerConfig rollingConfig;
    const std::string rollingConfigText =
        "root.asynchronous=false\n"
        "root.takeOver=true\n"
        "root.notConfigUse=default\n"
        "root.logToConsole=false\n"
        "root.logToFile=true\n"
        "root.fileLogLevel=TRACE\n"
        "root.filePattern=%m%n\n"
        "root.logFile=" + rollingBase.generic_string() + "\n"
        "root.logFileSuffix=.log\n"
        "root.fileRollingMode=NoRolling\n"
        "root.maxFileSize=1MiB\n"
        "root.maxBackupIndex=2\n"
        "root.lazyCreateLogFile=false\n";
    rollingConfig.Write(rollingConfigText.c_str());

    {
        LLBC_LoggerMgr manager;
        ASSERT_EQ(manager.Initialize(rollingConfig.Path()), LLBC_OK);
        LLBC_Logger *root = manager.GetRootLogger();
        ASSERT_NE(root, nullptr);

        const std::string payload(LLBC_CFG_LOG_FORMAT_BUF_SIZE - 64, 'r');
        for (int index = 0; index < 130; ++index)
        {
            const std::string message = "roll-" + std::to_string(index) + ":" + payload;
            ASSERT_EQ(root->Info("file-test", "/tmp/file.cpp", index, "FileFn", "%s", message.c_str()),
                      LLBC_OK);
        }
        ASSERT_EQ(root->Info("file-test", "/tmp/file.cpp", 99, "FileFn", "after-roll"), LLBC_OK);
        manager.Finalize();
    }

    ASSERT_TRUE(std::filesystem::exists(backupFile));
    ASSERT_TRUE(std::filesystem::exists(secondBackupFile));
    ASSERT_TRUE(std::filesystem::exists(rollingFile));
    EXPECT_NE(ReadFileContents(secondBackupFile).find("roll-0:"), std::string::npos);
    EXPECT_NE(ReadFileContents(backupFile).find("roll-65:"), std::string::npos);
    EXPECT_NE(ReadFileContents(rollingFile).find("after-roll\n"), std::string::npos);

    const std::filesystem::path hourlyBase = logDirectory / "hourly";
    ScopedLoggerConfig hourlyConfig;
    const std::string hourlyConfigText =
        "root.asynchronous=false\n"
        "root.takeOver=true\n"
        "root.notConfigUse=default\n"
        "root.logToConsole=false\n"
        "root.logToFile=true\n"
        "root.fileLogLevel=TRACE\n"
        "root.filePattern=%m%n\n"
        "root.logFile=" + hourlyBase.generic_string() + "\n"
        "root.logFileSuffix=.log\n"
        "root.fileRollingMode=Hourly\n"
        "root.lazyCreateLogFile=false\n";
    hourlyConfig.Write(hourlyConfigText.c_str());

    {
        LLBC_LoggerMgr manager;
        ASSERT_EQ(manager.Initialize(hourlyConfig.Path()), LLBC_OK);
        LLBC_Logger *root = manager.GetRootLogger();
        ASSERT_NE(root, nullptr);
        ASSERT_EQ(root->Info("file-test", "/tmp/file.cpp", 100, "FileFn", "hourly-message"),
                  LLBC_OK);
        manager.Finalize();
    }

    std::filesystem::path hourlyFile;
    for (const auto &entry : std::filesystem::directory_iterator(logDirectory))
    {
        const std::string name = entry.path().filename().string();
        if (entry.is_regular_file() && name.rfind("hourly.", 0) == 0 &&
            entry.path().extension() == ".log")
        {
            hourlyFile = entry.path();
            break;
        }
    }
    ASSERT_FALSE(hourlyFile.empty());
    EXPECT_NE(ReadFileContents(hourlyFile).find("hourly-message\n"), std::string::npos);
}

// Buffered asynchronous output must be made visible by a warning-level flush.
// On platforms that permit unlinking an open file, a file and its parent
// directory removed while the logger remains live must be recreated after the
// next periodic file check.
TEST(LogBasicsTest, FlushesAsyncFilesAndRecoversDeletedLogFiles)
{
    ScopedTempDirectory directory;
    const std::filesystem::path logDirectory = directory.Path() / "logs";

    const std::filesystem::path asyncBase = logDirectory / "async";
    const std::filesystem::path asyncFile = logDirectory / "async.log";
    ScopedLoggerConfig asyncConfig;
    const std::string asyncConfigText =
        "root.asynchronous=true\n"
        "root.independentThread=false\n"
        "root.takeOver=true\n"
        "root.notConfigUse=default\n"
        "root.logToConsole=false\n"
        "root.logToFile=true\n"
        "root.fileLogLevel=TRACE\n"
        "root.filePattern=%m%n\n"
        "root.logFile=" + asyncBase.generic_string() + "\n"
        "root.logFileSuffix=.log\n"
        "root.fileRollingMode=NoRolling\n"
        "root.fileBufferSize=4096\n"
        "root.lazyCreateLogFile=false\n";
    asyncConfig.Write(asyncConfigText.c_str());

    {
        LLBC_LoggerMgr manager;
        ASSERT_EQ(manager.Initialize(asyncConfig.Path()), LLBC_OK);
        LLBC_Logger *root = manager.GetRootLogger();
        ASSERT_NE(root, nullptr);
        ASSERT_EQ(root->Info("file-test", "/tmp/file.cpp", 1, "FileFn", "buffered-info"),
                  LLBC_OK);
        ASSERT_EQ(root->Warn("file-test", "/tmp/file.cpp", 2, "FileFn", "buffered-warn"),
                  LLBC_OK);

        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
        bool flushed = false;
        while (std::chrono::steady_clock::now() < deadline)
        {
            const std::string content = ReadFileContents(asyncFile);
            if (content.find("buffered-info\n") != std::string::npos &&
                content.find("buffered-warn\n") != std::string::npos)
            {
                flushed = true;
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
        EXPECT_TRUE(flushed);
        manager.Finalize();
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    const std::filesystem::path recoveryDirectory = directory.Path() / "removed";
    const std::filesystem::path recoveryBase = recoveryDirectory / "recovery";
    const std::filesystem::path recoveryFile = recoveryDirectory / "recovery.log";
    ScopedLoggerConfig recoveryConfig;
    const std::string recoveryConfigText =
        "root.asynchronous=false\n"
        "root.takeOver=true\n"
        "root.notConfigUse=default\n"
        "root.logToConsole=false\n"
        "root.logToFile=true\n"
        "root.fileLogLevel=TRACE\n"
        "root.filePattern=%m%n\n"
        "root.logFile=" + recoveryBase.generic_string() + "\n"
        "root.logFileSuffix=.log\n"
        "root.fileRollingMode=NoRolling\n"
        "root.lazyCreateLogFile=false\n";
    recoveryConfig.Write(recoveryConfigText.c_str());

    {
        LLBC_LoggerMgr manager;
        ASSERT_EQ(manager.Initialize(recoveryConfig.Path()), LLBC_OK);
        LLBC_Logger *root = manager.GetRootLogger();
        ASSERT_NE(root, nullptr);
        ASSERT_EQ(root->Info("file-test", "/tmp/file.cpp", 3, "FileFn", "before-delete"),
                  LLBC_OK);
        ASSERT_TRUE(std::filesystem::remove(recoveryFile));
        ASSERT_TRUE(std::filesystem::remove(recoveryDirectory));

        std::this_thread::sleep_for(std::chrono::milliseconds(600));
        ASSERT_EQ(root->Info("file-test", "/tmp/file.cpp", 4, "FileFn", "after-recovery"),
                  LLBC_OK);
        manager.Finalize();
    }

    ASSERT_TRUE(std::filesystem::exists(recoveryFile));
    const std::string recoveryContent = ReadFileContents(recoveryFile);
    EXPECT_EQ(recoveryContent.find("before-delete"), std::string::npos);
    EXPECT_NE(recoveryContent.find("after-recovery\n"), std::string::npos);
#endif
}

// JSON logs are structured diagnostic records. Use a local trace-level logger
// and captured stdout to verify typed Add() values, timestamp injection, JSON
// serialization, and safe truncation of oversized printf messages.
TEST(LogBasicsTest, EmitsStructuredJsonLogsWithoutReadingPastFormatBuffer)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    ScopedStdoutRedirect redirect;
    ASSERT_TRUE(redirect.Begin());

    ScopedLoggerConfig config;
    config.Write(
        "root.asynchronous=false\n"
        "root.takeOver=true\n"
        "root.notConfigUse=default\n"
        "root.addTimestampInJsonLog=true\n"
        "root.logToConsole=true\n"
        "root.consoleLogLevel=TRACE\n"
        "root.consolePattern=%m%n\n"
        "root.colourfulOutput=false\n"
        "root.logToFile=false\n"
        "root.fileLogLevel=FATAL\n");

    LLBC_LoggerMgr manager;
    ASSERT_EQ(manager.Initialize(config.Path()), LLBC_OK);
    LLBC_Logger *root = manager.GetRootLogger();
    ASSERT_NE(root, nullptr);

    std::string firstJson;
    ASSERT_EQ(root->SetLogHook(LLBC_LogLevel::Info, [&](const LLBC_LogData *data) {
        firstJson.assign(data->msg, data->msgLen);
    }), LLBC_OK);
    {
        LLBC_LogJsonMsg json(root, "json-test", LLBC_LogLevel::Info, "/tmp/json.cpp", 21, "JsonFn");
        json.Add("text", "hello")
            .Add("signed", static_cast<sint64>(-7))
            .Add("unsigned", static_cast<uint64>(9))
            .Add("enabled", true)
            .Add("variant", LLBC_Variant(LLBC_String("variant")))
            .Finish("message:%d", 42);
    }
    ASSERT_EQ(root->SetLogHook(LLBC_LogLevel::Info, nullptr), LLBC_OK);

    LLBC_Json::Document firstDoc;
    firstDoc.Parse(firstJson.c_str());
    ASSERT_FALSE(firstDoc.HasParseError());
    EXPECT_STREQ(firstDoc["text"].GetString(), "hello");
    EXPECT_EQ(firstDoc["signed"].GetInt64(), -7);
    EXPECT_EQ(firstDoc["unsigned"].GetUint64(), 9u);
    EXPECT_TRUE(firstDoc["enabled"].GetBool());
    EXPECT_STREQ(firstDoc["variant"].GetString(), "variant");
    EXPECT_STREQ(firstDoc["msg"].GetString(), "message:42");
    EXPECT_TRUE(firstDoc.HasMember("timestamp"));

    const std::string oversizedField(LLBC_CFG_LOG_FORMAT_BUF_SIZE, 'y');
    std::string oversizedFieldJson;
    ASSERT_EQ(root->SetLogHook(LLBC_LogLevel::Info, [&](const LLBC_LogData *data) {
        oversizedFieldJson.assign(data->msg, data->msgLen);
    }), LLBC_OK);
    {
        LLBC_LogJsonMsg json(root, "json-test", LLBC_LogLevel::Info, "/tmp/json.cpp", 22, "JsonFn");
        json.Add("payload", oversizedField.c_str()).Finish("short-message");
    }
    ASSERT_EQ(root->SetLogHook(LLBC_LogLevel::Info, nullptr), LLBC_OK);

    LLBC_Json::Document oversizedFieldDoc;
    oversizedFieldDoc.Parse(oversizedFieldJson.c_str());
    ASSERT_FALSE(oversizedFieldDoc.HasParseError());
    ASSERT_TRUE(oversizedFieldDoc.HasMember("msg"));
    EXPECT_FALSE(oversizedFieldDoc.HasMember("payload"));
    EXPECT_STREQ(oversizedFieldDoc["msg"].GetString(), "short-message");

    manager.Finalize();
    const std::string capturedBeforeLargeMessage = redirect.ReadCaptured();

    const std::string oversized(LLBC_CFG_LOG_FORMAT_BUF_SIZE + 128, 'x');
    {
        LLBC_LogJsonMsg json(nullptr, nullptr, LLBC_LogLevel::Info, nullptr, 0, nullptr);
        json.Finish("%s", oversized.c_str());
    }
    const std::string captured = redirect.ReadCaptured();
    EXPECT_NE(captured.find("\"message:42\""), std::string::npos);
    EXPECT_NE(captured.find("\"msg\""), std::string::npos);

    const std::string uninitializedJsonOutput =
        captured.substr(capturedBeforeLargeMessage.size());
    const size_t jsonBegin = uninitializedJsonOutput.find('{');
    ASSERT_NE(jsonBegin, std::string::npos);
    LLBC_Json::Document oversizedDoc;
    oversizedDoc.Parse(uninitializedJsonOutput.c_str() + jsonBegin);
    ASSERT_FALSE(oversizedDoc.HasParseError());
    ASSERT_TRUE(oversizedDoc.HasMember("msg"));
    const size_t maxJsonMessageLen =
        LLBC_CFG_LOG_FORMAT_BUF_SIZE - 1 - std::string("{\"msg\":\"\"}").size();
    EXPECT_EQ(static_cast<size_t>(oversizedDoc["msg"].GetStringLength()),
              maxJsonMessageLen);
    EXPECT_EQ(oversizedDoc["msg"].GetString()[0], 'x');
    EXPECT_EQ(oversizedDoc["msg"].GetString()[maxJsonMessageLen - 1], 'x');
#else
    GTEST_SKIP() << "stdout redirection is POSIX-specific";
#endif
}
