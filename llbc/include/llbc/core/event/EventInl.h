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
, _params()
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

template<typename KeyType>
std::enable_if_t<__LLBC_Inl_EventKeyMatch, const LLBC_Variant &>
LLBC_Event::GetParam(const KeyType &key) const
{
    auto it = _params.find(key);
    return it != _params.end() ? it->second : LLBC_INL_NS __nilVariant;
}

template<typename KeyType, typename ParamType>
std::enable_if_t<__LLBC_Inl_EventKeyMatch, void>
LLBC_Event::SetParam(const KeyType &key, const ParamType &param)
{
    if constexpr (LLBC_IsTemplSpec<KeyType, std::basic_string>::value)
    {
        auto heavyIt = _heavyKeys.find(key);
        if (heavyIt == _heavyKeys.end())
        {
            auto heavyKey = new std::string(key);
            heavyIt = _heavyKeys.insert(std::make_pair(heavyKey->c_str(), heavyKey)).first;
        }

        _params[heavyIt->first] = std::is_same_v<ParamType, LLBC_Variant> ? param : LLBC_Variant(param);
    }
    else
    {
        _params[key] = std::is_same_v<ParamType, LLBC_Variant> ? param : LLBC_Variant(param);
    }
}

inline const std::map<LLBC_CString, LLBC_Variant> &LLBC_Event::GetParams() const
{
    return _params;
}

inline std::map<LLBC_CString, LLBC_Variant> &LLBC_Event::GetMutableParams()
{
    return _params;
}

inline LLBC_Event * LLBC_Event::Clone() const
{
    auto *clone = new LLBC_Event(_id, false);
    clone->_params = _params;
    for(auto&[_, heavyKey] : _heavyKeys) clone->_heavyKeys[_.c_str()] = new std::string(*heavyKey);
    return clone;
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

template<typename KeyType>
LLBC_Variant &LLBC_Event::operator[](const KeyType &key)
{
    return const_cast<LLBC_Variant&>(GetParam(key));
}

template<typename KeyType>
const LLBC_Variant &LLBC_Event::operator[](const KeyType &key) const
{
    return GetParam(key);
}

inline void LLBC_Event::Reuse()
{
    _id = 0;
    _dontDelAfterFire = false;
    _params.clear();
    for(auto&[_, heavyKey] : _heavyKeys) delete heavyKey;
    _heavyKeys.clear();
    ClearExtData();
}

__LLBC_NS_END
