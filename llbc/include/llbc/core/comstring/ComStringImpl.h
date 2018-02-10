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

#ifdef __LLBC_CORE_COMSTRING_COMSTRING_H__ 

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(disable:4996)
#endif

__LLBC_NS_BEGIN

template <uint32 _Lmt>
LLBC_ComString<_Lmt>::LLBC_ComString()
{
    Clear();
}

template <uint32 _Lmt>
LLBC_ComString<_Lmt>::LLBC_ComString(const char *str)
{
    Clear();

    _length = MIN(strlen(str), _Lmt - 1);

    memcpy(_buff, str, _length);
    _buff[_length] = '\0';
}

template <uint32 _Lmt>
LLBC_ComString<_Lmt>::LLBC_ComString(const LLBC_ComString &str)
{
    Clear();

    _length = MIN(str.GetLength(), _Lmt);
    memcpy(_buff, str.GetCStr(), _length);
    _buff[_length] = '\0';
}

template <uint32 _Lmt>
LLBC_ComString<_Lmt>::LLBC_ComString(const LLBC_String &str)
{
    Clear();

    _length = MIN(str.length(), _Lmt - 1);

    memcpy(_buff, str.c_str(), _length);
    _buff[_length] = '\0';
}

template <uint32 _Lmt>
inline const char *LLBC_ComString<_Lmt>::data() const
{
    return _buff;
}

template <uint32 _Lmt>
inline uint32 LLBC_ComString<_Lmt>::size() const
{
    return _length;
}

template <uint32 _Lmt>
inline const char *LLBC_ComString<_Lmt>::c_str() const
{
    return _buff;
}

template <uint32 _Lmt>
inline const char *LLBC_ComString<_Lmt>::GetCStr() const
{
    return _buff;
}

template <uint32 _Lmt>
inline LLBC_String LLBC_ComString<_Lmt>::GetStr() const
{
    return _buff;
}

template <uint32 _Lmt>
inline uint32 LLBC_ComString<_Lmt>::length() const
{
    return _length;
}

template <uint32 _Lmt>
inline uint32 LLBC_ComString<_Lmt>::GetLength() const
{
    return _length;
}

template <uint32 _Lmt>
inline void LLBC_ComString<_Lmt>::Clear()
{
    _length = 0;
    _buff[_length] = '\0';
}

template <uint32 _Lmt>
inline uint32  LLBC_ComString<_Lmt>::GetUsableSize() const
{
    return _Lmt - _length - 1;
}

template <uint32 _Lmt>
inline bool LLBC_ComString<_Lmt>::IsFull() const
{
    return UNLIKELY(_length == _Lmt - 1);
}

template <uint32 _Lmt>
inline bool LLBC_ComString<_Lmt>::IsEmpty() const
{
    return UNLIKELY(_length == 0);
}

template <uint32 _Lmt>
void LLBC_ComString<_Lmt>::Format(const char *fmt, ...)
{
    this->Clear();

    va_list ap;

    int ret = 0;
    int bufSize = 0x100;
    char *buf = reinterpret_cast<char *>(::malloc(bufSize));
#if LLBC_TARGET_PLATFORM_WIN32
    while (true)
    {
        va_start(ap, fmt);
        ret = ::vsnprintf_s(buf, bufSize, _TRUNCATE, fmt, ap);
        va_end(ap);

        if (ret >= 0)
            break;

        bufSize <<= 1;
        buf = reinterpret_cast<char *>(::realloc(buf, bufSize));
    }
#else // LLBC_TARGET_PLATFORM_NON_WIN32
    while (true)
    {
        va_start(ap, fmt);
        ret = ::vsnprintf(buf, bufSize, fmt, ap);
        va_end(ap);

        // Worked, break.
        if (ret > -1 && ret < bufSize)
            break;

        // Try again with more space.
        if (ret > -1) // glibc 2.1 and later.
            bufSize = ret + 1;
        else         // glibc 2.0
            bufSize <<= 1;

        buf = reinterpret_cast<char *>(::realloc(buf, bufSize));
    }
#endif // LLBC_TARGET_PLATFORM_WIN32

    this->Append(buf, ret);
    ::free(buf);
}

