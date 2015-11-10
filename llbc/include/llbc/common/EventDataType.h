/**
 * @file    EventDataType.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/07/20
 * @version 1.0
 *
 * @brief   The event module data type encapsulation.
 */
#ifndef __LLBC_COM_EVENT_DATA_TYPE_H__
#define __LLBC_COM_EVENT_DATA_TYPE_H__

#include "llbc/common/PFConfig.h"

#include "llbc/common/Macro.h"
#include "llbc/common/StringDataType.h"

__LLBC_NS_BEGIN

/**
 * \brief The event listener stub data type encapsulation.
 */
typedef LLBC_String LLBC_ListenerStub;
const LLBC_String LLBC_INVALID_LISTENER_STUB;

__LLBC_NS_END

#endif // !__LLBC_COM_EVENT_DATA_TYPE_H__
