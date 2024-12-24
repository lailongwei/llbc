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

#ifndef LLBC_NLOHMANN_LLBC_JSON_H_
#define LLBC_NLOHMANN_LLBC_JSON_H_

#pragma once

#include "llbc/common/Common.h"
#include "llbc/core/nlohmann/json.hpp"

__LLBC_NS_BEGIN

/**
 * LLBCJson convert to LLBC_String
 * @param[in] value - LLBC_JsonValues
 * @param[out] outStr - output string
 */
LLBC_EXPORT void LLBC_JsonToString(const LLBCJson &value, LLBC_String &outStr);

/**
 * LLBCJson from LLBC_String.
 * @param[in] raw - json string
 */
LLBC_EXPORT LLBC_FORCE_INLINE LLBCJson LLBCJsonParseFrom(const LLBC_String &raw)
{
    return LLBCJson::parse(raw);
}

__LLBC_NS_END

#endif // LLBC_NLOHMANN_LLBC_JSON_H_
