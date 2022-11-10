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

#ifdef __LLBC_COM_STREAM_H__

#include "llbc/common/Template.h"
#include "llbc/common/Endian.h"

__LLBC_NS_BEGIN

inline LLBC_Stream::LLBC_Stream()
: _buf(nullptr)
, _pos(0)
, _cap(0)

, _endian(LLBC_DefaultEndian)

, _attach(false)
{
}

inline LLBC_Stream::LLBC_Stream(const LLBC_Stream &rhs)
: _buf(nullptr)
, _pos(0)
, _cap(0)

, _endian(LLBC_DefaultEndian)

, _attach(false)
{
    Assign(rhs);
}

inline LLBC_Stream::LLBC_Stream(const LLBC_Stream &rhs, bool attach)
: _buf(nullptr)
, _pos(0)
, _cap(0)

, _endian(LLBC_DefaultEndian)

, _attach(false)
{
    if (attach)
        Attach(rhs);
    else
        Assign(rhs);
}

inline LLBC_Stream::LLBC_Stream(size_t cap)
{
    if (cap > 0)
    {
        _buf = calloc(cap, 1);
        ASSERT(_buf && "LLBC_Stream object alloc memory from heap failed");
    }
    else
    {
        _buf = nullptr;
    }

    _pos = 0;
    _cap = cap;

    _endian = LLBC_DefaultEndian;

    _attach = false;
}

inline LLBC_Stream::LLBC_Stream(void *buf, size_t len, bool attach)
: _buf(nullptr)
, _pos(0)
, _cap(0)

, _endian(LLBC_DefaultEndian)
, _attach(false)

, _poolInst(nullptr)
{
    if (attach)
    {
        Attach(buf, len);
        return;
    }

    Write(buf, len);
    _pos = 0;
}

inline LLBC_Stream::~LLBC_Stream()
{
    if (!_attach)
        LLBC_XFree(_buf);
}

inline void LLBC_Stream::Attach(const LLBC_Stream &rhs)
{
    if (!_attach)
        LLBC_XFree(_buf);

    _buf = rhs._buf;
    _pos = rhs._pos;
    _cap = rhs._cap;

    _endian = rhs._endian;

    _attach = true;
}

inline void LLBC_Stream::Attach(void *buf, size_t len)
{
    if ((buf && len == 0) || (!buf && len > 0))
        return;

    if (!_attach)
        LLBC_XFree(_buf);

    _buf = buf;
    _pos = 0;
    _cap = len;

    _attach = true;
}

inline void LLBC_Stream::Assign(const LLBC_Stream &rhs)
{
    if (!_attach)
        LLBC_XFree(_buf);

    if (rhs._buf)
    {
        _buf = malloc(rhs._cap);
        memcpy(_buf, rhs._buf, rhs._cap);
    }
    else
    {
        _buf = nullptr;
    }

    _pos = rhs._pos;
    _cap = rhs._cap;

    _endian = rhs._endian;

    _attach = false;
}

inline void LLBC_Stream::Assign(void *buf, size_t len)
{
    if (!_attach)
        LLBC_XFree(_buf);

    if (buf && len > 0)
    {
        _buf = malloc(len);
        memcpy(_buf, buf, len);

        _cap = len;
    }
    else
    {
        _buf = nullptr;
        _cap = 0;
    }

    _pos = 0;

    _attach = false;
}

inline void *LLBC_Stream::Detach()
{
    void *tmp = _buf;

    _buf = nullptr;
    _pos = 0;
    _cap = 0;

    _attach = false;

    return tmp;
}

inline bool LLBC_Stream::IsAttach() const
{
    return _attach;
}

inline void LLBC_Stream::SetAttachAttr(bool attach)
{
    _attach = attach;
}

inline int LLBC_Stream::GetEndian() const
{
    return _endian;
}

inline void LLBC_Stream::SetEndian(int endian)
{
    if (LLBC_Endian::IsValid(endian))
        _endian = endian;
}

inline void LLBC_Stream::Swap(LLBC_Stream &another)
{
    LLBC_Swap(_buf, another._buf);
    LLBC_Swap(_pos, another._pos);
    LLBC_Swap(_cap, another._cap);

    LLBC_Swap(_endian,  another._endian);

    LLBC_Swap(_attach, another._attach);
}

inline size_t LLBC_Stream::GetPos() const
{
    return _pos;
}

inline void LLBC_Stream::SetPos(size_t pos)
{
    ASSERT(pos <= _cap);
    _pos = pos;
}

inline bool LLBC_Stream::Skip(long size)
{
    const size_t skipped = static_cast<
        size_t>(MAX(0, static_cast<long>(_pos) + size));

    if (skipped > _cap)
        return false;

    SetPos(skipped);

    return true;
}

