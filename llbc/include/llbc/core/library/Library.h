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
 * \breif Stand for library on different operation systems.
 */
class LLBC_EXPORT LLBC_Library
{
public:
    LLBC_Library();
    ~LLBC_Library();

public:
    /**
     * Open library.
     * @param[in] fileName - library name.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Open(const char *fileName);

    /**
     * Get library process address.
     * @param[in] procName - Pointer to a null-terminated string containing the function name.
     * @return LLBC_LibraryFun - library function address, if failed, return nullptr.
     */
    LLBC_LibraryFun GetProcAddress(const char *procName);

    /**
     * Close library.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Close();

    LLBC_DISABLE_ASSIGNMENT(LLBC_Library);

private:
    LLBC_LibraryHandle _handle;
};

__LLBC_NS_END
