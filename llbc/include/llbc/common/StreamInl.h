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

#include "llbc/common/Macro.h"
#include "llbc/common/Endian.h"
#include "llbc/common/TemplateDeduction.h"
#include "llbc/common/Errors.h"
#include "llbc/common/Exceptions.h"

__LLBC_NS_BEGIN

/**
 * \brief Stop std::array read.
 */
template <>
struct LLBC_Stream::__LLBC_STLArrayReader<0>
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
struct LLBC_Stream::__LLBC_STLArrayWriter<0>
{
    template <typename Arr>
    static bool Write(const Arr &arr, LLBC_NS LLBC_Stream &stream)
    {
        return true;
    }
};

/**
 * \brief Stop std::tuple read.
 */
template <>
struct LLBC_Stream::__LLBC_TupleReader<0>
{
    template <typename Tup>
    static bool Read(Tup &tup, LLBC_Stream &stream)
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
    static bool Write(const Tup &tup, LLBC_Stream &stream)
    {
        return true;
    }
};

LLBC_FORCE_INLINE LLBC_Stream::LLBC_Stream()
: _buf(nullptr)
, _readPos(0)
, _writePos(0)
, _cap(0)

, _endian(LLBC_DefaultEndian)
, _attach(false)

, _typedObjPool(nullptr)
{
}

LLBC_FORCE_INLINE LLBC_Stream::LLBC_Stream(size_t cap)
: _buf(cap > 0 ? LLBC_Malloc(sint8, cap) : nullptr)
, _readPos(0)
, _writePos(0)
, _cap(cap)

, _endian(LLBC_DefaultEndian)
, _attach(false)

, _typedObjPool(nullptr)
{
}

LLBC_FORCE_INLINE LLBC_Stream::LLBC_Stream(LLBC_Stream &&rhs) noexcept
: _buf(rhs._buf)
, _readPos(rhs._readPos)
, _writePos(rhs._writePos)
, _cap(rhs._cap)

, _endian(rhs._endian)
, _attach(rhs._attach)

// !!! rhs._typedObjPool not allow move.
, _typedObjPool(nullptr)
{
    rhs._buf = nullptr;
    rhs._readPos = 0;
    rhs._writePos = 0;
    rhs._cap = 0;

    // ! rhs._endian dont need reset.
    // rhs._endian = LLBC_DefaultEndian;
    rhs._attach = false;

    // !!! rhs._typedObjPool not allow move.
    // rhs._typedObjPool = nullptr;
}

LLBC_FORCE_INLINE LLBC_Stream::LLBC_Stream(const LLBC_Stream &rhs, bool attach)
: _buf(nullptr)
, _readPos(0)
, _writePos(0)
, _cap(0)

, _endian(LLBC_DefaultEndian)
, _attach(false)

, _typedObjPool(nullptr)
{
    attach ? Attach(rhs) : Assign(rhs);
}

LLBC_FORCE_INLINE LLBC_Stream::LLBC_Stream(void *buf, size_t size, bool attach)
: _buf(nullptr)
, _readPos(0)
, _writePos(0)
, _cap(0)

, _endian(LLBC_DefaultEndian)
, _attach(false)

, _typedObjPool(nullptr)
{
    attach ? Attach(buf, size) : Assign(buf, size);
}

LLBC_FORCE_INLINE LLBC_Stream::~LLBC_Stream()
{
    if (_buf && !_attach)
        free(_buf);
}

LLBC_FORCE_INLINE void LLBC_Stream::Attach(const LLBC_Stream &rhs)
{
    if (UNLIKELY(this == &rhs))
        return;

    if (_buf && !_attach)
        free(_buf);

    _buf = rhs._buf;
    _cap = rhs._cap;
    _readPos = 0;
    _writePos = rhs._writePos;

    _endian = rhs._endian;
    _attach = true;
}

LLBC_FORCE_INLINE void LLBC_Stream::Attach(void *buf, size_t size)
{
    if (_buf && !_attach)
        free(_buf);

    _readPos = 0;
    if (UNLIKELY(!buf || size == 0))
    {
        _buf = nullptr;
        _writePos = _cap = 0;
    }
    else
    {
        _buf = reinterpret_cast<sint8 *>(buf);
        _writePos = _cap = size;
    }

    _attach = true;
}

LLBC_FORCE_INLINE void *LLBC_Stream::Detach()
{
    if (!_buf)
        return nullptr;

    void * const buf = _buf;

    _buf = nullptr;
    _readPos = _writePos = _cap = 0;
    _attach = false;

    return buf;
}

LLBC_FORCE_INLINE bool LLBC_Stream::IsAttach() const
{
    return _attach;
}

LLBC_FORCE_INLINE void LLBC_Stream::SetAttach(bool attach)
{
    _attach = attach;
}

LLBC_FORCE_INLINE void LLBC_Stream::Assign(const LLBC_Stream &rhs)
{
    if (UNLIKELY(this == &rhs))
        return;

    _readPos = _writePos = 0;
    if (rhs._writePos != 0)
    {
        Write(rhs._buf, rhs._writePos);
        _readPos = rhs._readPos;
    }

    _endian = rhs._endian;
}