template <uint32 _Lmt>
LLBC_ComString<_Lmt> &LLBC_ComString<_Lmt>::AppendFormat(const char *fmt, ...)
{
    this->Clear();

    va_list ap;

    int ret = 0;
    int bufSize = 0x100;
    char *buf = reinterpret_cast<char *>(::malloc(bufSize));
#if LLBC_TARGET_PLATFORM_WIN32
    while (true)
    {
        va_start(ap, fmt);
        ret = ::vsnprintf_s(buf, bufSize, _TRUNCATE, fmt, ap);
        va_end(ap);

        if (ret >= 0)
            break;

        bufSize <<= 1;
        buf = reinterpret_cast<char *>(::realloc(buf, bufSize));
    }
#else // LLBC_TARGET_PLATFORM_NON_WIN32
    while (true)
    {
        va_start(ap, fmt);
        ret = ::vsnprintf(buf, bufSize, fmt, ap);
        va_end(ap);

        // Worked, break.
        if (ret > -1 && ret < bufSize)
            break;

        // Try again with more space.
        if (ret > -1) // glibc 2.1 and later.
            bufSize = ret + 1;
        else          // glibc 2.0
            bufSize <<= 1;

        buf = reinterpret_cast<char *>(::realloc(buf, bufSize));
    }
#endif // LLBC_TARGET_PLATFORM_WIN32

    this->Append(buf, ret);
    ::free(buf);

    return *this;
}

template <uint32 _Lmt>
inline LLBC_ComString<_Lmt> &LLBC_ComString<_Lmt>::Append(char ch)
{
    if (UNLIKELY(ch == '\0'))
        return *this;

    if (!this->IsFull())
    {
        _buff[_length++] = ch;
        _buff[_length] = '\0';
    }

    return *this;
}

template <uint32 _Lmt>
inline LLBC_ComString<_Lmt> &LLBC_ComString<_Lmt>::Append(const char *str)
{
    return this->Append(str, strlen(str));
}

template <uint32 _Lmt>
inline LLBC_ComString<_Lmt> &LLBC_ComString<_Lmt>::Append(const char *str, uint32 len)
{
    if ((len = MIN(len, this->GetUsableSize())) == 0)
    {
        return *this;
    }

    memcpy(_buff + _length, str, len);
    _length += len;
    _buff[_length] = '\0';

    return *this;
}

template <uint32 _Lmt>
LLBC_ComString<_Lmt> &LLBC_ComString<_Lmt>::Append(const LLBC_ComString &str)
{
    if (!this->IsFull())
    {
        uint32 length = MIN(_length + str.GetLength(), _Lmt - 1);
        memcpy(_buff + _length, str.GetCStr(), length - _length);

        _length = length;
        _buff[_length] = '\0';
    }

    return *this;
}

template <uint32 _Lmt>
inline LLBC_ComString<_Lmt> &LLBC_ComString<_Lmt>::Append(const LLBC_String &str)
{
    if (!this->IsFull())
    {
        uint32 length = MIN(_length + str.length(), _Lmt - 1);
        memcpy(_buff + _length, str.c_str(), length - _length);

        _length = length;
        _buff[_length] = '\0';
    }

    return *this;
}

template <uint32 _Lmt>
LLBC_ComString<_Lmt>::operator LLBC_String () const
{
    return LLBC_String(_buff);
}

template <uint32 _Lmt>
LLBC_ComString<_Lmt> &LLBC_ComString<_Lmt>::operator = (char ch)
{
    Clear();
    if (LIKELY(ch != '\0'))
    {
        _buff[_length++] = ch;
        _buff[_length] = '\0';
    }

    return *this;
}

