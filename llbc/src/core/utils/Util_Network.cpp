/**
 * @file    Util_Network.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/12/19
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/utils/Util_Network.h"

__LLBC_NS_BEGIN

bool LLBC_IsIPv4Addr(const LLBC_String &addr)
{
    if (UNLIKELY(addr.empty()))
        return false;

    typedef std::vector<LLBC_String> _Parts;
    const _Parts parts = addr.split(".");
    if (parts.size() != 4)
        return false;

    for (_Parts::const_iterator it = parts.begin();
         it != parts.end();
         it++)
    {
        const LLBC_String &part = *it;
        if (part.empty())
            return false;

        for (LLBC_String::size_type i = 0;
             i < part.length();
             i++)
            if (!('0' <= part[i] && part[i] <= '9'))
                return false;
    }

    return true;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