LLBC_FORCE_INLINE void LLBC_Stream::Assign(void *buf, size_t size)
{
    _readPos = _writePos = 0;
    if (!buf || size == 0)
        return;

    Write(buf, size);
}

LLBC_FORCE_INLINE void LLBC_Stream::Swap(LLBC_Stream &another)
{
    std::swap(_buf, another._buf);
    std::swap(_readPos, another._readPos);
    std::swap(_writePos, another._writePos);
    std::swap(_cap, another._cap);

    std::swap(_endian,  another._endian);
    std::swap(_attach, another._attach);
}

LLBC_FORCE_INLINE int LLBC_Stream::GetEndian() const
{
    return _endian;
}

LLBC_FORCE_INLINE void LLBC_Stream::SetEndian(int endian)
{
    if (LIKELY(LLBC_Endian::IsValid(endian)))
        _endian = endian;
}

LLBC_FORCE_INLINE size_t LLBC_Stream::GetReadPos() const
{
    return _readPos;
}

LLBC_FORCE_INLINE size_t LLBC_Stream::GetWritePos() const
{
    return _writePos;
}

LLBC_FORCE_INLINE bool LLBC_Stream::SetReadPos(size_t readPos)
{
    if (UNLIKELY(readPos > _writePos))
        return false;

    _readPos = readPos;
    return true;
}

LLBC_FORCE_INLINE bool LLBC_Stream::SetWritePos(size_t writePos)
{
    if (UNLIKELY(writePos > _cap))
        return false;

    _writePos = writePos;
    if (_readPos > _writePos)
        _readPos = _writePos;

    return true;
}

LLBC_FORCE_INLINE bool LLBC_Stream::SkipRead(sint64 skipSize)
{
    const auto newReadPos = static_cast<sint64>(_readPos) + skipSize;
    if (UNLIKELY(newReadPos < 0 ||
        newReadPos > static_cast<sint64>(_writePos)))
        return false;

    return SetReadPos(static_cast<size_t>(newReadPos));
}

LLBC_FORCE_INLINE bool LLBC_Stream::SkipWrite(sint64 skipSize)
{
    const auto newWritePos = static_cast<sint64>(_writePos) + skipSize;
    if (UNLIKELY(newWritePos < 0 ||
        newWritePos > static_cast<sint64>(_cap)))
        return false;

    return SetWritePos(static_cast<size_t>(newWritePos));
}

LLBC_FORCE_INLINE size_t LLBC_Stream::GetCap() const
{
    return _cap;
}

LLBC_FORCE_INLINE size_t LLBC_Stream::GetReadableSize() const
{
    return _writePos - _readPos;
}

LLBC_FORCE_INLINE size_t LLBC_Stream::GetWritableSize() const
{
    return _cap - _writePos;
}

template <typename T>
T *LLBC_Stream::GetBuf()
{
    return reinterpret_cast<T *>(_buf);
}

template <typename T>
const T *LLBC_Stream::GetBuf() const
{
    return reinterpret_cast<const T *>(_buf);
}

template <typename T>
T *LLBC_Stream::GetBufStartWithReadPos()
{
    return _buf ?
        reinterpret_cast<T *>(
            reinterpret_cast<char *>(_buf) + _readPos) :
            nullptr;
}
template <typename T>
const T *LLBC_Stream::GetBufStartWithReadPos() const
{
    return const_cast<LLBC_Stream *>(this)->GetBufStartWithReadPos<T>();
}

template <typename T>
T *LLBC_Stream::GetBufStartWithWritePos()
{
    return _buf ?
        reinterpret_cast<T *>(
            reinterpret_cast<char *>(_buf) + _writePos) :
            nullptr;
}

template <typename T>
const T *LLBC_Stream::GetBufStartWithWritePos() const
{
    return const_cast<LLBC_Stream *>(this)->GetBufStartWithWritePos<T>();
}

LLBC_FORCE_INLINE void LLBC_Stream::Insert(size_t pos, const void *buf, size_t size)
{
    Replace(pos, pos, buf, size);
}

LLBC_FORCE_INLINE void LLBC_Stream::Erase(size_t n0, size_t n1)
{
    Replace(n0, n1, nullptr, 0);
}

