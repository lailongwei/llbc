/**
 * @file    OS_Process.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/03/24
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_Process.h"

__LLBC_NS_BEGIN

int LLBC_GetCurrentProcessId()
{
#if LLBC_TARGET_PLATFORM_WIN32
    return static_cast<int>(::GetCurrentProcessId());
#else // Non-Win32
    return static_cast<int>(getpid());
#endif // Win32
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
