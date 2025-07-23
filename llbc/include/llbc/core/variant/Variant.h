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

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief Variant type enumeration.
 *      format(32bit): first_type[high 8 bit] second_type[low 8 bit]
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
    enum ENUM : uint16
    {
        // Nil type enumeration.
        NIL                  = 0x0000,

        // First type enumeration.
        RAW                  = 0x0100,
        STR                  = 0x0200,
        SEQ                  = 0x0400,
        DICT                 = 0x0800,

        // Row type enumeration.
        // Bit view(first type always equal RAW):
        //          [first type] [raw type] [signed/unsigned]
        //             8 bits      7 bits        1 bit     
        RAW_BOOL             = 0x0103, // 0000 0011
        RAW_SINT8            = 0x0105, // 0000 0101
        RAW_UINT8            = 0x0106, // 0000 0110
        RAW_SINT16           = 0x0109, // 0000 1001
        RAW_UINT16           = 0x010a, // 0000 1010
        RAW_SINT32           = 0x010d, // 0000 1101
        RAW_UINT32           = 0x010e, // 0000 1110
        RAW_LONG             = 0x0111, // 0001 0001
        RAW_ULONG            = 0x0112, // 0001 0010
        RAW_PTR              = 0x0114, // 0001 0100
        RAW_SINT64           = 0x0117, // 0001 0111
        RAW_UINT64           = 0x0118, // 0001 1000
        RAW_FLOAT            = 0x011b, // 0001 1011
        RAW_DOUBLE           = 0x011d, // 0001 1101

        // Str type enumeration.
        // ! Now, string type's second type only support LLBC_String type.
        // Bit view(first type always equal STR):
        //          [first type] [string type]
        //             8 bits       8 bits
        STR_DFT              = 0x0201,

        // Sequence enumeration.
        // Bit view(first type always equal SEQ):
        //          [first type] [dictionary type]
        //              8 bits       8 bits
        SEQ_DFT              = 0x0401,

        // Dictionary type enumeration.
        // Bit view(first type always equal DICT):
        //          [first type] [dictionary type]
        //              8 bits       8 bits
        DICT_DFT             = 0x0801,

        /////////////////////////////////////////////////////////////////////

        //! The first type mask value.
        MASK_FIRST_TYPE      = 0xff00,
        //! The raw type variant's signed mask value.
        MASK_RAW_SIGNED      = 0x0001
    };

    /**
     * Get type string representation.
     * @param[in] type - the variant type enumeration.
     * @return const LLBC_String & - the type string representation.
     */
    static const LLBC_String &Type2Str(int type);
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
LLBC_EXPORT std::ostream &operator<<(std::ostream &o, const LLBC_NS LLBC_Variant &variant);

__LLBC_NS_BEGIN

/**
 * \brief The basic variant type class enumeration.
 */
class LLBC_EXPORT LLBC_Variant
{
public:
/**
 * \brief The commonly used const variables define.
 */
    static const LLBC_Variant nil;

public:
    /**
     * Variant embedded data types typedef.
     */
    typedef LLBC_String Str;

    typedef std::vector<LLBC_Variant> Seq;
    typedef Seq::iterator SeqIter;
    typedef Seq::const_iterator SeqConstIter;
    typedef Seq::reverse_iterator SeqReverseIter;
    typedef Seq::const_reverse_iterator SeqConstReverseIter;

    typedef std::map<LLBC_Variant, LLBC_Variant> Dict;
    typedef Dict::iterator DictIter;
    typedef Dict::const_iterator DictConstIter;
    typedef Dict::reverse_iterator DictReverseIter;
    typedef Dict::const_reverse_iterator DictConstReverseIter;

    struct LLBC_EXPORT Holder
    {
        LLBC_VariantType::ENUM type;

        union DataType
        {
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
                Seq *seq;
            } obj;
        } data;

        Holder();
        ~Holder();

        LLBC_VariantType::ENUM GetFirstType() const;

