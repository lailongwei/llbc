/**
 * @file    JVHelper.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/05/04
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_HELPER_JV_HELPER_H__
#define __LLBC_CORE_HELPER_JV_HELPER_H__

#include "llbc/common/Common.h"

#include "llbc/core/json/json.h"
#include "llbc/core/variant/Variant.h"

__LLBC_NS_BEGIN

/**
 * \brief The interconversion of Variant/Json format data helper class encapsulation.
 */
class LLBC_EXPORT LLBC_JVHelper
{
public:
    /**
     * Convert json format data to variant.
     * @param[in] json - the json value.
     * @return LLBC_Variant - converted variant data.
     */
    static LLBC_Variant J2V(const LLBC_JsonValue &json);

    /**
     * Convert variant format data to json.
     * @param[in] var - the variant value.
     * @return LLBC_JsonValue - converted json data.
     */
    static LLBC_JsonValue V2J(const LLBC_Variant &var);
};

__LLBC_NS_END

#endif // !__LLBC_CORE_HELPER_JV_HELPER_H__
