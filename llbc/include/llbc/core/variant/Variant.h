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

#ifndef __LLBC_CORE_VARIANT_VARIANT_H__
#define __LLBC_CORE_VARIANT_VARIANT_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief Variant type enumeration.
 *      format(32bit): first_type[high 8 bit] second_type[low 24 bit]
 *          first_type:
 *              raw type:    The row data type, like int32, uint32 ...eg.
 *              string type: The string data type, use LLBC_String.
 *              Others... :  Not define.
 *          second type:
 *              ... ...
 */
class LLBC_EXPORT LLBC_VariantType
{
public:
    enum ENUM 
    {
        // Nil type enumeration.
        VT_NIL                  = 0x00000000,

        // First type enumeration.
        VT_RAW                  = 0x01000000,
        VT_STR                  = 0x02000000,
        VT_DICT                 = 0x04000000,

        // Row type enumeration.
        // Bit view(first type always equal VT_RAW):
        //          [first type] [raw type] [signed/unsigned]
        //             8 bits     23 bits        1 bit     
        VT_RAW_BOOL             = 0x01000003,
        VT_RAW_SINT8            = 0x01000005,
        VT_RAW_UINT8            = 0x01000008,
        VT_RAW_SINT16           = 0x01000011,
        VT_RAW_UINT16           = 0x01000020,
        VT_RAW_SINT32           = 0x01000041,
        VT_RAW_UINT32           = 0x01000080,
        VT_RAW_LONG             = 0x01000101,
        VT_RAW_ULONG            = 0x01000200,
        VT_RAW_SINT64           = 0x01000401,
        VT_RAW_UINT64           = 0x01000800,
        VT_RAW_FLOAT            = 0x01001001,
        VT_RAW_DOUBLE           = 0x01002001,

        // Str type enumeartion.
        // ! Now, string type's second type only support LLBC_String type.
        // Bit view(first type always equal VT_STR):
        //          [first type] [string type]
        //             8 bits       24 bits
        VT_STR_DFT              = 0x02000001,

        // Dictionary type enumeration.
        // Bit view(first type always equal VT_DICT):
        //          [first type] [dictionary type]
        //              8 bits       24 bits
        VT_DICT_DFT             = 0x04000001,

        /////////////////////////////////////////////////////////////////////

        //! The first type mask value.
        VT_MASK_FIRST_TYPE      = 0xff000000,
        //! The raw type variant's signed mask value.
        VT_MASK_RAW_SIGNED      = 0x00000001
    };

    /**
     * Get type string representation.
     * @param[in] type - the variant type enumeration.
     * @return const LLBC_String & - the type string representation.
     */
    static const LLBC_String &Type2Str(int type);

    /**
     * Init type to string representation dictionary, call by llbc library.
     */
    static void InitType2StrDict();

private:
    static std::map<int, LLBC_String> _typeDescs;
};

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * Pre-declare LLBC_Variant class.
 */
class LLBC_Variant;

__LLBC_NS_END

/**
 * \brief LLBC_Variant stream output function.
 */
extern LLBC_EXPORT std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_Variant &variant);

__LLBC_NS_BEGIN

/**
 * \brief The basic variant type class enumeration.
 */
class LLBC_EXPORT LLBC_Variant
{
public:
    typedef LLBC_String Str;

    typedef std::map<LLBC_Variant, LLBC_Variant> Dict;
    typedef Dict::iterator DictIter;
    typedef Dict::const_iterator DictConstIter;
    typedef Dict::reverse_iterator DictReverseIter;
    typedef Dict::const_reverse_iterator DictConstReverseIter;

    struct LLBC_EXPORT Holder
    {
        LLBC_VariantType::ENUM type;

        union RawType 
        {
            sint64 int64Val;
            uint64 uint64Val;

            double doubleVal;
        } raw;

        union ObjType
        {
            Str *str;
            Dict *dict;
        } obj;

        Holder();
        ~Holder();
    };

    /**
     * The variant data type enumerations.
     */
    enum
    {
        NIL = LLBC_VariantType::VT_NIL,

        BOOL = LLBC_VariantType::VT_RAW_BOOL,
        SINT8 = LLBC_VariantType::VT_RAW_SINT8,
        UINT8 = LLBC_VariantType::VT_RAW_UINT8,
        SINT16 = LLBC_VariantType::VT_RAW_SINT16,
        UINT16 = LLBC_VariantType::VT_RAW_UINT16,
        SINT32 = LLBC_VariantType::VT_RAW_SINT32,
        UINT32 = LLBC_VariantType::VT_RAW_UINT32,
        SINT64 = LLBC_VariantType::VT_RAW_SINT64,
        UINT64 = LLBC_VariantType::VT_RAW_UINT64,
        LONG = LLBC_VariantType::VT_RAW_LONG,
        ULONG = LLBC_VariantType::VT_RAW_ULONG,
        FLOAT = LLBC_VariantType::VT_RAW_FLOAT,
        DOUBLE = LLBC_VariantType::VT_RAW_DOUBLE,

