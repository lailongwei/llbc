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
#include "llbc/core/objectpool/Common.h"

#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/Logger.h"

#include "llbc/core/log/LogJsonMsg.h"
#include "llbc/core/log/LoggerManager.h"

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(disable:4996)
#endif

__LLBC_NS_BEGIN

namespace
{
    typedef LLBC_NS LLBC_LogLevel _LV;
}

LLBC_LogJsonMsg::LLBC_LogJsonMsg(bool loggerInited, LLBC_Logger *logger, const char *tag, int lv)
: _loggerInited(loggerInited)
, _logger(logger)
, _tag(tag)
, _lv(lv)
, _doc(*LLBC_GetObjectFromUnsafetyPool<LLBC_Json::Document>())
{
}

LLBC_LogJsonMsg::~LLBC_LogJsonMsg()
{
    LLBC_ReleaseObjectToUnsafetyPool(&_doc);
}

void LLBC_LogJsonMsg::Finish(const char *fmt, ...)
{
    if (UNLIKELY(!_loggerInited))
    {
        char *fmttedMsg; int msgLen;
        LLBC_FormatArg(fmt, fmttedMsg, msgLen);

        // doc add string with not copy
        _doc.AddMember("msg", LLBC_JsonValue(fmttedMsg, msgLen).Move(), _doc.GetAllocator());
        
        // _doc stringify
        LLBC_Json::StringBuffer buffer;
        LLBC_Json::Writer<LLBC_Json::StringBuffer> writer(buffer);
        _doc.Accept(writer);

        // free fmttedMsg should after _doc stringify
        LLBC_Free(fmttedMsg);

        UnInitOutput(_lv >= _LV::Warn ? stderr : stdout, buffer.GetString());
    }
    else if (_logger && _lv >= _logger->GetLogLevel())
    {
        char *fmttedMsg; int msgLen;
        LLBC_FormatArg(fmt, fmttedMsg, msgLen);

        // _doc add string with not copy
        _doc.AddMember("msg", LLBC_JsonValue(fmttedMsg, msgLen).Move(), _doc.GetAllocator());

        // _doc convert to string
        LLBC_Json::StringBuffer buffer;
        LLBC_Json::Writer<LLBC_Json::StringBuffer> writer(buffer);
        _doc.Accept(writer);

        // free fmttedMsg should after _doc call Accept function
        LLBC_Free(fmttedMsg);

        _logger->OutputNonFormat(_lv, _tag, __FILE__, __LINE__, buffer.GetString(), buffer.GetLength());
    }

    LLBC_Delete(this);
}

void LLBC_LogJsonMsg::UnInitOutput(FILE *to, const char *msg)
{
    LLBC_FilePrint(to, "[Log] %s\n", msg);
    if (to != stderr)
        LLBC_FlushFile(to);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
