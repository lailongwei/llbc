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

#include "llbc/common/Endian.h"
#include "llbc/common/TemplateDeduction.h"

__LLBC_NS_BEGIN

/**
 * \brief Stop std::array read.
 */
template <>
struct LLBC_Stream::__LLBC_ArrayReader<0>
{
    template <typename Arr>
    static bool Read(Arr &arr, LLBC_NS LLBC_Stream &stream)
    {
        return true;
    }
};

/**
 * \brief Stop std::array read.
 */
template <>
struct LLBC_Stream::__LLBC_ArrayWriter<0>
{
    template <typename Arr>
    static void Write(const Arr &arr, LLBC_NS LLBC_Stream &stream)
    {
    }
};

/**
 * \brief Stop std::tuple read.
 */
template <>
struct LLBC_Stream::__LLBC_TupleReader<0>
{
    template <typename Tup>
    static bool Read(Tup &tup, LLBC_Stream &stream, size_t &readCount)
    {
        return true;
    }
};

/**
 * \brief Stop std::tuple write.
 */
template <>
struct LLBC_Stream::__LLBC_TupleWriter<0>
{
    template <typename Tup>
    static void Write(const Tup &tup, LLBC_Stream &stream)
    {
    }
};

inline LLBC_Stream::LLBC_Stream()
: _buf(nullptr)
, _pos(0)
, _cap(0)

, _endian(LLBC_DefaultEndian)
, _attach(false)

, _poolInst(nullptr)
{
}

inline LLBC_Stream::LLBC_Stream(size_t cap)
: _buf(cap > 0 ? LLBC_Malloc(sint8, cap) : nullptr)
, _pos(0)
, _cap(cap)

, _endian(LLBC_DefaultEndian)
, _attach(false)

, _poolInst(nullptr)
{
}

inline LLBC_Stream::LLBC_Stream(LLBC_Stream &&rhs)
: _buf(rhs._buf)
, _pos(rhs._pos)
, _cap(rhs._cap)

, _endian(rhs._endian)
, _attach(rhs._attach)

// !!! rhs._poolInst not allow move.
, _poolInst(nullptr)
{
    rhs._buf = nullptr;
    rhs._pos = 0;
    rhs._cap = 0;

    rhs._endian = LLBC_DefaultEndian;
    rhs._attach = false;

    // !!! rhs._poolInst not allow move.
    // rhs._poolInst = nullptr;
}

inline LLBC_Stream::LLBC_Stream(const LLBC_Stream &rhs, bool attach)
: _buf(nullptr)
, _pos(0)
, _cap(0)

, _endian(LLBC_DefaultEndian)
, _attach(false)

, _poolInst(nullptr)
{
    attach ? Attach(rhs) : Assign(rhs);
}

inline LLBC_Stream::LLBC_Stream(void *buf, size_t size, bool attach)
: _buf(nullptr)
, _pos(0)
, _cap(0)

, _endian(LLBC_DefaultEndian)
, _attach(false)

, _poolInst(nullptr)
{
    attach ? Attach(buf, size) : Assign(buf, size);
}

inline LLBC_Stream::~LLBC_Stream()
{
    if (_buf && !_attach)
        free(_buf);
}

inline void LLBC_Stream::Attach(const LLBC_Stream &rhs)
{
    if (UNLIKELY(this == &rhs))
        return;

    if (_buf && !_attach)
        free(_buf);

    _buf = rhs._buf;
    _cap = rhs._cap;
    _pos = rhs._pos;
    _attach = true;
}

inline void LLBC_Stream::Attach(void *buf, size_t size)
{
    if (!_attach && _buf)
        free(_buf);

    _pos = 0;
    if (UNLIKELY(!buf || size == 0))
    {
        _buf = nullptr;
        _cap = 0;
    }
    else
    {
        _buf = reinterpret_cast<sint8 *>(buf);
        _cap = size;
    }

    _attach = true;
}

inline void *LLBC_Stream::Detach()
{
    void *tmp = _buf;

    _buf = nullptr;
    _pos = _cap = 0;

    _attach = false;

    return tmp;
}

inline bool LLBC_Stream::IsAttach() const
{
    return _attach;
}

inline void LLBC_Stream::SetAttach(bool attach)
{
    _attach = attach;
}

inline void LLBC_Stream::Assign(const LLBC_Stream &rhs)
{
    if (UNLIKELY(this == &rhs))
        return;

    _pos = 0;
    if (rhs._pos != 0)
        Write(rhs._buf, rhs._pos);
}

inline void LLBC_Stream::Assign(void *buf, size_t size)
{
    if (!buf || size == 0)
    {
        SetPos(0);
        return;
    }

    _pos = 0;
    Write(buf, size);
}

inline void LLBC_Stream::Swap(LLBC_Stream &another)
{
    std::swap(_buf, another._buf);
    std::swap(_pos, another._pos);
    std::swap(_cap, another._cap);

    std::swap(_endian,  another._endian);
    std::swap(_attach, another._attach);
}

inline int LLBC_Stream::GetEndian() const
{
    return _endian;
}

inline void LLBC_Stream::SetEndian(int endian)
{
    if (LIKELY(LLBC_Endian::IsValid(endian)))
        _endian = endian;
}

inline size_t LLBC_Stream::GetPos() const
{
    return _pos;
}

inline void LLBC_Stream::SetPos(size_t pos)
{
    _pos = MIN(pos, _cap);
}

