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
inline LLBC_Event::LLBC_Event(int id, bool dontDelAfterFire)
: _id(id)
, _dontDelAfterFire(dontDelAfterFire)
, _slimParams()
, _heavyKeys()
, _extData(nullptr)
, _extDataClearDeleg(nullptr) {}

inline LLBC_Event::~LLBC_Event()
{
    ClearExtData();
}

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

inline const LLBC_Variant &LLBC_Event::GetParam(const LLBC_CString &key) const
{
    const auto it = _slimParams.find(key);
    return it != _slimParams.end() ? it->second : LLBC_INL_NS __nilVariant;
}

inline void LLBC_Event::SetParam(const LLBC_CString &key, const LLBC_Variant &param)
{
    _slimParams[key] = param;
}

inline const std::map<LLBC_CString, LLBC_Variant> &LLBC_Event::GetParams() const
{
    return _slimParams;
}

inline std::map<LLBC_CString, LLBC_Variant> &LLBC_Event::GetMutableParams()
{
    return _slimParams;
}

inline LLBC_Event * LLBC_Event::Clone() const
{
    auto *clone = new LLBC_Event(_id, false);
    clone->_slimParams = _slimParams;
    for(auto&[_, heavyKey] : _heavyKeys) clone->_heavyKeys[_.c_str()] = new std::string(*heavyKey);
    return clone;
}

template<typename KeyType>
std::enable_if_t<LLBC_IsTemplSpec<KeyType, std::basic_string>::value, const LLBC_Variant &>
LLBC_Event::GetParam(const KeyType &key) const
{
    return GetParam(key);
}

template<typename KeyType>
std::enable_if_t<LLBC_IsTemplSpec<KeyType, std::basic_string>::value, void>
LLBC_Event::SetParam(const KeyType &key, const LLBC_Variant &param)
{
    std::map<LLBC_CString, std::string*>::iterator heavyIt = _heavyKeys.find(key);
    if (heavyIt == _heavyKeys.end())
    {
        auto heavyKey = new std::string(key);
        heavyIt = _heavyKeys.insert(std::make_pair(heavyKey->c_str(), heavyKey)).first;
    }

    const LLBC_CString slimKey(heavyIt->first);
    return SetParam(slimKey, param);
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

inline LLBC_Variant &LLBC_Event::operator[](const LLBC_CString &key)
{
    const auto it = _slimParams.find(key);
    return it == _slimParams.end() ? _slimParams.insert(std::make_pair(key, LLBC_Variant())).first->second : it->second;
}

inline const LLBC_Variant &LLBC_Event::operator[](const LLBC_CString &key) const
{
    const auto it = _slimParams.find(key);
    return it != _slimParams.end() ? it->second : LLBC_INL_NS __nilVariant;
}

template<typename KeyType>
std::enable_if_t<LLBC_IsTemplSpec<KeyType, std::basic_string>::value, LLBC_Variant &>
LLBC_Event::operator[](const KeyType &key)
{
    return GetParam(LLBC_CString(key));
}

template<typename KeyType>
std::enable_if_t<LLBC_IsTemplSpec<KeyType, std::basic_string>::value, const LLBC_Variant &>
LLBC_Event::operator[](const KeyType &key) const
{
    return GetParam(LLBC_CString(key));
}

inline void LLBC_Event::Reuse()
{
    _id = 0;
    _dontDelAfterFire = false;
    _slimParams.clear();
    for(auto&[_, heavyKey] : _heavyKeys) delete heavyKey;
    _heavyKeys.clear();
    ClearExtData();
}

__LLBC_NS_END
