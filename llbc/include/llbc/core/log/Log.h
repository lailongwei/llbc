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

#ifndef __LLBC_CORE_LOG_LOG_H__
#define __LLBC_CORE_LOG_LOG_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_Logger;
class LLBC_LoggerManager;

class LLBC_LogHelper;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * Get log helper object.
 * @return LLBC_LogHelper & - the log helper reference.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_LogHelper &__LLBC_GetLogHelper();

// Log macro define.
#ifdef Log
 #error Already defined Log macro in your application/library, please undef it first!
#else
 #define Log (__LLBC_GetLogHelper())
#endif // Log

/**
 * \brief The log helper class encapsulation.
 *        This helper class let your log message easy to output.
 */
class LLBC_EXPORT LLBC_LogHelper 
{
public:
    /**
     * Logger initialize/finalize support.
     */
    static int init(const LLBC_String &cfgFile);
    static void destroy();

public:
    /**
     * Output debug level message.
     */
    static void d(const char *fmt, ...);
    static void d2(const char *tag, const char *fmt, ...);
    template <typename Tag>
    static void d2(const char *fmt, ...);
    static void d3(const char *logger, const char *fmt, ...);
    static void d4(const char *logger, const char *tag, const char *fmt, ...);

    /**
     * Output info level message.
     */
    static void i(const char *fmt, ...);
    static void i2(const char *tag, const char *fmt, ...);
    template <typename Tag>
    static void i2(const char *fmt, ...);
    static void i3(const char *logger, const char *fmt, ...);
    static void i4(const char *logger, const char *tag, const char *fmt, ...);

    /**
     * Output warning level message.
     */
    static void w(const char *fmt, ...);
    static void w2(const char *tag, const char *fmt, ...);
    template <typename Tag>
    static void w2(const char *fmt, ...);
    static void w3(const char *logger, const char *fmt, ...);
    static void w4(const char *logger, const char *tag, const char *fmt, ...);

    /**
     * Output error level message.
     */
    static void e(const char *fmt, ...);
    static void e2(const char *tag, const char *fmt, ...);
    template <typename Tag>
    static void e2(const char *fmt, ...);
    static void e3(const char *logger, const char *fmt, ...);
    static void e4(const char *logger, const char *tag, const char *fmt, ...);

    /**
     * Output fatal level message.
     */
    static void f(const char *fmt, ...);
    static void f2(const char *tag, const char *fmt, ...);
    template <typename Tag>
    static void f2(const char *fmt, ...);
    static void f3(const char *logger, const char *fmt, ...);
    static void f4(const char *logger, const char *tag, const char *fmt, ...);

private:
    /**
     * Initialize Log help class.
     */
    static void Initialize(LLBC_LoggerManager *loggerManager);

    /**
     * Finalize Log helper class.
     */
    static void Finalize();

    /**
     * Friend class: LLBC_LoggerManager.
     *     Access methods:
     *         Initialize() - When logger manager initialize, this method will be called!
     *         Finalize() - When logger manager finalize, this method will be called!
     */
    friend class LLBC_LoggerManager;

private:
    /**
     * When logger component not initialize, will use this function to output message.
     */
    static void UnInitOutput(FILE *to, const char *msg);

private:
    static LLBC_Logger *_rootLogger;
    static LLBC_LoggerManager *_loggerManager;
};

__LLBC_NS_END

#include "llbc/core/log/LogImpl.h"

#endif // !__LLBC_CORE_LOG_LOG_H__
