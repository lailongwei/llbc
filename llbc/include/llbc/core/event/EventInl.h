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

__LLBC_INTERNAL_NS_BEGIN

static const LLBC_NS LLBC_Variant __nilVariant;

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

inline int LLBC_Event::GetId() const
{
    return _id;
}

inline void LLBC_Event::SetId(int id)
{
    _id = id;
}

inline bool LLBC_Event::IsDontDelAfterFire() const
{
    return _dontDelAfterFire;
}

inline void LLBC_Event::SetDontDelAfterFire(bool dontDelAfterFire)
{
    _dontDelAfterFire = dontDelAfterFire;
}

template<typename KeyType>
std::enable_if_t<LLBC_IsTemplSpec<KeyType, std::basic_string>::value, const LLBC_Variant &>
LLBC_Event::GetParams(const KeyType &key) const
{
    const auto it = _stringKeyParams.find(key);
    return it != _stringKeyParams.end() ? it->second : LLBC_INL_NS __nilVariant;
}

template<typename KeyType>
std::enable_if_t<LLBC_IsTemplSpec<KeyType, std::basic_string>::value, void>
LLBC_Event::SetParam(const KeyType &key, const LLBC_Variant &param)
{
    if (const auto it = _stringKeyParams.find(key); it == _stringKeyParams.end())
        _stringKeyParams.insert(std::make_pair(key, param));
    else
        it->second = param;
}

template <typename ParamType>
void LLBC_Event::SetParam(const LLBC_CString &key, const ParamType &param)
{
    return SetParam(key, LLBC_Variant(param));
}

template<typename KeyType, typename ParamType>
std::enable_if_t<LLBC_IsTemplSpec<KeyType, std::basic_string>::value, void>
LLBC_Event::SetParam(const KeyType &key, const ParamType &param)
{
    return SetParam(key, LLBC_Variant(param));
}

template <typename ParamType>
void LLBC_Event::SetParam(const int &key, const ParamType &param)
{
    return SetParam(key, LLBC_Variant(param));
}

template<typename KeyType>
std::enable_if_t<LLBC_IsTemplSpec<KeyType, std::basic_string>::value, LLBC_Variant &>
LLBC_Event::operator[](const KeyType &key)
{
    const auto it = _stringKeyParams.find(key);
    return it != _stringKeyParams.end() ? it->second : LLBC_INL_NS __nilVariant;
}

template<typename KeyType>
std::enable_if_t<LLBC_IsTemplSpec<KeyType, std::basic_string>::value, const LLBC_Variant &>
LLBC_Event::operator[](const KeyType &key) const
{
    const auto it = _stringKeyParams.find(key);
    return it != _stringKeyParams.end() ? it->second : LLBC_INL_NS __nilVariant;
}

inline void * LLBC_Event::GetExtData() const
{
    return _extData;
}

inline void LLBC_Event::SetExtData(void *extData, const LLBC_Delegate<void(void *)> &clearDeleg)
{
    ClearExtData();
    _extData = extData;
    _extDataClearDeleg = clearDeleg;
}

inline void LLBC_Event::ClearExtData()
{
    if (_extData)
    {
        if (_extDataClearDeleg)
            _extDataClearDeleg(_extData);
        _extData = nullptr;
    }

    _extDataClearDeleg = nullptr;
}

__LLBC_NS_END
