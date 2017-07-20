/**
 * @file    MonkeyPatchType.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017-07-20
 * @version 1.0
 *
 * @brief
 */
#ifndef __LULLBC_COM_MONKEY_PATCH_TYPE_H__
#define __LULLBC_COM_MONKEY_PATCH_TYPE_H__

#include "lullbc/common/Export.h"

/**
 * Monkey patch type enumerations.
 */
class LULLBC_HIDDEN lullbc_MonkeyPatchType
{
public:
    enum {
        Begin,

        // Patch table.
        // 1. patch table.concat, after patch, table.concat can concat any implemented __tostring methods object.
        Table = Begin,

        End
    };
};

#endif // !__LULLBC_COM_MONKEY_PATCH_TYPE_H__
