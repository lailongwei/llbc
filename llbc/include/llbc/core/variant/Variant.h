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
 *      format(16bit): first_type[high 8 bit] second_type[low 8 bit]
 *          first_type:
 *              raw type:        The row data type, like int32, uint32 ...eg.
 *              string type:     The string data type, use LLBC_String.
 *              sequence type:   The sequence data type, use LLBC_Variant::Seq.
 *              dictionary type: The dictionary data type, use LLBC_Variant::Dict.
 *              Others... :      ....
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
        //                              [first type] [raw type] [signed/unsigned]
        //                                  8 bits     7 bits        1 bit     
        RAW_BOOL             = 0x0102, //    0x01      0000001         0
        RAW_SINT8            = 0x0105, //    0x01      0000010         1
        RAW_UINT8            = 0x0106, //    0x01      0000011         0
        RAW_SINT16           = 0x0109, //    0x01      0000100         1
        RAW_UINT16           = 0x010a, //    0x01      0000101         0
        RAW_SINT32           = 0x010d, //    0x01      0000110         1
        RAW_UINT32           = 0x010e, //    0x01      0000111         0
        RAW_LONG             = 0x0111, //    0x01      0001000         1
        RAW_ULONG            = 0x0112, //    0x01      0001001         0
        RAW_PTR              = 0x0114, //    0x01      0001010         0
        RAW_SINT64           = 0x0117, //    0x01      0001011         1
        RAW_UINT64           = 0x0118, //    0x01      0001100         0
        RAW_FLOAT            = 0x011b, //    0x01      0001101         1
        RAW_DOUBLE           = 0x011d, //    0x01      0001110         1

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

    // RAW types.
    static constexpr std::array<ENUM, 14> RAW_TYPES {
        RAW_BOOL,

        RAW_SINT8,
        RAW_UINT8,

        RAW_SINT16,
        RAW_UINT16,

        RAW_SINT32,
        RAW_UINT32,

        RAW_LONG,
        RAW_ULONG,

        RAW_PTR,

        RAW_SINT64,
        RAW_UINT64,

        RAW_FLOAT,
        RAW_DOUBLE,
    };

    // STR types.
    static constexpr std::array<ENUM, 1> STR_TYPES {
        STR_DFT,
    };

    // SEQ types.
    static constexpr std::array<ENUM, 1> SEQ_TYPES {
        SEQ_DFT,
    };

    // DICT types.
    static constexpr std::array<ENUM, 1> DICT_TYPES {
        DICT_DFT,
    };

    // ALL types.
    static constexpr std::array<ENUM, 18> ALL_TYPES {
        // NIL(1):
        NIL,

        // RAW(14):
        RAW_BOOL,
        RAW_SINT8,
        RAW_UINT8,
        RAW_SINT16,
        RAW_UINT16,
        RAW_SINT32,
        RAW_UINT32,
        RAW_LONG,
        RAW_ULONG,
        RAW_PTR,
        RAW_SINT64,
        RAW_UINT64,
        RAW_FLOAT,
        RAW_DOUBLE,

        // STR(1):
        STR_DFT,

        // SEQ(1):
        SEQ_DFT,

        // DICT(1):
        DICT_DFT,
    };

public:
    /**
     * Type convertable check.
     * @return bool - return true if convertable to Variant, otherwise return false.
     */
    template <typename _Ty>
    static constexpr bool IsConvertable();

public:
    /**
     * Deduce variant type.
     */
    template <typename _Ty>
    static constexpr ENUM DeduceType();

