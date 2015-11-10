/**
 * @file    GUIDHelper.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/08/20
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_HELPER_GUID_HELPER_H__
#define __LLBC_CORE_HELPER_GUID_HELPER_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief The GUID helper class encapsulation.
 */
class LLBC_EXPORT LLBC_GUIDHelper
{
public:
    /**
     * Generate GUID.
     * @return LLBC_GUID - GUID value.
     */
    static LLBC_GUID Gen();

    /**
     * Format GUID.
     * @param[in] guid - GUID value.
     * @return LLBC_String - formatted GUID value.
     */
    static LLBC_String Format(LLBC_GUIDCRef guid);

    /**
     * Generate GUID and Format the GUID structure data to string format.
     * @return LLBC_String - the string format guid.
     */
    static LLBC_String GenStr();
};

__LLBC_NS_END

#endif // !__LLBC_CORE_HELPER_GUID_HELPER_H__

