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

#ifdef __LLBC_CORE_VARIANT_VARIANT_H__

__LLBC_NS_BEGIN

inline LLBC_Variant::LLBC_Variant()
{
}

inline LLBC_Variant::LLBC_Variant(const bool &boolVal)
{
    _holder.type = LLBC_VariantType::VT_RAW_BOOL;
    _holder.raw.int64Val = boolVal ? 1 : 0;
}

inline LLBC_Variant::LLBC_Variant(const sint8 &sint8Val)
{
    _holder.type = LLBC_VariantType::VT_RAW_SINT8;
    _holder.raw.int64Val = sint8Val;
}

inline LLBC_Variant::LLBC_Variant(const uint8 &uint8Val)
{
    _holder.type = LLBC_VariantType::VT_RAW_UINT8;
    _holder.raw.uint64Val = uint8Val;
}

inline LLBC_Variant::LLBC_Variant(const sint16 &sint16Val)
{
    _holder.type = LLBC_VariantType::VT_RAW_SINT16;
    _holder.raw.int64Val = sint16Val;
}

inline LLBC_Variant::LLBC_Variant(const uint16 &uint16Val)
{
    _holder.type = LLBC_VariantType::VT_RAW_UINT16;
    _holder.raw.uint64Val = uint16Val;
}

inline LLBC_Variant::LLBC_Variant(const sint32 &sint32Val)
{
    _holder.type = LLBC_VariantType::VT_RAW_SINT32;
    _holder.raw.int64Val = sint32Val;
}

inline LLBC_Variant::LLBC_Variant(const uint32 &uint32Val)
{
    _holder.type = LLBC_VariantType::VT_RAW_UINT32;
    _holder.raw.uint64Val = uint32Val;
}

inline LLBC_Variant::LLBC_Variant(const long &longVal)
{
    _holder.type = LLBC_VariantType::VT_RAW_LONG;
    _holder.raw.int64Val = longVal;
}

inline LLBC_Variant::LLBC_Variant(const ulong &ulongVal)
{
    _holder.type = LLBC_VariantType::VT_RAW_ULONG;
    _holder.raw.uint64Val = ulongVal;
}

template <typename _T>
inline LLBC_Variant::LLBC_Variant(const _T * const &ptrVal)
{
    _holder.type = LLBC_VariantType::VT_RAW_PTR;

    _holder.raw.uint64Val = 0;
    ::memcpy(&_holder.raw.uint64Val, &ptrVal, sizeof(_T *));
}

inline LLBC_Variant::LLBC_Variant(const sint64 &int64Val)
{
    _holder.type = LLBC_VariantType::VT_RAW_SINT64;
    _holder.raw.int64Val = int64Val;
}

inline LLBC_Variant::LLBC_Variant(const uint64 &uint64Val)
{
    _holder.type = LLBC_VariantType::VT_RAW_UINT64;
    _holder.raw.uint64Val = uint64Val;
}

inline LLBC_Variant::LLBC_Variant(const float &floatVal)
{
    _holder.type = LLBC_VariantType::VT_RAW_FLOAT;
    _holder.raw.doubleVal = floatVal;
}

inline LLBC_Variant::LLBC_Variant(const double &doubleVal)
{
    _holder.type = LLBC_VariantType::VT_RAW_DOUBLE;
    _holder.raw.doubleVal = doubleVal;
}

inline LLBC_Variant::LLBC_Variant(const std::string &strVal)
{
    _holder.type = LLBC_VariantType::VT_STR_DFT;
    if (!strVal.empty())
        _holder.obj.str = LLBC_New2(LLBC_String, strVal.data(), strVal.size());
}

inline LLBC_Variant::LLBC_Variant(const LLBC_String &strVal)
{
    _holder.type = LLBC_VariantType::VT_STR_DFT;
    if (!strVal.empty())
        _holder.obj.str = LLBC_New1(LLBC_String, strVal);
}

inline LLBC_Variant::LLBC_Variant(const LLBC_Variant::Dict &dictVal)
{
    _holder.type = LLBC_VariantType::VT_DICT_DFT;
    if (!dictVal.empty())
        _holder.obj.dict = LLBC_New1(Dict, dictVal);
}