public:
    /**
     * Get first type.
     * @param[in] type - the variant type.
     * @return int - the variant first type.
     */
    static constexpr ENUM GetFirstType(int type) { return static_cast<ENUM>(type & MASK_FIRST_TYPE); }

    /**
     * Type enum validate check.
     */
    static constexpr bool IsValid(int type);

    /**
     * Nil-Type introspection support methods. 
     */
    static constexpr bool IsNil(int type) { return type == NIL; }
    template <typename _Ty>
    static constexpr bool IsNil();

    /**
     * Raw-Type introspection support methods.
     */
    static constexpr bool IsRaw(int type) { return GetFirstType(type) == RAW; }
    template <typename _Ty>
    static constexpr bool IsRaw();

    static constexpr bool IsSigned(int type)
    {
        return IsRaw(type) && (type & MASK_RAW_SIGNED) == MASK_RAW_SIGNED;
    }
    template <typename _Ty>
    static constexpr bool IsSigned();

    static constexpr bool IsUnsigned(int type)
    {
        return IsRaw(type) && (type & MASK_RAW_SIGNED) != MASK_RAW_SIGNED;
    }
    template <typename _Ty>
    static constexpr bool IsUnsigned();

    /**
     * Str-Type introspection support methods.
     */
    static constexpr bool IsStr(int type) { return GetFirstType(type) == STR; }
    template <typename _Ty>
    static constexpr bool IsStr();

    /**
     * Seq-Type introspection support methods.
     */
    static constexpr bool IsSeq(int type) { return GetFirstType(type) == SEQ; }
    template <typename _Ty>
    static constexpr bool IsSeq();

    /**
     * Dict-Type introspection support methods.
     */
    static constexpr bool IsDict(int type) { return GetFirstType(type) == DICT; }
    template <typename _Ty>
    static constexpr bool IsDict();

    /**
     * Get type string representation.
     * @param[in] type - the variant type enumeration.
     * @return const LLBC_String & - the type string representation.
     */
    static const LLBC_String &Type2Str(int type);

    /**
     * Get type string representation(template function).
     * @return const LLBC_String & - the type string representation.
     */
    template <typename _Ty>
    static const LLBC_String &Type2Str();
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

private:
    /**
     * @The variant data union encapsulation.
     */
    union Data
    {
        union RawType
        {
            sint64 i64;
            uint64 ui64;
            double dbl;
        } raw;

        union ObjType
        {
            Str str;
            Seq seq;
            Dict dict;
        } obj;

        Data() = default;
        ~Data() = default;

        sint64 &i64() { return raw.i64; }
        const sint64 &i64() const { return raw.i64; }
        uint64 &ui64() { return raw.ui64; }
        const uint64 &ui64() const { return raw.ui64; }
        double &dbl() { return raw.dbl; }
        const double &dbl() const { return raw.dbl; }

        Str &str() { return obj.str; }
        const Str &str() const { return obj.str; }

        Seq &seq() { return obj.seq; }
        const Seq &seq() const { return obj.seq; }

        Dict &dict() { return obj.dict; }
        const Dict &dict() const { return obj.dict; }
    };

public:
    // Initialize the number to string fast access table.
    static void InitNumber2StrFastAccessTable();
    // Destroy the number to string fast access table.
    static void DestroyNumber2StrFastAccessTable();

public:
    // Constructors.
    LLBC_Variant();
    template <typename _Ty,
              std::enable_if_t<LLBC_VariantType::IsRaw<_Ty>(), int> = 0>
    explicit LLBC_Variant(const _Ty &raw);
    template <typename _Ty,
              std::enable_if_t<LLBC_VariantType::IsStr<_Ty>(), int> = 0>
    explicit LLBC_Variant(_Ty &&str);
    template <typename _Ty,
              std::enable_if_t<LLBC_VariantType::IsSeq<_Ty>(), int> = 0>
    explicit LLBC_Variant(_Ty &&seq);
    template <typename _Ty,
              std::enable_if_t<LLBC_VariantType::IsDict<_Ty>(), int> = 0>
    explicit LLBC_Variant(_Ty &&dict);
    LLBC_Variant(const LLBC_Variant &var);
    LLBC_Variant(LLBC_Variant &&var) noexcept;

public:
    // Assignment operators.
    template <typename _Ty,
              std::enable_if_t<LLBC_VariantType::IsRaw<_Ty>(), int> = 0>
    LLBC_Variant &operator=(const _Ty &raw);
    template <typename _Ty,
              std::enable_if_t<LLBC_VariantType::IsStr<_Ty>(), int> = 0>
    LLBC_Variant &operator=(_Ty &&str);
    template <typename _Ty,
              std::enable_if_t<LLBC_VariantType::IsSeq<_Ty>(), int> = 0>
    LLBC_Variant &operator=(_Ty &&seq);
    template <typename _Ty,
              std::enable_if_t<LLBC_VariantType::IsDict<_Ty>(), int> = 0>
    LLBC_Variant &operator=(_Ty &&dict);
    LLBC_Variant &operator=(const LLBC_Variant &var);
    LLBC_Variant &operator=(LLBC_Variant &&var) noexcept;

