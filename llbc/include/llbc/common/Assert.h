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

#include "llbc/common/Config.h"
#include "llbc/common/Macro.h"

__LLBC_NS_BEGIN
/**
 * llbc framework internal function(exported), used for process assert fail logic.
 * @param[in] cond - assert condition.
 * @param[in] file - the filename.
 * @param[in] line - the line number.
 * @param[in] func - the function name.
 */
void LLBC_EXPORT __LLBC_AssertFail(const char *cond,
                                   const char *file,
                                   unsigned int line,
                                   const char *func);
__LLBC_NS_END

// LLBC framework assert impl
#if LLBC_CFG_COM_ENABLE_LLBC_ASSERT
 #if LLBC_TARGET_PLATFORM_WIN32
  #define llbc_assert(cond)                                                      \
      (LIKELY(cond) ?                                                            \
           static_cast<void>(0) :                                                \
               LLBC_NS __LLBC_AssertFail(#cond, __FILE__, __LINE__, __FUNCSIG__))
 #else // Non-Win32
  #define llbc_assert(cond)                                                               \
      (LIKELY(cond) ?                                                                     \
           static_cast<void>(0) :                                                         \
               LLBC_NS __LLBC_AssertFail(#cond, __FILE__, __LINE__, __PRETTY_FUNCTION__))
 #endif // Win32
#else // Disable llbc_assert macro.
 #define llbc_assert(cond) (static_cast<void>(0))
#endif // Enable llbc_assert macro

