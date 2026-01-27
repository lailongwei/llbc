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

#include "llbc/core/utils/Util_Debug.h"

#include "llbc/core/os/OS_Time.h"
#include "llbc/core/log/LoggerMgr.h"

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(disable:4996)
#endif

__LLBC_INTERNAL_NS_BEGIN

static const LLBC_NS uint16 __g_hexTable[256] = 
{
    0x3030, 0x3130, 0x3230, 0x3330, 0x3430, 0x3530, 0x3630, 0x3730, 0x3830, 0x3930, 0x6130, 0x6230, 0x6330, 0x6430, 0x6530, 0x6630,
    0x3031, 0x3131, 0x3231, 0x3331, 0x3431, 0x3531, 0x3631, 0x3731, 0x3831, 0x3931, 0x6131, 0x6231, 0x6331, 0x6431, 0x6531, 0x6631,
    0x3032, 0x3132, 0x3232, 0x3332, 0x3432, 0x3532, 0x3632, 0x3732, 0x3832, 0x3932, 0x6132, 0x6232, 0x6332, 0x6432, 0x6532, 0x6632,
    0x3033, 0x3133, 0x3233, 0x3333, 0x3433, 0x3533, 0x3633, 0x3733, 0x3833, 0x3933, 0x6133, 0x6233, 0x6333, 0x6433, 0x6533, 0x6633,
    0x3034, 0x3134, 0x3234, 0x3334, 0x3434, 0x3534, 0x3634, 0x3734, 0x3834, 0x3934, 0x6134, 0x6234, 0x6334, 0x6434, 0x6534, 0x6634,
    0x3035, 0x3135, 0x3235, 0x3335, 0x3435, 0x3535, 0x3635, 0x3735, 0x3835, 0x3935, 0x6135, 0x6235, 0x6335, 0x6435, 0x6535, 0x6635,
    0x3036, 0x3136, 0x3236, 0x3336, 0x3436, 0x3536, 0x3636, 0x3736, 0x3836, 0x3936, 0x6136, 0x6236, 0x6336, 0x6436, 0x6536, 0x6636,
    0x3037, 0x3137, 0x3237, 0x3337, 0x3437, 0x3537, 0x3637, 0x3737, 0x3837, 0x3937, 0x6137, 0x6237, 0x6337, 0x6437, 0x6537, 0x6637,
    0x3038, 0x3138, 0x3238, 0x3338, 0x3438, 0x3538, 0x3638, 0x3738, 0x3838, 0x3938, 0x6138, 0x6238, 0x6338, 0x6438, 0x6538, 0x6638,
    0x3039, 0x3139, 0x3239, 0x3339, 0x3439, 0x3539, 0x3639, 0x3739, 0x3839, 0x3939, 0x6139, 0x6239, 0x6339, 0x6439, 0x6539, 0x6639,
    0x3061, 0x3161, 0x3261, 0x3361, 0x3461, 0x3561, 0x3661, 0x3761, 0x3861, 0x3961, 0x6161, 0x6261, 0x6361, 0x6461, 0x6561, 0x6661,
    0x3062, 0x3162, 0x3262, 0x3362, 0x3462, 0x3562, 0x3662, 0x3762, 0x3862, 0x3962, 0x6162, 0x6262, 0x6362, 0x6462, 0x6562, 0x6662,
    0x3063, 0x3163, 0x3263, 0x3363, 0x3463, 0x3563, 0x3663, 0x3763, 0x3863, 0x3963, 0x6163, 0x6263, 0x6363, 0x6463, 0x6563, 0x6663,
    0x3064, 0x3164, 0x3264, 0x3364, 0x3464, 0x3564, 0x3664, 0x3764, 0x3864, 0x3964, 0x6164, 0x6264, 0x6364, 0x6464, 0x6564, 0x6664,
    0x3065, 0x3165, 0x3265, 0x3365, 0x3465, 0x3565, 0x3665, 0x3765, 0x3865, 0x3965, 0x6165, 0x6265, 0x6365, 0x6465, 0x6565, 0x6665,
    0x3066, 0x3166, 0x3266, 0x3366, 0x3466, 0x3566, 0x3666, 0x3766, 0x3866, 0x3966, 0x6166, 0x6266, 0x6366, 0x6466, 0x6566, 0x6666,
};

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_String LLBC_Byte2Hex(const void *bytes, size_t len, char byteSep, size_t lineWidth)
{
    LLBC_String hexStr;
    if (!bytes || len == 0)
        return hexStr;

    size_t hexSize = len << 1;
    if (byteSep != '\0')
        hexSize += len;

    if (lineWidth > 0)
    {
        hexSize += len / lineWidth;
        if (len % lineWidth == 0)
            hexSize -= 1;
    }

    size_t byteIdx = 0;
    size_t hexStrIdx = 0;
    hexStr.resize(hexSize);
    while (true)
    {
        *reinterpret_cast<uint16*>(&hexStr[hexStrIdx]) =
            LLBC_INL_NS __g_hexTable[(reinterpret_cast<const uint8 *>(bytes))[byteIdx++]];
        hexStrIdx += 2;

        if (byteSep != '\0')
            hexStr[hexStrIdx++] = byteSep;

        if (byteIdx == len)
            break;

        if (lineWidth > 0 &&
            byteIdx % lineWidth == 0)
            hexStr[hexStrIdx++] = '\n';
    }

    #if LLBC_DEBUG
    llbc_assert(hexStrIdx == hexStr.size() && "llbc framework internal error");
    #endif

    return hexStr;
}

