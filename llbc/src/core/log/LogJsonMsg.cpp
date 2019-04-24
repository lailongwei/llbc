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
#include "llbc/core/thread/Guard.h"

#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/Logger.h"

#include "llbc/core/log/LogJsonMsg.h"

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(disable:4996)
#endif

__LLBC_NS_BEGIN

namespace
{
    typedef LLBC_NS LLBC_LogLevel _LV;
}

LLBC_LogJsonMsg::LLBC_LogJsonMsg(LLBC_Logger* logger, const char* tag, int lv) :
    _logger(logger),
    _tag(tag),
    _lv(lv)
{
}

LLBC_LogJsonMsg::~LLBC_LogJsonMsg()
{
}

void LLBC_LogJsonMsg::Finish(const char *fmt, ...)
{
    char *fmttedMsg; int msgLen;
    LLBC_FormatArg(fmt, fmttedMsg, msgLen);

    LLBC_FreeGuard<char> guard(fmttedMsg);

    _json["msg"] = fmttedMsg;

    const LLBC_String &jStr = _json.toStyledString();

    if (LIKELY(_logger))
        _logger->OutputNonFormat(_lv, _tag, __FILE__, __LINE__, jStr.c_str(), jStr.length());
    else
        UnInitOutput(_lv >= _LV::Warn ? stderr : stdout, jStr.c_str());

    delete this;
}

void LLBC_LogJsonMsg::UnInitOutput(FILE *to, const char *msg)
{
    LLBC_FilePrint(to, "[Log] %s\n", msg);
    if (to != stderr)
        LLBC_FlushFile(to);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"