inline int LLBC_Variant::GetType() const
{
    return _holder.type;
}

inline LLBC_Variant::Holder &LLBC_Variant::GetMutableHolder()
{
    return _holder;
}

inline const LLBC_Variant::Holder &LLBC_Variant::GetHolder() const
{
    return _holder;
}

inline bool LLBC_Variant::IsNil() const
{
    return (_holder.type  == LLBC_VariantType::VT_NIL);
}

inline bool LLBC_Variant::IsRaw() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW) == 
        LLBC_VariantType::VT_RAW);
}

inline bool LLBC_Variant::IsSignedRaw() const
{
    return (IsRaw() && 
        ((_holder.type & LLBC_VariantType::VT_MASK_RAW_SIGNED) == 
            LLBC_VariantType::VT_MASK_RAW_SIGNED));
}

inline bool LLBC_Variant::IsUnsignedRaw() const
{
    return (IsRaw() &&
        ((_holder.type & LLBC_VariantType::VT_MASK_RAW_SIGNED) !=
            LLBC_VariantType::VT_MASK_RAW_SIGNED));
}

inline bool LLBC_Variant::IsBool() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_BOOL) == 
        LLBC_VariantType::VT_RAW_BOOL);
}

inline bool LLBC_Variant::IsInt8() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_SINT8) ==
        LLBC_VariantType::VT_RAW_SINT8);
}

inline bool LLBC_Variant::IsUInt8() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_UINT8) ==
        LLBC_VariantType::VT_RAW_UINT8);
}

inline bool LLBC_Variant::IsInt16() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_SINT16) ==
        LLBC_VariantType::VT_RAW_SINT16);
}

inline bool LLBC_Variant::IsUInt16() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_UINT16) ==
        LLBC_VariantType::VT_RAW_UINT16);
}

inline bool LLBC_Variant::IsInt32() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_SINT32) ==
        LLBC_VariantType::VT_RAW_SINT32);
}

inline bool LLBC_Variant::IsUInt32() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_UINT32) ==
        LLBC_VariantType::VT_RAW_UINT32);
}

inline bool LLBC_Variant::IsLong() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_LONG) ==
        LLBC_VariantType::VT_RAW_LONG);
}

inline bool LLBC_Variant::IsULong() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_ULONG) ==
        LLBC_VariantType::VT_RAW_ULONG);
}

inline bool LLBC_Variant::IsPtr() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_PTR) ==
        LLBC_VariantType::VT_RAW_PTR);
}

inline bool LLBC_Variant::IsInt64() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_SINT64) ==
        LLBC_VariantType::VT_RAW_SINT64);
}

inline bool LLBC_Variant::IsUInt64() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_UINT64) ==
        LLBC_VariantType::VT_RAW_UINT64);
}

inline bool LLBC_Variant::IsFloat() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_FLOAT) ==
        LLBC_VariantType::VT_RAW_FLOAT);
}

inline bool LLBC_Variant::IsDouble() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_DOUBLE) ==
        LLBC_VariantType::VT_RAW_DOUBLE);
}

inline bool LLBC_Variant::IsStr() const
{
    return ((_holder.type & LLBC_VariantType::VT_STR_DFT) ==
        LLBC_VariantType::VT_STR_DFT);
}

inline bool LLBC_Variant::IsSeq() const
{
    return ((_holder.type & LLBC_VariantType::VT_SEQ_DFT) ==
        LLBC_VariantType::VT_SEQ_DFT);
}

inline bool LLBC_Variant::IsDict() const
{
    return ((_holder.type & LLBC_VariantType::VT_DICT_DFT) ==
        LLBC_VariantType::VT_DICT_DFT);
}

inline LLBC_Variant &LLBC_Variant::BecomeNil()
{
    if (IsNil())
        return *this;

    CleanTypeData(_holder.type);
    _holder.type = LLBC_VariantType::VT_NIL;

    return *this;
}

