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

#include "llbc/common/Config.h"
#include "llbc/common/Endian.h"

__LLBC_INTERNAL_NS_BEGIN

static union
{
    char strVal[4];
    int intVal;
} __g_endianTest = {{'l', '?', '?', 'b'}};

static const char *__g_endian_type_desc[LLBC_NS LLBC_Endian::UnknownEndian + 1] =
{
    "big endian",
    "little endian",
    "unknown endian"
};

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

bool LLBC_Endian::IsValid(int type)
{
    return (type >= LLBC_Endian::BigEndian &&
        type < LLBC_Endian::UnknownEndian) ? true : false;
}

const char *LLBC_Endian::Type2Str(int type)
{
    return LLBC_Endian::IsValid(type) ?
        LLBC_INTERNAL_NS __g_endian_type_desc[type] :
            LLBC_INTERNAL_NS __g_endian_type_desc[LLBC_Endian::UnknownEndian];
}

int LLBC_Endian::Str2Type(const char *str)
{
    if (!str)
        return LLBC_Endian::UnknownEndian;

    const size_t len = strlen(str);
    char *lowerCaseStr = reinterpret_cast<char *>(malloc(len + 1));
    for (size_t i = 0; i < len; ++i)
        lowerCaseStr[i] = tolower(str[i]);

    lowerCaseStr[len] = '\0';

    int type;
    if (memcmp(LLBC_INTERNAL_NS
        __g_endian_type_desc[LLBC_Endian::BigEndian], lowerCaseStr, len) == 0)
    {
        type = LLBC_Endian::BigEndian;
        goto finally;
    }
    else if (memcmp(LLBC_INTERNAL_NS
        __g_endian_type_desc[LLBC_Endian::LittleEndian], lowerCaseStr, len) == 0)
    {
        type = LLBC_Endian::LittleEndian;
        goto finally;
    }
    else
    {
        type = LLBC_Endian::UnknownEndian;
        goto finally;
    }

finally:
    free(lowerCaseStr);

    return type;
}

LLBC_EXPORT int LLBC_MachineEndian = LLBC_Endian::UnknownEndian;

# if LLBC_CFG_COMM_ORDER_IS_NET_ORDER
LLBC_EXPORT int LLBC_DefaultEndian = LLBC_Endian::NetEndian;
# else
LLBC_EXPORT int LLBC_DefaultEndian = LLBC_Endian::LittleEndian;
#endif

int LLBC_GetMachineEndianType()
{
    return static_cast<char>(LLBC_INTERNAL_NS __g_endianTest.intVal) == 'l' ?
        LLBC_Endian::LittleEndian : LLBC_Endian::BigEndian;
}

__LLBC_NS_END
