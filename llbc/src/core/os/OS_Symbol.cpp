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

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_Symbol.h"

__LLBC_NS_BEGIN

#if LLBC_CFG_OS_IMPL_SYMBOL

int LLBC_InitSymbol()
{
#if LLBC_TARGET_PLATFORM_WIN32
    if (::SymInitialize(::GetCurrentProcess(), NULL, TRUE) != TRUE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif // Win32
}

LLBC_String LLBC_CaptureStackBackTrace(size_t skipFrames, size_t captureFrames)
{
    LLBC_String backTrace;

    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    if (captureFrames == LLBC_INFINITE)
        captureFrames = LLBC_CFG_OS_SYMBOL_MAX_CAPTURE_FRAMES;
    else
        captureFrames = MIN(captureFrames, LLBC_CFG_OS_SYMBOL_MAX_CAPTURE_FRAMES);

    void **stack = libTls->coreTls.symbol.stack;

#if LLBC_TARGET_PLATFORM_WIN32
    DWORD displacement;
    HANDLE curProc = ::GetCurrentProcess();
    SYMBOL_INFO *symbol = libTls->coreTls.symbol.win32Symbol;
    const WORD frames = ::CaptureStackBackTrace(skipFrames + 1, captureFrames, stack, NULL);

    for (WORD frame = 0; frame != frames; frame++)
    {
        ::SymFromAddr(curProc, (DWORD64)stack[frame], 0, symbol);
        IMAGEHLP_LINE64 &imgHelpLine64 = libTls->coreTls.symbol.win32ImgHelpLine64;
        if (::SymGetLineFromAddr64(curProc, symbol->Address, &displacement, &imgHelpLine64) == TRUE)
        {
            backTrace.append_format("#%d 0x%x in %s at %s:%d", frames - frame - 1, (void *)symbol->Address,
                symbol->Name, imgHelpLine64.FileName, imgHelpLine64.LineNumber);
        }
        else
        {
            backTrace.append_format("#%d 0x%x in %s at %s:%d", frames - frame - 1, 
                (void *)symbol->Address, symbol->Name, "", 0);
        }

        if (frame != frames - 1)
            backTrace.append(1, '\n');
    }
#else // Non-Win32
    const int frames = ::backtrace(stack, captureFrames);
    char **strs = ::backtrace_symbols(stack, frames);
    for (int i = 0; i < frames; i++)
    {
        backTrace.append("%s", strs[i]);
        if (i != frames - 1)
            backTrace.append(1, '\n');
    }

    free(strs);
#endif

    return backTrace;
}

#endif // LLBC_CFG_OS_IMPL_SYMBOL

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
