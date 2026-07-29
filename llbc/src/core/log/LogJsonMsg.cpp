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

#include "llbc/core/os/OS_Time.h"
#include "llbc/core/os/OS_Console.h"
#include "llbc/core/objpool/ThreadSpecObjPool.h"

#include "llbc/core/log/LogTimeAccessor.h"
#include "llbc/core/log/Logger.h"
#include "llbc/core/log/LoggerMgr.h"

#include "llbc/core/log/LogJsonMsg.h"

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(disable:4996)
#endif

__LLBC_NS_BEGIN

LLBC_LogJsonMsg::LLBC_LogJsonMsg(LLBC_Logger *logger,
                                 const char *tag,
                                 int lv,
                                 const char *file,
                                 int line,
                                 const char *func)
: _logger(logger)
, _tag(tag)
, _lv(lv)
, _file(file)
, _line(line)
, _func(func)

, _doc(*LLBC_ThreadSpecObjPool::UnsafeAcquire<LLBC_Json::Document>())
{
    _doc.SetObject();
}

LLBC_LogJsonMsg::~LLBC_LogJsonMsg()
{
    LLBC_ThreadSpecObjPool::UnsafeRelease(&_doc);
}

void LLBC_LogJsonMsg::Finish(const char *fmt, ...)
{
    // Log level judge.
    if (LIKELY(_logger) &&
        _lv < _logger->GetLogLevel())
        return;

    // Reserve enough room for the JSON object wrapper and timestamp so the
    // message can still be added to the document without a copy.
    constexpr size_t jsonReservedSize = 64;
    static_assert(LLBC_CFG_LOG_FORMAT_BUF_SIZE + 1 > jsonReservedSize,
                  "Logger format buffer must be larger than JSON reserved size");

    // Format.
    va_list va;
    va_start(va, fmt);
    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    const size_t fmtBufSize = sizeof(libTls->coreTls.loggerFmtBuf) - jsonReservedSize;
    int len = vsnprintf(libTls->coreTls.loggerFmtBuf,
                        fmtBufSize,
                        fmt,
                        va);
    va_end(va);
    if (UNLIKELY(len < 0))
        return;
    if (UNLIKELY(static_cast<size_t>(len) >= fmtBufSize))
        len = static_cast<int>(fmtBufSize - 1);

    // Add time.
    const sint64 now =
            LIKELY(_logger) ? _logger->GetLogTimeAccessor().NowInMicroseconds() : LLBC_GetMicroseconds();
    if (LIKELY(_logger) && _logger->IsAddTimestampInJsonLog())
        this->Add("timestamp", now);

    // Doc add string with not copy.
    _doc.AddMember("msg",
                   LLBC_JsonValue(libTls->coreTls.loggerFmtBuf, len).Move(),
                   _doc.GetAllocator());

    // _doc stringify
    LLBC_Json::StringBuffer buffer;
    LLBC_Json::Writer<LLBC_Json::StringBuffer> writer(buffer);
    _doc.Accept(writer);

    // Output json log.
    if (LIKELY(_logger))
        _logger->NonFormatOutput(_lv,
                                 _tag,
                                 _file,
                                 _line,
                                 _func,
                                 now,
                                 buffer.GetString(),
                                 buffer.GetLength());
    else
        LLBC_LoggerMgrSingleton->UnInitOutput(
            _lv, _tag, nullptr, 0, nullptr, "%s", buffer.GetString());
}

__LLBC_NS_END
