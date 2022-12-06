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

#if LLBC_TARGET_PLATFORM_NON_WIN32
 #include <cxxabi.h>
 #include <execinfo.h>
#endif

#include "llbc/core/os/OS_Symbol.h"

__LLBC_NS_BEGIN

#if LLBC_CFG_OS_IMPL_SYMBOL

int LLBC_InitSymbol()
{
#if LLBC_TARGET_PLATFORM_WIN32
    if (::SymInitialize(::GetCurrentProcess(), nullptr, TRUE) != TRUE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // Non-Win32
    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_WIN32
}

int LLBC_CleanupSymbol()
{
#if LLBC_TARGET_PLATFORM_WIN32
    if (::SymCleanup(GetCurrentProcess()) != TRUE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // Non-Win32
    return LLBC_OK;
#endif // LLBC_TARGET_PLATFORM_WIN32
}

LLBC_String LLBC_CaptureStackBackTrace(size_t skipFrames, size_t captureFrames)
{
    LLBC_String backTrace;

    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    if (captureFrames == static_cast<size_t>(LLBC_INFINITE))
        captureFrames = LLBC_CFG_OS_SYMBOL_MAX_CAPTURE_FRAMES;
    else
        captureFrames = MIN(captureFrames, LLBC_CFG_OS_SYMBOL_MAX_CAPTURE_FRAMES);

    void **stack = libTls->coreTls.symbol.stack;

#if LLBC_TARGET_PLATFORM_WIN32
    DWORD displacement;
    HANDLE curProc = ::GetCurrentProcess();
    SYMBOL_INFO *symbol = libTls->coreTls.symbol.win32Symbol;
    const WORD frames = ::CaptureStackBackTrace(static_cast<DWORD>(skipFrames) + 1,
                                                static_cast<DWORD>(captureFrames),
                                                stack,
                                                nullptr);

    for (WORD frame = 0; frame != frames; ++frame)
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
    const int frames = backtrace(stack, captureFrames + skipFrames);
    char **strs = backtrace_symbols(stack, frames);
    if (LIKELY(strs))
    {
        for (int i = skipFrames; i < frames; ++i)
        {
            backTrace.append_format("#%d ", frames - i - 1);

            char *parenthesisEnd = nullptr;
            char *parenthesisBeg = strchr(strs[i], '(');
            if (parenthesisBeg)
            {
                parenthesisBeg += 1;
                parenthesisEnd = strchr(parenthesisBeg, ')');
            }

            if (parenthesisEnd &&
                parenthesisBeg != parenthesisEnd)
            {
                char *addrOffsetBeg = strchr(parenthesisBeg, '+');
                if (addrOffsetBeg == nullptr)
                    addrOffsetBeg= parenthesisEnd;

                const char oldAddrOffsetBegCh = *addrOffsetBeg;
                *addrOffsetBeg = '\0';

                int status = 0;
                size_t length = sizeof(libTls->commonTls.rtti);
                abi::__cxa_demangle(parenthesisBeg, libTls->commonTls.rtti, &length, &status);
                *addrOffsetBeg = oldAddrOffsetBegCh;
                if (status == 0)
                {
                    backTrace.append(strs[i], parenthesisBeg - strs[i]);
                    backTrace.append(libTls->commonTls.rtti);
                    backTrace.append(addrOffsetBeg);
                }
                else
                {
                    backTrace.append_format("%s", strs[i]);
                }
            }
            else
            {
                backTrace.append_format("%s", strs[i]);
            }

            if (i != frames - 1)
                backTrace.append(1, '\n');
        }

        free(strs);
    }
#endif // LLBC_TARGET_PLATFORM_WIN32

    return backTrace;
}

#endif // LLBC_CFG_OS_IMPL_SYMBOL

__LLBC_NS_END
