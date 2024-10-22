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

#include "llbc/core/Core.h"

__LLBC_NS_BEGIN

/**
 * \brief The poller type enumeration.
 */
class LLBC_EXPORT LLBC_PollerType
{
public:
    enum
    {
        Begin,

        SelectPoller = Begin,
#if LLBC_TARGET_PLATFORM_WIN32
        IocpPoller,     // Iocp poller only availables in WIN32 platform.
#elif LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID
        EpollPoller,    // Epoll poller availables on LINUX & ANDROID platforms.
#endif // LLBC_TARGET_PLATFORM_WIN32

        End
    };

public:
    /**
     * Check given poller type is validate or not.
     * @param[in] type - the poller type, see above type enumeration.
     * @return bool - return true if validate, otherwise return false.
     */
    static bool IsValid(int type);

    /**
     * Get poller type string representation.
     * @param[in] type - the poller type, see above type enumeration.
     * return const LLBC_String & - the poller type string representation.
     */
    static const LLBC_String &Type2Str(int type);

    /**
     * Get poller type enumeration from string representation.
     * @param[in] typeStr - the poller type string representation.
     * @return int - the poller type, if error occurred, return End value.
     */
    static int Str2Type(const LLBC_String &typeStr);
};

__LLBC_NS_END
