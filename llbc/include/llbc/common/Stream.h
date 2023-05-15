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
#include "llbc/common/Endian.h"
#include "llbc/common/BasicDataType.h"
#include "llbc/common/StringDataType.h"
#include "llbc/common/TemplateDeduction.h"

/** Some stream helper macros define **/
/*  DeSerialize/Read about macros define  */
// Begin read macro define, use to simple begin read object.
#define LLBC_STREAM_BEGIN_READ(stream, failRetType, failRetVal) \
    do {                                                        \
        failRetType __r_failRet = (failRetVal);                 \
        LLBC_NAMESPACE LLBC_Stream &__r_stream = (stream)       \

// DeSerialize macro define.
#define LLBC_STREAM_READ(field)                                 \
    if (!__r_stream.Read(field)) {                              \
        return __r_failRet;                                     \
    }                                                           \

#define LLBC_STREAM_READ_BUF(buf, len)                          \
    if (!__r_stream.Read(buf, len)) {                           \
        return __r_failRet;                                     \
    }                                                           \

// Peek macro define.
#define LLBC_STREAM_PEEK(field)                                 \
    if (!__r_stream.Peek(field)) {                              \
        return __r_failRet;                                     \
    }                                                           \

// Skip macro define, use to skip stream.
#define LLBC_STREAM_SKIP(size)                                  \
    if (!__r_stream.Skip(size)) {                               \
        return __r_failRet;                                     \
    }                                                           \

// End read macro define, use to stop stream read.
#define LLBC_STREAM_END_READ()                                  \
    } while(0)                                                  \

// End read macro define, like with previous, but this
// macro will return given value to stop function call.
#define LLBC_STREAM_END_READ_RET(retVal)                        \
    } while(0);                                                 \
    return (retVal)                                             \

/*  Serialize/Write about macros define  */
// Begin write macro define, use to simple begin write object.
#define LLBC_STREAM_BEGIN_WRITE(stream)                         \
    do {                                                        \
        LLBC_NAMESPACE LLBC_Stream &__w_stream = (stream)       \

// Serialize macro define.
#define LLBC_STREAM_WRITE(field)                                \
    __w_stream.Write(field)                                     \

#define LLBC_STREAM_WRITE_BUF(buf, len)                         \
    __w_stream.Write(buf, len)                                  \

// End write macro define, use to stop stream write.
#define LLBC_STREAM_END_WRITE()                                 \
    } while(0)                                                  \

// End write macro define, like with previous, but this
// macro will return given value to stop function call.
#define LLBC_STREAM_END_WRITE_RET(retVal)                       \
    } while(0);                                                 \
    return (retVal)                                             \


__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_Stream;
class LLBC_IObjectPoolInst;

__LLBC_NS_END

__LLBC_INTERNAL_NS_BEGIN

/**
 * \brief std::array reader template define.
 */
template <size_t N>
struct __LLBC_ArrayReader
{
    template <typename Arr>
    static bool Read(Arr &arr, LLBC_NS LLBC_Stream &stream)
    {
        if (UNLIKELY(!stream.Read(arr[N - 1])))
            return false;

        return __LLBC_ArrayReader<N - 1>::Read(arr, stream);
    }
};

/**
 * \brief Stop std::array read.
 */
template <>
struct __LLBC_ArrayReader<0>
{
    template <typename Arr>
    static bool Read(Arr &arr, LLBC_NS LLBC_Stream &stream)
    {
        return true;
    }
};

/**
 * \brief std::array writer template define.
 */
template <size_t N>
struct __LLBC_ArrayWriter
{
    template <typename Arr>
    static void Write(const Arr &arr, LLBC_NS LLBC_Stream &stream)
    {
        stream.Write(arr[N - 1]);
        __LLBC_ArrayWriter<N - 1>::Write(arr, stream);
    }
};

/**
 * \brief Stop std::array read.
 */
template <>
struct __LLBC_ArrayWriter<0>
{
    template <typename Arr>
    static void Write(const Arr &arr, LLBC_NS LLBC_Stream &stream)
    {
    }
};

/**
 * \brief std::tuple reader template define.
 */
