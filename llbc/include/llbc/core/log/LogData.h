/**
 * @file    LogData.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/09
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_LOG_LOG_DATA_H__
#define __LLBC_CORE_LOG_LOG_DATA_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief The log data structure encapsulation.
 */
struct LLBC_EXPORT LLBC_LogData
{
    char *msg;                            // Log message(allocate from heap).
    uint32 msgLen;                        // message length.

    int level;                            // Log level.
    const char *loggerName;               // Logger name.

    char *others;                         // Others data(allocate from heap).
    uint32 tagBeg;                        // Tag begin.
    uint32 tagLen;                        // Tag length.

    time_t logTime;                       // Log time.

    uint32 fileBeg;                       // Log source file name begin.
    uint32 fileLen;                       // Log source file name length.
    long line;                            // Log source file line number.

    LLBC_NativeThreadHandle threadHandle; // Log native thread handle.
};

__LLBC_NS_END

#endif // !__LLBC_CORE_LOG_LOG_DATA_H__
