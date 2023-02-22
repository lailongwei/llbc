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

#pragma once

#include "llbc/common/Common.h"

#if LLBC_CFG_LOG_USING_WITH_STREAM

__LLBC_NS_BEGIN

/**
 * \brief The Insertion operator for STL manipulators such as std::fixed.
 */
typedef std::ios_base & (*ios_base_manip)(std::ios_base &);

/**
 * \brief The log message buffer class encapsulation.
 */
class LLBC_EXPORT LLBC_LogMessageBuffer
{
public:
    LLBC_LogMessageBuffer();
    virtual ~LLBC_LogMessageBuffer();

public:
    /**
     * Appends string to buffer.
     * @param[in] msg - msg string to append.
     * @return LLBC_LogMessageBuffer & - message buffer.
     */
    LLBC_LogMessageBuffer &operator <<(const LLBC_String &msg);
    LLBC_LogMessageBuffer &operator <<(const std::basic_string<char> &msg);
    LLBC_LogMessageBuffer &operator <<(char *msg);
    LLBC_LogMessageBuffer &operator <<(const char *msg);
    LLBC_LogMessageBuffer &operator <<(char msg);

    /**
     * Built-in type stream output operator support.
     * @param[in] val - value to append.
     * @return std::basic_ostream<char> & - stream object.
     */
    std::basic_ostream<char> &operator <<(bool val);
    std::basic_ostream<char> &operator <<(uint8 val);
    std::basic_ostream<char> &operator <<(sint16 val);
    std::basic_ostream<char> &operator <<(uint16 val);
    std::basic_ostream<char> &operator <<(sint32 val);
    std::basic_ostream<char> &operator <<(uint32 val);
    std::basic_ostream<char> &operator <<(sint64 val);
    std::basic_ostream<char> &operator <<(uint64 val);
    std::basic_ostream<char> &operator <<(long val);
    std::basic_ostream<char> &operator <<(ulong val);
    std::basic_ostream<char> &operator <<(float val);
    std::basic_ostream<char> &operator <<(double val);
    std::basic_ostream<char> &operator <<(ldouble val);
    std::basic_ostream<char> &operator <<(void *val);

    /**
     * Insertion operator for STL manipulators such as std::fixed.
     */
    std::basic_ostream<char> &operator <<(ios_base_manip manip);

    /**
     * Cast to basic_ostream.
     */
    operator std::basic_ostream<char> &();

public:
    /**
     * Get content of buffer.
     * @return const LLBC_String & - content data.
     */
    const LLBC_String &str(LLBC_LogMessageBuffer &);
    const LLBC_String &str(std::basic_ostream<char> &);

private:
    LLBC_DISABLE_ASSIGNMENT(LLBC_LogMessageBuffer);
    
private:
    LLBC_String _buf;
    std::basic_ostringstream<char> *_stream;
};

__LLBC_NS_END

/**
 * operator << function define, using to adapt other implemented
 * stream output operator's class.
 */
template <typename T>
std::basic_ostream<char> &operator <<(LLBC_NS LLBC_LogMessageBuffer &msg, const T &val);

#include "llbc/core/log/LogMessageBufferInl.h"

#endif // LLBC_CFG_LOG_USING_WITH_STREAM


