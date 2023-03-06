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

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * The function maps the specified executable module into the address space of the calling process.
 * @param[in] fileName - module file name.
 * @return LLBC_LibraryHandle - Library handle.
 */
LLBC_EXPORT LLBC_LibraryHandle LLBC_LoadLibrary(const char *fileName);

/**
 * Retrieves the address of specified exported function from library.
 * @param[in] handle   - library handle.
 * @param[in] procName - function name.
 * @return LLBC_LibraryFun - library func address, if return nullptr, see LLBC_GetLastError().
 */
LLBC_EXPORT LLBC_LibraryFun LLBC_GetProcAddress(LLBC_LibraryHandle handle, const char *procName);

/**
 * Close library.
 * @param[in] handle - library handle.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_CloseLibrary(LLBC_LibraryHandle handle);

__LLBC_NS_END