template <uint32 _Lmt>
LLBC_ComString<_Lmt> &LLBC_ComString<_Lmt>::operator = (const char *str)
{
    Clear();
    return this->Append(str);
}

template <uint32 _Lmt>
LLBC_ComString<_Lmt> &LLBC_ComString<_Lmt>::operator = (const LLBC_ComString &str)
{
    Clear();
    return this->Append(str);
}

template <uint32 _Lmt>
LLBC_ComString<_Lmt> &LLBC_ComString<_Lmt>::operator = (const LLBC_String &str)
{
    Clear();
    return this->Append(str);
}

template <uint32 _Lmt>
LLBC_ComString<_Lmt> &LLBC_ComString<_Lmt>::operator += (char ch)
{
    return this->Append(ch);
}

template <uint32 _Lmt>
LLBC_ComString<_Lmt> &LLBC_ComString<_Lmt>::operator += (const char *str)
{
    return this->Append(str);
}

template <uint32 _Lmt>
LLBC_ComString<_Lmt> &LLBC_ComString<_Lmt>::operator += (const LLBC_ComString &str)
{
    return this->Append(str);
}

template <uint32 _Lmt>
LLBC_ComString<_Lmt> &LLBC_ComString<_Lmt>::operator += (const LLBC_String &str)
{
    return this->Append(str);
}

template <uint32 _Lmt>
bool LLBC_ComString<_Lmt>::operator == (const char *str) const
{
    uint32 length = strlen(str);
    if (length != _length)
        return false;

    return memcmp(_buff, str, _length) == 0;
}

template <uint32 _Lmt>
bool LLBC_ComString<_Lmt>::operator == (const LLBC_ComString &str) const
{
    if (this->_length != str._length)
        return false;
    else if (this->_length == 0)
        return true;

    return memcmp(_buff, str.GetCStr(), _length) == 0;
}

template <uint32 _Lmt>
bool LLBC_ComString<_Lmt>::operator == (const LLBC_String &str) const
{
    uint32 length = str.length();
    if (length != _length)
        return false;

    return memcmp(_buff, str.c_str(), _length) == 0;
}

template <uint32 _Lmt>
bool LLBC_ComString<_Lmt>::operator != (const char *str) const
{
    return !(*this == str);
}

template <uint32 _Lmt>
bool LLBC_ComString<_Lmt>::operator != (const LLBC_ComString &str) const
{
    return !(*this == str);
}

template <uint32 _Lmt>
bool LLBC_ComString<_Lmt>::operator != (const LLBC_String &str) const
{
    return !(*this == str);
}

template <uint32 _Lmt>
bool LLBC_ComString<_Lmt>::operator < (const LLBC_ComString &str) const
{
    if (_length == str.GetLength())
        return ::memcmp(_buff, str.GetCStr(), _length) < 0;

    return _length < str.GetLength();
}

template <uint32 _Lmt>
void LLBC_ComString<_Lmt>::Serialize(LLBC_Stream &stream) const
{
    stream.WriteBuffer(_buff, _length + 1);
}

template <uint32 _Lmt>
bool LLBC_ComString<_Lmt>::DeSerialize(LLBC_Stream &stream)
{
    Clear();

    char ch = '\0';
    if (!stream.Read(ch))
        return false;

    while (ch)
    {
        this->Append(ch);
        
        if (!stream.Read(ch))
            return false;
    }

    return true;
}

template <uint32 _Lmt>
void LLBC_ComString<_Lmt>::SerializeEx(LLBC_Stream &stream) const
{
    this->Serialize(stream);
}

template <uint32 _Lmt>
bool LLBC_ComString<_Lmt>::DeSerializeEx(LLBC_Stream &stream)
{
    return this->DeSerialize(stream);
}

__LLBC_NS_END

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(default:4996)
#endif

#endif // __LLBC_CORE_COMSTRING_COMSTRING_H__
