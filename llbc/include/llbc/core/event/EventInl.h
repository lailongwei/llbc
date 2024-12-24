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

, _extData(nullptr)
, _extDataClearDeleg(nullptr)
{

}

inline LLBC_Event::LLBC_Event(const LLBC_Event &other)
{
    _id = other._id;
    _dontDelAfterFire = other._dontDelAfterFire;

    for (auto &[slimKey, param] : other._params)
    {
        if (auto heavyKeyIt = other._heavyKeys.find(slimKey); heavyKeyIt != other._heavyKeys.end())
            SetParam(*heavyKeyIt->second, param);
        else
            SetParam(slimKey, param);
    }
}

inline LLBC_Event::LLBC_Event(LLBC_Event &&other) noexcept
{
    _id = other._id;
    _dontDelAfterFire = other._dontDelAfterFire;

    _params = std::move(other._params);
    _heavyKeys = std::move(other._heavyKeys);

    _extData = other._extData;
    _extDataClearDeleg = other._extDataClearDeleg;

    other._id = 0;
    other._dontDelAfterFire = false;
    other._extData = nullptr;
    other._extDataClearDeleg = nullptr;
}

inline LLBC_Event::~LLBC_Event()
{
    ClearExtData(true);
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
LLBC_Event::GetParam(const KeyType &key)
{
    auto it = _params.find(key);
    return it != _params.end() ? it->second : _params.emplace(key, LLBC_Variant()).first->second;
}

template<typename KeyType, typename ParamType>
std::enable_if_t<__LLBC_Inl_EventKeyMatch, void>
LLBC_Event::SetParam(const KeyType &key, const ParamType &param)
{
    auto paramIt = _params.find(key);
    if (paramIt != _params.end())
    {
        paramIt->second = param;
        return ;
    }

    if constexpr (LLBC_IsTemplSpec<KeyType, std::basic_string>::value)
    {
        auto heavyIt = _heavyKeys.find(key);
        if (heavyIt == _heavyKeys.end())
        {
            auto heavyKey = new std::string(key);
            heavyIt = _heavyKeys.emplace(heavyKey->c_str(), heavyKey).first;
        }

        _params.emplace(heavyIt->first, param);
    }
    else
    {
        _params.emplace(key, param);
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

inline void *LLBC_Event::GetExtData() const
{
    return _extData;
}

inline void LLBC_Event::SetExtData(void *extData, const LLBC_Delegate<void(void *)> &clearDeleg)
{
    ClearExtData(false);
    _extData = extData;
    if (clearDeleg)
    {
        if (_extDataClearDeleg)
            *_extDataClearDeleg = clearDeleg;
        else
            delete _extDataClearDeleg;
    }
}

inline void LLBC_Event::ClearExtData(bool delDeleg)
{
    if (_extData)
    {
        if (_extDataClearDeleg)
            (*_extDataClearDeleg)(_extData);
        _extData = nullptr;
    }

    if (delDeleg)
        LLBC_XDelete(_extDataClearDeleg);
}

template<typename KeyType>
LLBC_Variant &LLBC_Event::operator[](const KeyType &key)
{
    return GetParam(key);
}

template<typename KeyType>
const LLBC_Variant &LLBC_Event::operator[](const KeyType &key) const
{
    return GetParam(key);
}

inline LLBC_Event &LLBC_Event::operator=(const LLBC_Event &other)
{
    if (this == &other)
        return *this;

    Reuse();

    _id = other._id;
    _dontDelAfterFire = other._dontDelAfterFire;

    for (auto &[slimKey, param] : other._params)
    {
        if (auto heavyKeyIt = other._heavyKeys.find(slimKey); heavyKeyIt != other._heavyKeys.end())
            SetParam(*heavyKeyIt->second, param);
        else
            SetParam(slimKey, param);
    }

    return *this;
}

inline LLBC_Event &LLBC_Event::operator=(LLBC_Event &&other) noexcept
{
    if (this == &other)
        return *this;

    Reuse();

    _id = other._id;
    _dontDelAfterFire = other._dontDelAfterFire;

    _params = std::move(other._params);
    _heavyKeys = std::move(other._heavyKeys);

    _extData = other._extData;
    _extDataClearDeleg = other._extDataClearDeleg;

    other._id = 0;
    other._dontDelAfterFire = false;
    other._extData = nullptr;
    other._extDataClearDeleg = nullptr;

    return *this;
}

inline std::ostream &operator<<(std::ostream &o, const LLBC_Event &ev)
{
    o << "LLBC_Event("
    "id:"               << ev.GetId()              << ", "
    "dontDelAfterFire:" << ev.IsDontDelAfterFire() << ", "
    "ExtData:"          << ev.GetExtData()         << ", "
    "Params:{";
    for (auto it = ev.GetParams().begin(); it != ev.GetParams().end(); ++it)
    {
        const auto &[key, val] = *it;
        o << "[" << key << ":" << val.ToString() << "]" << (std::next(it) != ev.GetParams().end() ? ", " : "");
    }
    o << "})";

    return o;
}

inline void LLBC_Event::Reuse()
{
    ClearExtData(true);

    LLBC_STLHelper::DeleteContainer(_heavyKeys);
    _params.clear();

    _dontDelAfterFire = false;
    _id = 0;
}

#undef __LLBC_Inl_EventKeyMatch

__LLBC_NS_END
