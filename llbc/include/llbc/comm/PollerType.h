/**
 * @file    PollerType.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/11
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_POLLER_TYPE_H__
#define __LLBC_COMM_POLLER_TYPE_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

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

#endif // !__LLBC_COMM_POLLER_TYPE_H__