template <size_t N>
struct __LLBC_TupleReader
{
    template <typename Tup>
    static bool Read(Tup &tup, LLBC_NS LLBC_Stream &stream)
    {
        if (UNLIKELY(!stream.Read(std::get<N - 1>(tup))))
            return false;

        return __LLBC_TupleReader<N - 1>::Read(tup, stream);
    }
};

/**
 * \brief Stop std::tuple read.
 */
template <>
struct __LLBC_TupleReader<0>
{
    template <typename Tup>
    static bool Read(Tup &tup, LLBC_NS LLBC_Stream &stream, size_t &readCount)
    {
        return true;
    }
};

/**
 * \brief std::tuple writer template define.
 */
template <size_t N>
struct __LLBC_TupleWriter
{
    template <typename Tup>
    static void Write(const Tup &tup, LLBC_NS LLBC_Stream &stream)
    {
        stream.Write(std::get<N - 1>(tup));
        __LLBC_TupleWriter<N - 1>::Write(tup, stream);
    }
};

/**
 * \brief Stop std::tuple write.
 */
template <>
struct __LLBC_TupleWriter<0>
{
    template <typename Tup>
    static void Write(const Tup &tup, LLBC_NS LLBC_Stream &stream)
    {
    }
};

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

/**
 * \brief Stream class encapsulation, support serialize/deserialize operations.
 */
class LLBC_Stream final
{
public:
    static constexpr size_t npos = (std::numeric_limits<size_t>::max)();

public:
    /**
     * Default constructor.
     */
    LLBC_Stream();

    /**
     * Parameter constructor, constructor will allocate cap bytes in stream.
     * @param[in] cap - buffer cap, in bytes.
     */
    explicit LLBC_Stream(size_t cap);

    /**
     * Move constructor.
     */
    LLBC_Stream(LLBC_Stream &&rhs);

    /**
     * Copy right side stream, and can determine attach attribute, if
     * use true, like wrap behavior, you can in the new stream object
     * to do any operations.
     */
    LLBC_Stream(const LLBC_Stream &rhs, bool attach = false);

    /**
     * Parameter constructor, will copy or attach external buffer.
     * @param[in] buf    - external buffer pointer, not null.
     * @param[in] size   - indicate buffer size, in bytes, must greater than 0.
     * @param[in] attach - attach flag, if true, stream object will attach external buffer, default is true.
     */
    LLBC_Stream(void *buf, size_t size, bool attach = true);

    /**
     * Destructor, if stream buffer is allocate by itself, will delete.
     */
    ~LLBC_Stream();

public:
    /**
     * Attach another stream.
     * @param[in] rhs - another stream object.
     */
    void Attach(const LLBC_Stream &rhs);

    /**
     * Attach external buffer.
     * @param[in] buf  - external buffer pointer, not null.
     * @param[in] size - indicate external buffer size, in bytes, must greater than or equal to 0.
     */
    void Attach(void *buf, size_t size);

    /**
     * Detach the external buffer, if the buffer is external buffer.
     * @return void * - external buffer pointer.
     */
    void *Detach();

    /**
     * Get stream's buffer attach attribute.
     * @return bool - attribute attribute.
     */
    bool IsAttach() const;

    /**
     * Set stream's buffer attach attribute.
     * @param[in] attach - attach flag.
     */
    void SetAttach(bool attach);

    /**
     * stream assign operation.
     * @param[in] rhs - another stream object.
     */
    void Assign(const LLBC_Stream &rhs);

    /**
     * Buffer assign operation.
     * @param[in] buf  - buffer.
     * @param[in] size - buffer size.
     */
    void Assign(void *buf, size_t size);

    /**
     * Swap two stream objects.
     * @param[in/out] another - another stream.
     */
    void Swap(LLBC_Stream &another);

public:
    /**
     * Get endian type.
     * @return int - endian type.
     */
    int GetEndian() const;

    /**
     * Set endian type.
     * @param[in] endian type.
     */
    void SetEndian(int endian);

    /**
     * Get current buffer R/W position.
     * @return uint32 - current R/W position.
     */
    size_t GetPos() const;

    /**
     * Set current buffer R/W position.
     * @param[in] pos - new position, must less equal than buffer size.
     */
    void SetPos(size_t pos);

    /**
     * Skip the buffer R/W position.
     * @param[in] size - will skip's size, in bytes.
     * @return bool - return true if success, otherwise return false.
     */
    bool Skip(sint64 size);