inline void LLBC_Stream::Fill(size_t size)
{
    AutoRecap(size);
    memset((char *)_buf + _pos, 0, size);

    _pos += size;
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

inline void LLBC_Stream::Insert(size_t pos, const void *buf, size_t len)
{
    Replace(pos, pos, buf, len);
}

inline void LLBC_Stream::Replace(size_t n0, size_t n1, const void *buf, size_t len)
{
    ASSERT(buf && len && "LLBC_Stream::Insert() buf or len invalid!");

    // Swap n0, n1, if n0 > n1.
    if (UNLIKELY(n0 > n1))
    {
        size_t temp = n0;
        n0 = n1;
        n1 = temp;
    }

    ASSERT(n1 <= _pos && "LLBC_Stream::Replace() n1 > _pos!");

    const size_t eraseLen = n1 - n0;
    if (_pos + len - eraseLen > _cap)
    {
        ASSERT(!_attach && "LLBC_Stream::Insert() obj attach's buf limit");
        Recap(_cap + MAX(len, _cap));
    }

    uint8 *tmpBuf = nullptr;
    if (eraseLen == 0)
    {
        if (n1 == _pos)
            memcpy((uint8 *)_buf + n1, buf, len);
        else
        {
            tmpBuf = (uint8 *)malloc(_pos - n1);

            memcpy(tmpBuf, (uint8 *)_buf + n1, _pos - n1);
            memcpy((uint8 *)_buf + n1, buf, len);
            memcpy((uint8 *)_buf + n1 + len, tmpBuf, _pos - n1);

            free(tmpBuf);
        }

        _pos += len;
        return;
    }

    if (eraseLen == len)
    {
        memcpy((uint8 *)_buf + n0, buf, len);
        return;
    }

    if (n1 != _pos)
    {
        tmpBuf = (uint8 *)malloc(_pos - n1);
        memcpy(tmpBuf, (uint8 *)_buf + n1, _pos - n1);
    }

    memcpy((uint8 *)_buf + n0, buf, len);

    if (tmpBuf)
    {
        memcpy((uint8 *)_buf + n0 + len, tmpBuf, _pos - n1);
        free(tmpBuf);
    }

    _pos = _pos + len - (n1 - n0);
}

inline bool LLBC_Stream::Read(void *buf, size_t len)
{
    if (len <= 0)
        return true;
    else if (UNLIKELY(!buf) || _pos + len > _cap)
        return false;

    // check memory overlapped
    ASSERT(OverlappedCheck(buf, len) && "LLBC_Stream::Read(void *, size_t) buffer overlapped!");

    memcpy(buf, (const uint8 *)_buf + _pos, len);
    _pos += len;

    return true;
}

inline void LLBC_Stream::Write(const void *buf, size_t len)
{
    if (UNLIKELY(!buf || len <= 0))
        return;

    ASSERT(OverlappedCheck(buf, len) && "LLBC_Stream::Write(const void *, size_t) buffer overlapped!");

    AutoRecap(len);
    memcpy((uint8 *)_buf + _pos, buf, len);
    _pos += len;
}

inline void LLBC_Stream::Recap(size_t newCap)
{
    if (UNLIKELY(_attach))
        return;

    if (newCap > _cap)
    {
        _buf = realloc(_buf, newCap);
        ASSERT(_buf && "alloc memory from heap fail!");

        _cap = newCap;
    }
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

inline bool LLBC_Stream::ReadBool(bool &value)
{
    return Read(&value, sizeof(bool));
}

inline bool LLBC_Stream::ReadSInt8(sint8 &value)
{
    return Read(&value, sizeof(sint8));
}

inline bool LLBC_Stream::ReadUInt8(uint8 &value)
{
    return Read(&value, sizeof(uint8));
}

inline bool LLBC_Stream::ReadSInt16(sint16 &value)
{
    return ReadRawType(value);
}

inline bool LLBC_Stream::ReadUInt16(uint16 &value)
{
    return ReadRawType(value);
}

inline bool LLBC_Stream::ReadSInt32(sint32 &value)
{
    return ReadRawType(value);
}

inline bool LLBC_Stream::ReadUInt32(uint32 &value)
{
    return ReadRawType(value);
}

inline bool LLBC_Stream::ReadSInt64(sint64 &value)
{
    return ReadRawType(value);
}

inline bool LLBC_Stream::ReadUInt64(uint64 &value)
{
    return ReadRawType(value);
}

inline bool LLBC_Stream::ReadLong(long &value)
{
    return ReadRawType(value);
}

inline bool LLBC_Stream::ReadULong(ulong &value)
{
    return ReadRawType(value);
}

inline bool LLBC_Stream::ReadFloat(float &value)
{
    return ReadRawType(value);
}

inline bool LLBC_Stream::ReadDouble(double &value)
{
    return ReadRawType(value);
}

inline bool LLBC_Stream::ReadPtr(void *&value)
{
    return ReadRawType(value);
}

inline bool LLBC_Stream::ReadBool_2(bool failRet)
{
    Read(&failRet, sizeof(bool));
    return failRet;
}

inline sint8 LLBC_Stream::ReadSInt8_2(sint8 failRet)
{
    Read(&failRet, sizeof(sint8));
    return failRet;
}

inline uint8 LLBC_Stream::ReadUInt8_2(uint8 failRet)
{
    Read(&failRet, sizeof(uint8));
    return failRet;
}

inline sint16 LLBC_Stream::ReadSInt16_2(sint16 failRet)
{
    ReadRawType(failRet);
    return failRet;
}

inline uint16 LLBC_Stream::ReadUInt16_2(uint16 failRet)
{
    ReadRawType(failRet);
    return failRet;
}

inline sint32 LLBC_Stream::ReadSInt32_2(sint32 failRet)
{
    ReadRawType(failRet);
    return failRet;
}

inline uint32 LLBC_Stream::ReadUInt32_2(uint32 failRet)
{
    ReadRawType(failRet);
    return failRet;
}

inline sint64 LLBC_Stream::ReadSInt64_2(sint64 failRet)
{
    ReadRawType(failRet);
    return failRet;
}

inline uint64 LLBC_Stream::ReadUInt64_2(uint64 failRet)
{
    ReadRawType(failRet);
    return failRet;
}

inline long LLBC_Stream::ReadLong_2(long failRet)
{
    ReadRawType(failRet);
    return failRet;
}

inline ulong LLBC_Stream::ReadULong_2(ulong failRet)
{
    ReadRawType(failRet);
    return failRet;
}

inline float LLBC_Stream::ReadFloat_2(float failRet)
{
    ReadRawType(failRet);
    return failRet;
}

inline double LLBC_Stream::ReadDouble_2(double failRet)
{
    ReadRawType(failRet);
    return failRet;
}

inline void *LLBC_Stream::ReadPtr_2(void *failRet)
{
    ReadRawType(failRet);
    return failRet;
}

inline void LLBC_Stream::WriteBool(bool value)
{
    Write(&value, sizeof(bool));
}

inline void LLBC_Stream::WriteSInt8(sint8 value)
{
    Write(&value, sizeof(sint8));
}

inline void LLBC_Stream::WriteUInt8(uint8 value)
{
    Write(&value, sizeof(uint8));
}

inline void LLBC_Stream::WriteSint16(sint16 value)
{
    WriteRawType(value);
}

inline void LLBC_Stream::WriteUInt16(uint16 value)
{
    WriteRawType(value);
}

inline void LLBC_Stream::WriteSInt32(sint32 value)
{
    WriteRawType(value);
}

inline void LLBC_Stream::WriteUInt32(uint32 value)
{
    WriteRawType(value);
}

inline void LLBC_Stream::WriteSInt64(sint64 value)
{
    WriteRawType(value);
}

inline void LLBC_Stream::WriteUInt64(uint64 value)
{
    WriteRawType(value);
}

inline void LLBC_Stream::WriteLong(long value)
{
    WriteRawType(value);
}

inline void LLBC_Stream::WriteULong(ulong value)
{
    WriteRawType(value);
}

inline void LLBC_Stream::WriteFloat(float value)
{
    WriteRawType(value);
}

inline void LLBC_Stream::WriteDouble(double value)
{
    WriteRawType(value);
}

inline void LLBC_Stream::WritePtr(const void *value)
{
    WriteRawType(value);
}

inline LLBC_Stream &LLBC_Stream::operator =(const LLBC_Stream &rhs)
{
    Assign(rhs);
    return *this;
}

inline void LLBC_Stream::AutoRecap(size_t needCap)
{
    const size_t freeCap = GetFreeCap();
    if (freeCap >= needCap)
        return;

    if (_attach)
    {
        ASSERT(false && "stream obj attach's buf limit");
        return;
    }

    const size_t newSize = 
        MAX(_pos + needCap, MIN((_cap << 1), _cap + LLBC_CFG_COM_STREAM_AUTO_RESIZE_INCR_LIMIT));
    Recap((newSize + 0xf) & 0xfffffff0);
}

template <typename T>
inline bool LLBC_Stream::ReadRawType(T &value)
{
    if (!Read(&value, sizeof(T)))
        return false;

    if (_endian != LLBC_MachineEndian)
        LLBC_ReverseBytes(value);

    return true;
}

template <typename T>
inline void LLBC_Stream::WriteRawType(const T &value)
{
    if (_endian == LLBC_MachineEndian)
    {
        Write(&value, sizeof(T));
    }
    else
    {
        const T reversedVal = LLBC_ReverseBytes2(value);
        Write(&reversedVal, sizeof(T));
    }
}

inline bool LLBC_Stream::OverlappedCheck(const void *another, size_t len)
{
    if (!_buf)
        return true;

    return (((uint8 *)another + len <= _buf) || 
        (another >= (uint8 *)_buf + _cap));
}

__LLBC_NS_END

#endif // !__LLBC_COM_STREAM_H__
