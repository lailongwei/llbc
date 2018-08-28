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

#ifndef __LLBC_COM_STREAM_H__
#define __LLBC_COM_STREAM_H__

#include "llbc/common/PFConfig.h"

#include "llbc/common/Macro.h"
#include "llbc/common/BasicDataType.h"

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
    if (!__r_stream.ReadBuffer(buf, len)) {                     \
        return __r_failRet;                                     \
    }                                                           \

// DeSerializeEx macro define.
#define LLBC_STREAM_READ_EX(field)                              \
    if (!__r_stream.ReadEx(field)) {                            \
        return __r_failRet;                                     \
    }                                                           \

// Peek macro define.
#define LLBC_STREAM_PEEK(field)                                 \
    if (!__r_stream.Peek(field)) {                              \
        return __r_failRet;                                     \
    }                                                           \

// PeekEx macro define.
#define LLBC_STREAM_PEEK_EX(field)                              \
    if (!__r_stream.PeekEx(field)) {                            \
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
    __w_stream.WriteBuffer(buf, len)                            \

// SerializeEx macro define.
#define LLBC_STREAM_WRITE_EX(field)                             \
    __w_stream.WriteEx(field)                                   \

// Fill macro define ,use to fill some null bytes to stream.
#define LLBC_STREAM_FILL(size)                                  \
    __w_stream.Fill(size)                                       \

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
 * \brief Stream class encapsulation, support serialize/deserialize operations.
 */
class LLBC_Stream
{
public:
    static const size_t npos = -1;

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
     * Parameter constructor, constructor will allocate size bytes in stream.
     * @param[in] size - buffer size, in bytes.
     */
    explicit LLBC_Stream(size_t size);

    /**
     * Parameter constructor, will attach external buffer.
     * @param[in] buf - external buffer pointer, not null.
     * @param[in] len - indicate buffer size, in bytes, must greater than 0.
     */
    LLBC_Stream(void *buf, size_t len);

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
     * @param[in] len - indicate external buffer size, in bytes, must greater or equal than 0.
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
    void SetAttachAttr(bool attach);

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
     * @return bool    - return true if skip operation successed, otherwise return false.
     */
    bool Skip(long size);

    /**
     * Fill stream buffer, use '\0' to fill.
     * @param[in] size - the will fill buffer size, in bytes.
     */
    void Fill(size_t size);

    /**
     * Get current stream buffer size.
     * @return size_t - the stream buffer size, in bytes.
     */
    size_t GetSize() const;

    /**
     * Get current buffer pointer.
     * @return void * - current buffer pointer.
     */
    void *GetBuf() const;

    /**
     * Get buffer pointer start with position.
     * @return void * - buffer pointer.
     */
    void *GetBufStartWithPos() const;

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
     * Replace stream's buffer.
     * Note: 1. Performance warning.
     *       2. Do not try to replace attach attribute's stream.
     * @param[in] n0  - stream begin position.
     * @param[in] n1  - stream end position.
     * @param[in] buf - buffer.
     * @param[in] len - buffer length.
     */
    void Replace(size_t n0, size_t n1, const void *buf, size_t len);

    /**
     * Write buffer data to stream.
     * @param[in] buf - buffer pointer.
     * @param[in] len - buffer size, in bytes.
     */
    void WriteBuffer(const void *buf, size_t len);

    /**
     * Read buffer data from stream.
     * @param[out] buf  - buffer pointer.
     * @param[in]  size - require read size, in bytes.
     * @return bool - return true if successed, otherwise return false.
     */
    bool ReadBuffer(void *buf, size_t size);

    /**
     * Write template function, will automatch functio to write, if
     * this class exist Serialize method, will call Serialize method
     * to write, else use WriteBuffer(&obj, sizeof(obj)).
     * @param[in] obj - will serialize's object(can be set to any type's object).
     */
    template <typename T>
    void Write(const T &obj)
    {
        this->write_data<T>(obj, 0);
    }

    template <typename T, void (T::*)(LLBC_Stream &) const>
    struct write_fun;

    /**
     * T::Serialize method impl.
     */
    template <typename T>
    void write_data(const T &obj, write_fun<T, &T::Serialize> *)
    {
        obj.Serialize(*this);
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    /**
     * Adapted pair container, like: map.
     */
    template <typename T, typename T::iterator (T::*)(typename T::iterator, const std::pair<const typename T::key_type, typename T::referent_type> &)>
    struct adapted_stl_pair_write_fun;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
#if LLBC_TARGET_PLATFORM_NON_WIN32
    /**
     * Adapted non-pair container, like: vector, list, deque(not include queue, stack).
     * adapt insert() function.
     */
    template <typename T, typename T::iterator (T::*)(typename T::iterator, const typename T::value_type &)>
    struct adapted_stl_non_pair_write_fun;
#else // LLBC_TARGET_PLATFORM_WIN32
    /**
     * Adapted non-pair container, like: vector, list, deque(not include queue, stack, set).
     * adapt const version begin() and non-const begin() functions.
     */
    template <typename T, typename T::iterator (T::*)(), typename T::const_iterator (T::*)() const>
    struct adapted_stl_non_pair_write_fun;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

    /**
     * Adapted stl container type.
     */
    template <typename T>
    void write_data(const T &obj, ...)
    {
        return this->adapt_write_data<T>(obj, 0);
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32 
    /**
     * STL container pair type write method impl.
     */
    template <typename T>
    void adapt_write_data(const T &obj, adapted_stl_pair_write_fun<T, &T::insert> *)
    {
        this->Write<uint32>(static_cast<uint32>(obj.size()));
        typename T::const_iterator iter = obj.begin();
        for (; iter != obj.end(); iter++)
        {
            typedef typename T::key_type KeyType;
            typedef typename T::referent_type ValType;
            
            const KeyType &key = iter->first;
            this->Write<KeyType>(key);

            const ValType &val = iter->second;
            this->Write<ValType>(val);
        }
    }
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

    /**
     * STL container non-pair type write method impl.
     */
    template <typename T>
#if LLBC_TARGET_PLATFORM_NON_WIN32
    void adapt_write_data(const T &obj, adapted_stl_non_pair_write_fun<T, &T::insert> *)
#else
    void adapt_write_data(const T &obj, adapted_stl_non_pair_write_fun<T, &T::begin, &T::end> *)
#endif
    {
        this->Write<uint32>(static_cast<uint32>(obj.size()));
        typename T::const_iterator iter = obj.begin();
        for (; iter != obj.end(); iter++)
        {
            typedef typename T::value_type ElemType;

            const ElemType &elem = *iter;
            this->Write<ElemType>(elem);
        }
    }

    /**
     * Raw type write method impl.
     */
    template <typename T>
    void adapt_write_data(const T &obj, ...)
    {
        this->WriteBuffer(&obj, sizeof(obj));
    }

    /**
     * Write template function, will automatch function to write, see Write
     * method, but math method name is SerializeEx.
     * @param[in] obj - will serialize's object.
     */
    template <typename T>
    void WriteEx(const T &obj)
    {
        this->write_data_ex<T>(obj, 0);
    }

    /**
     * T::SerializeEx method impl.
     */
    template <typename T>
    void write_data_ex(const T &obj, write_fun<T, &T::SerializeEx> *)
    {
        obj.SerializeEx(*this);
    }

    /**
     * Adapted stl container type.
     */
    template <typename T>
    void write_data_ex(const T &obj, ...)
    {
        return this->adapt_write_data_ex<T>(obj, 0);
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    /**
     * STL container pair type writeex method impl.
     */
    template <typename T>
    void adapt_write_data_ex(const T &obj, adapted_stl_pair_write_fun<T, &T::insert> *)
    {
        this->WriteEx<uint32>(static_cast<uint32>(obj.size()));
        typename T::const_iterator iter = obj.begin();
        for (; iter != obj.end(); iter++)
        {
            typedef typename T::key_type KeyType;
            typedef typename T::referent_type ValType;

            const KeyType &key = iter->first;
            this->WriteEx<KeyType>(key);

            const ValType &val = iter->second;
            this->WriteEx<ValType>(val);
        }
    }
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

    /**
     * STL container non-pair type writeex method impl.
     */
    template <typename T>
#if LLBC_TARGET_PLATFORM_NON_WIN32
    void adapt_write_data_ex(const T &obj, adapted_stl_non_pair_write_fun<T, &T::insert> *)
#else
    void adapt_write_data_ex(const T &obj, adapted_stl_non_pair_write_fun<T, &T::begin, &T::end> *)
#endif
    {
        this->WriteEx<uint32>(static_cast<uint32>(obj.size()));
        typename T::const_iterator iter = obj.begin();
        for (; iter != obj.end(); iter++)
        {
            typedef typename T::value_type ElemType;

            const ElemType &elem = *iter;
            this->WriteEx<ElemType>(elem);
        }
    }

    /**
     * Raw type writeex method impl.
     */
    template <typename T>
    void adapt_write_data_ex(const T&obj, ...)
    {
        this->WriteBuffer(&obj, sizeof(obj));
    }

    /**
     * Read template function, will automatch function to read, if this class
     * exist DeSerialize method, will call DeSerialize method to read, otherwise
     * use ReadBuffer(&obj, sizeof(obj)).
     * @param[out] obj - will deserialize's object.
     * @return bool - return true if successed, otherwise return false.
     */
    template <typename T>
    bool Read(T &obj)
    {
        return this->read_data<T>(obj, 0);
    }

    template <typename T, bool (T::*)(LLBC_Stream &)>
    struct read_fun;

    /**
     * T::DeSerialize method impl.
     */
    template <typename T>
    bool read_data(T &obj, read_fun<T, &T::DeSerialize> *)
    {
        return obj.DeSerialize(*this);
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    /**
     * Adapted pair container, like: map.
     */
    template <typename T, typename T::iterator (T::*)(typename T::iterator, const std::pair<const typename T::key_type, typename T::referent_type> &)>
    struct adapted_stl_pair_read_fun;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
#if LLBC_TARGET_PLATFORM_NON_WIN32
    /**
     * Adapted non-pair container, like: vector, list, deque(not include queue, stack).
     * adapt insert() function.
     */
    template <typename T, typename T::iterator (T::*)(typename T::iterator, const typename T::value_type &)>
    struct adapted_stl_non_pair_read_fun;
#else // LLBC_TARGET_PLATFORM_WIN32.
    /**
     * Adapted non-pair container, like: vector, list, deque(not include queue, stack, set).
     * adapt const version begin() and non-const begin() functions.
     */
    template <typename T, typename T::iterator (T::*)(), typename T::const_iterator (T::*)() const>
    struct adapted_stl_non_pair_read_fun;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

    /**
     * Adapted stl container type.
     */
    template <typename T>
    bool read_data(T &obj, ...)
    {
        return this->adapt_read_data<T>(obj, 0);
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    /**
     * STL container pair type read method impl.
     */
    template <typename T>
    bool adapt_read_data(T &obj, adapted_stl_pair_read_fun<T, &T::insert> *)
    {
        obj.clear();

        uint32 count;
        if (!this->Read<uint32>(count))
            return false;

        typedef typename T::key_type KeyType;
        typedef typename T::referent_type ValType;
        for (register uint32 i = 0; i < count; i++)
        {
            KeyType key;
            if (!this->Read<KeyType>(key))
                return false;

            ValType val;
            if (!this->Read<ValType>(val))
                return false;

            obj.insert(std::make_pair(key, val));
        }

        return true;
    }
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

    /**
     * STL container non-pair type read method impl.
     */
    template <typename T>
#if LLBC_TARGET_PLATFORM_NON_WIN32
    bool adapt_read_data(T &obj, adapted_stl_non_pair_read_fun<T, &T::insert> *)
#else
    bool adapt_read_data(T &obj, adapted_stl_non_pair_read_fun<T, &T::begin, &T::end> *)
#endif
    {
        obj.clear();

        uint32 count;
        if (!this->Read<uint32>(count))
            return false;

        for (register uint32 i = 0; i < count; i++)
        {
            typedef typename T::value_type ElemType;

            ElemType elem;
            if (!this->Read<ElemType>(elem))
                return false;

            obj.insert(obj.end(), elem);
        }

        return true;
    }

    /**
     * Raw type read method impl.
     */
    template <typename T>
    bool adapt_read_data(T &obj, ...)
    {
        if (_size >= _pos + sizeof(T))
            return this->ReadBuffer(&obj, sizeof(T));

        return false;
    }

    /**
     * Read template function, like Read() function, but automatch function is
     * DeSerializeEx, see Read() function brief
     * @param[out] obj - will deserialize's object.
     * @return bool - return true if successed, otherwise return false.
     */
    template <typename T>
    bool ReadEx(T &obj)
    {
        return this->read_data_ex<T>(obj, 0);
    }

    /**
     * T::DeSerializeEx method impl.
     */
    template <typename T>
    bool read_data_ex(T &obj, read_fun<T, &T::DeSerializeEx> *)
    {
        return obj.DeSerializeEx(*this);
    }

    /**
     * Adapted stl container type.
     */
    template <typename T>
    bool read_data_ex(T &obj, ...)
    {
        return this->adapt_read_data_ex<T>(obj, 0);
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    /**
     * STL container pair type readex method impl.
     */
    template <typename T>
    bool adapt_read_data_ex(T &obj, adapted_stl_pair_read_fun<T, &T::insert> *)
    {
        obj.clear();

        uint32 count;
        if (!this->ReadEx<uint32>(count))
            return false;

        typedef typename T::key_type KeyType;
        typedef typename T::referent_type ValType;
        for (register uint32 i = 0; i < count; i++)
        {
            KeyType key;
            if (!this->ReadEx<KeyType>(key))
                return false;

            ValType val;
            if (!this->ReadEx<ValType>(val))
                return false;

            obj.insert(std::make_pair(key, val));
        }

        return true;
    }
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

    /**
     * STL container non-pair type readex method impl.
     */
    template <typename T>
#if LLBC_TARGET_PLATFORM_NON_WIN32
    bool adapt_read_data_ex(T &obj, adapted_stl_non_pair_read_fun<T, &T::insert> *)
#else
    bool adapt_read_data_ex(T &obj, adapted_stl_non_pair_read_fun<T, &T::begin, &T::end> *)
#endif
    {
        obj.clear();

        uint32 count;
        if (!this->ReadEx<uint32>(count))
            return false;

        for (register uint32 i = 0; i < count; i++)
        {
            typedef typename T::value_type ElemType;

            ElemType elem;
            if (!this->ReadEx<ElemType>(elem))
                return false;

            obj.insert(obj.end(), elem);
        }

        return true;
    }

    /**
     * Raw type readex method impl.
     */
    template <typename T>
    bool adapt_read_data_ex(T &obj, ...)
    {
        if (_size >= _pos + sizeof(T))
            return this->ReadBuffer(&obj, sizeof(T));

        return false;
    }

    /**
     * Peek data[use Read()], but don't modify stream object.
     * @param[out] obj - will deserialize's object.
     * @return bool - return true if successed, otherwise return false.
     */
    template <typename T>
    bool Peek(T &obj)
    {
        size_t oldPos = _pos;
        bool ret = Read<T>(obj);
        this->SetPos(oldPos);

        return ret;
    }

    /**
     * Peek data[use ReadEx()], but don't modify stream object.
     * @param[out] obj - will deserialize's object.
     * @return bool - return true if successed, otherwise return false.
     */
    template <typename T>
    bool PeekEx(T &obj)
    {
        size_t oldPos = _pos;
        bool ret = ReadEx<T>(obj);
        this->SetPos(oldPos);

        return ret;
    }

    /**
     * Stream input overlapped functions define.
     * @param[out] val - value reference.
     */
    template<typename T>
    LLBC_Stream &operator >>(T &val)
    {
        this->Read(val);
        return *this;
    }

    /**
     * Stream output overlapped functions define.
     * @param[in] val - const value reference.
     */
    template<typename T>
    LLBC_Stream &operator <<(const T &val)
    {
        this->Write(val);
        return *this;
    }

public:
    /**
     * Resize stream object.
     * @param[in] newSize - new size, must greater than current size.
     */
    void Resize(size_t newSize);

    /**
     * Cleanup stream.
     */
    void Cleanup();

public:
    /**
     * Raw data type read/write support.
     * Note: All this raw data type read/write API support endian type flag.
     */
    bool ReadBool(bool &value);
    bool ReadSInt8(sint8 &value);
    bool ReadUInt8(uint8 &value);
    bool ReadSInt16(sint16 &value);
    bool ReadUInt16(uint16 &value);
    bool ReadSInt32(sint32 &value);
    bool ReadUInt32(uint32 &value);
    bool ReadSInt64(sint64 &value);
    bool ReadUInt64(uint64 &value);
    bool ReadLong(long &value);
    bool ReadULong(ulong &value);
    bool ReadFloat(float &value);
    bool ReadDouble(double &value);
    bool ReadPtr(void *&value);

    bool ReadBool_2(bool failRet = false);
    sint8 ReadSInt8_2(sint8 failRet = 0);
    uint8 ReadUInt8_2(uint8 failRet = 0);
    sint16 ReadSInt16_2(sint16 failRet = 0);
    uint16 ReadUInt16_2(uint16 failRet = 0);
    sint32 ReadSInt32_2(sint32 failRet = 0);
    uint32 ReadUInt32_2(uint32 failRet = 0);
    sint64 ReadSInt64_2(sint64 failRet = 0);
    uint64 ReadUInt64_2(uint64 failRet = 0);
    long ReadLong_2(long failRet = 0);
    ulong ReadULong_2(ulong failRet = 0);
    float ReadFloat_2(float failRet = 0.0f);
    double ReadDouble_2(double failRet = 0.0);
    void *ReadPtr_2(void *failRet = NULL);

    void WriteBool(bool value);
    void WriteSInt8(sint8 value);
    void WriteUInt8(uint8 value);
    void WriteSint16(sint16 value);
    void WriteUInt16(uint16 value);
    void WriteSInt32(sint32 value);
    void WriteUInt32(uint32 value);
    void WriteSInt64(sint64 value);
    void WriteUInt64(uint64 value);
    void WriteLong(long value);
    void WriteULong(ulong value);
    void WriteFloat(float value);
    void WriteDouble(double value);
    void WritePtr(const void *value);

public:
    LLBC_Stream &operator =(const LLBC_Stream &rhs);

private:
    template <typename T>
    bool ReadRawType(T &value);
    
    template <typename T>
    void WriteRawType(const T &value);

    bool OverlappedCheck(const void *another, size_t len);

private:
    void *_buf;
    size_t _pos;
    size_t _size;

    int _endian;

    bool _attach;
};

__LLBC_NS_END

#include "llbc/common/StreamImpl.h"
#include "llbc/common/StreamSpecImpl.h"

#endif // !__LLBC_COM_STREAM_H__