    /**
     * Get current stream buffer capacity.
     * @return size_t - the stream buffer capacity, in bytes.
     */
    size_t GetCap() const;

    /**
     * @brief Get free capacity.
     *
     * @return size_t - free capacity, in bytes.
     */
    size_t GetFreeCap() const;

    /**
     * Get current buffer pointer.
     * @return void * - current buffer pointer.
     */
    template <typename T = void>
    T *GetBuf() const;

    /**
     * Get buffer pointer start with position.
     * @return void * - buffer pointer.
     */
    template <typename T = void>
    T *GetBufStartWithPos() const;

public:
    /**
     * Insert buffer to current string.
     * Note: 1. performance warning.
     *       2. Do not try to insert data to attach attribute's stream.
     * @param[in] pos  - stream insert position, if pos > this->pos, will auto adjust to this->pos.
     * @param[in] buf  - will insert buffer.
     * @param[in] size - buffer size.
     */
    void Insert(size_t pos, const void *buf, size_t size);

    /**
     * Erase specific range stream buffer.
     * @param[in] n0 - begin erase pos.
     * @param[in] n1 - end erase pos, if is npos, will erase [n0, end).
     */
    void Erase(size_t n0, size_t n1);

    /**
     * Replace stream's buffer.
     * Note: 1. Performance warning.
     *       2. Do not try to replace attach attribute's stream.
     * @param[in] n0   - stream begin position.
     * @param[in] n1   - stream end position, if is npos, will replace [n0, end) range stream buffer.
     * @param[in] buf  - buffer.
     * @param[in] size - buffer size.
     */
    void Replace(size_t n0, size_t n1, const void *buf, size_t size);

public:
    /**
     * Read buffer data from stream.
     * @param[out] buf  - buffer pointer.
     * @param[in]  size - require read size, in bytes.
     * @return bool - return true if successed, otherwise return false.
     */
    bool Read(void *buf, size_t size);

    /**
     * Write buffer data to stream.
     * @param[in] buf  - buffer pointer.
     * @param[in] size - buffer size, in bytes.
     */
    void Write(const void *buf, size_t size);

public:
    /**
     * Read object from stream.
     * @return Obj - the object value.
     */
    template <typename T>
    T Read()
    {
        T obj;
        return LIKELY(Read<T>(obj)) ? obj : T();
    }

    /**
     * Read arithmetic type object from stream.
     * @param[out] obj - already read object.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, bool>::type
    Read(T &obj)
    {
        const size_t readableSize = _cap - _pos;
        if (UNLIKELY(readableSize < sizeof(T)))
            return false;

        obj = *(reinterpret_cast<T *>(_buf + _pos));
        _pos += sizeof(T);

        if (_endian != LLBC_MachineEndian)
            LLBC_ReverseBytes(obj);

        return true;
    }

    template <typename T, size_t _ArrLen>
    typename std::enable_if<std::is_arithmetic<T>::value &&
                            std::is_same<T, char>::value, bool>::type
    Read(T (&arr)[_ArrLen])
    {
        uint32 size;
        if (UNLIKELY(!Read(size)))
            return false;

        if (size == 0)
        {
            arr[0] = '\0';
            return true;
        }
        else if (size > _ArrLen)
        {
            return false;
        }
        else // size <= _ArrLen
        {
            if (UNLIKELY(!Read(&arr[0], sizeof(T) * size)))
                return false;

            if (size < _ArrLen)
                arr[size] = '\0';

            return true;
        }
    }

    template <typename T, size_t _ArrLen>
    typename std::enable_if<std::is_arithmetic<T>::value &&
                            !std::is_same<T, char>::value, bool>::type
    Read(T (&arr)[_ArrLen])
    {
        uint32 size;
        if (UNLIKELY(!Read(size)))
            return false;

        if (size == 0)
            return true;
        else if (size > _ArrLen)
            return false;

        return Read(&arr[0], sizeof(T) * size);
    }

    template <typename T, size_t _ArrLen>
    typename std::enable_if<!std::is_arithmetic<T>::value, bool>::type
    Read(T (&arr)[_ArrLen])
    {
        uint32 size;
        if (UNLIKELY(!Read(size)))
            return false;

        if (size == 0)
            return true;
        else if (size > _ArrLen)
            return false;

        for (size_t i = 0; i < size; ++i)
        {
            if (UNLIKELY(!Read(&arr[i])))
                return false;
        }

        return true;
    }

    /**
     * Forbid string pointer read operation.
     */
    template <typename T>
    typename std::enable_if<std::is_pointer<T>::value &&
                            std::is_same<typename LLBC_ExtractPureType<T>::type, char>::value, bool>::type
    Read(T &obj)
    {
        static_assert(false && "Unsupported stream read operation!");
        return false;
    }

