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

#ifndef __LLBC_COM_BASIC_CSTRING_H__
#define __LLBC_COM_BASIC_CSTRING_H__

#include "llbc/common/PFConfig.h"

#include "llbc/common/OSHeader.h"
#include "llbc/common/BasicString.h"

__LLBC_NS_BEGIN

/**
 * \brief The const string class encapsulation(simple encapsulation).
 */
template <typename _Elem>
class LLBC_BasicCString
{
public:
    // The npos define.
    static constexpr size_t npos = static_cast<size_t>(-1);

public:
    /**
     * Constructors.
     */
    template <size_t _ArrLen>
    LLBC_BasicCString(const _Elem (&arr)[_ArrLen])
    : _cstr(arr)
    , _size(LIKELY(_ArrLen > 0) ? _ArrLen - 1 : 0)
    {
    }

    LLBC_BasicCString(const _Elem *cstr = nullptr, size_t size = npos)
    {
        if (cstr == nullptr || size == 0)
        {
            _cstr = "";
            _size = 0;
            return;
        }

        _cstr = cstr;
        _size = size == npos ? strlen(cstr) : size;
    }

    LLBC_BasicCString(const std::basic_string<_Elem> &stlStr)
    : _cstr(stlStr.data()), _size(stlStr.size())
    {
    }

    LLBC_BasicCString(const LLBC_BasicCString &other): _cstr(other._cstr), _size(other._size)
    {
    }

public:
    /**
     * Get const string.
     * @return const _Elem * - the const string.
     */
    const _Elem *GetStr() const
    {
        return _cstr;
    }

    /**
     * Get string size.
     * @return size_t - the string size.
     */
    size_t GetSize() const
    {
        return _size;
    }

public:
    // operator=
    template <size_t _ArrLen>
    LLBC_BasicCString &operator =(const _Elem (&arr)[_ArrLen])
    {
        _cstr = arr;
        _size = _ArrLen - 1;

        return *this;
    }
    LLBC_BasicCString &operator =(const _Elem *cstr)
    {
        _cstr = cstr;
        _size = strlen(cstr);

        return *this;
    }
    LLBC_BasicCString &operator=(const std::basic_string<_Elem> &stlStr)
    {
        _cstr = stlStr.data();
        _size = stlStr.size();

        return *this;
    }
    LLBC_BasicCString &operator=(const LLBC_BasicCString &other)
    {
        _cstr = other._cstr;
        _size = other._size;

        return *this;
    }

    // operator <
    template <size_t _ArrLen>
    bool operator <(const _Elem (&arr)[_ArrLen]) const
    {
        return operator<(LLBC_BasicCString(arr));
    }
    bool operator <(const _Elem *cstr) const
    {
        return operator<(LLBC_BasicCString(cstr));
    }
    bool operator <(const std::basic_string<_Elem> &stlStr) const
    {
        return operator<(LLBC_BasicCString(stlStr.data(), stlStr.size()));
    }
    bool operator <(const LLBC_BasicCString &other) const
    {
        return _cstr != other._cstr && 
                (_size < other._size ||
                 (_size == other._size && memcmp(_cstr, other._cstr, sizeof(_Elem) * _size) < 0));
    }

    // operator ==
    template <size_t _ArrLen>
    bool operator ==(const _Elem (&arr)[_ArrLen]) const
    {
        return operator==(LLBC_BasicCString(arr));
    }
    bool operator ==(const _Elem *cstr) const
    {
        return operator==(LLBC_BasicCString(cstr));
    }
    bool operator ==(const std::basic_string<_Elem> &stlStr) const
    {
        return operator==(LLBC_BasicCString(stlStr));
    }
    bool operator ==(const LLBC_BasicCString &other) const
    {
        return UNLIKELY(_cstr == other._cstr) ||
            (_size == other._size &&
             memcmp(_cstr, other._cstr, sizeof(_Elem) * _size) == 0);
    }

    // operator !=
    template <int _ArrLen>
    bool operator !=(const _Elem (&arr)[_ArrLen]) const
    {
        return !operator==(LLBC_BasicCString(arr));
    }
    bool operator !=(const _Elem *cstr) const
    {
        return !operator==(LLBC_BasicCString(cstr));
    }
    bool operator !=(const std::basic_string<_Elem> &stlStr) const
    {
        return !operator==(LLBC_BasicCString(stlStr));
    }
    bool operator !=(const LLBC_BasicCString &other) const
    {
        return !operator==(other);
    }

    // operator <=
    template <int _ArrLen>
    bool operator <=(const _Elem (&arr)[_ArrLen]) const
    {
        return !(LLBC_BasicCString(arr) < *this);
    }
    bool operator <=(const _Elem *cstr) const
    {
        return !(LLBC_BasicCString(cstr) < *this);
    }
    bool operator <=(const std::basic_string<_Elem> &stlStr) const
    {
        return !(LLBC_BasicCString(stlStr) < *this);
    }
    bool operator <=(const LLBC_BasicCString &other) const
    {
        return !(other < *this);
    }

    // operator >
    template <int _ArrLen>
    bool operator >(const _Elem (&other)[_ArrLen]) const
    {
        return LLBC_BasicCString(other) < *this;
    }
    bool operator >(const _Elem *other) const
    {
        return LLBC_BasicCString(other) < *this;
    }
    bool operator >(const std::basic_string<_Elem> &other) const
    {
        return LLBC_BasicCString(other) < *this;
    }
    bool operator >(const LLBC_BasicCString<_Elem> &other) const
    {
        return other < *this;
    }

    // operator >=
    template <int _ArrLen>
    bool operator >=(const _Elem (&arr)[_ArrLen]) const
    {
        return !(*this < LLBC_BasicCString(arr));
    }
    bool operator >=(const _Elem *cstr) const
    {
        return !(*this < LLBC_BasicCString(cstr));
    }
    bool operator >=(const std::basic_string<_Elem> &stlStr) const
    {
        return !(*this < LLBC_BasicCString(stlStr));
    }
    bool operator >=(const LLBC_BasicCString<_Elem> &other) const
    {
        return !(*this < other);
    }

private:
    const _Elem *_cstr;
    size_t _size;
};

/**
 * \brief The const string stream output function.
 */
template <typename _Elem>
std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_BasicCString<_Elem> &cstr)
{
    return o <<cstr.GetStr();
}

__LLBC_NS_END

namespace std
{

/**
 * \brief The std::hash<T> template class specilization: std::hash<LLBC_CString>.
 */
template <>
struct hash<LLBC_NS LLBC_BasicCString<char> >
{
    size_t operator()(const LLBC_NS LLBC_BasicCString<char> &cstr) const noexcept
    {
        // Use DJB hash algo
        size_t i, h;
        const char *str = cstr.GetStr();
        for (i = h = 0; i < cstr.GetSize(); ++i)
            h = ((h << 5) + h) ^ str[i];
        return h;
    };
};

/**
 * \brief The std::hash<T> template class specilization: std::hash<LLBC_CWString>.
 */
template <>
struct hash<LLBC_NS LLBC_BasicCString<wchar_t> >
{
    size_t operator()(const LLBC_NS LLBC_BasicCString<wchar_t> &cstr) const noexcept
    {
        // Use DJB hash algo
        size_t i, h;
        const wchar_t *str = cstr.GetStr();
        for (i = h = 0; i < cstr.GetSize(); ++i)
            h = ((h << 10) + h) ^ str[i];
        return h;
    };
};

}

#endif // !__LLBC_COM_BASIC_CSTRING_H__