        void Clear();

    private:
        friend class LLBC_Variant;
        void ClearData();
    };

public:
    // Initialize the number to string fast access table.
    static void InitNumber2StrFastAccessTable();
    // Destroy the number to string fast access table.
    static void DestroyNumber2StrFastAccessTable();

public:
    LLBC_Variant();

    // Constructors(all parameter constructors is explicit, copy constructor is non-explicit).
    explicit LLBC_Variant(const bool &b);
    explicit LLBC_Variant(const sint8 &i8);
    explicit LLBC_Variant(const uint8 &ui8);
    explicit LLBC_Variant(const sint16 &i16);
    explicit LLBC_Variant(const uint16 &ui16);
    explicit LLBC_Variant(const sint32 &i32);
    explicit LLBC_Variant(const uint32 &ui32);
    explicit LLBC_Variant(const long &l);
    explicit LLBC_Variant(const ulong &ul);
    template <typename _T>
    explicit LLBC_Variant(const _T * const &ptr);
    explicit LLBC_Variant(const sint64 &i64);
    explicit LLBC_Variant(const uint64 &ui64);
    explicit LLBC_Variant(const float &f);
    explicit LLBC_Variant(const double &d);
    template <typename _T,
              typename std::enable_if<std::is_enum<_T>::value, int>::type = 0>
    explicit LLBC_Variant(const _T &en);
    explicit LLBC_Variant(const char *str);
    explicit LLBC_Variant(const std::string &str);
    explicit LLBC_Variant(const LLBC_String &str);
    explicit LLBC_Variant(const LLBC_CString &str);
    template <typename _T1, typename _T2>
    explicit LLBC_Variant(const std::pair<_T1, _T2> &pa);
    explicit LLBC_Variant(const Seq &seq);
    template <typename _T>
    explicit LLBC_Variant(const std::vector<_T> &vec);
    template <typename _T>
    explicit LLBC_Variant(const std::list<_T> &lst);
    template <typename _T>
    explicit LLBC_Variant(const std::deque<_T> &dqe);
    template <typename _T>
    explicit LLBC_Variant(const std::queue<_T> &que);
    template <typename _T>
    explicit LLBC_Variant(const std::set<_T> &s);
    template <typename _T>
    explicit LLBC_Variant(const std::unordered_set<_T> &us);
    explicit LLBC_Variant(const Dict &dict);
    template <typename _Key, typename _Val>
    explicit LLBC_Variant(const std::map<_Key, _Val> &m);
    template <typename _Key, typename _Val>
    explicit LLBC_Variant(const std::unordered_map<_Key, _Val> &um);
    LLBC_Variant(const LLBC_Variant &var);
    LLBC_Variant(LLBC_Variant &&var) noexcept;