    /**
     * Write void pointer from stream.
     * @param[out] voidPtr - the void pointer.
     * @return bool - return 0 if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<std::is_pointer<T>::value &&
                            std::is_same<typename LLBC_ExtractPureType<T>::type, void>::value, bool>::type
    Read(T &voidPtr)
    {
        uint64 ptrVal;
        if (UNLIKELY(!Read(ptrVal)))
            return false;

        memcpy(&voidPtr, &ptrVal, MIN(sizeof(voidPtr), sizeof(ptrVal)));

        return true;
    }

    /**
     * Write non string pointer from stream.
     * @param[out] ptr - the non string pointer.
     * @return bool - return 0 if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<std::is_pointer<T>::value &&
                            !std::is_same<typename LLBC_ExtractPureType<T>::type, char>::value &&
                            !std::is_same<typename LLBC_ExtractPureType<T>::type, void>::value, bool>::type
    Read(T &ptr)
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

    /**
     * Read std::basic_string template spec type object from stream.
     * @param[out] str - already read object.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::basic_string>::value ||
                            LLBC_IsTemplSpec<T, LLBC_BasicString>::value, bool>::type
    Read(T &str)
    {
        uint32 size;
        if (UNLIKELY(!Read(size)))
            return false;

        str.clear();
        if (size == 0)
            return true;

        str.resize(size);
        if (UNLIKELY(!Read(const_cast<char *>(str.data()), str.size())))
        {
            str.clear();
            return false;
        }

        return true;
    }

    /**
     * Read std::vector/std::list/std::deque template spec type object from stream.
     * @param[out] container - already read object.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::vector>::value ||
                            LLBC_IsTemplSpec<T, std::list>::value ||
                            LLBC_IsTemplSpec<T, std::deque>::value, bool>::type
    Read(T &container)
    {
        uint32 size;
        if (UNLIKELY(!Read(size)))
            return false;

        container.resize(size);
        if (size == 0)
            return true;

        size_t numOfReads = 0;
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

    /**
     * Read std::queue/std::stack template spec type object from stream.
     * @param[out] container - already read object.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::queue>::value ||
                            LLBC_IsTemplSpec<T, std::stack>::value, bool>::type
    Read(T &container)
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

    /**
     * Read std::set/std::unordered_set template spec type object from stream.
     * @param[out] container - already read object.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::set>::value ||
                            LLBC_IsTemplSpec<T, std::unordered_set>::value, bool>::type
    Read(T &container)
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

    /**
     * Read std::map/std::unordered_map template spec type object from stream.
     * @param[out] container - already read object.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::map>::value ||
                            LLBC_IsTemplSpec<T, std::unordered_map>::value, bool>::type
    Read(T &container)
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

    /**
     * Read std::array template spec type object from stream.
     * @param[out] arr - already read object.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsSTLArraySpec<T, std::array>::value, bool>::type
    Read(T &arr)
    {
        return LLBC_INTERNAL_NS __LLBC_ArrayReader<std::tuple_size<T>::value>::Read(arr, *this);
    }

    /**
     * Read std::tuple template spec type object from stream.
     * @param[out] tup - already read object.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::tuple>::value, bool>::type
    Read(T &tup)
    {
        return LLBC_INTERNAL_NS __LLBC_TupleReader<std::tuple_size<T>::value>::Read(tup, *this);
    }

    /**
     * Read std::pair template spec type object from stream.
     * @param[out] p - already read object.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::pair>::value, bool>::type
    Read(T &p)
    {
        return Read(p.first) && Read(p.second);
    }

    /**
     * Read object from stream.
     * @param[out] obj - already read object.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<!std::is_arithmetic<T>::value &&
                            !std::is_array<T>::value &&
                            !std::is_pointer<T>::value &&
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
                            !LLBC_IsTemplSpec<T, std::pair>::value, bool>::type
    Read(T &obj)
    {
        return ReadImpl<T>(obj, 0);
    }

private:
    /**
     * Try adapt T::DeSerialize.
     */
    template <typename T, bool (T::*)(LLBC_Stream &)>
    struct upper_camel_case_deserializable_type;
    template <typename T>
    bool ReadImpl(T &obj, upper_camel_case_deserializable_type<T, &T::DeSerialize> *)
    {
        return obj.DeSerialize(*this);
    }

