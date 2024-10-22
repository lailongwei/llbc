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

template <>
inline LLBC_Variant LLBC_IniSection::GetValue(const LLBC_String &key, const LLBC_Variant &defaultValue) const
{
    _Values::const_iterator it = _values.find(key);
    if (it == _values.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return defaultValue;
    }

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    return it->second;
}

template <>
inline bool LLBC_IniSection::GetValue(const LLBC_String &key, const bool &defaultValue) const
{
    return this->GetValue(key, LLBC_Variant(defaultValue));
}

template <>
inline sint8 LLBC_IniSection::GetValue(const LLBC_String &key, const sint8 &defaultValue) const
{
    return this->GetValue(key, LLBC_Variant(defaultValue));
}

template <>
inline uint8 LLBC_IniSection::GetValue(const LLBC_String &key, const uint8 &defaultValue) const
{
    return this->GetValue(key, LLBC_Variant(defaultValue));
}

template <>
inline sint16 LLBC_IniSection::GetValue(const LLBC_String &key, const sint16 &defaultValue) const
{
    return this->GetValue(key, LLBC_Variant(defaultValue));
}

template <>
inline uint16 LLBC_IniSection::GetValue(const LLBC_String &key, const uint16 &defaultValue) const
{
    return this->GetValue(key, LLBC_Variant(defaultValue));
}

template <>
inline sint32 LLBC_IniSection::GetValue(const LLBC_String &key, const sint32 &defaultValue) const
{
    return this->GetValue(key, LLBC_Variant(defaultValue));
}

template <>
inline uint32 LLBC_IniSection::GetValue(const LLBC_String &key, const uint32 &defaultValue) const
{
    return this->GetValue(key, LLBC_Variant(defaultValue));
}

template <>
inline long LLBC_IniSection::GetValue(const LLBC_String &key, const long &defaultValue) const
{
    return this->GetValue(key, LLBC_Variant(defaultValue));
}

template <>
inline LLBC_NS ulong LLBC_IniSection::GetValue(const LLBC_String &key, const LLBC_NS ulong &defaultValue) const
{
    return this->GetValue(key, LLBC_Variant(defaultValue));
}

template <>
inline sint64 LLBC_IniSection::GetValue(const LLBC_String &key, const sint64 &defaultValue) const
{
    return this->GetValue(key, LLBC_Variant(defaultValue));
}

template <>
inline uint64 LLBC_IniSection::GetValue(const LLBC_String &key, const uint64 &defaultValue) const
{
    return this->GetValue(key, LLBC_Variant(defaultValue));
}

template <>
inline float LLBC_IniSection::GetValue(const LLBC_String &key, const float &defaultValue) const
{
    return this->GetValue(key, LLBC_Variant(defaultValue));
}

template <>
inline double LLBC_IniSection::GetValue(const LLBC_String &key, const double &defaultValue) const
{
    return this->GetValue(key, LLBC_Variant(defaultValue));
}

template <>
inline LLBC_String LLBC_IniSection::GetValue(const LLBC_String &key, const LLBC_String &defaultValue) const
{
    return this->GetValue(key, LLBC_Variant(defaultValue));
}

template <>
inline std::string LLBC_IniSection::GetValue(const LLBC_String &key, const std::string &defaultValue) const
{
    return this->GetValue(key, LLBC_Variant(LLBC_String(defaultValue))).AsStr();
}

template <typename _ValueType>
inline _ValueType LLBC_IniSection::GetValue(const LLBC_String &key, const _ValueType &defaultValue) const
{
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return defaultValue;
}

template <>
inline int LLBC_IniSection::SetValue(const LLBC_String &key, const LLBC_Variant &value, const LLBC_String &comment)
{
    if (key.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    _Values::iterator valIt = _values.find(key);
    if (valIt != _values.end())
        _values.erase(key);
    else
        _keys.push_back(key);

    _values.insert(std::make_pair(key, value));

    _comments.erase(key);
    _comments.insert(std::make_pair(key, comment));

    return LLBC_OK;
}

template <typename _ValueType>
inline int LLBC_IniSection::SetValue(const LLBC_String &key, const _ValueType &value, const LLBC_String &comment)
{
    return this->SetValue(key, LLBC_Variant(value), comment);
}

inline void LLBC_Ini::EndLine(LLBC_String &str)
{
    str.append(1, LLBC_LF_A);
}

template <typename _ValueType>
inline _ValueType LLBC_Ini::GetValue(const LLBC_String &sectionName,
                            const LLBC_String &key,
                            const _ValueType &defaultValue) const
{
    const LLBC_IniSection *section = GetSection(sectionName);
    if (section == nullptr)
        return defaultValue;

    return section->GetValue<_ValueType>(key, defaultValue);
}

template <typename _ValueType>
inline int LLBC_Ini::SetValue(const LLBC_String &sectionName,
                              const LLBC_String &key,
                              const _ValueType &value,
                              const LLBC_String &comment)
{
    LLBC_IniSection *section = GetSection(sectionName);
    if (section == nullptr)
        return LLBC_FAILED;

    return section->SetValue<_ValueType>(key, value, comment);
}

__LLBC_NS_END