LLBC_FORCE_INLINE void LLBC_Stream::Replace(size_t n0, size_t n1, const void *buf, size_t size)
{
    // Normalize n0, n1.
    if (UNLIKELY(n0 > n1))
        std::swap(n0, n1);
    if (n1 == npos || n1 > _writePos)
    {
        n1 = _writePos;
        if (n0 > n1)
            n0 = n1;
    }

    // Normalize size.
    if (UNLIKELY(!buf))
        size = 0;

    // Process perfect replace.
    const auto eraseSize = n1 - n0;
    if (size == eraseSize)
    {
        if (size > 0)
            memcpy(_buf + n0, buf, size);

        return;
    }

    // Backup back buf.
    sint8 *backBuf = nullptr;
    const size_t backBufSize = _writePos - n1;
    if (backBufSize > 0)
    {
        backBuf = LLBC_Malloc(sint8, backBufSize);
        memcpy(backBuf, _buf + n1, backBufSize);
    }

    // Calc new rpos.
    // - rpos <= n0: rpos
    // - rpos < n1: n0
    // - rpos >= n1: rpos + (size - (n1 - n0))
    const size_t newReadPos =
        _readPos <= n0 ? _readPos :
            (_readPos < n1 ? n0 : _readPos + size - eraseSize);

    // Copy 'will replace buf' + 'back buf' to stream.
    SetWritePos(n0);
    if (size > 0)
    {
        Write(buf, size);
    }
    if (backBuf)
    {
        Write(backBuf, backBufSize);
        free(backBuf);
    }

    // Update rpos/wpos.
    SetWritePos(n0 + size + backBufSize);
    SetReadPos(newReadPos);
}

LLBC_FORCE_INLINE bool LLBC_Stream::Read(void *buf, size_t size)
{
    if (UNLIKELY(!buf || size <= 0))
        return false;
    if (UNLIKELY( _readPos + size > _writePos))
        return false;

    memcpy(buf, _buf + _readPos, size);
    _readPos += size;

    return true;
}

LLBC_FORCE_INLINE bool LLBC_Stream::Write(const void *buf, size_t size)
{
    if (UNLIKELY((!buf || size <= 0) || !ReserveWritableSize(size)))
    {
        LLBC_SetLastError(LLBC_ERROR_NO_SUCH);
        return false;
    }

    memcpy(_buf + _writePos, buf, size);
    _writePos += size;

    return true;
}

template <typename T1, typename... OtherTypes>
bool LLBC_Stream::BatchRead(T1 &val1, OtherTypes &... otherVals)
{
    return Read(val1) && (Read(otherVals) && ...);
}

template <typename T1, typename... OtherTypes>
bool LLBC_Stream::BatchWrite(const T1& val1, const OtherTypes &... otherVals)
{
    return Write(val1) && (Write(otherVals) && ...);
}

template <typename T>
T LLBC_Stream::Read()
{
    T obj;
    return LIKELY(Read<T>(obj)) ? std::move(obj) : T();
}

template <typename T>
std::enable_if_t<std::is_arithmetic_v<T> || std::is_enum_v<T>, bool>
LLBC_Stream::Read(T &obj)
{
    if (UNLIKELY(!Read(&obj, sizeof(T))))
        return false;

    if (_endian != LLBC_MachineEndian)
        obj = LLBC_ReverseBytes(obj);

    return true;
}

template <typename T>
std::enable_if_t<std::is_pointer_v<T> &&
                   std::is_same_v<typename LLBC_ExtractPureType<T>::type, char>,
                 bool>
LLBC_Stream::Read(T &obj)
{
    llbc_assert(false && "Unsupported stream read operation(for char *)!");
    return false;
}

template <typename T>
std::enable_if_t<std::is_pointer_v<T> &&
                    std::is_same_v<typename LLBC_ExtractPureType<T>::type, void>,
                 bool>
LLBC_Stream::Read(T &voidPtr)
{
    uint64 ptrVal;
    if (UNLIKELY(!Read(ptrVal)))
        return false;

    memcpy(&voidPtr, &ptrVal, std::min(sizeof(voidPtr), sizeof(ptrVal)));

    return true;
}

template <typename T>
std::enable_if_t<std::is_pointer_v<T> &&
                    !std::is_same_v<typename LLBC_ExtractPureType<T>::type, char> &&
                    !std::is_same_v<typename LLBC_ExtractPureType<T>::type, void>,
                 bool>
LLBC_Stream::Read(T &ptr)
{
    bool innerCreate = false;
    if (!ptr)
    {
        innerCreate = true;
        ptr = new typename LLBC_ExtractPureType<T>::type;
    }

    if (UNLIKELY(!Read(*ptr)))
    {
        if (innerCreate)
            delete ptr;
        return false;
    }

    return true;
}

template <typename T, size_t _ArrLen>
std::enable_if_t<std::is_arithmetic_v<T> && std::is_same_v<T, char>,
                 bool>
LLBC_Stream::Read(T (&arr)[_ArrLen])
{
    uint32 size;
    if (UNLIKELY(!Read(size)))
        return false;

    if (size == 0)
    {
        if (_ArrLen > 0)
            arr[0] = T();
        return true;
    }

    if (size >= _ArrLen ||
        !Read(&arr[0], size))
    {
        _readPos -= sizeof(uint32);
        return false;
    }

    arr[size] = T();

    return true;
}

template <typename T, size_t _ArrLen>
std::enable_if_t<std::is_arithmetic_v<T> &&
                    (std::is_same_v<T, uint8> || std::is_same_v<T, bool>),
                 bool>
LLBC_Stream::Read(T (&arr)[_ArrLen])
{
    uint32 size;
    if (UNLIKELY(!Read(size)))
        return false;

    if (size == 0)
        return true;

    if (size > _ArrLen ||
        !Read(&arr[0], size))
    {
        _readPos -= sizeof(uint32);
        return false;
    }

    return true;
}

