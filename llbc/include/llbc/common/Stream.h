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
#include "llbc/common/StringDataType.h"
#include "llbc/common/TemplateDeduction.h"

/** Some stream helper macros define **/
/*  Deserialize/Read about macros define  */
// Begin read macro define, use to simple begin read object.
#define LLBC_STREAM_BEGIN_READ(stream, failRetType, failRetVal) \
    do {                                                        \
        failRetType __r_failRet = (failRetVal);                 \
        LLBC_NAMESPACE LLBC_Stream &__r_stream = (stream)       \

// Deserialize macro define.
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
template <typename Obj>
class LLBC_TypedObjPool;

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
     * Parameter constructor, constructor will allocate cap bytes in stream.
     * @param[in] cap - buffer cap, in bytes.
     */
    explicit LLBC_Stream(size_t cap);

    /**
     * Move constructor.
     */
    LLBC_Stream(LLBC_Stream &&rhs) noexcept;

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
     * Get stream READ position.
     * @return size_t - READ position.
     */
    size_t GetReadPos() const;

    /**
     * Get stream WRITE position.
     * @return size_t - WRITE position.
     */
    size_t GetWritePos() const;

    /**
     * Set stream READ position.
     * @param[in] readPos - new READ position, must be <= WRITE pos.
     * @return bool - return true if success, otherwise return false.
     */
    bool SetReadPos(size_t readPos);

    /**
     * Set stream WRITE position.
     * @param[in] writePos - new WRITE position, must be <= stream capacity.
     * @return bool - return true if success, otherwise return false.
     */
    bool SetWritePos(size_t writePos);

    /**
     * Skip the buffer READ position.
     * @param[in] skipSize - will skip's size, in bytes.
     * @return bool - return true if success, otherwise return false.
     */
    bool SkipRead(sint64 skipSize);

    /**
     * Skip the buffer WRITE position.
     * @param[in] skipSize - will skip's size, in bytes.
     * @return bool - return true if success, otherwise return false.
     */
    bool SkipWrite(sint64 skipSize);

    /**
     * Get stream capacity.
     * @return size_t - the stream buffer capacity, in bytes.
     */
    size_t GetCap() const;

    /**
     * Get readable size.
     * @return size_t - the readable size, in bytes.
     */
    size_t GetReadableSize() const;

    /**
     * Get writable size.
     * @return size_t - the writable size, in bytes.
     */
    size_t GetWritableSize() const;

    /**
     * Get current buffer(mutable).
     * @return T * - current buffer pointer.
     */
    template <typename T = void>
    T *GetBuf();
    /**
     * Get current buffer(const).
     * @return const T * - current buffer pointer.
     */
    template <typename T = void>
    const T *GetBuf() const;

    /**
     * Get buffer start with READ position(mutable).
     * @return T * - buffer pointer.
     */
    template <typename T = void>
    T *GetBufStartWithReadPos();
    /**
     * Get buffer start with READ position(const).
     * @return const T * - buffer pointer.
     */
    template <typename T = void>
    const T *GetBufStartWithReadPos() const;

    /**
     * Get buffer start with WRITE position(mutable).
     * @return const T * - buffer pointer.
     */
    template <typename T = void>
    T *GetBufStartWithWritePos();
    /**
     * Get buffer pointer start with WRITE position.
     * @return const T * - buffer pointer.
     */
    template <typename T = void>
    const T *GetBufStartWithWritePos() const;

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
    T Read();

    /**
     * Read arithmetic/enumeration type object from stream.
     * @param[out] obj - already read object.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value ||
                            std::is_enum<T>::value, bool>::type
    Read(T &obj);

    /**
     * Forbid string pointer read operation.
     */
    template <typename T>
    typename std::enable_if<std::is_pointer<T>::value &&
                                std::is_same<typename LLBC_ExtractPureType<T>::type, char>::value,
                            bool>::type
    Read(T &obj);

    /**
     * Read void pointer from stream.
     * @param[out] voidPtr - the void pointer.
     * @return bool - return 0 if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<std::is_pointer<T>::value &&
                                std::is_same<typename LLBC_ExtractPureType<T>::type, void>::value,
                            bool>::type
    Read(T &voidPtr);

    /**
     * Read non string pointer from stream.
     * @param[out] ptr - the non string pointer.
     * @return bool - return 0 if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<std::is_pointer<T>::value &&
                                !std::is_same<typename LLBC_ExtractPureType<T>::type, char>::value &&
                                !std::is_same<typename LLBC_ExtractPureType<T>::type, void>::value,
                            bool>::type
    Read(T &ptr);

    /**
     * Read char array from stream.
     * @param[out] arr - char array.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T, size_t _ArrLen>
    typename std::enable_if<std::is_arithmetic<T>::value &&
                                std::is_same<T, char>::value,
                            bool>::type
    Read(T(&arr)[_ArrLen]);

    /**
     * Read uint8/bool array from stream.
     * @param[out] arr - uint8/bool array.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T, size_t _ArrLen>
    typename std::enable_if<std::is_arithmetic<T>::value &&
                                (std::is_same<T, uint8>::value ||
                                 std::is_same<T, bool>::value),
                            bool>::type
    Read(T(&arr)[_ArrLen]);

    /**
     * Read non sint8/uint8/bool arithmetic array or enumeration array from stream.
     * @param[out] arr - arithmetic/enumeration array.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T, size_t _ArrLen>
    typename std::enable_if<(std::is_arithmetic<T>::value &&
                             (!std::is_same<T, char>::value &&
                              !std::is_same<T, uint8>::value &&
                              !std::is_same<T, bool>::value)) ||
                                std::is_enum<T>::value,
                            bool>::type
    Read(T(&arr)[_ArrLen]);

    /**
     * Read non arithmetic array and non enumeration array from stream.
     * @param[out] arr - array.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T, size_t _ArrLen>
    typename std::enable_if<!std::is_arithmetic<T>::value &&
                            !std::is_enum<T>::value, bool>::type
    Read(T(&arr)[_ArrLen]);

    /**
     * Read std::basic_string template spec type object from stream.
     * @param[out] str - already read object.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::basic_string>::value ||
                                LLBC_IsTemplSpec<T, LLBC_BasicString>::value,
                            bool>::type
    Read(T &str);

    /**
     * Read std::vector/std::list/std::deque template spec type object from stream.
     * @param[out] container - already read object.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::vector>::value ||
                                LLBC_IsTemplSpec<T, std::list>::value ||
                                LLBC_IsTemplSpec<T, std::deque>::value,
                            bool>::type
    Read(T &container);

    /**
     * Read std::queue/std::stack template spec type object from stream.
     * @param[out] container - already read object.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::queue>::value ||
                                LLBC_IsTemplSpec<T, std::stack>::value,
                            bool>::type
    Read(T &container);

    /**
     * Read std::set/std::unordered_set template spec type object from stream.
     * @param[out] container - already read object.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::set>::value ||
                                LLBC_IsTemplSpec<T, std::unordered_set>::value,
                            bool>::type
    Read(T &container);

    /**
     * Read std::map/std::unordered_map template spec type object from stream.
     * @param[out] container - already read object.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::map>::value ||
                                LLBC_IsTemplSpec<T, std::unordered_map>::value,
                            bool>::type
    Read(T &container);

private:
    /**
     * \brief std::array reader template define.
     */
    template <size_t N>
    struct __LLBC_STLArrayReader
    {
        template <typename Arr>
        static bool Read(Arr &arr, LLBC_NS LLBC_Stream &stream)
        {
            if (UNLIKELY(!stream.Read(arr[N - 1])))
                return false;
    
            return __LLBC_STLArrayReader<N - 1>::Read(arr, stream);
        }
    };

public:
    /**
     * Read std::array template spec type object from stream.
     * @param[out] arr - already read object.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsSTLArraySpec<T, std::array>::value, bool>::type
    Read(T &arr);

private:
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

public:
    /**
     * Read std::tuple template spec type object from stream.
     * @param[out] tup - already read object.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::tuple>::value, bool>::type
    Read(T &tup);

    /**
     * Read std::pair template spec type object from stream.
     * @param[out] p - already read object.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::pair>::value, bool>::type
    Read(T &p);

    /**
     * Read object from stream.
     * @param[out] obj - already read object.
     * @return bool - return true if success, otherwise return false.
     */
    template <typename T>
    typename std::enable_if<!std::is_arithmetic<T>::value &&
                                !std::is_enum<T>::value &&
                                !std::is_pointer<T>::value &&
                                !std::is_array<T>::value &&
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
                            bool>::type
    Read(T &obj);

private:
    /**
     * Try adapt T::DeSerialize.
     */
    template <typename T, bool (T::*)(LLBC_Stream &)>
    struct upper_camel_case_deserializable_type;
    template <typename T>
    bool ReadImpl(T &obj, upper_camel_case_deserializable_type<T, &T::DeSerialize> *);

    /**
     * Try adapt T::Deserialize.
     */
    template <typename T>
    bool ReadImpl(T &obj, upper_camel_case_deserializable_type<T, &T::Deserialize> *);

    /**
     * Try adapt T::DeSer.
     */
    template <typename T, bool (T::*)(LLBC_Stream &)>
    struct upper_camel_case_short_deserializable_type;
    template <typename T>
    bool ReadImpl(T &obj, upper_camel_case_short_deserializable_type<T, &T::DeSer> *);

    /**
     * Try adapt T::Deser.
     */
    template <typename T>
    bool ReadImpl(T &obj, upper_camel_case_short_deserializable_type<T, &T::Deser> *);

    /**
     * Try adapt T::deserialize.
     */
    template <typename T, bool (T::*)(LLBC_Stream &)>
    struct lower_camel_case_deserializable_type;
    template <typename T>
    bool ReadImpl(T &obj, lower_camel_case_deserializable_type<T, &T::deserialize> *);

    /**
     * Try adapt T::deser.
     */
    template <typename T, bool (T::*)(LLBC_Stream &)>
    struct lower_camel_case_short_deserializable_type;
    template <typename T>
    bool ReadImpl(T &obj, lower_camel_case_short_deserializable_type<T, &T::deser> *);

    /**
     * Try adapt protobuf2 mesage object.
     */
    template <typename T, bool (T::*)() const, int (T::*)() const>
    class protobuf2_type;
    template <typename T>
    bool ReadImpl(T &obj, protobuf2_type<T, &T::IsInitialized, &T::ByteSize> *);

    /**
     * Try adapt protobuf3 mesage object.
     */
    template <typename T, bool (T::*)() const, size_t (T::*)() const>
    class protobuf3_type;
    template <typename T>
    bool ReadImpl(T &obj, protobuf3_type<T, &T::IsInitialized, &T::ByteSizeLong> *);

    /**
     * Final Read implement: memcpy obj memory.
     */
    template <typename T>
    bool ReadImpl(T &obj, ...);

public:
    /**
     * Write arithmetic type object to stream.
     * @param[in] obj - the will write object.
     */
    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value ||
                                std::is_enum<T>::value,
                            void>::type
    Write(const T &obj);

    /**
     * Write string pointer to stream.
     * @param[in] str - the string pointer.
     */
    template <typename T>
    typename std::enable_if<std::is_pointer<T>::value &&
                                std::is_same<typename LLBC_ExtractPureType<T>::type, char>::value,
                            void>::type
    Write(const T &str);

    /**
     * Write void pointer to stream.
     * @param[in] voidPtr - the void pointer.
     */
    template <typename T>
    typename std::enable_if<std::is_pointer<T>::value &&
                                std::is_same<typename LLBC_ExtractPureType<T>::type, void>::value,
                            void>::type
    Write(const T &voidPtr);

    /**
     * Write non string pointer to stream.
     * @param[in] ptr - the non string pointer.
     */
    template <typename T>
    typename std::enable_if<std::is_pointer<T>::value &&
                                !std::is_same<typename LLBC_ExtractPureType<T>::type, char>::value &&
                                !std::is_same<typename LLBC_ExtractPureType<T>::type, void>::value,
                            void>::type
    Write(const T &ptr);

    /**
     * Write char array to stream.
     * @param[in] arr - the char arithmetic array.
     */
    template <typename T, size_t _ArrLen>
    typename std::enable_if<std::is_arithmetic<T>::value &&
                                std::is_same<T, char>::value,
                            void>::type
    Write(const T(&arr)[_ArrLen]);

    /**
     * Write uint8/bool array to stream.
     * @param[in] arr - the uint8/bool arithmetic array.
     */
    template <typename T, size_t _ArrLen>
    typename std::enable_if<std::is_arithmetic<T>::value &&
                                (std::is_same<T, uint8>::value ||
                                 std::is_same<T, bool>::value),
                            void>::type
    Write(const T(&arr)[_ArrLen]);

    /**
     * Write non-char arithmetic array to stream.
     * @param[in] arr - the non-char arithmetic array.
     */
    template <typename T, size_t _ArrLen>
    typename std::enable_if<(std::is_arithmetic<T>::value &&
                             (!std::is_same<T, char>::value &&
                              !std::is_same<T, uint8>::value &&
                              !std::is_same<T, bool>::value)) ||
                                !std::is_arithmetic<T>::value,
                            void>::type
    Write(const T(&arr)[_ArrLen]);

    /**
     * Write std::basic_string template spec type object to stream.
     * @param[in] str - the will write object.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::basic_string>::value ||
                                LLBC_IsTemplSpec<T, LLBC_BasicString>::value,
                            void>::type
    Write(const T &str);

    /**
     * Write std::vector/std::list/std::deque/std::set/std::stack template spec type object to stream.
     * @param[in] container - the will write object.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::vector>::value ||
                                LLBC_IsTemplSpec<T, std::list>::value ||
                                LLBC_IsTemplSpec<T, std::deque>::value ||
                                LLBC_IsTemplSpec<T, std::set>::value ||
                                LLBC_IsTemplSpec<T, std::unordered_set>::value,
                            void>::type
    Write(const T &container);

    /**
     * !!! Forbid std::queue/std::stack template spec type object write !!!.
     * @param[in] container - the will write object.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::queue>::value ||
                                LLBC_IsTemplSpec<T, std::stack>::value,
                            void>::type
    Write(const T &container);

    /**
     * Write std::map/std::unordered_map template spec type object to stream.
     * @param[in] container - the will write object.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::map>::value ||
                                LLBC_IsTemplSpec<T, std::unordered_map>::value,
                            void>::type
    Write(const T &container);

private:
    /**
     * \brief std::array writer template define.
     */
    template <size_t N>
    struct __LLBC_STLArrayWriter
    {
        template <typename Arr>
        static void Write(const Arr &arr, LLBC_NS LLBC_Stream &stream)
        {
            stream.Write(arr[N - 1]);
            __LLBC_STLArrayWriter<N - 1>::Write(arr, stream);
        }
    };

public:
    /**
     * Write std::array template spec type object to stream.
     * @param[in] arr - the will write object.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsSTLArraySpec<T, std::array>::value, void>::type
    Write(const T &arr);

private:
    /**
     * \brief std::tuple writer template define.
     */
    template <size_t N>
    struct __LLBC_TupleWriter
    {
        template <typename Tup>
        static void Write(const Tup &tup, LLBC_Stream &stream)
        {
            stream.Write(std::get<N - 1>(tup));
            __LLBC_TupleWriter<N - 1>::Write(tup, stream);
        }
    };

public:
    /**
     * Write std::tuple template spec type object to stream.
     * @param[in] tup - the will write object.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::tuple>::value, void>::type
    Write(const T &tup);

    /**
     * Write std::pair template spec type object to stream.
     * @param[in] p - the will write object.
     */
    template <typename T>
    typename std::enable_if<LLBC_IsTemplSpec<T, std::pair>::value, void>::type
    Write(const T &p);

    /**
     * Write object to stream.
     * @param[in] obj - the will write object.
     */
    template <typename T>
    typename std::enable_if<!std::is_arithmetic<T>::value &&
                                !std::is_enum<T>::value &&
                                !std::is_pointer<T>::value &&
                                !std::is_array<T>::value &&
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
                            void>::type
    Write(const T &obj);

private:
    /**
     * Try adapt T::Serialize.
     */
    template <typename T, void (T::*)(LLBC_Stream &) const>
    struct upper_camel_case_serializable_type;
    template <typename T>
    void WriteImpl(const T &obj, upper_camel_case_serializable_type<T, &T::Serialize> *);

    /**
     * Try adapt T::Ser.
     */
    template <typename T, void (T::*)(LLBC_Stream &) const>
    struct upper_camel_case_short_serializable_type;
    template <typename T>
    void WriteImpl(const T &obj, upper_camel_case_short_serializable_type<T, &T::Ser> *);

    /**
     * Try adapt T::serialize.
     */
    template <typename T, void (T::*)(LLBC_Stream &) const>
    struct lower_camel_case_serializable_type;
    template <typename T>
    void WriteImpl(const T &obj, lower_camel_case_serializable_type<T, &T::serialize> *);

    /**
     * Try adapt T::ser.
     */
    template <typename T, void (T::*)(LLBC_Stream &) const>
    struct lower_camel_case_short_serializable_type;
    template <typename T>
    void WriteImpl(const T &obj, lower_camel_case_short_serializable_type<T, &T::ser> *);

    /**
     * Try adapt protobuf2 message object.
     */
    template <typename T>
    void WriteImpl(const T &obj, protobuf2_type<T, &T::IsInitialized, &T::ByteSize> *);

    /**
     * Try adapt protobuf3 message object.
     */
    template <typename T>
    void WriteImpl(const T &obj, protobuf3_type<T, &T::IsInitialized, &T::ByteSizeLong> *);

    /**
     * Final write implement: memcpy obj memory.
     */
    template <typename T>
    void WriteImpl(const T &obj, ...);

public:
    /**
     * Peek data[use Read()], but don't modify stream object.
     * @param[out] obj - will deserialize's object.
     * @return bool - return true if successed, otherwise return false.
     */
    template <typename T>
    bool Peek(T &obj);

    /**
     * Stream input overlapped functions define.
     * @param[out] val - value reference.
     */
    template<typename T>
    LLBC_Stream &operator>>(T &val);

    /**
     * Stream output overlapped functions define.
     * @param[in] val - const value reference.
     */
    template<typename T>
    LLBC_Stream &operator<<(const T &val);

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
     * Object-Pool reflection support.
     */
    LLBC_TypedObjPool<LLBC_Stream> *GetTypedObjPool() const;
    void SetTypedObjPool(LLBC_TypedObjPool<LLBC_Stream> *typedObjPool);

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
    LLBC_Stream &operator=(LLBC_Stream &&rhs) noexcept;

private:
    bool ReserveWritableSize(size_t writableSize);

private:
    sint8 *_buf;
    size_t _readPos;
    size_t _writePos;
    size_t _cap;

    int _endian;
    bool _attach;

    LLBC_TypedObjPool<LLBC_Stream> *_typedObjPool;
};

__LLBC_NS_END

/**
 * Stream output operator support. 
 */
std::ostream &operator<<(std::ostream &o, const LLBC_NS LLBC_Stream &stream);

#include "llbc/common/StreamInl.h"

