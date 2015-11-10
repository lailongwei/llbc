/**
 * @file    LLBC_ComString.h
 * @author  Longwei Lai
 * @date    2021/10/02
 * @brief   The common string encapsulation.
 */
#ifndef __LLBC_CORE_COMSTRING_COMSTRING_H__ 
#define __LLBC_CORE_COMSTRING_COMSTRING_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief The common string encapsulation.
 *        ComString class is limited size string encapsulation class.
 */
template <uint32 _Lmt = 32>
class LLBC_ComString
{
public:
    LLBC_ComString();
    LLBC_ComString(const LLBC_ComString &str);
    LLBC_ComString(const char *str);
    LLBC_ComString(const LLBC_String &str);

public:
    const char *data() const;
    const char *c_str() const;
    const char *GetCStr() const;
    LLBC_String GetStr() const;

    static uint32 GetLimitValue()
    {
        return _Lmt;
    }

    uint32 size() const;
    uint32 length() const;
    uint32 GetLength() const;

    void Clear();

    uint32 GetUsableSize() const;

    bool IsFull() const;

    bool IsEmpty() const;

    void Format(const char *fmt, ...);
    LLBC_ComString &AppendFormat(const char *fmt, ...);

    LLBC_ComString &Append(char ch);
    LLBC_ComString &Append(const char *str);
    LLBC_ComString &Append(const char *str, uint32 len);
    LLBC_ComString &Append(const LLBC_ComString &str);
    LLBC_ComString &Append(const LLBC_String &str);

public:
    operator LLBC_String () const;

    LLBC_ComString &operator = (char ch);
    LLBC_ComString &operator = (const char *str);
    LLBC_ComString &operator = (const LLBC_ComString &str);
    LLBC_ComString &operator = (const LLBC_String &str);

    LLBC_ComString &operator += (char ch);
    LLBC_ComString &operator += (const char *str);
    LLBC_ComString &operator += (const LLBC_ComString &str);
    LLBC_ComString &operator += (const LLBC_String &str);

    bool operator == (const char *str) const;
    bool operator == (const LLBC_ComString &str) const;
    bool operator == (const LLBC_String &str) const;

    bool operator != (const char *str) const;
    bool operator != (const LLBC_ComString &str) const;
    bool operator != (const LLBC_String &str) const;

public:
    bool operator < (const LLBC_ComString &str) const;

public:
    void Serialize(LLBC_Stream &stream) const;
    bool DeSerialize(LLBC_Stream &stream);

    void SerializeEx(LLBC_Stream &stream) const;
    bool DeSerializeEx(LLBC_Stream &stream);

private:
    uint32 _length;
    char _buff[_Lmt];
};

__LLBC_NS_END

#include "llbc/core/comstring/ComStringImpl.h"

__LLBC_NS_BEGIN

/**
 * \brief LLBC_ComString4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192.
 */
typedef LLBC_ComString<4>    LLBC_ComString4;
typedef LLBC_ComString<8>    LLBC_ComString8;
typedef LLBC_ComString<16>   LLBC_ComString16;
typedef LLBC_ComString<32>   LLBC_ComString32;
typedef LLBC_ComString<64>   LLBC_ComString64;
typedef LLBC_ComString<128>  LLBC_ComString128;
typedef LLBC_ComString<256>  LLBC_ComString256;
typedef LLBC_ComString<512>  LLBC_ComString512;
typedef LLBC_ComString<1024> LLBC_ComString1024;
typedef LLBC_ComString<2048> LLBC_ComString2048;
typedef LLBC_ComString<4096> LLBC_ComString4096;
typedef LLBC_ComString<8192> LLBC_ComString8192;
typedef LLBC_ComString<16384> LLBC_ComString16384;
typedef LLBC_ComString<32768> LLBC_ComString32768;

__LLBC_NS_END

#endif // !__LLBC_CORE_COMSTRING_COMSTRING_H__