public:
    // Type introspection support methods.
    LLBC_VariantType::ENUM GetType() const { return _type; }
    LLBC_VariantType::ENUM GetFirstType() const { return LLBC_VariantType::GetFirstType(_type); }

    template <typename... _Tys>
    bool Is() const;

    LLBC_DEPRECATED_EX("Use Is<void>() instead")
    bool IsNil() const { return LLBC_VariantType::IsNil(_type); }

    bool IsRaw() const { return LLBC_VariantType::IsRaw(_type); }
    bool IsSigned() const { return LLBC_VariantType::IsSigned(_type); }
    bool IsUnsigned() const { return LLBC_VariantType::IsUnsigned(_type); }

    LLBC_DEPRECATED_EX("Use Is<LLBC_Variant::Str>() instead")
    bool IsStr() const { return LLBC_VariantType::IsStr(_type); }

    LLBC_DEPRECATED_EX("Use Is<LLBC_Variant::Seq>() instead")
    bool IsSeq() const { return LLBC_VariantType::IsSeq(_type); }

    LLBC_DEPRECATED_EX("Use Is<LLBC_Variant::Dict>() instead")
    bool IsDict() const { return LLBC_VariantType::IsDict(_type); }

public:
    // Variant mutable-data access(Dangerous).
    Data &GetData() { return _data; }
    // Variant imutable-data access(Dangerous).
    const Data &GetData() const { return _data; }

public:
    // Change variant internal type.
    template <typename _Ty>
    LLBC_Variant &Become();
    LLBC_Variant &Become(LLBC_VariantType::ENUM ty);
    LLBC_DEPRECATED_EX("Use Become<void>() or Become(LLBC_VariantType::NIL) instead")
    LLBC_Variant &BecomeNil() { return Become(LLBC_VariantType::NIL); }

public:
    // Explicit value fetch.
    template <typename _Ty>
    std::enable_if_t<LLBC_VariantType::IsNil<_Ty>(), const LLBC_Variant &>
    As() const;

    template <typename _Ty>
    std::enable_if_t<!std::is_reference_v<_Ty> && LLBC_VariantType::IsRaw<_Ty>(), _Ty>
    As() const;

    template <typename _Ty>
    std::enable_if_t<!std::is_reference_v<_Ty> &&
                        (std::is_same_v<std::remove_cv_t<_Ty>, std::string> ||
                         std::is_same_v<std::remove_cv_t<_Ty>, LLBC_String>),
                     _Ty>
    As() const;
    template <typename _Ty>
    std::enable_if_t<!std::is_reference_v<_Ty> &&
                        (std::is_same_v<std::remove_cv_t<_Ty>, std::string_view> ||
                         std::is_same_v<std::remove_cv_t<_Ty>, LLBC_CString>),
                      _Ty>
    As() const;
    template <typename _Ty>
    std::enable_if_t<!std::is_reference_v<_Ty> &&
                        LLBC_VariantType::IsStr<_Ty>() &&
                        (std::is_pointer_v<_Ty> &&
                         std::is_same_v<std::remove_cv_t<std::remove_pointer_t<_Ty>>, char>),
                     _Ty>
    As(size_t *strLen = nullptr) const;

    template <typename _Ty>
    std::enable_if_t<std::is_same_v<std::remove_cv_t<std::remove_reference_t<_Ty>>, LLBC_Variant::Seq>,
                     const LLBC_Variant::Seq &>
    As() const { return Is<Seq>() ? _data.seq() : _emptySeq; }
    template <typename _Ty>
    std::enable_if_t<!std::is_reference_v<_Ty> &&
                        !std::is_same_v<std::remove_cv_t<_Ty>, LLBC_Variant::Seq> &&
                        LLBC_VariantType::IsSeq<_Ty>(),
                      _Ty>
    As() const;

    template <typename _Ty>
    std::enable_if_t<std::is_same_v<std::remove_cv_t<std::remove_reference_t<_Ty>>, LLBC_Variant::Dict>,
                     const LLBC_Variant::Dict &>
    As() const { return Is<Dict>() ? _data.dict() : _emptyDict; }
    template <typename _Ty>
    std::enable_if_t<!std::is_reference_v<_Ty> &&
                        !std::is_same_v<std::remove_cv_t<_Ty>, LLBC_Variant::Dict> &&
                        LLBC_VariantType::IsDict<_Ty>(),
                     _Ty>
    As() const;

    bool AsLooseBool() const;

