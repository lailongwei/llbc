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

__LLBC_NS_BEGIN

LLBC_FORCE_INLINE void LLBC_CPURelax()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
 #if LLBC_TARGET_PROCESSOR == LLBC_PROCESSOR_ARM || \
     LLBC_TARGET_PROCESSOR == LLBC_PROCESSOR_ARM_THUMB || \
         LLBC_TARGET_PROCESSOR == LLBC_PROCESSOR_ARM_64
    asm volatile("yield" ::: "memory");
 #elif LLBC_TARGET_PROCESSOR == LLBC_PROCESSOR_X86 || \
       LLBC_TARGET_PROCESSOR == LLBC_PROCESSOR_X86_64
    asm volatile ("rep;nop" : : : "memory");
 #else
    asm volatile ("nop");
 #endif
#else // WIN32 platform
    YieldProcessor();
#endif // Non-WIN32 platform
}

__LLBC_NS_END