template <typename T, size_t _ArrLen>
std::enable_if_t<(std::is_arithmetic_v<T> &&
                  (!std::is_same_v<T, char> && !std::is_same_v<T, uint8> && !std::is_same_v<T, bool>)) ||
                 std::is_enum_v<T>,
                 bool>
LLBC_Stream::Read(T (&arr)[_ArrLen])
{
    uint32 size;
    if (UNLIKELY(!Read(size)))
        return false;

    if (size == 0)
        return true;

    if (size > _ArrLen ||
        !Read(&arr[0], sizeof(T) * size))
    {
        _readPos -= sizeof(uint32);
        return false;
    }

    if (_endian != LLBC_MachineEndian)
    {
        for (uint32 i = 0; i < size; ++i)
            arr[i] = LLBC_ReverseBytes(arr[i]);
    }

    return true;
}

template <typename T, size_t _ArrLen>
std::enable_if_t<!std::is_arithmetic_v<T> && !std::is_enum_v<T>, bool>
LLBC_Stream::Read(T(&arr)[_ArrLen])
{
    uint32 size;
    if (UNLIKELY(!Read(size)))
        return false;

    if (size == 0)
        return true;

    if (size > _ArrLen)
    {
        _readPos -= sizeof(uint32);
        return false;
    }

    for (uint32 i = 0; i < size; ++i)
    {
        if (!Read(arr[i]))
            return false;
    }

    return true;
}

template <typename T>
std::enable_if_t<LLBC_IsTemplSpec<T, std::basic_string>::value ||
                    LLBC_IsTemplSpec<T, LLBC_BasicString>::value,
                 bool>
LLBC_Stream::Read(T &str)
{
    uint32 size;
    if (UNLIKELY(!Read(size)))
        return false;

    if (size == 0)
    {
        str.clear();
        return true;
    }

    str.resize(size);
    if (UNLIKELY(!Read(const_cast<char *>(str.data()), str.size())))
    {
        str.clear();
        return false;
    }

    return true;
}

template <typename T>
std::enable_if_t<LLBC_IsTemplSpec<T, std::vector>::value ||
                    LLBC_IsTemplSpec<T, std::list>::value ||
                    LLBC_IsTemplSpec<T, std::deque>::value,
                 bool>
LLBC_Stream::Read(T &container)
{
    uint32 size;
    if (UNLIKELY(!Read(size)))
        return false;

    if (size == 0)
    {
        container.clear();
        return true;
    }

    size_t numOfReads = 0;
    container.resize(size);
    const typename T::iterator endIt = container.end();
    for (typename T::iterator it = container.begin(); it != endIt; ++it)
    {
        typename T::value_type &value = *it;
        if (UNLIKELY(!Read(value)))
        {
            container.resize(numOfReads);
            return false;
        }

        ++numOfReads;
    }

    return true;
}

template <typename T>
std::enable_if_t<LLBC_IsTemplSpec<T, std::queue>::value ||
                    LLBC_IsTemplSpec<T, std::stack>::value,
                 bool>
LLBC_Stream::Read(T &container)
{
    uint32 size;
    if (UNLIKELY(!Read(size)))
        return false;

    container.clear();
    if (size == 0)
        return true;

    typename T::value_type value;
    for (uint32 i = 0; i < size; ++i)
    {
        if (UNLIKELY(!Read(value)))
            return false;

        container.push(std::move(value));
    }

    return true;
}

template <typename T>
std::enable_if_t<LLBC_IsTemplSpec<T, std::set>::value ||
                    LLBC_IsTemplSpec<T, std::unordered_set>::value,
                 bool>
LLBC_Stream::Read(T &container)
{
    uint32 size;
    if (UNLIKELY(!Read(size)))
        return false;

    container.clear();
    if (size == 0)
        return true;

    typename T::value_type value;
    for (uint32 i = 0; i < size; ++i)
    {
        if (UNLIKELY(!Read(value)))
            return false;

        container.insert(std::move(value));
    }

    return true;
}

template <typename T>
std::enable_if_t<LLBC_IsTemplSpec<T, std::map>::value ||
                    LLBC_IsTemplSpec<T, std::unordered_map>::value,
                 bool>
LLBC_Stream::Read(T &container)
{
    uint32 size;
    if (UNLIKELY(!Read(size)))
        return false;

    container.clear();
    if (size == 0)
        return true;

    typename T::key_type key;
    typename T::mapped_type mapped;
    for (uint32 i = 0; i < size; ++i)
    {
        if (UNLIKELY(!Read(key) || !Read(mapped)))
            return false;

        container.emplace(std::move(key), std::move(mapped));
    }

    return true;
}

template <typename T>
std::enable_if_t<LLBC_IsSTLArraySpec<T, std::array>::value, bool>
LLBC_Stream::Read(T &arr)
{
    uint32 arrSize;
    if (!Read(arrSize) ||
        arrSize != static_cast<uint32>(std::tuple_size<T>::value))
        return false;

    return __LLBC_STLArrayReader<std::tuple_size<T>::value>::Read(arr, *this);
}

