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

#ifndef __LLBC_CORE_CORE_H__
#define __LLBC_CORE_CORE_H__

#include "llbc/common/Common.h"

#include "llbc/core/os/Common.h"
#include "llbc/core/algo/Algo.h"
#include "llbc/core/bundle/Common.h"
#include "llbc/core/utils/Common.h"
#include "llbc/core/sampler/Common.h"
#include "llbc/core/helper/Common.h"
#include "llbc/core/file/Common.h"
#include "llbc/core/comstring/Common.h"
#include "llbc/core/variant/Common.h"
#include "llbc/core/rapidjson/json.h"
#include "llbc/core/config/Common.h"
#include "llbc/core/time/Common.h"
#include "llbc/core/event/Common.h"
#include "llbc/core/timer/Common.h"
#include "llbc/core/thread/Common.h"
#include "llbc/core/singleton/Common.h"
#include "llbc/core/log/Common.h"
#include "llbc/core/random/Common.h"
#include "llbc/core/entity/Common.h"
#include "llbc/core/transcoder/Common.h"
#include "llbc/core/library/Common.h"
#include "llbc/core/objbase/ObjBase.h"
#include "llbc/core/objectpool/Common.h"

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
