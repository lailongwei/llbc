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

#include "llbc/common/Macro.h"
#include "llbc/common/Errno.h"
#include "llbc/common/Errors.h"
#include "llbc/common/Stream.h"
#include "llbc/common/BasicCString.h"

__LLBC_NS_BEGIN

template <typename _Elem,
          typename _Traits = std::char_traits<_Elem>,
          typename _Ax = std::allocator<_Elem> >
class LLBC_BasicString : public
    std::basic_string<_Elem, _Traits, _Ax>
{
    static_assert(sizeof(_Elem) <= 2, "LLBC_BasicString not support sizeof(_Elem) > 2 element type!");

    typedef LLBC_BasicString<_Elem, _Traits, _Ax> _This;
    typedef std::basic_string<_Elem, _Traits, _Ax> _Base;

    typedef std::vector<LLBC_BasicString<_Elem, _Traits, _Ax> > _These;

public:
    typedef typename _Base::size_type size_type;
    typedef typename _Base::value_type value_type;

    typedef typename _Base::iterator iterator;
    typedef typename _Base::const_iterator const_iterator;

    typedef typename _Base::pointer pointer;
    typedef typename _Base::const_pointer const_pointer;

    typedef _These These;

public:
#if LLBC_TARGET_PLATFORM_NON_WIN32
    static const size_type npos = _Base::npos;
#endif

public:
    // Constructors.
    explicit LLBC_BasicString(const _Ax &al = _Ax()):_Base(al) {  }
    LLBC_BasicString(const _This &rhs):_Base(rhs) {  }
    LLBC_BasicString(_This &&rhs):_Base(std::move(rhs)) {  }
    LLBC_BasicString(const _Base &rhs):_Base(rhs) {  }
    LLBC_BasicString(_Base &&rhs):_Base(std::move(rhs)) {  }
    LLBC_BasicString(const LLBC_BasicCString<_Elem> &rhs):_Base(rhs.c_str(), rhs.size()) {  }
    LLBC_BasicString(const _This &rhs, size_type pos, size_type n):_Base(rhs, pos, n) {  }
    LLBC_BasicString(const _Elem *s, const _Ax &al = _Ax()):_Base(al) { if (s) _Base::append(s); }
    LLBC_BasicString(const _Elem *s, size_type n, const _Ax &al = _Ax()):_Base(s, n, al) {  }
    LLBC_BasicString(size_type n, _Elem c, const _Ax &al = _Ax()):_Base(n, c, al) {  }

    // operator =
    _This &operator =(const _This &rhs)
    {
        _Base::operator =(rhs);
        return *this;
    }

    _This &operator =(_This &&rhs)
    {
        _Base::operator=(std::move(rhs));
        return *this;
    }

    _This &operator =(const _Base &rhs)
    {
        _Base::operator =(rhs);
        return *this;
    }

    _This &operator =(_Base &&rhs)
    {
        _Base::operator=(std::move(rhs));
        return *this;
    }

    _This &operator =(const LLBC_BasicCString<_Elem> &cstr)
    {
        _Base::assign(cstr.c_str(), cstr.size());
        return *this;
    }

    _This &operator =(const _Elem *s)
    {
        if (LIKELY(s))
            _Base::operator =(s);
        else
            this->clear();

        return *this;
    }

    _This &operator =(const _Elem &c)
    {
        _Base::operator =(c);
        return *this;
    }

    // operator +
    _This operator +(const _This &rhs) const
    {
        _This ret = *this;
        ret.append(rhs);
        return ret;
    }

    _This operator +(const _Base &rhs) const
    {
        _This ret = *this;
        ret.append(rhs);
        return ret;
    }

    _This operator +(const LLBC_BasicCString<_Elem> &cstr) const
    {
        _This ret = *this;
        ret.append(cstr);
        return ret;
    }

    _This operator +(const _Elem *s) const
    {
        _This ret = *this;
        ret.append(s);
        return ret;
    }

    _This operator +(const _Elem &c) const
    {
        _This ret = *this;
        ret.append(1, c);
        return ret;
    }

    // operator +=
    _This &operator +=(const _This &rhs)
    {
        return append(rhs);
    }

    _This &operator +=(const _Base &rhs)
    {
        return append(rhs);
    }

    _This &operator +=(const LLBC_BasicCString<_Elem> &cstr)
    {
        return append(cstr.c_str(), cstr.size());
    }

    _This &operator +=(const _Elem *s)
    {
        if (LIKELY(s))
            return append(s);

        return *this;
    }

    _This &operator +=(const _Elem &c)
    {
        return append(1, c);
    }

    // operator *
    _This operator *(int right) const
    {
        if (this->empty() || right == 1)
            return *this;

        _This copy(*this);
        copy *= right;
        return copy;
    }

    _This &operator *=(int right)
    {
        if (this->empty() || right == 1)
            return *this;

        if (right <= 0)
        {
            this->clear();
            return *this;
        }

        const _This unitStr(*this);
        const size_type unitStrSize = this->size();

        this->resize(unitStrSize * right);
        _Elem *buf = const_cast<_Elem *>(this->data());
        for (int i = 1; i < right; ++i)
            memcpy(buf + i * unitStrSize, unitStr.data(), sizeof(_Elem) * unitStrSize);

        return *this;
    }

    // operator ==
    bool operator ==(const _This &str) const
    {
        return this->compare(str) == 0;
    }

    bool operator ==(const _Base &str) const
    {
        return this->compare(str) == 0;
    }

    bool operator ==(const LLBC_BasicCString<_Elem> &cstr) const
    {
        return this->compare(cstr) == 0;
    }

    bool operator ==(const _Elem *s) const
    {
        return this->compare(s) == 0;
    }

    bool operator ==(const _Elem &c) const
    {
        return this->compare(c) == 0;
    }

    // operator !=
    bool operator !=(const _This &str) const
    {
        return this->compare(str) != 0;
    }

    bool operator !=(const _Base &str) const
    {
        return this->compare(str) != 0;
    }

    bool operator !=(const LLBC_BasicCString<_Elem> &cstr) const
    {
        return this->compare(cstr) != 0;
    }

    bool operator !=(const _Elem *s) const
    {
        return this->compare(s) != 0;
    }

    bool operator !=(const _Elem &c) const
    {
        return this->compare(c) != 0;
    }

    // operator <
    bool operator <(const _This &str) const
    {
        return this->compare(str) < 0;
    }

    bool operator <(const _Base &str) const
    {
        return this->compare(str) < 0;
    }

    bool operator <(const LLBC_BasicCString<_Elem> &cstr) const
    {
        return this->compare(cstr) < 0;
    }

    bool operator <(const _Elem *s) const
    {
        return this->compare(s) < 0;
    }

    bool operator <(const _Elem &c) const
    {
        return this->compare(c) < 0;
    }

    // operator <=
    bool operator <=(const _This &str) const
    {
        return this->compare(str) <= 0;
    }

    bool operator <=(const _Base &str) const
    {
        return this->compare(str) <= 0;
    }

    bool operator <=(const LLBC_BasicCString<_Elem> &cstr) const
    {
        return this->compare(cstr) <= 0;
    }

    bool operator <=(const _Elem *s) const
    {
        return this->compare(s) <= 0;
    }

    bool operator <=(const _Elem &c) const
    {
        return this->compare(c) <= 0;
    }

    // operator >
    bool operator >(const _This &str) const
    {
        return this->compare(str) > 0;
    }

    bool operator >(const _Base &str) const
    {
        return this->compare(str) > 0;
    }

    bool operator >(const LLBC_BasicCString<_Elem> &cstr) const
    {
        return this->compare(cstr) > 0;
    }

    bool operator >(const _Elem *s) const
    {
        return this->compare(s) > 0;
    }

    bool operator >(const _Elem &c) const
    {
        return this->compare(c) > 0;
    }

    // operator >=
    bool operator >=(const _This &str) const
    {
        return this->compare(str) >= 0;
    }

    bool operator >=(const _Base &str) const
    {
        return this->compare(str) >= 0;
    }

    bool operator >=(const LLBC_BasicCString<_Elem> &cstr) const
    {
        return this->compare(cstr) >= 0;
    }

    bool operator >=(const _Elem *s) const
    {
        return this->compare(s) >= 0;
    }

    bool operator >=(const _Elem &c) const
    {
        return this->compare(c) >= 0;
    }

    // operator []
    _Elem &operator [](size_type off)
    {
        return (*((_Base *)this))[off];
    }

    const _Elem &operator [](size_type off) const
    {
        return (*((_Base *)this))[off];
    }

    // at operations.
    _Elem &at(size_type off)
    {
        return _Base::at(off);
    }

    const _Elem &at(size_type off) const
    {
        return _Base::at(off);
    }

    // resize
    void resize(size_type count)
    {
        _Base::resize(count);
    }

    // append operations.
    _This &append(const _Base &str)
    {
        _Base::append(str);
        return *this;
    }

    _This &append(const _This &str)
    {
        _Base::append(str);
        return *this;
    }

    _This &append(const LLBC_BasicCString<_Elem> &cstr)
    {
        _Base::append(cstr.c_str(), cstr.size());
        return *this;
    }

    _This &append(const _This &str, size_type pos, size_type n)
    {
        _Base::append(str, pos, n);
        return *this;
    }

    _This &append(const _Elem *s)
    {
        _Base::append(s);
        return *this;
    }

    _This &append(const _Elem *s, size_type n)
    {
        _Base::append(s, n);
        return *this;
    }

    _This &append(size_type n, _Elem c)
    {
        _Base::append(n, c);
        return *this;
    }

    _This &append(iterator first, iterator last)
    {
        _Base::append(first, last);
        return *this;
    }

    _This &append(const_pointer first, const_pointer last)
    {
        _Base::append(first, last);
        return *this;
    }

    _This &append(const_iterator first, const_iterator last)
    {
        _Base::append(first, last);
        return *this;
    }

    // assign operations.
    _This &assign(const _This &str)
    {
        _Base::assign(str);
        return *this;
    }

    _This &assign(const _This &str, size_type pos, size_type n)
    {
        _Base::assign(str, pos, n);
        return *this;
    }

    _This &assign(const _Elem *s)
    {
        _Base::assign(s);
        return *this;
    }

    _This &assign(const _Elem *s, size_type n)
    {
        _Base::assign(s, n);
        return *this;
    }

    _This &assign(const_iterator first, const_iterator last)
    {
        _Base::assign(first, last);
        return *this;
    }

    // insert operations.
    _This &insert(size_type p0, const _This &str)
    {
        _Base::insert(p0, str);
        return *this;
    }

    _This &insert(size_type p0, const _This &str, size_type pos, size_type n)
    {
        _Base::insert(p0, str, pos, n);
        return *this;
    }

    _This &insert(size_type p0, const _Elem *s)
    {
        _Base::insert(p0, s);
        return *this;
    }

    _This &insert(size_type p0, const _Elem *s, size_type n)
    {
        _Base::insert(p0, s, n);
        return *this;
    }

    _This &insert(size_type p0, size_type n, _Elem c)
    {
        _Base::insert(p0, n, c);
        return *this;
    }

    iterator insert(iterator it, _Elem c)
    {
        return _Base::insert(it, c);
    }

    void insert(iterator it, size_type n, _Elem c)
    {
        _Base::insert(it, n, c);
    }

    void insert(iterator it, const_iterator first, const_iterator last)
    {
        _Base::insert(it, first, last);
    }

    // erase operations.
    _This &erase(size_type p0, size_type n = _This::npos)
    {
        _Base::erase(p0, n);
        return *this;
    }

    iterator erase(iterator it)
    {
        return _Base::erase(it);
    }

    iterator erase(iterator first, iterator last)
    {
        return _Base::erase(first, last);
    }

    // replace operations.
    _This &replace(size_type p0, size_type n0, const _This &str)
    {
        _Base::replace(p0, n0, str);
        return *this;
    }

    _This &replace(size_type p0, size_type n0, const _This &str, size_type pos, size_type n)
    {
        _Base::replace(p0, n0, str, pos, n);
        return *this;
    }

    _This &replace(size_type p0, size_type n0, const _Elem *s)
    {
        _Base::replace(p0, n0, s);
        return *this;
    }

    _This &replace(size_type p0, size_type n0, const _Elem *s, size_type n)
    {
        _Base::replace(p0, n0, s, n);
        return *this;
    }

    _This &replace(size_type p0, size_type n0, size_type n, _Elem c)
    {
        _Base::replace(p0, n0, n, c);
        return *this;
    }

    _This &replace(iterator first0, iterator last0, const _This &str)
    {
        _Base::replace(first0, last0, str);
        return *this;
    }

    _This &replace(iterator first0, iterator last0, const _Elem *s)
    {
        _Base::replace(first0, last0, s);
        return *this;
    }

    _This &replace(iterator first0, iterator last0, const _Elem *s, size_type n)
    {
        _Base::replace(first0, last0, s, n);
        return *this;
    }

    _This &replace(iterator first0, iterator last0, size_type n, _Elem c)
    {
        _Base::replace(first0, last0, n, c);
        return *this;
    }

    _This &replace(iterator first0, iterator last0, const_iterator first, const_iterator last)
    {
        _Base::replace(first0, last0, first, last);
        return *this;
    }

    _These split(const _Elem &sep, size_type max_split = -1, bool strip_empty = false) const
    {
        return this->split(_This(1, sep), max_split, false, strip_empty);
    }

    // findreplace operation
    _This &findreplace(const _Elem &c1, const _Elem &c2, int count = -1)
    {
        if (c1 == c2)
            return *this;

        for (size_type i = 0; i < this->size(); ++i)
        {
            if ((*this)[i] == c1)
                replace(i, 1, 1, c2);
        }

        return *this;
    }

    _This &findreplace(const _This &s1, const _This &s2, int count = -1)
    {
        if (s1 == s2)
            return *this;

        size_type found = 0;
        while ((found = find(s1, found)) != _This::npos)
        {
            replace(found, s1.size(), s2);
            found += s2.size();
        }

        return *this;
    }

    // split operation
    _These split(const _This &sep, size_type max_split = -1, bool with_elem = false, bool strip_empty = false) const
    {
        _These substrs;
        if (sep.empty() || max_split == 0 || this->empty())
        {
            if (!strip_empty)
                substrs.push_back(*this);

            return substrs;
        }

        size_type idx = 0;
        size_type splitTimes = 0;
        const size_type step = with_elem ? 1 : sep.size();
        for (; splitTimes < max_split; ++splitTimes)
        {
            size_type findIdx = _This::npos;
            if (with_elem)
            {
                for (size_t i = 0; i < sep.size(); ++i)
                {
                    findIdx = this->find(sep[i], idx);
                    if (findIdx != _This::npos)
                        break;
                }
            }
            else
            {
                findIdx = this->find(sep, idx);
            }

            if (findIdx == _This::npos)
                break;

            if (strip_empty && findIdx == idx)
            {
                if ((idx = findIdx + step) == this->size())
                    break;

                continue;
            }

            substrs.push_back(this->substr(idx, findIdx - idx));
            if ((idx = findIdx + step) == this->size() && !strip_empty)
            {
                substrs.push_back(_This());
                break;
            }
        }

        if (idx != this->size())
            substrs.push_back(this->substr(idx));

        return substrs;
    }

    // swap operations.
    void swap(_This &str)
    {
        _Base::swap(str);
    }

    // assign to raw array(templated).
    template <size_t _ArrLen>
    void assign_to_raw_array(_Elem (&arr)[_ArrLen]) const
    {
        assign_to_raw_array(arr, _ArrLen);
    }

    // assign to raw array.
    void assign_to_raw_array(_Elem *arr, size_t arr_len) const
    {
        if (UNLIKELY(!arr || arr_len == 0))
            return;

        if (this->empty())
        {
            arr[0] = _Elem();
        }
        else if (this->length() < arr_len)
        {
            memcpy(arr, this->data(), sizeof(_Elem) * this->length());
            arr[this->length()] = _Elem();
        }
        else
        {
            memcpy(arr, this->data(), sizeof(_Elem) * (arr_len - 1));
            arr[arr_len - 1] = _Elem();
        }
    }

    // find operations.
    size_type find(const _This &str, size_type pos = 0) const
    {
        return _Base::find(str, pos);
    }

    size_type find(const _Elem *s, size_type pos, size_type n) const
    {
        return _Base::find(s, pos, n);
    }

    size_type find(const _Elem *s, size_type pos = 0) const
    {
        return _Base::find(s, pos);
    }

    size_type find(_Elem c, size_type pos = 0) const
    {
        return _Base::find(c, pos);
    }

    // Reverse find operations.
    size_type rfind(const _This &str, size_type pos = _This::npos) const
    {
        return _Base::rfind(str, pos);
    }

    size_type rfind(const _Elem *s, size_type pos, size_type n) const
    {
        return _Base::rfind(s, pos, n);
    }

    size_type rfind(const _Elem *s, size_type pos = _This::npos) const
    {
        return _Base::rfind(s, pos);
    }

    size_type rfind(_Elem c, size_type pos = _This::npos) const
    {
        return _Base::rfind(c, pos);
    }

    // find first of operations.
    size_type find_first_of(const _This &str, size_type pos = 0) const
    {
        return _Base::find_first_of(str, pos);
    }

    size_type find_first_of(const _Elem *s, size_type pos, size_type n) const
    {
        return _Base::find_first_of(s, pos, n);
    }

    size_type find_first_of(const _Elem *s, size_type pos = 0) const
    {
        return _Base::find_first_of(s, pos);
    }

    size_type find_first_of(_Elem c, size_type pos = 0) const
    {
        return _Base::find_first_of(c, pos);
    }

    // find last of operations.
    size_type find_last_of(const _This &str, size_type pos = _This::npos) const
    {
        return _Base::find_last_of(str, pos);
    }

    size_type find_last_of(const _Elem *s, size_type pos, size_type n) const
    {
        return _Base::find_last_of(s, pos, n);
    }

    size_type find_last_of(const _Elem *s, size_type pos = _This::npos) const
    {
        return _Base::find_last_of(s, pos);
    }

    size_type find_last_of(_Elem c, size_type pos = _This::npos) const
    {
        return _Base::find_last_of(c, pos);
    }

    size_type find_first_not_of(const _This &str, size_type pos = 0) const
    {
        return _Base::find_first_not_of(str, pos);
    }

    size_type find_first_not_of(const _Elem *s, size_type pos, size_type n) const
    {
        return _Base::find_first_not_of(s, pos, n);
    }

    size_type find_first_not_of(const _Elem *s, size_type pos = 0) const
    {
        return _Base::find_first_not_of(s, pos);
    }

    size_type find_first_not_of(_Elem c, size_type pos = 0) const
    {
        return _Base::find_first_not_of(c, pos);
    }

    size_type find_last_not_of(const _This &str, size_type pos = _This::npos) const
    {
        return _Base::find_last_not_of(str, pos);
    }

    size_type find_last_not_of(const _Elem *s, size_type pos, size_type n) const
    {
        return _Base::find_last_not_of(s, pos, n);
    }

    size_type find_last_not_of(const _Elem *s, size_type pos = _This::npos) const
    {
        return _Base::find_last_not_of(s, pos);
    }

    size_type find_last_not_of(_Elem c, size_type pos = _This::npos) const
    {
        return _Base::find_last_not_of(c, pos);
    }

    // substr operations.
    _This substr(size_type pos = 0, size_type n = _This::npos) const
    {
        return _Base::substr(pos, n);
    }

    // tolower/toupper operations.
    _This tolower() const
    {
        const _Elem *buf = this->data();
        const size_type size = this->size();

        _This lower;
        lower.resize(size);
        for (size_type i = 0; i < size; ++i)
        {
            if (buf[i] >= 0x41 && buf[i] <= 0x5A)
                lower[i] = buf[i] + 0x20;
            else
                lower[i] = buf[i];
        }

        return lower;
    }

    _This toupper() const
    {
        const _Elem *buf = this->data();
        const size_type size = this->size();

        _This upper;
        upper.resize(size);
        for (size_type i = 0; i < size; ++i)
            if (buf[i] >= 0x61 && buf[i] <= 0x7a)
                upper[i] = buf[i] - 0x20;
            else
                upper[i] = buf[i];

        return upper;
    }

    // compare operations.
    int compare(const _This &str) const
    {
        return _Base::compare(str);
    }

    int compare(const LLBC_BasicCString<_Elem> &cstr) const
    {
        return _Base::compare(0, cstr.size(), cstr.c_str());
    }

    int compare(size_type p0, size_type n0, const _This &str)
    {
        return _Base::compare(p0, n0, str);
    }

    int compare(size_type p0, size_type n0, const _This &str, size_type pos, size_type n) const
    {
        return _Base::compare(p0, n0, str, pos, n);
    }

    int compare(const _Elem *s) const
    {
        return _Base::compare(s);
    }

    int compare(size_type p0, size_type n0, const _Elem *s) const
    {
        return _Base::compare(p0, n0, s);
    }

    int compare(size_type p0, size_type n0, const _Elem *s, size_type pos) const
    {
        return _Base::compare(p0, n0, s, pos);
    }

    int compare(const _Elem &c) const
    {
        size_type size = this->size();
        if (size == 0)
            return -1;

        _Elem thisC = this->at(0);
        return (thisC > c ? 1 :
                    (thisC < c ? -1 :
                        (size == 1 ? 0 : 1)));
    }

    // isalpha/isupper/islower
    static bool isalpha(const _Elem &c)
    {
        if (sizeof(_Elem) == 1)
        {
            return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
        }
        else if (sizeof(_Elem) == 2)
        {
            char ch = static_cast<char>(c);
            return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z');
        }
        else
        {
            return false;
        }
    }

    static bool isalpha(const _This &s)
    {
        if (s.empty())
            return false;

        for (size_type i = 0; i < s.size(); ++i)
        {
            if (!isalpha(s[i]))
                return false;
        }

        return true;
    }

    bool isalpha() const
    {
        return isalpha(*this);
    }

    static bool islower(const _Elem &c)
    {
        if (sizeof(_Elem) == 1)
        {
            return 'a' <= c && c <= 'z';
        }
        else if (sizeof(_Elem) == 2)
        {
            char ch = static_cast<char>(c);
            return 'a' <= ch && ch <= 'z';
        }
        else
        {
            return false;
        }
    }

    static bool islower(const _This &s)
    {
        if (s.empty())
            return false;

        bool foundLower = false;
        for (size_type i = 0; i < s.size(); ++i)
        {
            if (isupper(s[i]))
                return false;
            else if (islower(s[i]))
                foundLower = true;
        }

        return foundLower;
    }

    bool islower() const
    {
        return islower(*this);
    }

    static bool isupper(const _Elem &c)
    {
        if (sizeof(_Elem) == 1)
        {
            return 'A' <= c && c <= 'Z';
        }
        else if (sizeof(_Elem) == 2)
        {
            char ch = static_cast<char>(c);
            return 'A' <= ch && ch <= 'Z';
        }
        else
        {
            return false;
        }
    }

    static bool isupper(const _This &s)
    {
        if (s.empty())
            return false;

        bool foundUpper = false;
        for (size_type i = 0; i < s.size(); ++i)
        {
            if (islower(s[i]))
                return false;
            else if (isupper(s[i]))
                foundUpper = true;
        }

        return foundUpper;
    }

    bool isupper() const
    {
        return isupper(*this);
    }

    // isdigit
    static bool isdigit(const _Elem &c)
    {
        if (sizeof(_Elem) == 1)
        {
            return '0' <=c && c <= '9';
        }
        else if (sizeof(_Elem) == 2)
        {
            char ch = reinterpret_cast<char>(c);
            return '0' <= ch && ch <= '9';
        }
        else
        {
            return false;
        }
    }

    static bool isdigit(const _This &s)
    {
        if (s.empty())
            return false;

        for (size_type i = 0; i < s.size(); ++i)
        {
            if (!isdigit(s[i]))
                return false;
        }

        return true;
    }

    bool isdigit() const
    {
        return isdigit(*this);
    }

    // isspace: space[' '],carriage return['\r'],line feed['\n'],form feed['\f'],horizontal tab['\t'],vertical tab['\v']
    static bool isspace(const _Elem &c)
    {
        if (sizeof(_Elem) == 1)
        {
            return  c == ' ' || c == '\t' || c == '\v' || c == '\r' || c == '\n' || c == '\f';
        }
        else if (sizeof(_Elem) == 2)
        {
            char ch = reinterpret_cast<char>(c);
            return  ch == ' ' || ch == '\t' || ch == '\v' || ch == '\r' || ch == '\n' || ch == '\f';
        }
        else
        {
            return false;
        }
    }

    static bool isspace(const _This &s)
    {
        if (s.empty())
            return false;

        for (size_type i = 0; i < s.size(); ++i)
        {
            if (!isspace((*s)[i]))
                return false;
        }

        return false;
    }

    bool isspace() const
    {
        return isspace(*this);
    }

    // startswith/endswith
    bool startswith(const _This &s) const
    {
        if (s.empty())
            return true;

        return (this->size() >= s.size() && memcmp(s.data(), this->data(), s.size() * sizeof(_Elem)) == 0);
    }

    bool endswith(const _This &s) const
    {
        if (s.empty())
            return true;

        return (this->size() >= s.size() &&
            memcmp(s.data(), this->data() + (this->size() - s.size()) * sizeof(_Elem), s.size() * sizeof(_Elem)) == 0);
    }

public:
    friend std::basic_ostream<_Elem> &operator <<(std::basic_ostream<_Elem> &o, const _This &str)
    {
        o.write(str.data(), str.size());
        return o;
    }

public:
    // format operation: format
    _This &format(const _Elem *fmt, ...)
    {
        if (UNLIKELY(sizeof(_Elem) != sizeof(char)))
            return *this;

        // if fmt args is null, clear and return.
        if (!fmt)
        {
            this->clear();
            return *this;
        }

        // if string obj is empty, try detach format require buffers and resize it.
        va_list ap;
        if (this->empty())
        {
            va_start(ap, fmt);
            int len = vsnprintf(nullptr, 0, fmt, ap);
            va_end(ap);

            if (len <= 0)
                return *this;

            this->resize(len);
        }

        // try format.
        va_start(ap, fmt);
        int len = vsnprintf(const_cast<char *>(this->data()),
                            this->size() + 1,
                            fmt,
                            ap);
        va_end(ap);
        if (len <= static_cast<int>(this->size()))
        {
            if (len < 0)
                this->clear();
            else if (len < static_cast<int>(this->size()))
                this->resize(len);

            return *this;
        }

        // resize, try format again.
        this->resize(len);
        va_start(ap, fmt);
        len = vsnprintf(const_cast<char *>(this->data()),
                        this->size() + 1,
                        fmt,
                        ap);
        va_end(ap);
        if (len != static_cast<int>(this->size()))
            this->clear();

        return *this;
    }

    // format operation: append format
    _This &append_format(const _Elem *fmt, ...)
    {
        if (UNLIKELY(sizeof(_Elem) != sizeof(char)))
            return *this;

        // if fmt args is null, return.
        if (!fmt)
            return *this;

        // try detach detach format require buffers and resize it.
        va_list ap;
        const size_type oldSize = this->size();
        va_start(ap, fmt);
        int len =::vsnprintf(nullptr, 0, fmt, ap);
        va_end(ap);
        if (len <= 0)
            return *this;

        // exec format.
        this->resize(oldSize + len);
        va_start(ap, fmt);
        len = vsnprintf(const_cast<char *>(this->data() + oldSize),
                        len + 1,
                        fmt,
                        ap);
        va_end(ap);
        if (oldSize + len != this->size())
            this->resize(oldSize);

        return *this;
    }

public:
    // strip operation: strip left.
    _This &lstrip(const _This &chars = _This())
    {
        _This willStripChars;
        if (chars.empty())
        {
            if (sizeof(_Elem) == 1)
                willStripChars.append(reinterpret_cast<const _Elem *>(" \t\v\r\n\f"));
            else if (sizeof(_Elem) == 2)
                willStripChars.append(reinterpret_cast<const _Elem *>(L" \t\v\r\n\f"));
            else
                return *this;
        }
        else
        {
            willStripChars = chars;
        }

        _This &thisRef = *this;
        size_type stripTo = 0;
        for (size_type i = 0; i != thisRef.size(); ++i)
        {
            bool found = false;
            const _Elem &now = thisRef[i];
            for (size_type j = 0; j != willStripChars.size(); ++j)
            {
                if (now == willStripChars[j])
                {
                    found = true;
                    break;
                }
            }

            if (found)
                stripTo = i + 1;
            else
                break;
        }

        if (stripTo != 0)
            this->erase(0, stripTo);

        return *this;
    }

    _This lstrip(const _This &chars = _This()) const
    {
        _This copyThis(*this);
        return copyThis.lstrip(chars);
    }

    // strip operation: strip right.
    _This &rstrip(const _This &chars = _This())
    {
        _This willStripChars;
        if (chars.empty())
        {
            if (sizeof(_Elem) == 1)
                willStripChars.append(reinterpret_cast<const _Elem *>(" \t\v\r\n\f"));
            else if (sizeof(_Elem) == 2)
                willStripChars.append(reinterpret_cast<const _Elem *>(L" \t\v\r\n\f"));
            else
                return *this;
        }
        else
        {
            willStripChars = chars;
        }

        _This &thisRef = *this;
        const long thisSize = static_cast<long>(thisRef.size());

        long stripFrom = thisSize;
        for (long i = thisSize - 1; i >= 0; --i)
        {
            bool found = false;
            const _Elem &now = thisRef[i];
            for (size_type j = 0; j != willStripChars.size(); ++j)
            {
                if (now == willStripChars[j])
                {
                    found = true;
                    break;
                }
            }

            if (found)
                stripFrom = i;
            else
                break;
        }

        if (stripFrom != thisSize)
            thisRef.erase(stripFrom);

        return thisRef;
    }

    _This rstrip(const _This &chars = _This()) const
    {
        _This copyThis(*this);
        return copyThis.rstrip(chars);
    }

    // strip operation: trip left & trip right
    _This &strip(const _This &chars = _This())
    {
        return this->lstrip(chars).rstrip(chars);
    }

    _This strip(const _This &chars = _This()) const
    {
        _This copyThis(*this);
        return copyThis.lstrip(chars).rstrip(chars);
    }

public:
    // escape support: escape string
    _This &escape(const _This &willbeEscapeChars, const _Elem &escapeChar)
    {
        if (this->empty())
            return *this;

        const long len = static_cast<long>(this->size());
        for (long i = len - 1; i >= 0; --i)
        {
            const _Elem &ch = (*this)[i];
            if (ch == escapeChar ||
                willbeEscapeChars.find(ch) != _This::npos)
                this->insert(i, 1, escapeChar);
        }

        return *this;
    }

    _This escape(const _This &willbeEscapeChars, const _Elem &escapeChar) const
    {
        if (this->empty())
            return *this;

        return _This(*this).escape(willbeEscapeChars, escapeChar);
    }

    // escape support: unescape string
    _This &unescape(const _Elem &escapeChar)
    {
        if (this->empty())
            return *this;

        const long len = static_cast<long>(this->size());
        for (long i = len - 1; i >= 0; --i)
        {
            const _Elem &ch = (*this)[i];
            if (ch == escapeChar)
            {
                if (i > 0 && (*this)[i - 1] == escapeChar)
                    this->erase(i--, 1);
                else
                    this->erase(i, 1);
            }
        }

        return *this;
	}

    _This unescape(const _Elem &escapeChar) const
    {
        return _This(*this).unescape(escapeChar);
    }

public:
    // utf8 support: calculate utf8 length.
    size_type length_with_utf8() const
    {
        if (UNLIKELY(sizeof(_Elem) != sizeof(char)))
            return 0;

        size_type count = 0;
        size_type bytePos = 0;
        while ((bytePos = _This::next_utf8_char_pos(bytePos)) != _This::npos)
            count++;

        return count;
    }

    // utf8 support: substring with utf8.
    _This substr_with_utf8(size_type pos = 0, size_type n = _This::npos) const
    {
        if (UNLIKELY(sizeof(_Elem) != sizeof(char)))
            return _This();

        size_type utf8Len = this->length_with_utf8();
        if (pos >= utf8Len || n == 0)
            return _This();

        _These substrs;
        this->split_utf8_string(static_cast<long>(pos), substrs);
        if (substrs.empty())
            return _This();

        _This str1 = *substrs.rbegin();
        utf8Len = str1.length_with_utf8();
        pos = (n == _This::npos || n > utf8Len) ? utf8Len : n;

        substrs.clear();
        str1.split_utf8_string(static_cast<long>(pos), substrs);
        if (substrs.empty())
            return _This();

        return substrs[0];
    }

    // utf8 support: split utf8 string.
    void split_utf8_string(long charIndex, _These &strs) const
    {
        strs.clear();
        if (UNLIKELY(sizeof(_Elem) != sizeof(char)))
        {
            strs.push_back(*this);
            return;
        }
        else if (charIndex == 0)
        {
            strs.push_back(*this);
            return;
        }

        size_type utf8Count = _This::length_with_utf8();
        if (UNLIKELY(utf8Count == _This::npos))
        {
            strs.push_back(*this);
            return;
        }

        charIndex = (charIndex < 0) ?
            static_cast<long>(utf8Count) + charIndex : charIndex;
        if (charIndex <= 0 || charIndex >= static_cast<long>(utf8Count))
        {
            strs.push_back(*this);
            return;
        }

        size_type bytePos = 0;
        size_type charPos = 0;
        while (static_cast<long>(charPos) != charIndex)
        {
            bytePos = _This::next_utf8_char_pos(bytePos);
            charPos++;
        }

        strs.push_back(_This::substr(0, bytePos));
        strs.push_back(_This::substr(bytePos));
    }

    // utf8 support: scatter utf8 string.
    void scatter_utf8_string(_These &chars, size_type scatterCount = 0) const
    {
        chars.clear();

        if (scatterCount == 0)
            scatterCount = _This::npos;
        else if (scatterCount != _This::npos)
            scatterCount -= 1;

        if (scatterCount == 0)
        {
            chars.push_back(*this);
            return;
        }

        size_type curPos = 0;
        size_type prevPos = 0;
        size_type curScatterCount = 0;
        while ((curPos = this->next_utf8_char_pos(prevPos)) != _This::npos)
        {
            chars.push_back(_This::substr(prevPos, curPos - prevPos));

            if (scatterCount != _This::npos && ++curScatterCount >= scatterCount)
            {
                chars.push_back(_This::substr(curPos));
                break;
            }

            prevPos = curPos;
        }
    }

    // utf8 support: check string has utf8 bomb.
    bool has_utf8_bomb() const
    {
        if (sizeof(_Elem) != sizeof(char) || _This::size() < 3)
            return false;

        return (memcmp(reinterpret_cast<const char *>(this->data()),
            reinterpret_cast<const char *>("\xef\xbb\xbf"), 3) == 0) ? true : false;

    }

    // utf8 support: add utf8 bomb.
    void add_utf8_bomb()
    {
        if (sizeof(_Elem) == sizeof(char) && !this->has_utf8_bomb())
            this->insert(0, reinterpret_cast<const _Elem *>("\xef\xbb\xbf"));
    }

    // utf8 support: trim utf8 bomb.
    void trim_utf8_bomb()
    {
        if (this->has_utf8_bomb())
            _Base::erase(0, 3);
    }

public:
    void serialize(LLBC_Stream &stream) const
    {
        stream.Write(static_cast<uint32>(this->size()));
        stream.Write(_Base::data(), _Base::size() * sizeof(_Elem));
    }

    bool deserialize(LLBC_Stream &stream)
    {
        uint32 len;
        if (!stream.Read(len))
            return false;

        _Base::clear();
        if (len == 0)
            return true;

        this->resize(len);
        return stream.Read(const_cast<char *>(this->data()), sizeof(_Elem) * this->size());
    }

private:
    size_type next_utf8_char_pos(size_type &beginBytePos) const
    {
        if (sizeof(_Elem) != sizeof(char))
            return _This::npos;

        if (beginBytePos == 0 && this->has_utf8_bomb())
            beginBytePos += 3;

        if (beginBytePos == _This::npos || beginBytePos >= this->size())
            return _This::npos;

        size_type waitCheckCount = _This::npos;

        // 0xxx xxxx
        // Encoding len: 1 byte.
        uint8 ch = static_cast<uint8>(_Base::at(beginBytePos));
        if ((ch & 0x80) == 0x00)
            waitCheckCount = 0;
        // 110x xxxx
        // Encoding len: 2 bytes.
        else if ((ch & 0xe0) == 0xc0)
            waitCheckCount = 1;
        // 1110 xxxx
        // Encoding len: 3 bytes.
        else if ((ch & 0xf0) == 0xe0)
            waitCheckCount = 2;
        // 1111 0xxx
        // Encoding len: 4 bytes.
        else if ((ch & 0xf8) == 0xf0)
            waitCheckCount = 3;
        // 1111 10xx
        // Encoding len: 5 bytes.
        else if ((ch & 0xfc) == 0xf8)
            waitCheckCount = 4;
        // 1111 110x
        // Encoding len: 6 bytes.
        else if ((ch & 0xfe) == 0xfc)
            waitCheckCount = 5;

        if (waitCheckCount == _This::npos)
            return _This::npos;

        size_type curPos = beginBytePos + 1;
        size_type endPos = curPos + waitCheckCount;
        if (endPos > _This::size())
            return _This::npos;

        for (; curPos != endPos; ++curPos)
        {
            ch = static_cast<uint8>(_Base::at(curPos));
            if ((ch & 0xc0) != 0x80)
                return _This::npos;
        }

        return endPos;
    }
};

__LLBC_NS_END
