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
 #include <uuid/uuid.h>
#endif // Non-Win32

#include "llbc/core/helper/GUIDHelper.h"

__LLBC_NS_BEGIN

LLBC_GUID LLBC_GUIDHelper::Gen()
{
    LLBC_GUID guid;
    memset(&guid, 0, sizeof(LLBC_GUID));

#if LLBC_TARGET_PLATFORM_NON_WIN32
    uuid_t uuid;
    uuid_generate(uuid);

    guid.Data1 = (static_cast<uint32>(uuid[0]) << 24) |
                 (static_cast<uint32>(uuid[1]) << 16) |
                 (static_cast<uint32>(uuid[2]) << 8) |
                 static_cast<uint32>(uuid[3]);
    guid.Data2 = static_cast<uint16>((static_cast<uint16>(uuid[4]) << 8) |
                                     static_cast<uint16>(uuid[5]));
    guid.Data3 = static_cast<uint16>((static_cast<uint16>(uuid[6]) << 8) |
                                     static_cast<uint16>(uuid[7]));
    memcpy(guid.Data4, uuid + 8, sizeof(guid.Data4));
#else
    ::CoCreateGuid(&guid);
#endif

    return guid;
}

LLBC_String LLBC_GUIDHelper::Format(LLBC_GUIDCRef guid)
{
    LLBC_String str;
    str.format("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", 
            static_cast<unsigned int>(guid.Data1),
            static_cast<unsigned int>(guid.Data2),
            static_cast<unsigned int>(guid.Data3),
            static_cast<unsigned int>(guid.Data4[0]),
            static_cast<unsigned int>(guid.Data4[1]),
            static_cast<unsigned int>(guid.Data4[2]),
            static_cast<unsigned int>(guid.Data4[3]),
            static_cast<unsigned int>(guid.Data4[4]),
            static_cast<unsigned int>(guid.Data4[5]),
            static_cast<unsigned int>(guid.Data4[6]),
            static_cast<unsigned int>(guid.Data4[7]));

    return str;
}

LLBC_String LLBC_GUIDHelper::GenStr()
{
    return Format(Gen());
}

__LLBC_NS_END