template <typename T>
std::enable_if_t<LLBC_IsTemplSpec<T, std::tuple>::value, bool>
LLBC_Stream::Read(T &tup)
{
    uint32 tupSize;
    if (!Read(tupSize) ||
        tupSize != static_cast<uint32>(std::tuple_size<T>::value))
        return false;

    return __LLBC_TupleReader<std::tuple_size<T>::value>::Read(tup, *this);
}

template <typename T>
std::enable_if_t<LLBC_IsTemplSpec<T, std::pair>::value, bool>
LLBC_Stream::Read(T &p)
{
    return Read(p.first) && Read(p.second);
}

template <typename T>
std::enable_if_t<!std::is_arithmetic_v<T> &&
                    !std::is_enum_v<T> &&
                    !std::is_pointer_v<T> &&
                    !std::is_array_v<T> &&
                    !(LLBC_IsTemplSpec<T, std::basic_string>::value ||
                      LLBC_IsTemplSpec<T, LLBC_BasicString>::value) &&
                    !LLBC_IsTemplSpec<T, std::vector>::value &&
                    !LLBC_IsTemplSpec<T, std::list>::value &&
                    !LLBC_IsTemplSpec<T, std::deque>::value &&
                    !LLBC_IsTemplSpec<T, std::queue>::value &&
                    !LLBC_IsTemplSpec<T, std::stack>::value &&
                    !LLBC_IsTemplSpec<T, std::set>::value &&
                    !LLBC_IsTemplSpec<T, std::unordered_set>::value &&
                    !LLBC_IsTemplSpec<T, std::map>::value &&
                    !LLBC_IsTemplSpec<T, std::unordered_map>::value &&
                    !LLBC_IsSTLArraySpec<T, std::array>::value &&
                    !LLBC_IsTemplSpec<T, std::tuple>::value &&
                    !LLBC_IsTemplSpec<T, std::pair>::value,
                 bool>
LLBC_Stream::Read(T &obj)
{
    return ReadImpl<T>(obj, 0);
}

template <typename T>
bool LLBC_Stream::ReadImpl(T &obj, upper_camel_case_deserializable_type<T, &T::DeSerialize> *)
{
    return obj.DeSerialize(*this);
}

template <typename T>
bool LLBC_Stream::ReadImpl(T &obj, upper_camel_case_deserializable_type<T, &T::Deserialize> *)
{
    return obj.Deserialize(*this);
}

template <typename T>
bool LLBC_Stream::ReadImpl(T &obj, upper_camel_case_short_deserializable_type<T, &T::DeSer> *)
{
    return obj.DeSer(*this);
}

template <typename T>
bool LLBC_Stream::ReadImpl(T &obj, upper_camel_case_short_deserializable_type<T, &T::Deser> *)
{
    return obj.Deser(*this);
}

template <typename T>
bool LLBC_Stream::ReadImpl(T &obj, lower_camel_case_deserializable_type<T, &T::deserialize> *)
{
    return obj.deserialize(*this);
}

template <typename T>
bool LLBC_Stream::ReadImpl(T &obj, lower_camel_case_short_deserializable_type<T, &T::deser> *)
{
    return obj.deser(*this);
}

template <typename T>
bool LLBC_Stream::ReadImpl(T &obj, protobuf2_type<T, &T::IsInitialized, &T::ByteSize> *)
{
    uint32 pbDataSize;
    if (UNLIKELY(!Read(pbDataSize)))
        return false;

    if (UNLIKELY(GetReadableSize() < pbDataSize))
        return false;

    if (!obj.ParseFromArray(GetBufStartWithReadPos(), static_cast<int>(pbDataSize)))
    {
        _readPos -= sizeof(uint32);
        return false;
    }

    _readPos += pbDataSize;
    return true;
}

template <typename T>
bool LLBC_Stream::ReadImpl(T &obj, protobuf3_type<T, &T::IsInitialized, &T::ByteSizeLong> *)
{
    uint32 pbDataSize;
    if (UNLIKELY(!Read(pbDataSize)))
        return false;

    if (UNLIKELY(GetReadableSize() < pbDataSize))
        return false;

    if (!obj.ParseFromArray(GetBufStartWithReadPos(), static_cast<int>(pbDataSize)))
    {
        _readPos -= sizeof(uint32);
        return false;
    }

    _readPos += pbDataSize;
    return true;
}

template <typename T>
std::enable_if_t<std::is_trivial_v<T>, bool>
LLBC_Stream::ReadImpl(T &obj, ...)
{
    return Read(&obj, sizeof(T));
}

template <typename T>
std::enable_if_t<!std::is_trivial_v<T>, bool>
LLBC_Stream::ReadImpl(T &obj, ...)
{
    llbc_assert(false && "Read non-trivial object is unsupported for now!");
    return false;
}