    /**
     * Try adapt T::Deserialize.
     */
    template <typename T>
    bool ReadImpl(T &obj, upper_camel_case_deserializable_type<T, &T::Deserialize> *)
    {
        return obj.Deserialize(*this);
    }

    /**
     * Try adapt T::deserialize.
     */
    template <typename T, bool (T::*)(LLBC_Stream &)>
    struct lower_camel_case_deserializable_type;
    template <typename T>
    bool ReadImpl(T &obj, lower_camel_case_deserializable_type<T, &T::deserialize> *)
    {
        return obj.deserialize(*this);
    }

    /**
     * Try adapt protobuf mesage object.
     */
    template <typename T, bool (T::*)() const, int (T::*)() const>
    class protobuf_type;
    template <typename T>
    bool ReadImpl(T &obj, protobuf_type<T, &T::IsInitialized, &T::ByteSize> *)
    {
        uint32 pbDataSize;
        if (UNLIKELY(Read(pbDataSize) == false))
            return false;

        bool ret = obj.ParseFromArray(reinterpret_cast<char *>(_buf) + _pos, static_cast<int>(pbDataSize));
        if (!ret)
            return false;

        _pos += pbDataSize;
        return true;
    }

    /**
     * Final Read implement: memcpy obj memory.
     */
    template <typename T>
    bool ReadImpl(T &obj, ...)
    {
        if (_cap >= _pos + sizeof(T))
            return Read(&obj, sizeof(T));

        return false;
    }

public:
    /**
     * Write arithmetic type object to stream.
     * @param[in] obj - the will write object.
     */
    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, void>::type
    Write(const T &obj)
    {
        ReserveFreeCap(sizeof(T));
        if (_endian != LLBC_MachineEndian)
        {
            T obj2(obj);
            LLBC_ReverseBytes(obj2);
            memcpy(_buf + _pos, &obj2, sizeof(T));
        }
        else
        {
            memcpy(_buf + _pos, &obj, sizeof(T));
        }

        _pos += sizeof(T);
    }

    /**
     * Write char arithmetic array to stream.
     * @param[in] arr - the char arithmetic array.
     */
    template <typename T, size_t _ArrLen>
    typename std::enable_if<std::is_arithmetic<T>::value &&
                            std::is_same<T, char>::value, void>::type
    Write(const T (&arr)[_ArrLen])
    {
        if (_ArrLen == 0)
        {
            Write(0u);
        }
        else if (arr[_ArrLen - 1] == '\0')
        {

            Write(static_cast<uint32>(_ArrLen) - 1);
            Write(&arr[0], sizeof(T) * (_ArrLen - 1));
        }
        else
        {
            Write(static_cast<uint32>(_ArrLen));
            Write(&arr[0], sizeof(T) * _ArrLen);
        }
    }

    /**
     * Write non-char arithmetic array to stream.
     * @param[in] arr - the non-char arithmetic array.
     */
    template <typename T, size_t _ArrLen>
    typename std::enable_if<std::is_arithmetic<T>::value &&
                            !std::is_same<T, char>::value, void>::type
    Write(const T (&arr)[_ArrLen])
    {
        Write(static_cast<uint32>(_ArrLen));
        Write(&arr[0], sizeof(arr));
    }

    /**
     * Write non-arithmetic raw-array to stream.
     * @param[in] arr - the raw-array.
     */
    template <typename T, size_t _ArrLen>
    typename std::enable_if<!std::is_arithmetic<T>::value, void>::type
    Write(const T (&arr)[_ArrLen])
    {
        Write(static_cast<uint32>(_ArrLen));
        for (size_t i = 0; i < _ArrLen; ++i)
            Write(arr[i]);
    }