uint64 LLBC_Stopwatch::_frequency = 0;

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(default:4996)
#endif

LLBC_FuncTracer::LLBC_FuncTracer(const LLBC_CString &fileName, 
                                 int lineNo,
                                 const LLBC_CString &funcName, 
                                 bool traceMem,
                                 sint64 uniqueId,
                                 const LLBC_CString &loggerName)
: _logger(nullptr)
, _stopWatch(true, traceMem)
{
    LLBC_String uniqueStr;
    uniqueStr.format("%lld", uniqueId);
    Init(fileName, lineNo, funcName, traceMem, uniqueStr, loggerName);
}

LLBC_FuncTracer::LLBC_FuncTracer(const LLBC_CString &fileName, 
                                 int lineNo,
                                 const LLBC_CString &funcName, 
                                 bool traceMem,
                                 const LLBC_CString &uniqueStr,
                                 const LLBC_CString &loggerName)
: _logger(nullptr)
, _stopWatch(true, traceMem)
{
    Init(fileName, lineNo, funcName, traceMem, uniqueStr, loggerName);
}

void LLBC_FuncTracer::Init(const LLBC_CString &fileName, 
                           int lineNo,
                           const LLBC_CString &funcName, 
                           bool traceMem,
                           const LLBC_CString &uniqueStr,
                           const LLBC_CString &loggerName)
{
    if (!loggerName.empty())
        _logger = LLBC_LoggerMgrSingleton->GetLogger(loggerName);

    if (UNLIKELY(!_logger))
    {
        _logger = LLBC_LoggerMgrSingleton->GetRootLogger();
        if (UNLIKELY(!_logger))
            return;
    }
    
    if (_logger->GetLogLevel() != LLBC_LogLevel::Trace)
        return;

    _traceUniqInfo.format("%s:%d:%s:%s",
                          LLBC_Directory::BaseName(fileName).c_str(),
                          lineNo,
                          funcName.c_str(),
                          uniqueStr.c_str());

    LLOG_TRACE4(_logger->GetLoggerName().c_str(), 
                "FuncTrace", 
                "%s|Enter(trace mem?:%s)",
                _traceUniqInfo.c_str(),
                traceMem ? "true" : "false");
}

LLBC_FuncTracer::~LLBC_FuncTracer()
{
    if (LIKELY(_logger) && _logger->GetLogLevel() == LLBC_LogLevel::Trace) 
    {
        const auto memDiff = _stopWatch.GetMemSnapshotDiff();
        const auto cost = _stopWatch.Elapsed();
        if (_stopWatch.IsTraceMemEnabled())
            LLOG_TRACE4(_logger->GetLoggerName().c_str(), 
                        "FuncTrace",
                        "%s|Leave cost:%lld.%03lld ms, memory diff(virt:%lld res:%lld share:%lld)",
                        _traceUniqInfo.c_str(),
                        cost.GetTotalMillis(),
                        cost.GetTotalMicros() % 1000,
                        memDiff._memVirt,
                        memDiff._memRes,
                        memDiff._memShr);
        else
            LLOG_TRACE4(_logger->GetLoggerName().c_str(),
                        "FuncTrace",
                        "%s|Leave cost:%lld.%03lld ms",
                        _traceUniqInfo.c_str(),
                        cost.GetTotalMillis(),
                        cost.GetTotalMicros() % 1000);
    }
}
__LLBC_NS_END

