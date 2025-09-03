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

#include "llbc/core/time/TimeSpan.h"

__LLBC_NS_BEGIN

/**
 * \brief The log time accessor class encapsulation.
 */
class LLBC_EXPORT LLBC_LogTimeAccessor
{
public:
    /**
     * Default ctor.
     */
    LLBC_LogTimeAccessor();

public:
    /**
     * Get log time offset.
     * @return sint64 - the log time offset, in microseconds.
     */
    sint64 GetLogTimeOffset() const;

    /**
     * @param[in] logTimeOffset - the log time offset.
     */
    void SetLogTimeOffset(const LLBC_TimeSpan &logTimeOffset);

    /**
     * Get now log time in milliseconds.
     * @return sint64 - now log time in milliseconds.
     */
    sint64 NowInMilliseconds() const;

    /**
     * Get real now log time in milliseconds.
     * @return sint64 - real now log time in milliseconds.
     */
    sint64 RealNowInMilliseconds() const;

    /**
     * Get now log time in microseconds.
     * @return sint64 - now log time in microseconds.
     */
    sint64 NowInMicroseconds() const;
    /**
     * Get real now time in microseconds.
     * @return sint64 - real now time in microseconds.
     */
    sint64 RealNowInMicroseconds() const;

private:
    sint64 _logTimeOffset;
};

__LLBC_NS_END

#include "llbc/core/log/LogTimeAccessorInl.h"
