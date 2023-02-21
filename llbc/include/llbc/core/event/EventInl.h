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

template <typename ParamType>
inline LLBC_Event &LLBC_Event::SetParam(int key, const ParamType &param)
{
    const LLBC_Variant varParam(param);
    return SetParam(key, varParam);
}

template <typename ParamType>
inline LLBC_Event &LLBC_Event::SetParam(const char *key, const ParamType &param)
{
    const LLBC_Variant varParam(param);
    return SetParam(key, varParam);
}

template <typename ParamType>
inline LLBC_Event &LLBC_Event::SetParam(const LLBC_String &key, const ParamType &param)
{
    const LLBC_Variant varParam(param);
    return SetParam(key, varParam);
}

inline std::map<int, LLBC_Variant> &LLBC_Event::GetMutableIntKeyParams()
{
    if (!_intKeyParams)
        _intKeyParams = new std::map<int, LLBC_Variant>();
    return *_intKeyParams;
}

inline size_t LLBC_Event::GetIntKeyParamsCount() const
{
    return _intKeyParams != nullptr ? _intKeyParams->size() : 0;
}

inline size_t LLBC_Event::GetConstantStrKeyParamsCount() const
{
    return _constantStrKeyParams != nullptr ? _constantStrKeyParams->size() : 0;
}

inline std::map<LLBC_CString, LLBC_Variant> &LLBC_Event::GetMutableConstantStrKeyParams()
{
    if (!_constantStrKeyParams)
        _constantStrKeyParams = new std::map<LLBC_CString, LLBC_Variant>();

    return *_constantStrKeyParams;
}

inline size_t LLBC_Event::GetStrKeyParamsCount() const
{
    return _strKeyParams != nullptr ? _strKeyParams->size() : 0;
}

inline std::map<LLBC_String, LLBC_Variant> &LLBC_Event::GetMutableStrKeyParams()
{
    if (!_strKeyParams)
        _strKeyParams = new std::map<LLBC_String, LLBC_Variant>();

    return *_strKeyParams;
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
