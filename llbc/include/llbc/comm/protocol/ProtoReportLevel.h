/**
 * @file    ProtoReportLevel.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/24
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_PROTO_REPORT_LEVEL_H__
#define __LLBC_COMM_PROTO_REPORT_LEVEL_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

__LLBC_NS_BEGIN

/**
 * \brief The protocol report level enumerations.
 */
class LLBC_EXPORT LLBC_ProtoReportLevel
{
public:
    enum
    {
        Begin,
         
        Debug = Begin,
        Info,
        Warn,
        Error,

        End
    };

public:
    /**
     * Check given protocol layer legal or not.
     * @param[in] level - the protocol report level.
     * @return bool - return true if validate, otherwise return -1.
     */
    static bool IsValid(int level);

    /**
     * Get the protocol report level string representation.
     * @param[in] level - the protocol level.
     * @return const LLBC_String & - the protocol level.
     */
    static const LLBC_String &Level2Str(int level);
};

__LLBC_NS_END

#endif // !__LLBC_COMM_PROTO_REPORT_LEVEL_H__