        STR = LLBC_VariantType::VT_STR_DFT,
        DICT = LLBC_VariantType::VT_DICT_DFT
    };

    /**
     * The variant mask enumerations.
     */
    enum
    {
        MASK_FIRST = LLBC_VariantType::VT_MASK_FIRST_TYPE,
        MASK_SIGNED = LLBC_VariantType::VT_MASK_RAW_SIGNED,
    };

public:
    LLBC_Variant();

    // Constructors(all parameter constructors is explicit, copy constructor is non-explicit).
    explicit LLBC_Variant(const bool &boolVal);
    explicit LLBC_Variant(const sint8 &sint8Val);
    explicit LLBC_Variant(const uint8 &uint8Val);
    explicit LLBC_Variant(const sint16 &sint16Val);
    explicit LLBC_Variant(const uint16 &uint16Val);
    explicit LLBC_Variant(const sint32 &sint32Val);
    explicit LLBC_Variant(const uint32 &uint32Val);
    explicit LLBC_Variant(const long &longVal);
    explicit LLBC_Variant(const ulong &ulongVal);
    explicit LLBC_Variant(const sint64 &int64Val);
    explicit LLBC_Variant(const uint64 &uint64Val); 
    explicit LLBC_Variant(const float &floatVal);
    explicit LLBC_Variant(const double &doubleVal);
    explicit LLBC_Variant(const char *cstrVal);
    explicit LLBC_Variant(const std::string &strVal);
    explicit LLBC_Variant(const LLBC_String &strVal);
    explicit LLBC_Variant(const Dict &dictVal);
    LLBC_Variant(const LLBC_Variant &varVal);

    // Fetch variant data type and holder data.
    int GetType() const;
    const struct Holder &GetHolder() const;

    // Type diagnose.
    bool IsNil() const;
    bool IsRaw() const;
    bool IsSignedRaw() const;
    bool IsUnsignedRaw() const;
    bool IsBool() const;
    bool IsInt8() const;
    bool IsUInt8() const;
    bool IsInt16() const;
    bool IsUInt16() const;
    bool IsInt32() const;
    bool IsUInt32() const;
    bool IsLong() const;
    bool IsULong() const;
    bool IsInt64() const;
    bool IsUInt64() const;
    bool IsFloat() const;
    bool IsDouble() const;
    bool IsStr() const;
    bool IsDict() const;

    // Type convert.
    LLBC_Variant &BecomeNil();
    LLBC_Variant &BecomeBool();
    LLBC_Variant &BecomeInt8();
    LLBC_Variant &BecomeUInt8();
    LLBC_Variant &BecomeInt16();
    LLBC_Variant &BecomeUInt16();
    LLBC_Variant &BecomeInt32();
    LLBC_Variant &BecomeUInt32();
    LLBC_Variant &BecomeLong();
    LLBC_Variant &BecomeULong();
    LLBC_Variant &BecomeInt64();
    LLBC_Variant &BecomeUInt64();
    LLBC_Variant &BecomeFloat();
    LLBC_Variant &BecomeDouble();
    LLBC_Variant &BecomeStr();
    LLBC_Variant &BecomeDict();

    // Real data fetch.
    bool AsBool() const;
    sint8 AsInt8() const;
    uint8 AsUInt8() const;
    sint16 AsInt16() const;
    uint16 AsUInt16() const;
    sint32 AsInt32() const;
    uint32 AsUInt32() const;
    long AsLong() const;
    unsigned long AsULong() const;
    sint64 AsInt64() const;
    uint64 AsUInt64() const;
    float AsFloat() const;
    double AsDouble() const;
    LLBC_String AsStr() const;
    const Dict &AsDict() const;

    operator bool () const;
    operator sint8 () const;
    operator uint8 () const;
    operator sint16 () const;
    operator uint16 () const;
    operator sint32 () const;
    operator uint32 () const;
    operator long () const;
    operator ulong () const;
    operator sint64 () const;
    operator uint64 () const;
    operator float () const;
    operator double () const;
    operator LLBC_String () const;
    operator const Dict &() const;

