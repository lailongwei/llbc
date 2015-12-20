/**
 * @file    Util_Debug.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/1/29
 * @version 1.0
 *
 * @brief   LLBC library debug util encapsulation.
 */
#ifndef __LLBC_CORE_UTILS_UTIL_DEBUG_H__
#define __LLBC_CORE_UTILS_UTIL_DEBUG_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Convert byte array to string format(hexadecimal format).
 * @param[in] buf - buffer pointer.
 * @param[in] len - buffer length.
 * @param[in] lineWidth - line width, default is 16.
 * @return std::string - the formatted string data.
 */
LLBC_EXTERN LLBC_EXPORT std::string LLBC_Byte2Hex(const void *buf, size_t len, uint32 lineWidth = 16);

/**
 * Trace function define.
 * Note: Like ActionScript Language, trace function use to simple output some message.
 *       Just useful in debug mode(defined DEBUG macro).
 * @param[in] format - string format argument.
 * @param[in] ...    - optional arguments.
 */
#ifndef trace 
 #ifdef LLBC_DEBUG
  #define trace(format, ...) LLBC_FilePrint(stdout, format, ##__VA_ARGS__)
  #define traceline(format, ...) LLBC_FilePrintLine(stdout, format, ##__VA_ARGS__)
 #else
  #define trace(format, ...)
  #define traceline(format, ...)
 #endif
#endif

/**
 * \brief CPU tick counter, is a high-resoultion preformance counter.
 */
class LLBC_EXPORT LLBC_CPUTime
{
public:
    typedef uint64 CPUTimeCount;

public:
    LLBC_CPUTime();
    LLBC_CPUTime(CPUTimeCount count);
    ~LLBC_CPUTime();

public:
    static LLBC_CPUTime Current();
    
    CPUTimeCount ToSeconds() const;

    CPUTimeCount ToMilliSeconds() const;

    CPUTimeCount ToMicroSeconds() const;

    std::string ToString() const;

    LLBC_CPUTime operator +(const LLBC_CPUTime &right) const;
    LLBC_CPUTime operator -(const LLBC_CPUTime &right) const;
    LLBC_CPUTime &operator +=(const LLBC_CPUTime &right);
    LLBC_CPUTime &operator -=(const LLBC_CPUTime &right);

    bool operator <(const LLBC_CPUTime &right) const;
    bool operator >(const LLBC_CPUTime &right) const;
    bool operator <=(const LLBC_CPUTime &right) const;
    bool operator >=(const LLBC_CPUTime &right) const;
    bool operator ==(const LLBC_CPUTime &right) const;
    bool operator !=(const LLBC_CPUTime &right) const;

public:
#if LLBC_TARGET_PLATFORM_WIN32
    static void InitFrequency();
#endif

private:
#if LLBC_TARGET_PLATFORM_WIN32
    static CPUTimeCount sm_freq;
#endif

    CPUTimeCount m_count;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_UTILS_UTIL_DEBUG_H__

