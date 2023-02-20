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
#include "llbc/common/BasicDataType.h"
#include "llbc/common/Endian.h"

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
class LLBC_IObjectPoolInst;

__LLBC_NS_END

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
     * Copy constructor.
     */
    LLBC_Stream(const LLBC_Stream &rhs);

    /**
     * Copy right side stream, and can determine attach attribute, if
     * use true, like wrap behavior, you can in the new stream object
     * to do any operations.
     */
    LLBC_Stream(const LLBC_Stream &rhs, bool attach);

    /**
     * Parameter constructor, constructor will allocate cap bytes in stream.
     * @param[in] cap - buffer cap, in bytes.
     */
    explicit LLBC_Stream(size_t cap);

    /**
     * Parameter constructor, will copy or attach external buffer.
     * @param[in] buf    - external buffer pointer, not null.
     * @param[in] len    - indicate buffer size, in bytes, must greater than 0.
     * @param[in] attach - attach flag, if true, stream object will attach external buffer, default is true.
     */
    LLBC_Stream(void *buf, size_t len, bool attach = true);

    /**
     * Destructor, if stream buffer is allocate by itself, will delete.
     */
    ~LLBC_Stream();

    /**
     * Attach another stream.
     * @param[in] rhs - another stream object.
     */
    void Attach(const LLBC_Stream &rhs);

    /**
     * Attach external buffer.
     * @param[in] buf - external buffer pointer, not null.
     * @param[in] len - indicate external buffer size, in bytes, must greater than or equal to 0.
     */
    void Attach(void *buf, size_t len);

    /**
     * stream assign operation.
     * @param[in] rhs - another stream object.
     */
    void Assign(const LLBC_Stream &rhs);

    /**
     * Buffer assign operation.
     * @param[in] buf - buffer.
     * @param[in] len - buffer size.
     */
    void Assign(void *buf, size_t len);

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
     * Swap two stream objects.
     * @param[in/out] another - another stream.
     */
    void Swap(LLBC_Stream &another);

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
    bool Skip(int size);

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

    /**
     * Insert buffer to current string.
     * Note: 1. performance warning.
     *       2. Do not try to insert data to attach attribute's stream.
     * @param[in] pos - stream insert position, if pos > this->pos, will auto adjust to this->pos.
     * @param[in] buf - will insert buffer.
     * @param[in] len - buffer length.
     */
    void Insert(size_t pos, const void *buf, size_t len);

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
     * @param[in] n0  - stream begin position.
     * @param[in] n1  - stream end position, if is npos, will replace [n0, end) range stream buffer.
     * @param[in] buf - buffer.
     * @param[in] len - buffer length.
     */
    void Replace(size_t n0, size_t n1, const void *buf, size_t len);

    /**
     * Read buffer data from stream.
     * @param[out] buf  - buffer pointer.
     * @param[in]  size - require read size, in bytes.
     * @return bool - return true if successed, otherwise return false.
     */
    bool Read(void *buf, size_t size);

    /**
     * Write buffer data to stream.
     * @param[in] buf - buffer pointer.
     * @param[in] len - buffer size, in bytes.
     */
    void Write(const void *buf, size_t len);

