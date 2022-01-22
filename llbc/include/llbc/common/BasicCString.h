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
    /**
     * Constructor.
     * @param[in] cstr: the const string.
     */
    LLBC_BasicCString(const _Elem *cstr):_cstr(cstr) {  }

public:
    /**
     * Get const string.
     * @return const _Elem * - the const string.
     */
    const _Elem *GetCStr() const
    {
        return _cstr;
    }

public:
    // operator <
    bool operator <(const _Elem *other) const
    {
        return operator<(LLBC_BasicCString<_Elem>(other));
    }
    bool operator <(const LLBC_BasicCString<_Elem> &other) const
    {
        if (_cstr == other._cstr)
            return false;

        return ::strcmp(_cstr, other._cstr) < 0;
    }
    bool operator <(const std::basic_string<_Elem> &other) const
    {
        return operator<(other.c_str());
    }

    // operator ==
    bool operator ==(const _Elem *other) const
    {
        return operator==(LLBC_BasicCString<_Elem>(other));
    }
    bool operator ==(const LLBC_BasicCString<_Elem> &other) const
    {
        return _cstr == other._cstr || ::strcmp(_cstr, other._cstr) == 0;
    }
    bool operator ==(const std::basic_string<_Elem> &other) const
    {
        return operator==(other.c_str());
    }

    // operator !=
    bool operator !=(const _Elem *other) const
    {
        return operator!=(LLBC_BasicCString<_Elem>(other));
    }
    bool operator !=(const LLBC_BasicCString<_Elem> &other) const
    {
        return !(*this == other);
    }
    bool operator !=(const std::basic_string<_Elem> &other) const
    {
        return operator!=(other.c_str());
    }

    // operator <=
    bool operator <=(const _Elem *other) const
    {
        return operator<=(LLBC_BasicCString<_Elem>(other));
    }
    bool operator <=(const LLBC_BasicCString<_Elem> &other) const
    {
        return !(other < *this);
    }
    bool operator <=(const std::basic_string<_Elem> &other) const
    {
        return operator<=(other.c_str());
    }

    // operator >
    bool operator >(const _Elem *other) const
    {
        return operator>(LLBC_BasicCString<_Elem>(other));
    }
    bool operator >(const LLBC_BasicCString<_Elem> &other) const
    {
        return other < *this;
    }
    bool operator >(const std::basic_string<_Elem> &other) const
    {
        return operator>(other.c_str());
    }

    // operator >=
    bool operator >=(const _Elem *other) const
    {
        return operator>=(LLBC_BasicCString<_Elem>(other));
    }
    bool operator >=(const LLBC_BasicCString<_Elem> &other) const
    {
        return !(other < *this);
    }
    bool operator >=(const std::basic_string<_Elem> &other) const
    {
        return operator>=(other.c_str());
    }

private:
    const char *_cstr;
};

/**
 * \brief The const string stream output function.
 */
template <typename _Elem>
std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_BasicCString<_Elem> &cstr)
{
    if (cstr.GetCStr())
        return o <<cstr.GetCStr();
    else
        return o <<"";
}

__LLBC_NS_END

#endif // !__LLBC_COM_BASIC_CSTRING_H__
