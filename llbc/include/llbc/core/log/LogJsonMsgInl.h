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

#include "llbc/core/variant/Variant.h"

#pragma once

__LLBC_NS_BEGIN

inline LLBC_LogJsonMsg &LLBC_LogJsonMsg::Add(const char *key, const char* value)
{
    _doc.AddMember(LLBC_Json::StringRef(key),
                   LLBC_JsonValue(value, strlen(value)).Move(),
                   _doc.GetAllocator());
    return *this;
}

template <>
inline LLBC_LogJsonMsg &LLBC_LogJsonMsg::Add(const char *key, const sint64 &value)
{
    _doc.AddMember(LLBC_Json::StringRef(key),
                   LLBC_JsonValue(static_cast<int64_t>(value)).Move(),
                   _doc.GetAllocator());
    return *this;
}

template <>
inline LLBC_LogJsonMsg &LLBC_LogJsonMsg::Add(const char *key, const uint64 &value)
{
    _doc.AddMember(LLBC_Json::StringRef(key),
                   LLBC_JsonValue(static_cast<uint64_t>(value)).Move(),
                   _doc.GetAllocator());
    return *this;
}

template <>
inline LLBC_LogJsonMsg &LLBC_LogJsonMsg::Add(const char *key, const LLBC_Variant &value)
{
    LLBC_String str = value.ValueToString();
    _doc.AddMember(LLBC_Json::StringRef(key),
                   LLBC_JsonValue(str.c_str(), str.length(), _doc.GetAllocator()).Move(),
                   _doc.GetAllocator());
    return *this;
}

template <typename T>
LLBC_LogJsonMsg &LLBC_LogJsonMsg::Add(const char *key, const T &value)
{
    _doc.AddMember(LLBC_Json::StringRef(key),
                   LLBC_JsonValue(value).Move(),
                   _doc.GetAllocator());
    return *this;
}

__LLBC_NS_END