template <typename T>
std::enable_if_t<std::is_arithmetic_v<T> || std::is_enum_v<T>, bool>
LLBC_Stream::Write(const T &obj)
{
    if (_endian != LLBC_MachineEndian)
    {
        const T obj2 = LLBC_ReverseBytes(obj);
        return Write(&obj2, sizeof(T));
    }
    else
    {
        return Write(&obj, sizeof(T));
    }
}

template <typename T>
std::enable_if_t<std::is_pointer_v<T> &&
                 std::is_same_v<typename LLBC_ExtractPureType<T>::type, char>, bool>
LLBC_Stream::Write(const T &str)
{
    if (UNLIKELY(!str))
    {
        return Write(0u);
    }

    const uint32 strLen = strlen(str);
    if (UNLIKELY(!Write(strLen)))
    {
        return false;
    }

    if (strLen > 0)
        return Write(str, strLen);
    return true;
}

template <typename T>
std::enable_if_t<std::is_pointer_v<T> &&
                 std::is_same_v<typename LLBC_ExtractPureType<T>::type, void>, bool>
LLBC_Stream::Write(const T &voidPtr)
{
    uint64 pointVal = 0;
    memcpy(&pointVal, &voidPtr, sizeof(voidPtr));

    return Write(pointVal);
}

template <typename T>
std::enable_if_t<std::is_pointer<T>::value &&
                 !std::is_same_v<typename LLBC_ExtractPureType<T>::type, char> &&
                 !std::is_same_v<typename LLBC_ExtractPureType<T>::type, void>, bool>
LLBC_Stream::Write(const T &ptr)
{
    return LIKELY(ptr) ?
                Write(*ptr) :
                    Write(typename LLBC_ExtractPureType<T>::type());
}

template <typename T, size_t _ArrLen>
std::enable_if_t<std::is_arithmetic_v<T> && std::is_same_v<T, char>, bool>
LLBC_Stream::Write(const T (&arr)[_ArrLen])
{
    if (_ArrLen == 0)
    {
        return Write(0u);
    }
    else if (arr[_ArrLen - 1] == '\0')
    {
        if (_ArrLen == 1)
        {
            return Write(0u);
        }
        else
        {
            return Write(static_cast<uint32>(_ArrLen - 1)) && Write(&arr[0], sizeof(T) * (_ArrLen - 1));
        }
    }
    else
    {
        return Write(static_cast<uint32>(_ArrLen)) && Write(&arr[0], sizeof(T) * _ArrLen);
    }
}

template <typename T, size_t _ArrLen>
std::enable_if_t<std::is_arithmetic_v<T> && (std::is_same_v<T, uint8>|| std::is_same_v<T, bool>), bool>
LLBC_Stream::Write(const T (&arr)[_ArrLen])
{
    if (_ArrLen == 0)
    {
        return Write(0u);
    }
    else
    {
        return Write(static_cast<uint32>(_ArrLen)) && Write(&arr[0], sizeof(T) * _ArrLen);
    }
}

template <typename T, size_t _ArrLen>
std::enable_if_t<(std::is_arithmetic_v<T> &&
                  (!std::is_same_v<T, char> &&
                   !std::is_same_v<T, uint8> &&
                   !std::is_same_v<T, bool>)) ||
                 !std::is_arithmetic_v<T>, bool>
LLBC_Stream::Write(const T (&arr)[_ArrLen])
{
    if (UNLIKELY(!Write(static_cast<uint32>(_ArrLen))))
    {
        return false;
    }

    for (size_t i = 0; i < _ArrLen; ++i)
    {
        if (UNLIKELY(!Write(arr[i])))
        {
            return false;
        }
    }
    
    return true;
}

template <typename T>
std::enable_if_t<LLBC_IsTemplSpec<T, std::basic_string>::value ||
                 LLBC_IsTemplSpec<T, LLBC_BasicString>::value,
                bool>
LLBC_Stream::Write(const T &str)
{
    return Write(static_cast<uint32>(str.size())) && Write(str.data(), str.size());
}

template <typename T>
std::enable_if_t<LLBC_IsTemplSpec<T, std::vector>::value ||
                 LLBC_IsTemplSpec<T, std::list>::value ||
                 LLBC_IsTemplSpec<T, std::deque>::value ||
                 LLBC_IsTemplSpec<T, std::set>::value ||
                 LLBC_IsTemplSpec<T, std::unordered_set>::value,
                bool>
LLBC_Stream::Write(const T &container)
{
    if (UNLIKELY(!Write(static_cast<uint32>(container.size()))))
        return false;

    if (container.empty())
        return true;

    const typename T::const_iterator endIt = container.end();
    for (typename T::const_iterator it = container.begin();
         it != endIt;
         ++it)
    {
        if (UNLIKELY(!Write(*it)))
            return false;
    }

    return true;
}

template <typename T>
std::enable_if_t<LLBC_IsTemplSpec<T, std::queue>::value ||
                 LLBC_IsTemplSpec<T, std::stack>::value, bool>
LLBC_Stream::Write(const T &container)
{
    llbc_assert(false && "Write std::queue/std::stack is not supported for now");
    return false;
}

