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
    enum ENUM : uint32
    {
        // Nil type enumeration.
        VT_NIL                  = 0x00000000,

        // First type enumeration.
        VT_RAW                  = 0x01000000,
        VT_STR                  = 0x02000000,
        VT_SEQ                  = 0x04000000,
        VT_DICT                 = 0x08000000,

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
        VT_RAW_PTR              = 0x01000400,
        VT_RAW_SINT64           = 0x01000801,
        VT_RAW_UINT64           = 0x01001000,
        VT_RAW_FLOAT            = 0x01002001,
        VT_RAW_DOUBLE           = 0x01004001,

        // Str type enumeartion.
        // ! Now, string type's second type only support LLBC_String type.
        // Bit view(first type always equal VT_STR):
        //          [first type] [string type]
        //             8 bits       24 bits
        VT_STR_DFT              = 0x02000001,

        // Sequence enumeration.
        // Bit view(first type always equal VT_SEQ):
        //          [first type] [dictionary type]
        //              8 bits       24 bits
        VT_SEQ_DFT              = 0x04000001,

        // Dictionary type enumeration.
        // Bit view(first type always equal VT_DICT):
        //          [first type] [dictionary type]
        //              8 bits       24 bits
        VT_DICT_DFT             = 0x08000001,

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
    static std::map<LLBC_VariantType::ENUM, LLBC_String> _typeDescs;
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
LLBC_EXPORT std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_Variant &variant);

__LLBC_NS_BEGIN

/**
 * \brief The basic variant type class enumeration.
 */
class LLBC_EXPORT LLBC_Variant
{
public:
/**
 * \brief The unually used const variables define.
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
        LONG = LLBC_VariantType::VT_RAW_LONG,
        ULONG = LLBC_VariantType::VT_RAW_ULONG,
        PTR = LLBC_VariantType::VT_RAW_PTR,
        SINT64 = LLBC_VariantType::VT_RAW_SINT64,
        UINT64 = LLBC_VariantType::VT_RAW_UINT64,
        FLOAT = LLBC_VariantType::VT_RAW_FLOAT,
        DOUBLE = LLBC_VariantType::VT_RAW_DOUBLE,

        STR = LLBC_VariantType::VT_STR_DFT,
        SEQ = LLBC_VariantType::VT_SEQ_DFT,
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
    explicit LLBC_Variant(const char *str);
    explicit LLBC_Variant(const std::string &str);
    explicit LLBC_Variant(const LLBC_String &str);
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
    LLBC_Variant(LLBC_Variant &&var);

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
    LLBC_Variant &BecomeSeq();
    LLBC_Variant &BecomeDict();
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
    operator LLBC_String () const;
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

    // Dictionary type variant object specify operate methods.
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

    // Dictionary type variant object specify operate methods.
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

    LLBC_Variant &operator [](const LLBC_Variant &key);
    const LLBC_Variant &operator [](const LLBC_Variant &key) const;

    template <typename _Key>
    LLBC_Variant &operator [](const _Key &key);
    template <typename _Key>
    const LLBC_Variant &operator [](const _Key &key) const;

    // assignment operators.
    LLBC_Variant &operator =(bool b);
    LLBC_Variant &operator =(sint8 i8);
    LLBC_Variant &operator =(uint8 ui8);
    LLBC_Variant &operator =(sint16 i16);
    LLBC_Variant &operator =(uint16 ui16);
    LLBC_Variant &operator =(sint32 i32);
    LLBC_Variant &operator =(uint32 ui32);
    LLBC_Variant &operator =(long l);
    LLBC_Variant &operator =(unsigned long ul);
    LLBC_Variant &operator =(const char * const &str);
    template <typename _T>
    LLBC_Variant &operator =(const _T * const &ptr);
    LLBC_Variant &operator =(const sint64 &i64);
    LLBC_Variant &operator =(const uint64 &ui64);
    LLBC_Variant &operator =(float f);
    LLBC_Variant &operator =(const double &d);
    LLBC_Variant &operator =(const LLBC_String &str);
    LLBC_Variant &operator =(const Seq &seq);
    template <typename _T>
    LLBC_Variant &operator =(const std::vector<_T> &vec);
    template <typename _T>
    LLBC_Variant &operator =(const std::list<_T> &lst);
    template <typename _T>
    LLBC_Variant &operator =(const std::queue<_T> &que);
    template <typename _T>
    LLBC_Variant &operator =(const std::set<_T> &s);
    LLBC_Variant &operator =(const Dict &dict);
    template <typename _Key, typename _Val>
    LLBC_Variant &operator =(const std::map<_Key, _Val> &m);
    LLBC_Variant &operator =(const LLBC_Variant &var);
    LLBC_Variant &operator =(LLBC_Variant &&var);

    // Relational operators.
    bool operator ==(const LLBC_Variant &another) const;
    bool operator !=(const LLBC_Variant &another) const;

    bool operator <(const LLBC_Variant &another) const;
    bool operator >(const LLBC_Variant &another) const;
    bool operator <=(const LLBC_Variant &another) const;
    bool operator >=(const LLBC_Variant &another) const;

    // Relational operators.
    template <typename _T>
    bool operator ==(const _T &another) const;
    template <typename _T>
    bool operator !=(const _T &another) const;

    template <typename _T>
    bool operator <(const _T &another) const;
    template <typename _T>
    bool operator >(const _T &another) const;
    template <typename _T>
    bool operator <=(const _T &another) const;
    template <typename _T>
    bool operator >=(const _T &another) const;

    // Arithmetic operators.
    LLBC_Variant operator +(const LLBC_Variant &another) const;
    LLBC_Variant operator -(const LLBC_Variant &another) const;
    LLBC_Variant operator *(const LLBC_Variant &another) const;
    LLBC_Variant operator /(const LLBC_Variant &another) const;

    LLBC_Variant &operator +=(const LLBC_Variant &another);
    LLBC_Variant &operator -=(const LLBC_Variant &another);
    LLBC_Variant &operator *=(const LLBC_Variant &another);
    LLBC_Variant &operator /=(const LLBC_Variant &another);

    // Arithmetic operators(template base).
    template <typename _T>
    LLBC_Variant operator +(const _T &another) const;
    template <typename _T>
    LLBC_Variant operator -(const _T &another) const;
    template <typename _T>
    LLBC_Variant operator *(const _T &another) const;
    template <typename _T>
    LLBC_Variant operator /(const _T &another) const;

    template <typename _T>
    LLBC_Variant &operator +=(const _T &another);
    template <typename _T>
    LLBC_Variant &operator -=(const _T &another);
    template <typename _T>
    LLBC_Variant &operator *=(const _T &another);
    template <typename _T>
    LLBC_Variant &operator /=(const _T &another);

    // Type to string.
    const LLBC_String &TypeToString() const;
    // Value to string.
    LLBC_String ValueToString() const;
    // To string.
    LLBC_String ToString() const;

    // Serialize / DeSerialize support.
    void Serialize(LLBC_Stream &stream) const;
    bool DeSerialize(LLBC_Stream &stream);

public:
    friend std::ostream &::operator <<(std::ostream &o, const LLBC_Variant &variant);

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
    void CpToBinaryCont(_BinaryContainer &binaryCont);

    bool IsStrX() const;
    bool IsSeqX() const;
    bool IsDictX() const;

    LLBC_Variant &BecomeStrX();
    LLBC_Variant &BecomeSeqX();
    LLBC_Variant &BecomeDictX();

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


