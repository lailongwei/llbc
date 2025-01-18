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

#ifndef LLBC_JSON_NLOHMANN_LLBC_JSON_INL_H
#define LLBC_JSON_NLOHMANN_LLBC_JSON_INL_H

#include "llbc/common/Common.h"
#include "llbc/core/json/nlohmann/json.hpp"

__LLBC_NS_BEGIN

/**
 * LLBC_Json convert to LLBC_String
 * @param[in] value - LLBC_Json 
 * @param[out] outStr - output string
 */
LLBC_EXPORT LLBC_FORCE_INLINE void LLBC_JsonToString(const LLBC_Json &value, LLBC_String &outStr, bool isPretty = false)
{
    if(isPretty)
    {
        outStr = value.dump(4);
        return;
    }

    outStr = value.dump();
}

/**
 * 自动回收json对象
 */
class LLBC_EXPORT LLBC_AutoJson
{
public:
    LLBC_AutoJson(LLBC_Json *ptr) noexcept;
    LLBC_AutoJson(const LLBC_AutoJson &other) = delete;
    LLBC_AutoJson(LLBC_AutoJson &&other) noexcept;
    ~LLBC_AutoJson();
    
    /**
     * no copy
     */
    LLBC_AutoJson& operator= (const LLBC_AutoJson &other) = delete;

    /**
     * movable
     */
    LLBC_AutoJson& operator= (LLBC_AutoJson &&other) noexcept;

    /**
     * easy pointer
     */
    LLBC_Json *operator->() noexcept;
    const LLBC_Json *operator->() const noexcept;

    /**
     * 解引用
    */
    LLBC_Json &operator *();
    const LLBC_Json &operator *() const;
    
    /**
     * check nullptr
     */
    operator bool() const noexcept;

    LLBC_Json *_json = nullptr;
};

LLBC_FORCE_INLINE LLBC_AutoJson::LLBC_AutoJson(LLBC_Json *ptr) noexcept
    :_json(ptr)
{
    
}

LLBC_FORCE_INLINE LLBC_AutoJson::LLBC_AutoJson(LLBC_AutoJson &&other) noexcept
{
    _json = other._json;
    other._json = nullptr;
}

LLBC_FORCE_INLINE LLBC_AutoJson::~LLBC_AutoJson()
{
    if(_json != nullptr)
    {
        delete _json;
        _json = nullptr;
    }
}

LLBC_FORCE_INLINE LLBC_AutoJson& LLBC_AutoJson::operator= (LLBC_AutoJson &&other) noexcept
{
    _json = other._json;
    other._json = nullptr;
    return *this;
}

LLBC_FORCE_INLINE LLBC_Json *LLBC_AutoJson::operator->() noexcept
{
    return _json;
}

LLBC_FORCE_INLINE const LLBC_Json *LLBC_AutoJson::operator->() const noexcept
{
    return _json;
}

LLBC_FORCE_INLINE LLBC_AutoJson::operator bool() const noexcept
{
    return _json != nullptr;
}

LLBC_FORCE_INLINE LLBC_Json &LLBC_AutoJson::operator *()
{
    return *_json;
}

LLBC_FORCE_INLINE const LLBC_Json &LLBC_AutoJson::operator *() const
{
    return *_json;
}

__LLBC_NS_END

#endif // LLBC_JSON_NLOHMANN_LLBC_JSON_INL_H