inline bool LLBC_Stream::Skip(sint64 size)
{
    const sint64 newPos = static_cast<sint64>(_pos) + size;
    if (UNLIKELY(newPos < 0 || newPos > static_cast<sint64>(_cap)))
        return false;

    _pos = static_cast<size_t>(newPos);
    return true;
}

inline size_t LLBC_Stream::GetCap() const
{
    return _cap;
}

inline size_t LLBC_Stream::GetFreeCap() const
{
    return _cap - _pos;
}

template <typename T>
T *LLBC_Stream::GetBuf() const
{
    return reinterpret_cast<T *>(_buf);
}

template <typename T>
T *LLBC_Stream::GetBufStartWithPos() const
{
    return reinterpret_cast<T *>(GetBuf<char>() + _pos);
}

inline void LLBC_Stream::Insert(size_t pos, const void *buf, size_t size)
{
    Replace(pos, pos, buf, size);
}

inline void LLBC_Stream::Erase(size_t n0, size_t n1)
{
    Replace(n0, n1, nullptr, 0);
}

inline void LLBC_Stream::Replace(size_t n0, size_t n1, const void *buf, size_t size)
{
    // Normalize n0, n1.
    if (n1 == npos || n1 > _pos)
    {
        n1 = _pos;
        if (n0 > n1)
            n0 = n1;
    }
    else if (UNLIKELY(n0 > n1))
    {
        std::swap(n0, n1);
    }

    // Normalize size.
    if (UNLIKELY(!buf))
        size = 0;

    // Process perfect replace.
    const auto eraseSize = n1 - n0;
    if (size == eraseSize)
    {
        if (size > 0)
        {
            const auto oldPos = _pos;
            SetPos(n0);
            Write(buf, size);

            SetPos(oldPos);
        }

        return;
    }

    // Copy back buf.
    sint8 *backBuf = nullptr;
    const size_t backBufSize = _pos - n1;
    if (backBufSize > 0)
    {
        backBuf = LLBC_Malloc(sint8, backBufSize);
        memcpy(backBuf, _buf + n1, backBufSize);
    }

    // Copy 'will replace buf' + 'back buf' to stream.
    SetPos(n0);
    if (size > 0)
        Write(buf, size);
    if (backBuf)
    {
        Write(backBuf, backBufSize);
        free(backBuf);
    }
}

inline bool LLBC_Stream::Read(void *buf, size_t size)
{
    if (UNLIKELY(size <= 0))
        return true;
    if (UNLIKELY(!buf || _pos + size > _cap))
        return false;

    memcpy(buf, _buf + _pos, size);
    _pos += size;

    return true;
}

inline void LLBC_Stream::Write(const void *buf, size_t size)
{
    if (UNLIKELY((!buf || size <= 0) ||
                 !ReserveFreeCap(size)))
        return;

    memcpy(_buf + _pos, buf, size);
    _pos += size;
}

inline bool LLBC_Stream::Recap(size_t newCap)
{
    if (UNLIKELY(_attach || newCap < _cap))
        return false;

    if (newCap > _cap)
    {
        _buf = LLBC_Realloc(sint8, _buf, newCap);
        _cap = newCap;
    }

    return true;
}

inline void LLBC_Stream::Clear()
{
    if (_attach)
    {
        _buf = nullptr;
        _pos = _cap = 0;

        _attach = false;
    }
    else
    {
        _pos = 0;
    }
}

inline void LLBC_Stream::MarkPoolObject(LLBC_IObjectPoolInst &poolInst)
{
    _poolInst = &poolInst;
}

inline LLBC_IObjectPoolInst *LLBC_Stream::GetPoolInst()
{
    return _poolInst;
}

inline LLBC_String LLBC_Stream::ToString() const
{
    LLBC_String repr;
    repr.append_format("Stream[buf:%p, pos:%lu, cap:%lu", _buf, _pos, _cap);
    repr.append_format(", attach:%s, endian:%s]",
                       (_attach ? "true" : "false"), LLBC_Endian::Type2Str(_endian));

    return repr;
}

inline LLBC_Stream &LLBC_Stream::operator=(const LLBC_Stream &rhs)
{
    if (UNLIKELY(this == &rhs))
        return *this;

    Assign(rhs);
    return *this;
}

inline LLBC_Stream &LLBC_Stream::operator=(LLBC_Stream &&rhs)
{
    if (_buf && !_attach)
        free(_buf);

    _buf = rhs._buf;
    _pos = rhs._pos;
    _cap = rhs._cap;
    _endian = rhs._endian;
    _attach = rhs._attach;
    // !!! rhs._poolInst not allow move.
    // _poolInst = rhs._poolInst;

    rhs._buf = nullptr;
    rhs._pos = 0;
    rhs._cap = 0;
    rhs._endian = LLBC_DefaultEndian;
    rhs._attach = false;

    return *this;
}

inline bool LLBC_Stream::ReserveFreeCap(size_t freeCap)
{
    const size_t nowFreeCap = GetFreeCap();
    if (nowFreeCap >= freeCap)
        return true;

    const size_t newCap =
        MAX(_pos + freeCap, MIN(
            (_cap << 1), _cap + LLBC_CFG_COM_STREAM_AUTO_RESIZE_INCR_LIMIT));
    return Recap((newCap + 0xf) & 0xfffffff0);
}

inline bool LLBC_Stream::OverlappedCheck(const void *another, size_t size) const
{
    if (!_buf || !another)
        return true;

    return ((reinterpret_cast<const sint8 *>(another) + size <= _buf) || (another >= _buf + _cap));
}

__LLBC_NS_END

inline std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_Stream &stream)
{
    return o << stream.ToString();
}