public:
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

        obj = *(reinterpret_cast<T *>(reinterpret_cast<char *>(_buf) + _pos));
        _pos += sizeof(T);

        if (_endian != LLBC_MachineEndian)
            LLBC_ReverseBytes(obj);

        return true;
    }

    /**
     * Read STL string type object from stream.
     * @param[out] obj - already read object.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<std::is_base_of<T, std::string>::value, bool>::type
    Read(T &obj)
    {
        uint32 len = 0;
        if (!Read(len))
            return false;

        obj.clear();
        if (len == 0)
            return true;

        obj.resize(len);
        return Read(const_cast<char *>(obj.data()), obj.size());
    }

    /**
     * Read object from stream.
     * @param[out] obj - already read object.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<!std::is_arithmetic<T>::value &&
                            !std::is_base_of<T, std::string>::value, bool>::type
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
     * Try adapt T::ParseFrommArray(protobuf message obj).
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
     * Try adapt STL unsorted unary container(std::list/std::vector/std::deque/...).
     */
    template <typename T,
              void (T::*)(const typename T::value_type &),
              void (T::*)(typename T::size_type)>
    struct stl_unsorted_unary_container;
    template <typename T>
    bool ReadImpl(T &obj, stl_unsorted_unary_container<T, &T::push_back, &T::resize> *)
    {
        uint32 count;
        if (!Read(count))
            return false;

        obj.clear();
        if (count == 0)
            return true;

        typename T::value_type val;
        for (uint32 i = 0; i < count; ++i)
        {
            if (!Read(val))
                return false;

            obj.emplace_back(val);
        }

        return true;
    }

    /**
     * Try adapt STL sorted unary container(std::set/std::unordered_set/...).
     */
    template <typename T>
    typename std::enable_if<std::is_same<typename T::key_type,
                                         typename T::value_type>::value,
                            bool>::type
    ReadImpl(T &obj, T *)
    {
        uint32 count;
        if (!Read(count))
            return false;

        obj.clear();
        if (count == 0)
            return true;

        typename T::key_type key;
        for (uint32 i = 0; i < count; ++i)
        {
            if (!Read(key))
                return false;

            obj.emplace(key);
        }

        return true;
    }

    /**
     * Try adapt STL binary container.
     */
    template <typename T>
    typename std::enable_if<std::is_same<typename T::value_type,
                                         std::pair<const typename T::key_type, typename T::mapped_type>>::value,
                            bool>::type
    ReadImpl(T &obj, T *)
    {
        uint32 count;
        if (!Read(count))
            return false;

        obj.clear();
        if (count == 0)
            return true;

        typename T::key_type key;
        typename T::mapped_type mapped;
        for (uint32 i = 0; i < count; ++i)
        {
            if (!Read(key) ||
                !Read(mapped))
                return false;

            obj.emplace(key, mapped);
        }

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
        AutoRecap(sizeof(T));
        if (_endian != LLBC_MachineEndian)
        {
            T obj2(obj);
            LLBC_ReverseBytes(obj2);
            memcpy(reinterpret_cast<char *>(_buf) + _pos, &obj2, sizeof(T));
        }
        else
        {
            memcpy(reinterpret_cast<char *>(_buf) + _pos, &obj, sizeof(T));
        }

        _pos += sizeof(T);
    }

    /**
     * Write std::string type object to stream.
     * @param[in] obj - the will write object.
     */
    template <typename T>
    typename std::enable_if<std::is_base_of<std::string, T>::value, void>::type
    Write(const T &obj)
    {
        Write(static_cast<uint32>(obj.size()));
        Write(obj.data(), obj.size());
    }

    /**
     * Write object to stream.
     * @param[in] obj - the will write object.
     */
    template <typename T>
    typename std::enable_if<!std::is_arithmetic<T>::value &&
                            !std::is_base_of<std::string, T>::value, void>::type
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
     * Try adapt T::SerializeToArray(protobuf message object).
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
     * Try adapt STL unsorted unary container(std::list/std::vector/std::deque/...).
     */
    template <typename T>
    void WriteImpl(const T &obj, stl_unsorted_unary_container<T, &T::push_back, &T::resize> *)
    {
        Write(static_cast<uint32>(obj.size()));
        for (typename T::const_iterator it = obj.begin(); it != obj.end(); ++it)
            Write(*it);
    }

    /**
     * Try adapt STL sorted unary container(std::set, std::unordered_set/...).
     */
    template <typename T>
    typename std::enable_if<std::is_same<typename T::key_type,
                                         typename T::value_type>::value,
                            void>::type
    WriteImpl(const T &obj, T *)
    {
        Write(static_cast<uint32>(obj.size()));
        for (typename T::const_iterator it = obj.begin(); it != obj.end(); ++it)
            Write(*it);
    }

    /**
     * Try adapt STL binary container(std::map/std::unordered_map/...).
     */
    template <typename T>
    typename std::enable_if<std::is_same<typename T::value_type,
                                         std::pair<const typename T::key_type, typename T::mapped_type> >::value,
                            void>::type
    WriteImpl(const T &obj, T *)
    {
        Write(static_cast<uint32>(obj.size()));
        for (typename T::const_iterator it = obj.begin(); it != obj.end(); ++it)
        {
            Write(it->first);
            Write(it->second);
        }
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
        bool ret = Read(obj);
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

public:
    LLBC_Stream &operator=(const LLBC_Stream &rhs);

private:
    bool AutoRecap(size_t needCap);
    bool OverlappedCheck(const void *another, size_t len) const;

private:
    sint8 *_buf;
    size_t _pos;
    size_t _cap;

    int _endian;
    bool _attach;

    LLBC_IObjectPoolInst *_poolInst;
};

__LLBC_NS_END

#include "llbc/common/StreamInl.h"


