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


#pragma once

#include "llbc/common/PFConfig.h"
#include "llbc/common/Compiler.h"

#include "llbc/common/Config.h"

#include "llbc/common/OSHeader.h"
// #include "llbc/common/ThirdHeader.h"
#include "llbc/common/Macro.h"
#include "llbc/common/BasicDataType.h"
#include "llbc/common/Define.h"
#include "llbc/common/Template.h"
#include "llbc/common/Endian.h"
#include "llbc/common/Stream.h"
#include "llbc/common/StringDataType.h"
#include "llbc/common/EventDataType.h"
#include "llbc/common/SocketDataType.h"
#include "llbc/common/Exceptions.h"
#include "llbc/common/Functional.h"

#include "llbc/common/Errno.h"
#include "llbc/common/Errors.h"

#include "llbc/common/RTTI.h"

#include "llbc/common/Version.h"

#include "llbc/common/LibTlsInl.h"

__LLBC_NS_BEGIN

/**
 * LLBC library Common-Module startup function, before use any Common module's
 * functions or classes, must call this function to initialize the module.
 * @return int - return 0 if success, otherwise return -1.
 */

LLBC_HIDDEN int __LLBC_CommonStartup();

/**
 * LLBC library Common-Module cleanup function.
 */
LLBC_HIDDEN void __LLBC_CommonCleanup();

/**
 * Get entry thread native thread handle.
 * @return LLBC_NativeThreadHandle - the native thread handle.
 */
LLBC_EXPORT LLBC_NativeThreadHandle __LLBC_GetEntryThreadNativeThreadHandle();

__LLBC_NS_END
