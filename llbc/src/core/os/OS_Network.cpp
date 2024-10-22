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

#include "llbc/core/os/OS_Network.h"

__LLBC_NS_BEGIN

int LLBC_GetAddrInfo(const char *nodename,
                     const char *servname,
                     const struct addrinfo *hints,
                     struct addrinfo **res)
{
    int ret = getaddrinfo(nodename, servname, hints, res);
    if (ret != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_GAI);
        LLBC_SetSubErrorNo(ret);

        return LLBC_FAILED;
    }

    return LLBC_OK;
}

void LLBC_FreeAddrInfo(struct addrinfo *ai)
{
    freeaddrinfo(ai);
}

__LLBC_NS_END
