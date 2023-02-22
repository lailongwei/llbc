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

#include "llbc/common/PFConfig.h"

#include "llbc/common/OSHeader.h"

__LLBC_NS_BEGIN

/**
 * \brief The const string class encapsulation(simple encapsulation).
 */
template <typename _Elem>
class LLBC_BasicCString
{
public:
    // STL-like typedefs.
    typedef _Elem value_type;
    typedef size_t size_type;
    typedef long difference_type;
    typedef value_type * pointer;
    typedef const value_type * const_pointer;

    // The npos define.
    static constexpr size_type npos = static_cast<size_type>(-1);

public:
    /**
     * Default constructor when value_type == char.
     */
    template <typename _CtorElem = _Elem,
              typename = typename std::enable_if<std::is_same<_CtorElem, char>::value, _CtorElem>::type>
    #if LLBC_TARGET_PLATFORM_WIN32
    LLBC_BasicCString(nullptr_t _ = nullptr)
    #else
    LLBC_BasicCString(std::nullptr_t _ = nullptr)
    #endif
    : _cstr("")
    , _size(0)
    {
    }
    
    /**
     * Default constructor when value_type == wchar_t.
     */
    template <typename _CtorElem = _Elem,
              typename = typename std::enable_if<std::is_same<_CtorElem, wchar_t>::value, _CtorElem>::type,
              typename = _CtorElem>
    #if LLBC_TARGET_PLATFORM_WIN32
    LLBC_BasicCString(nullptr_t _ = nullptr)
    #else
    LLBC_BasicCString(std::nullptr_t _ = nullptr)
    #endif
    : _cstr(L"")
    , _size(0)
    {}

    /**
     * Parameter constructor when parameter type is literal string.
     */
    template <size_type _ArrLen>
    LLBC_BasicCString(const value_type (&arr)[_ArrLen])
    : _cstr(arr)
    , _size(LIKELY(_ArrLen > 0) ? _ArrLen - 1 : 0)
    {
    }

    /**
     * Parameter constructor when parameter type is char pointer.
     */
    template <typename _Ptr,
              typename = typename std::enable_if<std::is_pointer<_Ptr>::value &&
                                                 std::is_same<
                                                     typename std::remove_const<
                                                         typename std::remove_pointer<_Ptr>::type>::type, char>::value, _Ptr>::type>
    LLBC_BasicCString(_Ptr cstr, size_type size = npos)
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

    /**
     * Parameter constructor when parameter type is wchar_t pointer.
     */
    template <typename _Ptr,
              typename = typename std::enable_if<std::is_pointer<_Ptr>::value &&
                                                 std::is_same<
                                                     typename std::remove_const<
                                                         typename std::remove_pointer<_Ptr>::type>::type, wchar_t>::value, _Ptr>::type,

              typename = _Ptr>
    LLBC_BasicCString(_Ptr cstr, size_type size = npos)
    {
        if (cstr == nullptr == size == 0)
        {
            _cstr = L"";
            _size = 0;
            return;
        }

        _cstr = cstr;
        _size = size == npos ? wcslen(cstr) : size;
    }

    /**
     * Parameter constructor when parameter type is std::basic_string<value_type>.
     */
    LLBC_BasicCString(const std::basic_string<value_type> &stlStr)
    : _cstr(stlStr.data())
    , _size(stlStr.size())
    {
    }

    /**
     * Copy constructor.
     */
    LLBC_BasicCString(const LLBC_BasicCString &other)
    : _cstr(other._cstr)
    , _size(other._size)
    {
    }

public:
    /**
     * Get const string.
     * @return const_pointer - the const string.
     */
    const_pointer c_str() const
    {
        return _cstr;
    }

    /**
     * Get const string.
     * @return const_pointer - the const string.
     */
    const_pointer data() const
    {
        return _cstr;
    }

    /**
     * Get string size.
     * @return size_type - the string size.
     */
    size_type size() const
    {
        return _size;
    }

    /**
     * Get string size.
     * @return size_type - the string size.
     */
    size_type length() const
    {
        return _size;
    }

    /**
     * Check string is empty.
     * @reutrn bool - empty flag.
     */
    bool empty() const
    {
        return _size == 0;
    }

public:
    /**
     * nullptr_t assignment operator when value_type == char.
     */
    template <typename _CtorElem = _Elem,
              typename = typename std::enable_if<std::is_same<_CtorElem, char>::value, _CtorElem>::type>
    #if LLBC_TARGET_PLATFORM_WIN32
    LLBC_BasicCString &operator =(nullptr_t _)
    #else
    LLBC_BasicCString &operator =(std::nullptr_t _)
    #endif
    {
        _cstr = "";
        _size = 0;

        return *this;
    }

    /**
     * nullptr_t assignment operator when value_type == wchar_t.
     */
    template <typename _CtorElem = _Elem,
              typename = typename std::enable_if<std::is_same<_CtorElem, wchar_t>::value, _CtorElem>::type,
              typename = _CtorElem>
    #if LLBC_TARGET_PLATFORM_WIN32
    LLBC_BasicCString &operator =(nullptr_t _)
    #else
    LLBC_BasicCString &operator =(std::nullptr_t _)
    #endif
    {
        _cstr = L"";
        _size = 0;

        return *this;
    }

    /**
     * Literal string assignment operator.
     */
    template <size_type _ArrLen>
    LLBC_BasicCString &operator =(const value_type (&arr)[_ArrLen])
    {
        _cstr = arr;
        _size = LIKELY(_ArrLen > 0) ? _ArrLen - 1 : 0;

        return *this;
    }