    /**
     * Write string pointer to stream.
     * @param[in] str - the string pointer.
     */
    template <typename T>
    typename std::enable_if<std::is_pointer<T>::value &&
                            std::is_same<typename LLBC_ExtractPureType<T>::type, char>::value, void>::type
    Write(const T &str)
    {
        if (UNLIKELY(!str))
        {
            Write(0u);
            return;
        }

        const uint32 strLen = strlen(str);
        Write(strLen);
        if (strLen > 0)
            Write(str, strLen);
    }

    /**
     * Write void pointer to stream.
     * @param[in] voidPtr - the void pointer.
     */
    template <typename T>
    typename std::enable_if<std::is_pointer<T>::value &&
                            std::is_same<typename LLBC_ExtractPureType<T>::type, void>::value, void>::type
    Write(const T &voidPtr)
    {
        uint64 pointVal = 0;
        memcpy(&pointVal, &voidPtr, sizeof(voidPtr));

        Write(pointVal);
    }

    /**
     * Write non string pointer to stream.
     * @param[in] ptr - the non string pointer.
     */
    template <typename T>
    typename std::enable_if<std::is_pointer<T>::value &&
                            !std::is_same<typename LLBC_ExtractPureType<T>::type, char>::value &&
                            !std::is_same<typename LLBC_ExtractPureType<T>::type, void>::value, void>::type
    Write(const T &ptr)
    {
        LIKELY(ptr) ?
            Write(*ptr) :
                Write(typename LLBC_ExtractPureType<T>::type());
        
    }

    /**
     * Write std::basic_string template spec type object to stream.
     * @param[in] str - the will write object.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::basic_string>::value ||
                            LLBC_IsTemplSpec<T, LLBC_BasicString>::value, void>::type
    Write(const T &str)
    {
        Write(static_cast<uint32>(str.size()));
        Write(str.data(), str.size());
    }

    /**
     * Write std::vector/std::list/std::deque/std::set/std::stack template spec type object to stream.
     * @param[in] container - the will write object.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::vector>::value ||
                            LLBC_IsTemplSpec<T, std::list>::value ||
                            LLBC_IsTemplSpec<T, std::deque>::value ||
                            LLBC_IsTemplSpec<T, std::set>::value ||
                            LLBC_IsTemplSpec<T, std::unordered_set>::value ||
                            LLBC_IsTemplSpec<T, std::stack>::value, void>::type
    Write(const T &container)
    {
        Write(static_cast<uint32>(container.size()));
        if (container.empty())
            return;

        const typename T::const_iterator endIt = container.end();
        for (typename T::const_iterator it = container.begin();
             it != endIt;
             ++it)
            Write(*it);
    }

    /**
     * !!! Forbid std::queue/std::stack template spec type object write !!!.
     * @param[in] container - the will write object.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::queue>::value ||
                            LLBC_IsTemplSpec<T, std::stack>::value, void>::type
    Write(const T &container)
    {
        static_assert(false && "Write std::queue/std::stack is not supported for now");
    }

    /**
     * Write std::map/std::unordered_map template spec type object to stream.
     * @param[in] container - the will write object.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::map>::value ||
                            LLBC_IsTemplSpec<T, std::unordered_map>::value, void>::type
    Write(const T &container)
    {
        Write(static_cast<uint32>(container.size()));
        if (container.empty())
            return;

        const typename T::const_iterator endIt = container.end();
        for (typename T::const_iterator it = container.begin();
             it != endIt;
             ++it)
        {
            Write(it->first);
            Write(it->second);
        }
    }

    /**
     * Write std::array template spec type object to stream.
     * @param[in] arr - the will write object.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsSTLArraySpec<T, std::array>::value, void>::type
    Write(const T &arr)
    {
        LLBC_INTERNAL_NS __LLBC_ArrayWriter<std::tuple_size<T>::value>::Write(arr, *this);
    }

    /**
     * Write std::tuple template spec type object to stream.
     * @param[in] tup - the will write object.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::tuple>::value, void>::type
    Write(const T &tup)
    {
        LLBC_INTERNAL_NS __LLBC_TupleWriter<std::tuple_size<T>::value>::Write(tup, *this);
    }

    /**
     * Write std::pair template spec type object to stream.
     * @param[in] p - the will write object.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::pair>::value, void>::type
    Write(const T &p)
    {
        Write(p.first);
        Write(p.second);
    }

    /**
     * Write object to stream.
     * @param[in] obj - the will write object.
     */
    template <typename T>
    typename std::enable_if<!std::is_arithmetic<T>::value &&
                            !std::is_array<T>::value &&
                            !std::is_pointer<T>::value &&
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
                            !LLBC_IsTemplSpec<T, std::pair>::value, void>::type
    Write(const T &obj)
    {
        WriteImpl<T>(obj, 0);
    }

private:
    /**
     * Try adapt T::Serialize.
     */
    template <typename T, void (T::*)(LLBC_Stream &) const>
    struct upper_camel_case_serializable_type;
    template <typename T>
    void WriteImpl(const T &obj, upper_camel_case_serializable_type<T, &T::Serialize> *)
    {
        obj.Serialize(*this);
    }

