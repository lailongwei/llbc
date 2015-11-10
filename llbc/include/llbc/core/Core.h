/**
 * @file    Core.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/01
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_CORE_H__
#define __LLBC_CORE_CORE_H__

#include "llbc/common/Common.h"

#include "llbc/core/os/Common.h"
#include "llbc/core/bundle/Common.h"
#include "llbc/core/utils/Common.h"
#include "llbc/core/sampler/Common.h"
#include "llbc/core/helper/Common.h"
#include "llbc/core/file/Common.h"
#include "llbc/core/comstring/Common.h"
#include "llbc/core/variant/Common.h"
#include "llbc/core/json/json.h"
#include "llbc/core/config/Common.h"
#include "llbc/core/time/Common.h"
#include "llbc/core/event/Common.h"
#include "llbc/core/timer/Common.h"
#include "llbc/core/thread/Common.h"
#include "llbc/core/singleton/Singleton.h"
#include "llbc/core/log/Common.h"
#include "llbc/core/random/Common.h"
#include "llbc/core/entity/Common.h"
#include "llbc/core/transcoder/Common.h"
#include "llbc/core/library/Common.h"

__LLBC_NS_BEGIN

/**
 * LLBC library Core-Module startup function, before use any Core-Module's
 * functions or classes, must call this function to initialize the module.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN LLBC_HIDDEN int __LLBC_CoreStartup();

/**
 * LLBC library Core-Module cleanup function.
 */
LLBC_EXTERN LLBC_HIDDEN void __LLBC_CoreCleanup();

__LLBC_NS_END

#endif // !__LLBC_CORE_CORE_H__