public:
    // Implicit value fetch.
    template <typename _Ty>
    operator _Ty() const { return As<_Ty>(); }

public:
    // Common operation method: Clear().
    // - For NIL: no effect.
    // - For RAW: reset data to RawType().
    // - For STR: clear string(Str::clear()).
    // - For SEQ: clear sequence(Seq::clear()).
    // - For DICT: clear dictionary(Dict::clear()).
    void Clear();

    // Common operation method: IsEmpty().
    // - For NIL: always return true.
    // - For RAW: always reutrn true.
    // - For STR: return true if string is empty, otherwise return false(not empty).
    // - For SEQ: return true if sequence is empty, otherwise return false(not empty).
    // - For DICT: return true if dictionary is empty, otherwise return false(not empty).
    bool IsEmpty() const;

    // Common operation method: Size().
    // - For NIL: always return 0.
    // - For RAW: always reutrn 0.
    // - For STR: return string size.
    // - For SEQ: return sequence size.
    // - For DICT: return dictionary size.
    size_t Size() const;

    // Common operation method: Capacity().
    // - For NIL: always return 0.
    // - For RAW: always reutrn 0.
    // - For STR: return string capacity.
    // - For SEQ: return sequence capacity.
    // - For DICT: return dictionary size(same with Size() method).
    size_t Capacity() const;

    // Common operatin method: Count().
    // For NIL: always return 0.
    // For RAW: always return 0.
    // For STR: return matched sub string count(key auto converted to STR type before search).
    // For SEQ: return matched element count.
    // For DICT: return 1 if found, otherwise reutrn 0.
    template <typename _Key>
    size_t Count(const _Key &key) const { return CountImpl(key, false); }

    // Common operation method: Contains().
    // For NIL: always return false.
    // For RAW: always return false.
    // For STR: return true if found in STR, otherwise return false.
    // For SEQ: return true if found in SEQ, otherwise return false.
    // For DIT: return true if found, otherwise return false.
    template <typename _Key>
    bool Contains(const _Key &key) const {return CountImpl(key, true) >= 1; }

public:
    // Str type variant object specify Operate methods.
    void StrResize(Str::size_type newSize, Str::value_type ch = Str::value_type());
    void StrReserve(Str::size_type newCap);

public:
    // Sequence type variant object specify Operate methods.
    SeqIter SeqBegin() { return Become<Seq>()._data.seq().begin(); }
    SeqIter SeqEnd() { return Become<Seq>()._data.seq().end(); }
    SeqConstIter SeqBegin() const { return As<Seq>().begin(); }
    SeqConstIter SeqEnd() const { return As<Seq>().end(); }
    SeqReverseIter SeqReverseBegin() { return Become<Seq>()._data.seq().rbegin(); }
    SeqReverseIter SeqReverseEnd() { return Become<Seq>()._data.seq().rend(); }
    SeqConstReverseIter SeqReverseBegin() const { return As<Seq>().rbegin(); }
    SeqConstReverseIter SeqReverseEnd() const { return As<Seq>().rend(); }

    Seq::reference SeqFront() { return Become<Seq>()._data.seq().front(); }
    Seq::reference SeqBack() { return Become<Seq>()._data.seq().back(); }
    Seq::const_reference SeqFront() const { return As<Seq>().front(); }
    Seq::const_reference SeqBack() const { return As<Seq>().back(); }

    template <typename _Ty>
    SeqIter SeqInsert(SeqIter it, _Ty &&val);
    template <typename _Ty>
    SeqIter SeqInsert(SeqIter it, Seq::size_type n, const _Ty &val);
    SeqIter SeqInsert(SeqIter it, SeqConstIter first, SeqConstIter last);

    template <typename... _Tys>
    SeqIter SeqBatchInsert(SeqIter it, _Tys &&... vals);

    template <typename _Ty>
    void SeqPushBack(_Ty &&val) { Become<Seq>()._data.seq().emplace_back(std::forward<_Ty>(val)); }
    void SeqPopBack();

    template <typename... _Tys>
    void SeqBatchPushBack(_Tys &&... vals);

    template <typename _Ty>
    void SeqResize(Seq::size_type newSize, const _Ty &val = _Ty());
    void SeqReserve(Seq::size_type newCap) { Become<Seq>()._data.seq().reserve(newCap); }

    SeqIter SeqErase(SeqIter it);
    SeqIter SeqErase(SeqConstIter it);
    SeqIter SeqErase(SeqIter first, SeqIter last);
    SeqIter SeqErase(SeqConstIter first, SeqConstIter last);
    template <typename _Ty>
    size_t SeqErase(const _Ty &val, size_t eraseCount = LLBC_INFINITE, bool stableErase = true);

