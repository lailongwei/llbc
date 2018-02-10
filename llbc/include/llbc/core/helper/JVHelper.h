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

#ifndef __LLBC_CORE_HELPER_JV_HELPER_H__
#define __LLBC_CORE_HELPER_JV_HELPER_H__

#include "llbc/common/Common.h"

#include "llbc/core/json/json.h"
#include "llbc/core/variant/Variant.h"

__LLBC_NS_BEGIN

/**
 * \brief The interconversion of Variant/Json format data helper class encapsulation.
 */
class LLBC_EXPORT LLBC_JVHelper
{
public:
    /**
     * Convert json format data to variant.
     * @param[in] json - the json value.
     * @return LLBC_Variant - converted variant data.
     */
    static LLBC_Variant J2V(const LLBC_JsonValue &json);

    /**
     * Convert variant format data to json.
     * @param[in] var - the variant value.
     * @return LLBC_JsonValue - converted json data.
     */
    static LLBC_JsonValue V2J(const LLBC_Variant &var);
};

__LLBC_NS_END

#endif // !__LLBC_CORE_HELPER_JV_HELPER_H__