template <typename T>
std::enable_if_t<LLBC_IsTemplSpec<T, std::map>::value ||
                 LLBC_IsTemplSpec<T, std::unordered_map>::value, bool>
LLBC_Stream::Write(const T &container)
{
    if (UNLIKELY(!Write(static_cast<uint32>(container.size()))))
        return false;

    if (container.empty())
        return true;

    const typename T::const_iterator endIt = container.end();
    for (typename T::const_iterator it = container.begin();
         it != endIt;
         ++it)
    {
        if (UNLIKELY(!Write(it->first) || !Write(it->second)))
            return false;
    }

    return true;
}

template <typename T>
std::enable_if_t<LLBC_IsSTLArraySpec<T, std::array>::value, bool>
LLBC_Stream::Write(const T &arr)
{
    return Write(static_cast<uint32>(std::tuple_size<T>::value)) && 
               __LLBC_STLArrayWriter<std::tuple_size<T>::value>::Write(arr, *this);
}

template <typename T>
std::enable_if_t<LLBC_IsTemplSpec<T, std::tuple>::value, bool>
LLBC_Stream::Write(const T &tup)
{
    return Write(static_cast<uint32>(std::tuple_size<T>::value)) &&
               __LLBC_TupleWriter<std::tuple_size<T>::value>::Write(tup, *this);
}

template <typename T>
std::enable_if_t<LLBC_IsTemplSpec<T, std::pair>::value, bool>
LLBC_Stream::Write(const T &p)
{
    return Write(p.first) && Write(p.second);
}

template <typename T>
std::enable_if_t<!std::is_arithmetic_v<T> &&
                 !std::is_enum_v<T> &&
                 !std::is_pointer_v<T> &&
                 !std::is_array_v<T> &&
                 !(LLBC_IsTemplSpec<T, std::basic_string>::value ||
                   LLBC_IsTemplSpec<T, LLBC_BasicString>::value) &&
                 !LLBC_IsTemplSpec<T, std::vector>::value &&
                 !LLBC_IsTemplSpec<T, std::list>::value &&
                 !LLBC_IsTemplSpec<T, std::deque>::value &&
                 !LLBC_IsTemplSpec<T, std::queue>::value &&
                 !LLBC_IsTemplSpec<T, std::stack>::value &&
                 !LLBC_IsTemplSpec<T, std::set>::value &&
                 !LLBC_IsTemplSpec<T, std::unordered_set>::value &&
                 !LLBC_IsTemplSpec<T, std::map>::value &&
                 !LLBC_IsTemplSpec<T, std::unordered_map>::value &&
                 !LLBC_IsTemplSpec<T, std::tuple>::value &&
                 !LLBC_IsSTLArraySpec<T, std::array>::value &&
                 !LLBC_IsTemplSpec<T, std::pair>::value,
                 bool>
LLBC_Stream::Write(const T &obj)
{
    return WriteImpl<T>(obj, 0);
}

template <typename T>
bool LLBC_Stream::WriteImpl(const T &obj, upper_camel_case_serializable_type<T, &T::Serialize> *)
{
    return obj.Serialize(*this);
}

template <typename T>
bool LLBC_Stream::WriteImpl(const T &obj, upper_camel_case_short_serializable_type<T, &T::Ser> *)
{
    return obj.Ser(*this);
}

template <typename T>
bool LLBC_Stream::WriteImpl(const T &obj, lower_camel_case_serializable_type<T, &T::serialize> *)
{
    return obj.serialize(*this);
}

template <typename T>
bool LLBC_Stream::WriteImpl(const T &obj, lower_camel_case_short_serializable_type<T, &T::ser> *)
{
    return obj.ser(*this);
}

template <typename T>
bool LLBC_Stream::WriteImpl(const T &obj, protobuf2_type<T, &T::IsInitialized, &T::ByteSize> *)
{
    // Check initialized first.
    obj.CheckInitialized();

    // Recap Stream.
    const size_t needSize = static_cast<size_t>(obj.ByteSize());
    if (!ReserveWritableSize(needSize + sizeof(uint32)))
    {
        LLBC_SetLastError(LLBC_ERROR_NO_SUCH);
        return false;
    }

    Write(static_cast<uint32>(needSize));
    obj.SerializeToArray(reinterpret_cast<char*>(_buf) + _writePos, static_cast<int>(needSize));
    _writePos += needSize;

    return true;
}

template <typename T>
bool LLBC_Stream::WriteImpl(const T &obj, protobuf3_type<T, &T::IsInitialized, &T::ByteSizeLong> *)
{
    // Check initialized first.
    obj.CheckInitialized();

    // Recap Stream.
    const size_t needSize = obj.ByteSizeLong();
    if (!ReserveWritableSize(needSize + sizeof(uint32)))
    {
        LLBC_SetLastError(LLBC_ERROR_NO_SUCH);
        return false;
    }

    Write(static_cast<uint32>(needSize));
    obj.SerializeToArray(reinterpret_cast<char *>(_buf) + _writePos, static_cast<int>(needSize));
    _writePos += needSize;

    return true;
}