public:
    // Dictionary type variant object specify Operate methods.
    DictIter DictBegin() { return Become<Dict>()._data.dict().begin(); }
    DictIter DictEnd() { return Become<Dict>()._data.dict().end(); }
    DictConstIter DictBegin() const { return As<Dict>().begin(); }
    DictConstIter DictEnd() const { return As<Dict>().end(); }
    DictReverseIter DictReverseBegin() { return Become<Dict>()._data.dict().rbegin(); }
    DictReverseIter DictReverseEnd() { return Become<Dict>()._data.dict().rend(); }
    DictConstReverseIter DictReverseBegin() const { return As<Dict>().rbegin(); }
    DictConstReverseIter DictReverseEnd() const { return As<Dict>().rend(); }

    std::pair<DictIter, bool> DictInsert(const Dict::value_type &val);
    std::pair<DictIter, bool> DictInsert(Dict::value_type &&val);
    template <typename _Key, typename _Val>
    std::pair<DictIter, bool> DictInsert(_Key &&key, _Val &&val);

    template <typename _Key>
    DictIter DictFind(const _Key &key);
    template <typename _Key>
    DictConstIter DictFind(const _Key &key) const;

    DictIter DictErase(DictIter it);
    DictIter DictErase(DictConstIter it);
    DictIter DictErase(DictIter first, DictIter last);
    DictIter DictErase(DictConstIter first, DictConstIter last);
    template <typename... _Keys>
    size_t DictErase(const _Keys &... keys);

public:
    template <typename _Key>
    LLBC_Variant &operator[](const _Key &key);
    template <typename _Key>
    const LLBC_Variant &operator[](const _Key &key) const;

public:
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
    const LLBC_String &TypeToString() const { return LLBC_VariantType::Type2Str(_type); }
    // Value to string.
    LLBC_String ValueToString() const { return As<Str>(); }
    // To string.
    LLBC_String ToString() const;

    // Serialize / Deserialize support.
    void Serialize(LLBC_Stream &stream) const;
    bool Deserialize(LLBC_Stream &stream);

public:
    friend std::ostream &::operator<<(std::ostream &o, const LLBC_Variant &variant);

private:
    friend class LLBC_VariantTraits;
    friend class ::std::hash<LLBC_Variant>;

    template <typename _Key>
    size_t CountImpl(const _Key &key, bool returnIfFound) const;

    void Reset(LLBC_VariantType::ENUM newType);

    template <typename _64Ty>
    _64Ty AsSignedOrUnsigned64() const;

    template <typename _Key>
    Dict::size_type DictEraseOne(const _Key &key);

private:
    LLBC_VariantType::ENUM _type;
    Data _data;

    static Str **_num2StrFastAccessTbl;

    static const Str _trueStr;
    static const Str _falseStr;
    static const Str _emptySeqStr;
    static const Str _emptyDictStr;
    static const std::string _trueSTLStr;
    static const std::string _falseSTLStr;
    static const std::string _emptySTLSeqStr;
    static const std::string _emptySTLDictStr;

    static const Str _emptyStr;
    static const std::string _emptySTLStr;
    static const Seq _emptySeq;
    static const Dict _emptyDict;
};

__LLBC_NS_END

#include "llbc/core/variant/VariantInl.h"