    /**
     * String pointer assignment operator when value_type == char.
     */
    template <typename _Ptr,
              typename = typename std::enable_if<std::is_pointer<_Ptr>::value &&
                                                 std::is_same<
                                                     typename std::remove_const<
                                                         typename std::remove_pointer<_Ptr>::type>::type, char>::value, _Ptr>::type>
    LLBC_BasicCString &operator =(_Ptr cstr)
    {
        if (LIKELY(cstr))
        {
            _cstr = cstr;
            _size = strlen(cstr);
        }
        else
        {
            _cstr = "";
            _size = 0;
        }

        return *this;
    }

    /**
     * String pointer assignment operator when value_type == char.
     */
    template <typename _Ptr,
              typename = typename std::enable_if<std::is_pointer<_Ptr>::value &&
                                                 std::is_same<
                                                     typename std::remove_const<
                                                         typename std::remove_pointer<_Ptr>::type>::type, char>::value, _Ptr>::type,
              typename = _Ptr>
    LLBC_BasicCString &operator =(_Ptr cstr)
    {
        if (LIKELY(cstr))
        {
            _cstr = cstr;
            _size = wcslen(cstr);
        }
        else
        {
            _cstr = L"";
            _size = 0;
        }

        return *this;
    }

    /**
     * std::basic_string<value_type> assignment operator.
     */
    LLBC_BasicCString &operator =(const std::basic_string<value_type> &stlStr)
    {
        _cstr = stlStr.c_str();
        _size = stlStr.size();

        return *this;
    }

    /**
     * Copy assignment operator.
     */
    LLBC_BasicCString &operator =(const LLBC_BasicCString &other)
    {
        memcpy(this, &other, sizeof(other));
        return *this;
    }

public:
    // operator []
    const value_type &operator [](size_type index) const
    {
        #if LLBC_DEBUG
        ASSERT(LIKELY(index < _size) && "index out of range");
        #endif // LLBC_DEBUG

        return _cstr[index];
    }

    // operator +
    LLBC_BasicCString operator +(difference_type offset) const
    {
        #if LLBC_DEBUG
        ASSERT(((offset >= 0 && offset <= static_cast<difference_type>(_size)) ||
                (offset < 0 && std::abs(offset) < *((difference_type *)(&_cstr)))) &&
               "index out of range");
        #endif // LLBC_DEBUG

        return LLBC_BasicCString(_cstr + offset, _size - offset);
    }

    // operator -
    LLBC_BasicCString operator -(difference_type offset) const
    {
        return operator+(-offset);
    }

public:
    // operator <
    template <typename _Other>
    typename std::enable_if<!std::is_same<_Other, LLBC_BasicCString>::value, bool>::type
    operator <(const _Other &other) const
    {
        return *this < LLBC_BasicCString(other);
    }

    template <typename _Other>
    typename std::enable_if<std::is_same<_Other, LLBC_BasicCString>::value, bool>::type
    operator <(const _Other &other) const
    {
        return _cstr != other._cstr &&
                (_size < other._size ||
                 (_size == other._size &&
                  memcmp(_cstr, other._cstr, sizeof(value_type) * _size) < 0));
    }

    // operator ==
    template <typename _Other>
    typename std::enable_if<!std::is_same<_Other, LLBC_BasicCString>::value, bool>::type
    operator ==(const _Other &other) const
    {
        return this->operator==(LLBC_BasicCString(other));
    }

    template <typename _Other>
    typename std::enable_if<std::is_same<_Other, LLBC_BasicCString>::value, bool>::type
    operator ==(const _Other &other) const
    {
        return _cstr == other._cstr ||
            (_size == other._size &&
             memcmp(_cstr, other._cstr, sizeof(value_type) * _size) == 0);
    }

    // operator !=
    template <typename _Other>
    bool operator !=(const _Other &other) const
    {
        return !operator==(other);
    }

    // operator <=
    template <typename _Other>
    typename std::enable_if<!std::is_same<_Other, LLBC_BasicCString>::value, bool>::type
    operator <=(const _Other &other) const
    {
        return !(LLBC_BasicCString(other) < *this);
    }

    template <typename _Other>
    typename std::enable_if<std::is_same<_Other, LLBC_BasicCString>::value, bool>::type
    operator <=(const _Other &other) const
    {
        return !(other < *this);
    }

    // operator >
    template <typename _Other>
    typename std::enable_if<!std::is_same<_Other, LLBC_BasicCString>::value, bool>::type
    operator >(const _Other &other) const
    {
        return LLBC_BasicCString(other) < *this;
    }

    template <typename _Other>
    typename std::enable_if<std::is_same<_Other, LLBC_BasicCString>::value, bool>::type
    operator >(const _Other &other) const
    {
        return other < *this;
    }

    // operator >=
    template <typename _Other>
    typename std::enable_if<!std::is_same<_Other, LLBC_BasicCString>::value, bool>::type
    operator >=(const _Other &other) const
    {
        return !(*this < LLBC_BasicCString(other));
    }

    template <typename _Other>
    typename std::enable_if<std::is_same<_Other, LLBC_BasicCString>::value, bool>::type
    operator >=(const _Other &other) const
    {
        return !(*this < other);
    }

private:
    const_pointer _cstr;
    size_type _size;
};

/**
 * \brief The const string stream output function.
 */
template <typename _Elem>
std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_BasicCString<_Elem> &cstr)
{
    return o <<cstr.c_str();
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
        // Use DJB hash algo.
        size_t h;
        const char *str = cstr.c_str();
        for (size_t i = h = 0; i < cstr.size(); ++i)
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
        // Use DJB hash algo.
        size_t h;
        const wchar_t *str = cstr.c_str();
        for (size_t i = h = 0; i < cstr.size(); ++i)
            h = ((h << 10) + h) ^ str[i];
        return h;
    };
};

}