inline LLBC_Variant &LLBC_Variant::BecomeBool()
{
    if (!IsBool())
        *this = LLBC_Variant(AsBool());

    return *this;
}

inline LLBC_Variant &LLBC_Variant::BecomeInt8()
{
    if (!IsInt8())
        *this = AsInt8();

    return *this;
}

inline LLBC_Variant &LLBC_Variant::BecomeUInt8()
{
    if (!IsUInt8())
        *this = AsUInt8();

    return *this;
}

inline LLBC_Variant &LLBC_Variant::BecomeInt16()
{
    if (!IsInt16())
        *this = AsInt16();

    return *this;
}

inline LLBC_Variant &LLBC_Variant::BecomeUInt16()
{
    if (!IsUInt16())
        *this = AsUInt16();

    return *this;
}

inline LLBC_Variant &LLBC_Variant::BecomeInt32()
{
    if (!IsInt32())
        *this = AsInt32();

    return *this;
}

inline LLBC_Variant &LLBC_Variant::BecomeUInt32()
{
    if (!IsUInt32())
        *this = AsUInt32();

    return *this;
}

inline LLBC_Variant &LLBC_Variant::BecomeLong()
{
    if (!IsLong())
        *this = AsLong();

    return *this;
}

inline LLBC_Variant &LLBC_Variant::BecomeULong()
{
    if (!IsULong())
        *this = AsULong();

    return *this;
}

inline LLBC_Variant &LLBC_Variant::BecomePtr()
{
    if (!IsPtr())
        *this = AsPtr<void>();

    return *this;
}

inline LLBC_Variant &LLBC_Variant::BecomeInt64()
{
    if (!IsInt64())
        *this = AsInt64();

    return *this;
}

inline LLBC_Variant &LLBC_Variant::BecomeUInt64()
{
    if (!IsUInt64())
        *this = AsUInt64();

    return *this;
}

inline LLBC_Variant &LLBC_Variant::BecomeFloat()
{
    if (!IsFloat())
        *this = AsFloat();

    return *this;
}

inline LLBC_Variant &LLBC_Variant::BecomeDouble()
{
    if (!IsDouble())
        *this = AsDouble();

    return *this;
}

inline LLBC_Variant &LLBC_Variant::BecomeStr()
{
    if (!IsStr())
    {
        CleanTypeData(_holder.type);
        _holder.type = LLBC_VariantType::VT_STR_DFT;
    }

    return *this;
}

inline LLBC_Variant &LLBC_Variant::BecomeSeq()
{
    if (!IsSeq())
    {
        CleanTypeData(_holder.type);
        _holder.type = LLBC_VariantType::VT_SEQ_DFT;
    }

    return *this;
}

inline LLBC_Variant &LLBC_Variant::BecomeDict()
{
    if (!IsDict())
    {
        CleanTypeData(_holder.type);
        _holder.type = LLBC_VariantType::VT_DICT_DFT;
    }

    return *this;
}

inline sint8 LLBC_Variant::AsInt8() const
{
    return static_cast<sint8>(AsInt64());
}

inline uint8 LLBC_Variant::AsUInt8() const
{
    return static_cast<uint8>(AsUInt64());
}

inline sint16 LLBC_Variant::AsInt16() const
{
    return static_cast<sint16>(AsInt64());
}

inline uint16 LLBC_Variant::AsUInt16() const
{
    return static_cast<uint16>(AsUInt64());
}

inline sint32 LLBC_Variant::AsInt32() const
{
    return static_cast<sint32>(AsInt64());
}

inline uint32 LLBC_Variant::AsUInt32() const
{
    return static_cast<uint32>(AsUInt64());
}

inline long LLBC_Variant::AsLong() const
{
    return static_cast<long>(AsInt64());
}

inline unsigned long LLBC_Variant::AsULong() const
{
    return static_cast<unsigned long>(AsUInt64());
}

template <typename _T>
inline _T *LLBC_Variant::AsPtr() const
{
    return reinterpret_cast<_T *>(AsUInt64());
}

inline float LLBC_Variant::AsFloat() const
{
    return static_cast<float>(AsDouble());
}

inline LLBC_Variant::operator bool() const
{
    return AsBool();
}