template <typename T>
std::enable_if_t<std::is_trivial_v<T>, bool>
LLBC_Stream::WriteImpl(const T &obj, ...)
{
    return Write(&obj, sizeof(obj));
}

template <typename T>
std::enable_if_t<!std::is_trivial_v<T>, bool>
LLBC_Stream::WriteImpl(const T &obj, ...)
{
    llbc_assert(false && "Write non-trivial object is unsupported for now!");
    return false;
}


LLBC_FORCE_INLINE bool LLBC_Stream::Serialize(LLBC_Stream& stream) const
{
    if (UNLIKELY(this == &stream))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);
        return false;
    }

    uint32 serSize = static_cast<uint32>(GetReadableSize());
    if (UNLIKELY(!stream.Write(serSize)))
        return false;

    if (UNLIKELY(serSize == 0))
        return true;

    return stream.Write(GetBufStartWithReadPos(), serSize);
}

LLBC_FORCE_INLINE bool LLBC_Stream::Deserialize(LLBC_Stream& stream)
{
    if (UNLIKELY(this == &stream)) {
        LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);
        return false;
    }

    _readPos = _writePos = 0;
    uint32 bufSize = 0;
    stream.Read(bufSize);
    if (UNLIKELY(bufSize == 0)) 
        return true;
    
    return Write(stream.GetBufStartWithReadPos(), bufSize) && stream.SkipRead(bufSize);
}

template <typename T>
bool LLBC_Stream::Peek(T &obj)
{
    size_t oldReadPos = _readPos;
    const bool ret = Read(obj);
    _readPos = oldReadPos;

    return ret;
}

template<typename T>
LLBC_Stream &LLBC_Stream::operator>>(T &val)
{
    if (!Read(val))
        LLBC_THROW(LLBC_IOException, "LLBC_Stream Read failed!");

    return *this;
}

template<typename T>
LLBC_Stream &LLBC_Stream::operator<<(const T &val)
{
    if (!Write(val))
        LLBC_THROW(LLBC_IOException, "LLBC_Stream Write failed!");

    return *this;
}

LLBC_FORCE_INLINE bool LLBC_Stream::Recap(size_t newCap)
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

LLBC_FORCE_INLINE void LLBC_Stream::Clear()
{
    if (_attach)
    {
        _buf = nullptr;
        _readPos = _writePos = _cap = 0;

        _attach = false;
    }
    else
    {
        _readPos = _writePos = 0;
    }
    _endian = LLBC_DefaultEndian;
}

LLBC_FORCE_INLINE LLBC_TypedObjPool<LLBC_Stream> *LLBC_Stream::GetTypedObjPool() const
{
    return _typedObjPool;
}

LLBC_FORCE_INLINE void LLBC_Stream::SetTypedObjPool(LLBC_TypedObjPool<LLBC_Stream> *typedObjPool)
{
    _typedObjPool = typedObjPool;
}

LLBC_FORCE_INLINE LLBC_String LLBC_Stream::ToString() const
{
    LLBC_String repr;
    repr.append_format("Stream[%p, rpos:%lu, wpos:%lu, cap:%lu, attached:%s endian:%s]",
                       _buf, _readPos, _writePos, _cap,
                       _attach ? "true" : "false", LLBC_Endian::Type2Str(_endian));

    return repr;
}

LLBC_FORCE_INLINE LLBC_Stream &LLBC_Stream::operator=(const LLBC_Stream &rhs)
{
    if (UNLIKELY(this == &rhs))
        return *this;

    Assign(rhs);
    return *this;
}

LLBC_FORCE_INLINE LLBC_Stream &LLBC_Stream::operator=(LLBC_Stream &&rhs) noexcept
{
    if (_buf && !_attach)
        free(_buf);

    _buf = rhs._buf;
    _readPos = rhs._readPos;
    _writePos = rhs._writePos;
    _cap = rhs._cap;
    _endian = rhs._endian;
    _attach = rhs._attach;
    // !!! rhs._typedObjPool not allow move.
    // _typedObjPool = rhs._typedObjPool;

    rhs._buf = nullptr;
    rhs._readPos = 0;
    rhs._writePos = 0;
    rhs._cap = 0;
    // !!! rhs._endian dont need reset.
    // rhs._endian = LLBC_DefaultEndian;
    rhs._attach = false;

    return *this;
}

LLBC_FORCE_INLINE bool LLBC_Stream::ReserveWritableSize(size_t writableSize)
{
    if (writableSize <= GetWritableSize())
        return true;

    const size_t newCap =
        MAX(_writePos + writableSize, MIN(
            (_cap << 1), _cap + LLBC_CFG_COM_STREAM_AUTO_RESIZE_INCR_LIMIT));
    return Recap((newCap + 0xf) & 0xfffffff0);
}

__LLBC_NS_END

LLBC_FORCE_INLINE std::ostream &operator<<(std::ostream &o, const LLBC_NS LLBC_Stream &stream)
{
    return o << stream.ToString();
}