    // Dictionary type variant object specify operate methods.
    DictIter Begin();
    DictIter End();
    DictConstIter Begin() const;
    DictConstIter End() const;
    DictReverseIter ReverseBegin();
    DictReverseIter ReverseEnd();
    DictConstReverseIter ReverseBegin() const;
    DictConstReverseIter ReverseEnd() const;

    std::pair<DictIter, bool> Insert(const Dict::key_type &key, const Dict::mapped_type &val);
    std::pair<DictIter, bool> Insert(const Dict::value_type &val);

    template <typename _Kty, typename _Ty>
    std::pair<DictIter, bool> Insert(const _Kty &key, const _Ty &val);

    DictIter Find(const Dict::key_type &key);
    DictConstIter Find(const Dict::key_type &key) const;

    template <typename _Kty>
    DictIter Find(const _Kty &key);
    template <typename _Kty>
    DictConstIter Find(const _Kty &key) const;

    void Erase(DictIter it);
    Dict::size_type Erase(const Dict::key_type &key);
    void Erase(DictIter first, DictIter last);

    template <typename _Kty>
    Dict::size_type Erase(const _Kty &key);

    Dict::mapped_type &operator [](const Dict::key_type &key);
    const Dict::mapped_type &operator [](const Dict::key_type &key) const;

    template <typename _Kty>
    Dict::mapped_type &operator [](const _Kty &key);
    template <typename _Kty>
    const Dict::mapped_type &operator [](const _Kty &key) const;

    // assignment operators.
    LLBC_Variant &operator =(sint8 val);
    LLBC_Variant &operator =(uint8 val);
    LLBC_Variant &operator =(sint16 val);
    LLBC_Variant &operator =(uint16 val);
    LLBC_Variant &operator =(sint32 val);
    LLBC_Variant &operator =(uint32 val);
    LLBC_Variant &operator =(long val);
    LLBC_Variant &operator =(unsigned long val);
    LLBC_Variant &operator =(const sint64 &val);
    LLBC_Variant &operator =(const uint64 &val);
    LLBC_Variant &operator =(float val);
    LLBC_Variant &operator =(const double &val);
    LLBC_Variant &operator =(const LLBC_String &val);
    LLBC_Variant &operator =(const Dict &val);
    LLBC_Variant &operator =(const LLBC_Variant &val);

    // Relational operators.
    bool operator ==(const LLBC_Variant &another) const;
    bool operator !=(const LLBC_Variant &another) const;

    bool operator <(const LLBC_Variant &another) const;
    bool operator >(const LLBC_Variant &another) const;
    bool operator <=(const LLBC_Variant &another) const;
    bool operator >=(const LLBC_Variant &another) const;

    // Arithmetic operators.
    LLBC_Variant operator +(const LLBC_Variant &another) const;
    LLBC_Variant operator -(const LLBC_Variant &another) const;
    LLBC_Variant operator *(const LLBC_Variant &another) const;
    LLBC_Variant operator /(const LLBC_Variant &another) const;

    LLBC_Variant &operator +=(const LLBC_Variant &another);
    LLBC_Variant &operator -=(const LLBC_Variant &another);
    LLBC_Variant &operator *=(const LLBC_Variant &another);
    LLBC_Variant &operator /=(const LLBC_Variant &another);

    // Type to string.
    const LLBC_String &TypeToString() const;
    // Value to string.
    LLBC_String ValueToString() const;
    // To string.
    LLBC_String ToString() const;

    // Serialize / DeSerialize support.
    void Serialize(LLBC_Stream &stream) const;
    bool DeSerialize(LLBC_Stream &stream);

    // SerializeEx / DeSerializeEx support.
    void SerializeEx(LLBC_Stream &stream) const;
    bool DeSerializeEx(LLBC_Stream &stream);

public:
    friend std::ostream &::operator <<(std::ostream &o, const LLBC_Variant &variant);

private:
    friend class LLBC_VariantTraits;

    void SetType(int type);

    Holder &GetHolder();

    void CleanRawData();
    void CleanStrData();
    void CleanDictData();
    void CleanTypeData(int type);

    void OptimizePerformance();

private:
    struct Holder _holder;
};

/**
 * \brief The Variant dictionary encapsulation.
 */
 typedef std::map<LLBC_NS LLBC_String, LLBC_NS LLBC_Variant> LLBC_VariantDictionary;
 typedef LLBC_VariantDictionary::iterator LLBC_VariantDictionaryIter;
 typedef LLBC_VariantDictionary::const_iterator LLBC_VariantDictionaryCIter;

__LLBC_NS_END

#include "llbc/core/variant/VariantImpl.h"

#endif // !__LLBC_CORE_VARIANT_VARIANT_H__
