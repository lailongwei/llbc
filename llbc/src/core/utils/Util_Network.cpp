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

#include "llbc/core/utils/Util_Network.h"

__LLBC_NS_BEGIN

bool LLBC_IsIPv4Addr(const LLBC_String &addr)
{
    if (UNLIKELY(addr.empty()))
        return false;

    typedef std::vector<LLBC_String> _Parts;
    const _Parts parts = addr.split(".");
    if (parts.size() != 4)
        return false;

    for (_Parts::const_iterator it = parts.begin();
         it != parts.end();
         it++)
    {
        const LLBC_String &part = *it;
        if (part.empty())
            return false;

        for (LLBC_String::size_type i = 0;
             i < part.length();
             i++)
            if (!('0' <= part[i] && part[i] <= '9'))
                return false;
    }

    return true;
}

__LLBC_NS_END