    /**
     * Try adapt T::serialize.
     */
    template <typename T, void (T::*)(LLBC_Stream &) const>
    struct lower_camel_case_serializable_type;
    template <typename T>
    void WriteImpl(const T &obj, lower_camel_case_serializable_type<T, &T::serialize> *)
    {
        obj.serialize(*this);
    }
    /**
     * Try adapt protobuf message object.
     */
    template <typename T>
    void WriteImpl(const T &obj, protobuf_type<T, &T::IsInitialized, &T::ByteSize> *)
    {
        // Check initialized first.
        obj.CheckInitialized();

        // Recap Stream.
        size_t needSize = static_cast<size_t>(obj.ByteSize());
        if ((_cap - _pos) < needSize + sizeof(uint32))
            Recap(_cap + (needSize + sizeof(uint32)));

        Write(static_cast<uint32>(needSize));
        obj.SerializeToArray(reinterpret_cast<char *>(_buf) + _pos, static_cast<int>(needSize));
        _pos += needSize;
    }

    /**
     * Final write implement: memcpy obj memory.
     */
    template <typename T>
    void WriteImpl(const T &obj, ...)
    {
        Write(&obj, sizeof(obj));
    }

public:
    /**
     * Peek data[use Read()], but don't modify stream object.
     * @param[out] obj - will deserialize's object.
     * @return bool - return true if successed, otherwise return false.
     */
    template <typename T>
    bool Peek(T &obj)
    {
        size_t oldPos = _pos;
        const bool ret = Read(obj);
        SetPos(oldPos);

        return ret;
    }

    /**
     * Stream input overlapped functions define.
     * @param[out] val - value reference.
     */
    template<typename T>
    LLBC_Stream &operator >>(T &val)
    {
        Read(val);
        return *this;
    }

    /**
     * Stream output overlapped functions define.
     * @param[in] val - const value reference.
     */
    template<typename T>
    LLBC_Stream &operator <<(const T &val)
    {
        Write(val);
        return *this;
    }

public:
    /**
     * Recapacity stream.
     * @param[in] newCap - new capacity, must greater than current capacity.
     * @return bool - return 0 if success, otherwise return false.
     */
    bool Recap(size_t newCap);

    /**
     * Clear stream.
     */
    void Clear();

public:
    /**
     * Object-Pool reflection support: Mark pool object.
     */
    void MarkPoolObject(LLBC_IObjectPoolInst &poolInst);

    /**
     * Object-Pool reflection support: Get pool instance.
     */
    LLBC_IObjectPoolInst *GetPoolInst();

    /**
     * Get stream string representation.
     * @return LLBC_String - the stream string representation.
     */
    LLBC_String ToString() const;

public:
    /**
     * Copy assignment.
     */
    LLBC_Stream &operator=(const LLBC_Stream &rhs);

    /**
     * Move assignment.
     */
    LLBC_Stream &operator=(LLBC_Stream &&rhs);

private:
    bool ReserveFreeCap(size_t freeCap);
    bool OverlappedCheck(const void *another, size_t size) const;

private:
    sint8 *_buf;
    size_t _pos;
    size_t _cap;

    int _endian;
    bool _attach;

    LLBC_IObjectPoolInst *_poolInst;
};

__LLBC_NS_END

/**
 * Stream output operator support. 
 */
std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_Stream &stream);

#include "llbc/common/StreamInl.h"