public:
    // Fetch variant data type and holder data.
    LLBC_VariantType::ENUM GetType() const;
    LLBC_VariantType::ENUM GetFirstType() const;
    struct Holder *GetMutableHolder();
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
    bool IsPtr() const;
    bool IsInt64() const;
    bool IsUInt64() const;
    bool IsFloat() const;
    bool IsDouble() const;
    bool IsStr() const;
    bool IsSeq() const;
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
    LLBC_Variant &BecomePtr();
    LLBC_Variant &BecomeInt64();
    LLBC_Variant &BecomeUInt64();
    LLBC_Variant &BecomeFloat();
    LLBC_Variant &BecomeDouble();
    LLBC_Variant &BecomeStr();
    LLBC_Variant &BecomeStrX();
    LLBC_Variant &BecomeSeq();
    LLBC_Variant &BecomeSeqX();
    LLBC_Variant &BecomeDict();
    LLBC_Variant &BecomeDictX();
    LLBC_Variant &Become(LLBC_VariantType::ENUM ty);

    // Real data fetch.
    bool AsBool() const;
    bool AsLooseBool() const;
    sint8 AsInt8() const;
    uint8 AsUInt8() const;
    sint16 AsInt16() const;
    uint16 AsUInt16() const;
    sint32 AsInt32() const;
    uint32 AsUInt32() const;
    long AsLong() const;
    unsigned long AsULong() const;
    template <typename _Ty>
    _Ty *AsPtr() const;
    sint64 AsInt64() const;
    uint64 AsUInt64() const;
    float AsFloat() const;
    double AsDouble() const;
    template <typename _T>
    typename std::enable_if<std::is_enum<_T>::value, _T>::type
    AsEnum() const;
    LLBC_String AsStr() const;
    const Seq &AsSeq() const;
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
    template <typename _Ty>
    operator _Ty * () const;
    operator sint64 () const;
    operator uint64 () const;
    operator float () const;
    operator double () const;
    template <typename _T,
              typename std::enable_if<std::is_enum<_T>::value, int>::type = 0>
    operator _T () const;
    operator LLBC_String () const;
    template <typename _T1, typename _T2>
    operator typename std::pair<_T1, _T2>() const;
    operator const Seq &() const;
    template <typename _Ty>
    operator std::vector<_Ty>() const;
    template <typename _Ty>
    operator std::set<_Ty>() const;
    template <typename _Ty>
    operator std::unordered_set<_Ty>() const;
    template <typename _Ty>
    operator std::queue<_Ty>() const;
    template <typename _Ty>
    operator std::deque<_Ty>() const;
    operator const Dict &() const;
    template <typename _Key, typename _Val>
    operator std::map<_Key, _Val>() const;
    template <typename _Key, typename _Val>
    operator std::unordered_map<_Key, _Val>() const;

    // Common operation methods.
    void Clear();
    bool IsEmpty() const;
    size_t Size() const;
    size_t Capacity() const;

    // Sequence type variant object specify Operate methods.
    SeqIter SeqBegin();
    SeqIter SeqEnd();
    SeqConstIter SeqBegin() const;
    SeqConstIter SeqEnd() const;
    SeqReverseIter SeqReverseBegin();
    SeqReverseIter SeqReverseEnd();
    SeqConstReverseIter SeqReverseBegin() const;
    SeqConstReverseIter SeqReverseEnd() const;

    Seq::reference SeqFront();
    Seq::reference SeqBack();
    Seq::const_reference SeqFront() const;
    Seq::const_reference SeqBack() const;

    SeqIter SeqInsert(SeqIter it, const Seq::value_type &val);
    void SeqInsert(SeqIter it, Seq::size_type n, const Seq::value_type &val);
    void SeqInsert(SeqIter it, SeqConstIter first, SeqConstIter last);

    template <typename _Ty>
    SeqIter SeqInsert(SeqIter it, const _Ty &val);
    template <typename _Ty>
    void SeqInsert(SeqIter it, Seq::size_type n, const _Ty &val);

    template <typename _Ty1, typename... _Tys>
    typename ::std::enable_if<::std::is_same<_Ty1, LLBC_Variant>::value, void>::type
    SeqPushBack(_Ty1 &&val1, _Tys &&... vals);
    template <typename _Ty1, typename... _Tys>
    typename ::std::enable_if<!::std::is_same<_Ty1, LLBC_Variant>::value, void>::type
    SeqPushBack(_Ty1 &&val1, _Tys &&... vals);

    void SeqPopBack();

    void SeqResize(Seq::size_type n, const Seq::value_type &val = Seq::value_type::nil);
    template <typename _Ty>
    void SeqResize(Seq::size_type n, const _Ty &val = _Ty());

    void SeqReserve(Seq::size_type n);

    SeqIter SeqErase(SeqIter it);
    SeqIter SeqErase(SeqIter first, SeqIter last);
    void SeqErase(const Seq::value_type &val);
    template <typename _Ty>
    void SeqErase(const _Ty &val);

    // Dictionary type variant object specify Operate methods.
    DictIter DictBegin();
    DictIter DictEnd();
    DictConstIter DictBegin() const;
    DictConstIter DictEnd() const;
    DictReverseIter DictReverseBegin();
    DictReverseIter DictReverseEnd();
    DictConstReverseIter DictReverseBegin() const;
    DictConstReverseIter DictReverseEnd() const;

    std::pair<DictIter, bool> DictInsert(const Dict::key_type &key, const Dict::mapped_type &val);
    std::pair<DictIter, bool> DictInsert(const Dict::value_type &val);

    template <typename _Key, typename _Val>
    std::pair<DictIter, bool> DictInsert(const _Key &key, const _Val &val);

    DictIter DictFind(const Dict::key_type &key);
    DictConstIter DictFind(const Dict::key_type &key) const;

    template <typename _Key>
    DictIter DictFind(const _Key &key);
    template <typename _Key>
    DictConstIter DictFind(const _Key &key) const;

    DictIter DictErase(DictIter it);
    DictIter DictErase(DictIter first, DictIter last);

    template <typename _Key1, typename... _Keys>
    typename ::std::enable_if<::std::is_same<_Key1, LLBC_Variant>::value, typename LLBC_Variant::Dict::size_type>::type
    DictErase(_Key1 &&key1, _Keys &&... keys);
    template <typename _Key1, typename... _Keys>
    typename ::std::enable_if<!::std::is_same<_Key1, LLBC_Variant>::value, typename LLBC_Variant::Dict::size_type>::type
    DictErase(_Key1 &&key1, _Keys &&... keys);

    LLBC_Variant &operator[](const LLBC_Variant &key);
    const LLBC_Variant &operator[](const LLBC_Variant &key) const;

    template <typename _Key>
    LLBC_Variant &operator[](const _Key &key);
    template <typename _Key>
    const LLBC_Variant &operator[](const _Key &key) const;

    // assignment operators.
    LLBC_Variant &operator=(bool b);
    LLBC_Variant &operator=(sint8 i8);
    LLBC_Variant &operator=(uint8 ui8);
    LLBC_Variant &operator=(sint16 i16);
    LLBC_Variant &operator=(uint16 ui16);
    LLBC_Variant &operator=(sint32 i32);
    LLBC_Variant &operator=(uint32 ui32);
    LLBC_Variant &operator=(long l);
    LLBC_Variant &operator=(unsigned long ul);
    LLBC_Variant &operator=(const char * const &str);
    template <typename _T>
    LLBC_Variant &operator=(const _T * const &ptr);
    LLBC_Variant &operator=(const sint64 &i64);
    LLBC_Variant &operator=(const uint64 &ui64);
    LLBC_Variant &operator=(float f);
    LLBC_Variant &operator=(const double &d);
    template <typename _T,
              typename std::enable_if<std::is_enum<_T>::value, int>::type = 0>
    LLBC_Variant& operator=(const _T &en);
    LLBC_Variant &operator=(const std::string &str);
    LLBC_Variant &operator=(const LLBC_String &str);
    LLBC_Variant &operator=(const LLBC_CString &str);
    template <typename _T1, typename _T2>
    LLBC_Variant &operator=(const std::pair<_T1, _T2> &pa);
    LLBC_Variant &operator=(const Seq &seq);
    template <typename _T>
    LLBC_Variant &operator=(const std::vector<_T> &vec);
    template <typename _T>
    LLBC_Variant &operator=(const std::list<_T> &lst);
    template <typename _T>
    LLBC_Variant &operator=(const std::queue<_T> &que);
    template <typename _T>
    LLBC_Variant &operator=(const std::set<_T> &s);
    LLBC_Variant &operator=(const Dict &dict);
    template <typename _Key, typename _Val>
    LLBC_Variant &operator=(const std::map<_Key, _Val> &m);
    LLBC_Variant &operator=(const LLBC_Variant &var);
    LLBC_Variant &operator=(LLBC_Variant &&var) noexcept;

    // Relational operators.
    bool operator==(const LLBC_Variant &another) const;
    bool operator!=(const LLBC_Variant &another) const;

    bool operator<(const LLBC_Variant &another) const;
    bool operator>(const LLBC_Variant &another) const;
    bool operator<=(const LLBC_Variant &another) const;
    bool operator>=(const LLBC_Variant &another) const;

    // Relational operators.
    template <typename _T>
    bool operator==(const _T &another) const;
    template <typename _T>
    bool operator!=(const _T &another) const;

    template <typename _T>
    bool operator<(const _T &another) const;
    template <typename _T>
    bool operator>(const _T &another) const;
    template <typename _T>
    bool operator<=(const _T &another) const;
    template <typename _T>
    bool operator>=(const _T &another) const;

    // Arithmetic operators.
    LLBC_Variant operator+(const LLBC_Variant &another) const;
    LLBC_Variant operator-(const LLBC_Variant &another) const;
    LLBC_Variant operator*(const LLBC_Variant &another) const;
    LLBC_Variant operator/(const LLBC_Variant &another) const;
    LLBC_Variant operator%(const LLBC_Variant &another) const;

    LLBC_Variant &operator+=(const LLBC_Variant &another);
    LLBC_Variant &operator-=(const LLBC_Variant &another);
    LLBC_Variant &operator*=(const LLBC_Variant &another);
    LLBC_Variant &operator/=(const LLBC_Variant &another);
    LLBC_Variant &operator%=(const LLBC_Variant &another);

    // Arithmetic operators(template base).
    template <typename _T>
    LLBC_Variant operator+(const _T &another) const;
    template <typename _T>
    LLBC_Variant operator-(const _T &another) const;
    template <typename _T>
    LLBC_Variant operator*(const _T &another) const;
    template <typename _T>
    LLBC_Variant operator/(const _T &another) const;
    template <typename _T>
    LLBC_Variant operator%(const _T &another) const;

    template <typename _T>
    LLBC_Variant &operator+=(const _T &another);
    template <typename _T>
    LLBC_Variant &operator-=(const _T &another);
    template <typename _T>
    LLBC_Variant &operator*=(const _T &another);
    template <typename _T>
    LLBC_Variant &operator/=(const _T &another);
    template <typename _T>
    LLBC_Variant &operator%=(const _T &another);

    // Type to string.
    const LLBC_String &TypeToString() const;
    // Value to string.
    LLBC_String ValueToString() const;
    // To string.
    LLBC_String ToString() const;

    // Serialize / Deserialize support.
    void Serialize(LLBC_Stream &stream) const;
    bool Deserialize(LLBC_Stream &stream);

public:
    friend std::ostream &::operator<<(std::ostream &o, const LLBC_Variant &variant);

private:
    friend class LLBC_VariantTraits;

    void SetType(int type);
    void ClearData();

    void CtFromRaw(uint64 raw, LLBC_VariantType::ENUM ty);
    template <typename _T, typename _UnaryContainer>
    void CtFromUnaryCont(const _UnaryContainer &unaryCont);
    template <typename _Key, typename _Val, typename _BinaryContainer>
    void CtFromBinaryCont(const _BinaryContainer &binaryCont);

    template <typename _Key, typename _Val, typename _BinaryContainer>
    void CpToBinaryCont(_BinaryContainer &binaryCont) const;

    template <typename _64Ty>
    _64Ty AsSignedOrUnsigned64() const;

    bool IsStrX() const;
    bool IsSeqX() const;
    bool IsDictX() const;

    void SeqPushBack();
    void SeqPushBackElem(const Seq::value_type &val);

    Dict::size_type DictErase();
    Dict::size_type DictEraseKey(const Dict::key_type &key);

private:
    struct Holder _holder;
    static Str **_num2StrFastAccessTbl;
};

__LLBC_NS_END

#include "llbc/core/variant/VariantInl.h"