inline LLBC_Variant::operator sint8() const
{
    return AsInt8();
}

inline LLBC_Variant::operator uint8() const
{
    return AsUInt8();
}

inline LLBC_Variant::operator sint16() const
{
    return AsInt16();
}

inline LLBC_Variant::operator uint16() const
{
    return AsUInt16();
}

inline LLBC_Variant::operator sint32() const
{
    return AsInt32();
}

inline LLBC_Variant::operator uint32() const
{
    return AsUInt32();
}

inline LLBC_Variant::operator long() const
{
    return AsLong();
}

inline LLBC_Variant::operator ulong() const
{
    return AsULong();
}

template <typename _T>
inline LLBC_Variant::operator _T *() const
{
    return AsPtr<_T>();
}

inline LLBC_Variant::operator sint64() const
{
    return AsInt64();
}

inline LLBC_Variant::operator uint64() const
{
    return AsUInt64();
}

inline LLBC_Variant::operator float() const
{
    return AsFloat();
}

inline LLBC_Variant::operator double() const
{
    return AsDouble();
}

inline LLBC_Variant::operator LLBC_String () const
{
    return AsStr();
}

inline LLBC_Variant::operator const LLBC_Variant::Seq &() const
{
    return AsSeq();
}

inline LLBC_Variant::operator const LLBC_Variant::Dict &() const
{
    return AsDict();
}

template <typename _T>
inline LLBC_Variant &LLBC_Variant::operator =(const _T * const &val)
{
    CleanTypeData(_holder.type);

    _holder.type = LLBC_VariantType::VT_RAW_PTR;

    _holder.raw.uint64Val = 0;
    ::memcpy(&_holder.raw.uint64Val, &val, sizeof(_T *));

    return *this;
}

template <typename _Ty>
inline LLBC_Variant::SeqIter LLBC_Variant::SeqInsert(SeqIter it, const _Ty &val)
{
    return this->SeqInsert(it, LLBC_Variant(val));
}
template <typename _Ty>
void LLBC_Variant::SeqInsert(SeqIter it, Seq::size_type n, const _Ty &val)
{
    this->SeqInsert(it, n, LLBC_Variant(val));
}

template <typename _Ty>
void LLBC_Variant::SeqPushBack(const _Ty &val)
{
    this->SeqPushBack(LLBC_Variant(val));
}

template <typename _Ty>
void LLBC_Variant::SeqResize(Seq::size_type n, const _Ty &val)
{
    this->SeqResize(n, LLBC_Variant(val));
}

template <typename _Ty>
void LLBC_Variant::SeqErase(const _Ty &val)
{
    this->SeqErase(LLBC_Variant(val));
}

template <typename _Kty, typename _Ty>
inline std::pair<LLBC_Variant::DictIter, bool> LLBC_Variant::DictInsert(const _Kty &key, const _Ty &val)
{
    return this->DictInsert(LLBC_Variant::Dict::key_type(key),
        LLBC_Variant::Dict::mapped_type(val));
}

template <typename _Kty>
inline LLBC_Variant::DictIter LLBC_Variant::DictFind(const _Kty &key)
{
    return this->DictFind(LLBC_Variant::Dict::key_type(key));
}

template <typename _Kty>
inline LLBC_Variant::DictConstIter LLBC_Variant::DictFind(const _Kty &key) const
{
    return this->Find(LLBC_Variant::Dict::key_type(key));
}

template <typename _Kty>
inline LLBC_Variant::Dict::size_type LLBC_Variant::DictErase(const _Kty &key)
{
    return this->DictErase(LLBC_Variant::Dict::key_type(key));
}

template <typename _Kty>
inline LLBC_Variant &LLBC_Variant::operator [](const _Kty &key)
{
    return this->operator [](LLBC_Variant(key));
}

template <typename _Kty>
inline const LLBC_Variant &LLBC_Variant::operator [](const _Kty &key) const
{
    return this->operator [](LLBC_Variant(key));
}

__LLBC_NS_END

#endif // __LLBC_CORE_VARIANT_VARIANT_H__
