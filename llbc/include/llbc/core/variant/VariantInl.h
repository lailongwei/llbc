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

#include "llbc/core/variant/VariantTraits.h"
#include "llbc/core/objbase/KeyHashAlgorithm.h"

__LLBC_NS_BEGIN

inline LLBC_Variant::LLBC_Variant()
{
}

inline LLBC_Variant::LLBC_Variant(const bool &b)
{
    _holder.type = LLBC_VariantType::VT_RAW_BOOL;
    _holder.data.raw.int64Val = b ? 1 : 0;
}

inline LLBC_Variant::LLBC_Variant(const sint8 &i8)
{
    _holder.type = LLBC_VariantType::VT_RAW_SINT8;
    _holder.data.raw.int64Val = static_cast<sint64>(i8);
}

inline LLBC_Variant::LLBC_Variant(const uint8 &ui8)
{
    _holder.type = LLBC_VariantType::VT_RAW_UINT8;
    _holder.data.raw.uint64Val = ui8;
}

inline LLBC_Variant::LLBC_Variant(const sint16 &i16)
{
    _holder.type = LLBC_VariantType::VT_RAW_SINT16;
    _holder.data.raw.int64Val = i16;
}

inline LLBC_Variant::LLBC_Variant(const uint16 &ui16)
{
    _holder.type = LLBC_VariantType::VT_RAW_UINT16;
    _holder.data.raw.uint64Val = ui16;
}

inline LLBC_Variant::LLBC_Variant(const sint32 &i32)
{
    _holder.type = LLBC_VariantType::VT_RAW_SINT32;
    _holder.data.raw.int64Val = i32;
}

inline LLBC_Variant::LLBC_Variant(const uint32 &ui32)
{
    _holder.type = LLBC_VariantType::VT_RAW_UINT32;
    _holder.data.raw.uint64Val = ui32;
}

inline LLBC_Variant::LLBC_Variant(const long &l)
{
    _holder.type = LLBC_VariantType::VT_RAW_LONG;
    _holder.data.raw.int64Val = l;
}

inline LLBC_Variant::LLBC_Variant(const ulong &ul)
{
    _holder.type = LLBC_VariantType::VT_RAW_ULONG;
    _holder.data.raw.uint64Val = ul;
}

template <typename _T>
inline LLBC_Variant::LLBC_Variant(const _T * const &ptr)
{
    _holder.type = LLBC_VariantType::VT_RAW_PTR;

    _holder.data.raw.uint64Val = 0;
    memcpy(&_holder.data.raw.uint64Val, &ptr, sizeof(_T *));
}

inline LLBC_Variant::LLBC_Variant(const sint64 &i64)
{
    _holder.type = LLBC_VariantType::VT_RAW_SINT64;
    _holder.data.raw.int64Val = i64;
}

inline LLBC_Variant::LLBC_Variant(const uint64 &ui64)
{
    _holder.type = LLBC_VariantType::VT_RAW_UINT64;
    _holder.data.raw.uint64Val = ui64;
}

inline LLBC_Variant::LLBC_Variant(const float &f)
{
    _holder.type = LLBC_VariantType::VT_RAW_FLOAT;
    _holder.data.raw.doubleVal = f;
}

inline LLBC_Variant::LLBC_Variant(const double &d)
{
    _holder.type = LLBC_VariantType::VT_RAW_DOUBLE;
    _holder.data.raw.doubleVal = d;
}

inline LLBC_Variant::LLBC_Variant(const std::string &str)
{
    _holder.type = LLBC_VariantType::VT_STR_DFT;
    if (!str.empty())
        _holder.data.obj.str = new LLBC_String(str.data(), str.size());
}

inline LLBC_Variant::LLBC_Variant(const LLBC_String &str)
{
    _holder.type = LLBC_VariantType::VT_STR_DFT;
    if (!str.empty())
        _holder.data.obj.str = new LLBC_String(str.data(), str.size());
}

inline LLBC_Variant::LLBC_Variant(const Seq &seq)
{
    _holder.type = LLBC_VariantType::VT_SEQ_DFT;
    if (!seq.empty())
        _holder.data.obj.seq = new Seq(seq.begin(), seq.end());
}

template <typename _T>
LLBC_Variant::LLBC_Variant(const std::vector<_T> &seq)
{
    CtFromUnaryCont<_T, std::vector<_T> >(seq);
}
template <typename _T>
LLBC_Variant::LLBC_Variant(const std::list<_T> &lst)
{
    CtFromUnaryCont<_T, std::list<_T> >(lst);
}

template <typename _T>
LLBC_Variant::LLBC_Variant(const std::deque<_T> &dqe)
{
    CtFromUnaryCont<_T, std::deque<_T> >(dqe);
}
template <typename _T>
LLBC_Variant::LLBC_Variant(const std::queue<_T> &que)
{
    CtFromUnaryCont<_T, std::queue<_T> >(que);
}

template <typename _T>
LLBC_Variant::LLBC_Variant(const std::set<_T> &s)
{
    CtFromUnaryCont<_T, std::set<_T> >(s);
}

template <typename _T>
LLBC_Variant::LLBC_Variant(const std::unordered_set<_T> &us)
{
    CtFromUnaryCont<_T, std::unordered_set<_T> >(us);
}

inline LLBC_Variant::LLBC_Variant(const LLBC_Variant::Dict &dict)
{
    _holder.type = LLBC_VariantType::VT_DICT_DFT;
    _holder.data.obj.dict = new Dict(dict);
}

template <typename _Key, typename _Val>
LLBC_Variant::LLBC_Variant(const std::map<_Key, _Val> &m)
{
    CtFromBinaryCont<_Key, _Val, std::map<_Key, _Val> >(m);
}

template <typename _Key, typename _Val>
LLBC_Variant::LLBC_Variant(const std::unordered_map<_Key, _Val> &um)
{
    CtFromBinaryCont<_Key, _Val, std::unordered_map<_Key, _Val> >(um);
}

inline LLBC_VariantType::ENUM LLBC_Variant::GetType() const
{
    return _holder.type;
}

inline LLBC_VariantType::ENUM LLBC_Variant::GetFirstType() const
{
    return _holder.GetFirstType();
}

inline LLBC_Variant::Holder *LLBC_Variant::GetMutableHolder()
{
    return &_holder;
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
    return ((_holder.type & LLBC_VariantType::VT_RAW) == LLBC_VariantType::VT_RAW);
}

inline bool LLBC_Variant::IsSignedRaw() const
{
    return (IsRaw() && ((_holder.type & LLBC_VariantType::VT_MASK_RAW_SIGNED) == LLBC_VariantType::VT_MASK_RAW_SIGNED));
}

inline bool LLBC_Variant::IsUnsignedRaw() const
{
    return (IsRaw() && ((_holder.type & LLBC_VariantType::VT_MASK_RAW_SIGNED) != LLBC_VariantType::VT_MASK_RAW_SIGNED));
}

inline bool LLBC_Variant::IsBool() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_BOOL) == LLBC_VariantType::VT_RAW_BOOL);
}

inline bool LLBC_Variant::IsInt8() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_SINT8) == LLBC_VariantType::VT_RAW_SINT8);
}

inline bool LLBC_Variant::IsUInt8() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_UINT8) == LLBC_VariantType::VT_RAW_UINT8);
}

inline bool LLBC_Variant::IsInt16() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_SINT16) == LLBC_VariantType::VT_RAW_SINT16);
}

inline bool LLBC_Variant::IsUInt16() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_UINT16) == LLBC_VariantType::VT_RAW_UINT16);
}

inline bool LLBC_Variant::IsInt32() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_SINT32) == LLBC_VariantType::VT_RAW_SINT32);
}

inline bool LLBC_Variant::IsUInt32() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_UINT32) == LLBC_VariantType::VT_RAW_UINT32);
}

inline bool LLBC_Variant::IsLong() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_LONG) == LLBC_VariantType::VT_RAW_LONG);
}

inline bool LLBC_Variant::IsULong() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_ULONG) == LLBC_VariantType::VT_RAW_ULONG);
}

inline bool LLBC_Variant::IsPtr() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_PTR) == LLBC_VariantType::VT_RAW_PTR);
}

inline bool LLBC_Variant::IsInt64() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_SINT64) == LLBC_VariantType::VT_RAW_SINT64);
}

inline bool LLBC_Variant::IsUInt64() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_UINT64) == LLBC_VariantType::VT_RAW_UINT64);
}

inline bool LLBC_Variant::IsFloat() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_FLOAT) == LLBC_VariantType::VT_RAW_FLOAT);
}

inline bool LLBC_Variant::IsDouble() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_DOUBLE) == LLBC_VariantType::VT_RAW_DOUBLE);
}

inline bool LLBC_Variant::IsStr() const
{
    return ((_holder.type & LLBC_VariantType::VT_STR_DFT) == LLBC_VariantType::VT_STR_DFT);
}

inline bool LLBC_Variant::IsSeq() const
{
    return ((_holder.type & LLBC_VariantType::VT_SEQ_DFT) == LLBC_VariantType::VT_SEQ_DFT);
}

inline bool LLBC_Variant::IsDict() const
{
    return ((_holder.type & LLBC_VariantType::VT_DICT_DFT) == LLBC_VariantType::VT_DICT_DFT);
}

inline LLBC_Variant &LLBC_Variant::BecomeNil()
{
    return Become(LLBC_VariantType::VT_NIL);
}

inline LLBC_Variant &LLBC_Variant::BecomeBool()
{
    return Become(LLBC_VariantType::VT_RAW_BOOL);
}

inline LLBC_Variant &LLBC_Variant::BecomeInt8()
{
    return Become(LLBC_VariantType::VT_RAW_SINT8);
}

inline LLBC_Variant &LLBC_Variant::BecomeUInt8()
{
    return Become(LLBC_VariantType::VT_RAW_UINT8);
}

inline LLBC_Variant &LLBC_Variant::BecomeInt16()
{
    return Become(LLBC_VariantType::VT_RAW_SINT16);
}

inline LLBC_Variant &LLBC_Variant::BecomeUInt16()
{
    return Become(LLBC_VariantType::VT_RAW_UINT16);
}

inline LLBC_Variant &LLBC_Variant::BecomeInt32()
{
    return Become(LLBC_VariantType::VT_RAW_SINT32);
}

inline LLBC_Variant &LLBC_Variant::BecomeUInt32()
{
    return Become(LLBC_VariantType::VT_RAW_UINT32);
}

inline LLBC_Variant &LLBC_Variant::BecomeLong()
{
    return Become(LLBC_VariantType::VT_RAW_LONG);
}

inline LLBC_Variant &LLBC_Variant::BecomeULong()
{
    return Become(LLBC_VariantType::VT_RAW_ULONG);
}

inline LLBC_Variant &LLBC_Variant::BecomePtr()
{
    return Become(LLBC_VariantType::VT_RAW_PTR);
}

inline LLBC_Variant &LLBC_Variant::BecomeInt64()
{
    return Become(LLBC_VariantType::VT_RAW_SINT64);
}

inline LLBC_Variant &LLBC_Variant::BecomeUInt64()
{
    return Become(LLBC_VariantType::VT_RAW_UINT64);
}

inline LLBC_Variant &LLBC_Variant::BecomeFloat()
{
    return Become(LLBC_VariantType::VT_RAW_FLOAT);
}

inline LLBC_Variant &LLBC_Variant::BecomeDouble()
{
    return Become(LLBC_VariantType::VT_RAW_DOUBLE);
}

inline LLBC_Variant &LLBC_Variant::BecomeStr()
{
    return Become(LLBC_VariantType::VT_STR_DFT);
}

inline LLBC_Variant &LLBC_Variant::BecomeSeq()
{
    return Become(LLBC_VariantType::VT_SEQ_DFT);
}

inline LLBC_Variant &LLBC_Variant::BecomeDict()
{
    return Become(LLBC_VariantType::VT_DICT_DFT);
}

inline LLBC_Variant &LLBC_Variant::Become(LLBC_VariantType::ENUM ty)
{
    if (_holder.type != ty)
    {
        _holder.ClearData();
        _holder.type = ty;
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

template <typename _Ty>
inline _Ty *LLBC_Variant::AsPtr() const
{
    return reinterpret_cast<_Ty *>(AsUInt64());
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

template <typename _Ty>
LLBC_Variant::operator _Ty *() const
{
    return AsPtr<_Ty>();
}

template <>
inline LLBC_Variant::operator char *() const
{
    static thread_local char emptyMutableEmptyStr[1] = {'\0'};
    return IsStrX() ?  const_cast<char *>(_holder.data.obj.str->c_str()) : emptyMutableEmptyStr;
}

template <>
inline LLBC_Variant::operator const char *() const
{
    return IsStrX() ? _holder.data.obj.str->c_str() : "";
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

template <typename _ElemTy>
LLBC_Variant::operator std::vector<_ElemTy>() const
{
    std::vector<_ElemTy> v;
    if (IsSeqX() && !IsEmpty())
        v.insert(_holder.data.obj.seq->begin(), _holder.data.obj.seq->end());

    return v;
}

template <typename _ElemTy>
LLBC_Variant::operator std::set<_ElemTy>() const
{
    std::set<_ElemTy> s;
    if (IsEmpty())
        return s;

    if (IsSeq())
    {
        SeqConstIter endIt = _holder.data.obj.seq->end();
        for (SeqConstIter it = _holder.data.obj.seq->begin(); it != endIt; ++it)
            s.insert(*it);
    }
    else if (IsDict())
    {
        DictConstIter endIt = _holder.data.obj.dict->end();
        for (DictConstIter it = _holder.data.obj.dict->begin(); it != endIt; ++it)
            s.insert(it->first);
    }

    return s;
}

template <typename _ElemTy>
LLBC_Variant::operator std::unordered_set<_ElemTy>() const
{
    std::unordered_set<_ElemTy> us;
    if (IsEmpty())
        return us;

    if (IsSeq())
    {
        SeqConstIter endIt = _holder.data.obj.seq->end();
        for (SeqConstIter it = _holder.data.obj.seq->begin(); it != endIt; ++it)
            us.insert(*it);
    }
    else if (IsDict())
    {
        DictConstIter endIt = _holder.data.obj.dict->end();
        for (DictConstIter it = _holder.data.obj.dict->begin(); it != endIt; ++it)
            us.insert(it->first);
    }

    return us;
}

template <typename _ElemTy>
LLBC_Variant::operator std::queue<_ElemTy>() const
{
    std::queue<_ElemTy> q;
    if (IsEmpty())
        return q;

    if (IsSeq())
    {
        SeqConstIter endIt = _holder.data.obj.seq->end();
        for (SeqConstIter it = _holder.data.obj.seq->begin(); it != endIt; ++it)
            q.push(*it);
    }
    else if (IsDict())
    {
        DictConstIter endIt = _holder.data.obj.dict->end();
        for (DictConstIter it = _holder.data.obj.dict->begin(); it != endIt; ++it)
            q.push(*it);
    }

    return q;
}

template <typename _Ty>
LLBC_Variant::operator std::deque<_Ty>() const
{
    std::deque<_Ty> dq;
    if (IsEmpty())
        return dq;

    if (IsSeq())
    {
        SeqConstIter endIt = _holder.data.obj.seq->end();
        for (SeqConstIter it = _holder.data.obj.seq->begin(); it != endIt; ++it)
            dq.push_back(*it);
    }
    else if (IsDict())
    {
        DictConstIter endIt = _holder.data.obj.dict->end();
        for (DictConstIter it = _holder.data.obj.dict->begin(); it != endIt; ++it)
            dq.push_back(*it);
    }

    return dq;
}

inline LLBC_Variant::operator const LLBC_Variant::Dict &() const
{
    return AsDict();
}

template <typename _Key, typename _Val>
LLBC_Variant::operator std::map<_Key, _Val>() const
{
    std::map<_Key, _Val> m;
    if (IsDictX())
        CpToBinaryCont(m);

    return m;
}

template <typename _Key, typename _Val>
LLBC_Variant::operator std::unordered_map<_Key, _Val>() const
{
    std::map<_Key, _Val> m;
    if (IsDictX())
        CpToBinaryCont(m);

    return m;
}

inline void LLBC_Variant::Clear()
{
    if (IsStrX())
        _holder.data.obj.str->clear();
    else if (IsSeqX())
        _holder.data.obj.seq->clear();
    else if (IsDictX())
        _holder.data.obj.dict->clear();
    else if (IsRaw())
        _holder.data.raw.int64Val = 0;
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

template <typename _Ty1, typename... _Tys>
typename ::std::enable_if<::std::is_same<_Ty1, LLBC_Variant>::value, void>::type
LLBC_Variant::SeqPushBack(_Ty1 &&val1, _Tys &&... vals)
{
    this->SeqPushBackElem(val1);
    this->SeqPushBack(std::forward<_Tys>(vals)...);
}

template <typename _Ty1, typename... _Tys>
typename ::std::enable_if<!::std::is_same<_Ty1, LLBC_Variant>::value, void>::type
LLBC_Variant::SeqPushBack(_Ty1 &&val1, _Tys &&... vals)
{
    this->SeqPushBackElem(Seq::value_type(val1));
    this->SeqPushBack(std::forward<_Tys>(vals)...);
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

template <typename _Key, typename _Val>
std::pair<LLBC_Variant::DictIter, bool> LLBC_Variant::DictInsert(const _Key &key, const _Val &val)
{
    return this->DictInsert(Dict::key_type(key), Dict::mapped_type(val));
}

template <typename _Key>
LLBC_Variant::DictIter LLBC_Variant::DictFind(const _Key &key)
{
    return this->DictFind(Dict::key_type(key));
}

template <typename _Key>
LLBC_Variant::DictConstIter LLBC_Variant::DictFind(const _Key &key) const
{
    return this->DictFind(Dict::key_type(key));
}

template <typename _Key1, typename... _Keys>
typename ::std::enable_if<::std::is_same<_Key1, LLBC_Variant>::value, typename LLBC_Variant::Dict::size_type>::type
LLBC_Variant::DictErase(_Key1 &&key1, _Keys &&... keys)
{
    return this->DictEraseKey(key1) + this->DictErase(std::forward<_Keys>(keys)...);
}

template <typename _Key1, typename... _Keys>
typename ::std::enable_if<!::std::is_same<_Key1, LLBC_Variant>::value, typename LLBC_Variant::Dict::size_type>::type
LLBC_Variant::DictErase(_Key1 &&key1, _Keys &&... keys)
{
    return this->DictEraseKey(Dict::key_type(key1)) + this->DictErase(std::forward<_Keys>(keys)...);
}

template <typename _Key>
LLBC_Variant &LLBC_Variant::operator [](const _Key &key)
{
    return this->operator [](LLBC_Variant(key));
}

template <typename _Key>
const LLBC_Variant &LLBC_Variant::operator [](const _Key &key) const
{
    return this->operator [](LLBC_Variant(key));
}

template <typename _T>
LLBC_Variant &LLBC_Variant::operator =(const _T * const &ptr)
{
    _holder.ClearData();
    _holder.type = LLBC_VariantType::VT_RAW_PTR;

    memcpy(&_holder.data.raw.uint64Val, &ptr, sizeof(_T *));

    return *this;
}

template <typename _T>
LLBC_Variant &LLBC_Variant::operator =(const std::vector<_T> &vec)
{
    CtFromUnaryCont<_T, std::vector<_T> >(vec);
    return *this;
}

template <typename _T>
LLBC_Variant &LLBC_Variant::operator =(const std::list<_T> &lst)
{
    CtFromUnaryCont<_T, std::list<_T> >(lst);
    return *this;
}

template <typename _T>
LLBC_Variant &LLBC_Variant::operator =(const std::queue<_T> &que)
{
    CtFromUnaryCont<_T, std::queue<_T> >(que);
    return *this;
}

template <typename _T>
LLBC_Variant &LLBC_Variant::operator =(const std::set<_T> &s)
{
    CtFromUnaryCont<_T, std::set<_T> >(s);
    return *this;
}

template <typename _Key, typename _Val>
LLBC_Variant &LLBC_Variant::operator =(const std::map<_Key, _Val> &m)
{
    CtFromBinaryCont<_Key, _Val, std::map<_Key, _Val> >(m);
    return *this;
}

inline LLBC_Variant &LLBC_Variant::operator =(const LLBC_Variant &var)
{
    LLBC_VariantTraits::assign(*this, var);
    return *this;
}

inline bool LLBC_Variant::operator ==(const LLBC_Variant &another) const
{
    return LLBC_VariantTraits::eq(*this, another);
}

inline bool LLBC_Variant::operator !=(const LLBC_Variant &another) const
{
    return LLBC_VariantTraits::ne(*this, another);
}

inline bool LLBC_Variant::operator <(const LLBC_Variant &another) const
{
    return LLBC_VariantTraits::lt(*this, another);
}

inline bool LLBC_Variant::operator >(const LLBC_Variant &another) const
{
    return LLBC_VariantTraits::gt(*this, another);
}

inline bool LLBC_Variant::operator <=(const LLBC_Variant &another) const
{
    return LLBC_VariantTraits::le(*this, another);
}

inline bool LLBC_Variant::operator >=(const LLBC_Variant &another) const
{
    return LLBC_VariantTraits::ge(*this, another);
}

template <typename _T>
bool LLBC_Variant::operator==(const _T &another) const
{
    return operator==(LLBC_Variant(another));
}

template <typename _T>
bool LLBC_Variant::operator!=(const _T &another) const
{
    return operator!=(LLBC_Variant(another));
}

template <typename _T>
bool LLBC_Variant::operator<(const _T &another) const
{
    return operator<(LLBC_Variant(another));
}

template <typename _T>
bool LLBC_Variant::operator>(const _T &another) const
{
    return operator>(LLBC_Variant(another));
}

template <typename _T>
bool LLBC_Variant::operator<=(const _T &another) const
{
    return operator<=(LLBC_Variant(another));
}

template <typename _T>
bool LLBC_Variant::operator>=(const _T &another) const
{
    return operator>=(LLBC_Variant(another));
}

inline LLBC_Variant LLBC_Variant::operator +(const LLBC_Variant &another) const
{
    return LLBC_VariantTraits::add(*this, another);
}

inline LLBC_Variant LLBC_Variant::operator -(const LLBC_Variant &another) const
{
    return LLBC_VariantTraits::sub(*this, another);
}

inline LLBC_Variant LLBC_Variant::operator *(const LLBC_Variant &another) const
{
    return LLBC_VariantTraits::mul(*this, another);
}

inline LLBC_Variant LLBC_Variant::operator /(const LLBC_Variant &another) const
{
    return LLBC_VariantTraits::div(*this, another);
}

inline LLBC_Variant &LLBC_Variant::operator +=(const LLBC_Variant &another)
{
    LLBC_VariantTraits::add_equal(*this, another);
    return *this;
}

inline LLBC_Variant &LLBC_Variant::operator -=(const LLBC_Variant &another)
{
    LLBC_VariantTraits::sub_equal(*this, another);
    return *this;
}

inline LLBC_Variant &LLBC_Variant::operator *=(const LLBC_Variant &another)
{
    LLBC_VariantTraits::mul_equal(*this, another);
    return *this;
}

inline LLBC_Variant &LLBC_Variant::operator /=(const LLBC_Variant &another)
{
    LLBC_VariantTraits::div_equal(*this, another);
    return *this;
}

template <typename _T>
LLBC_Variant LLBC_Variant::operator+(const _T &another) const
{
    return operator+(LLBC_Variant(another));
}

template <typename _T>
LLBC_Variant LLBC_Variant::operator-(const _T &another) const
{
    return operator-(LLBC_Variant(another));
}

template <typename _T>
LLBC_Variant LLBC_Variant::operator*(const _T &another) const
{
    return operator*(LLBC_Variant(another));
}

template <typename _T>
LLBC_Variant LLBC_Variant::operator/(const _T &another) const
{
    return operator/(LLBC_Variant(another));
}

template <typename _T>
LLBC_Variant &LLBC_Variant::operator+=(const _T &another)
{
    return operator+=(LLBC_Variant(another));
}

template <typename _T>
LLBC_Variant & LLBC_Variant::operator-=(const _T &another)
{
    return operator-=(LLBC_Variant(another));
}

template <typename _T>
LLBC_Variant &LLBC_Variant::operator*=(const _T &another)
{
    return operator*=(LLBC_Variant(another));
}

template <typename _T>
LLBC_Variant &LLBC_Variant::operator/=(const _T &another)
{
    return operator/=(LLBC_Variant(another));
}

inline void LLBC_Variant::SetType(int type)
{
    _holder.type = static_cast<LLBC_VariantType::ENUM>(type);
}

inline void LLBC_Variant::ClearData()
{
    _holder.ClearData();
}

inline const LLBC_String &LLBC_Variant::TypeToString() const
{
    return LLBC_VariantType::Type2Str(_holder.type);
}

inline LLBC_String LLBC_Variant::ValueToString() const
{
    return AsStr();
}

inline LLBC_String LLBC_Variant::ToString() const
{
    LLBC_String strRepr;
    strRepr.reserve(64);
    strRepr.append("Variant(", 8);

    strRepr.append(TypeToString());
    strRepr.append(1, '|');
    strRepr.append(ValueToString());

    strRepr.append(1, ')');

    return strRepr;
}

inline void LLBC_Variant::CtFromRaw(uint64 raw, LLBC_VariantType::ENUM ty)
{
}

template <typename _T, typename _UnaryContainer>
void LLBC_Variant::CtFromUnaryCont(const _UnaryContainer &unaryCont)
{
    // become variant type to <Seq> type.
    BecomeSeq();

    // clear members.
    Seq *&seq = _holder.data.obj.seq;
    if (seq)
        seq->clear();

    // if giving unary container is empty, return.
    if (unaryCont.empty())
        return;

    // create sequence object if it has not been created before.
    if (!seq)
        seq = new Seq;
    // makesure the capacity greater than or equal to the giving unary container elements size.
    if (seq->capacity() < unaryCont.size())
        seq->reserve(unaryCont.size());

    // execute elements copy.
    const typename _UnaryContainer::const_iterator endIt = unaryCont.end();
    for (typename _UnaryContainer::const_iterator it = unaryCont.begin();
         it != endIt;
         ++it)
        seq->emplace_back(*it);
}

template <typename _Key, typename _Val, typename _BinaryContainer>
void LLBC_Variant::CtFromBinaryCont(const _BinaryContainer &binaryCont)
{
    // become variant type to <Dict> type.
    BecomeDict();

    // clear members.
    Dict *&dict = _holder.data.obj.dict;
    if (dict)
        dict->clear();

    // if giving binary container is empty, return.
    if (binaryCont.empty())
        return;

    // create dictionary object if it has not been created before.
    if (!dict)
        dict = new Dict;

    // execute elements copy.
    const typename _BinaryContainer::const_iterator endIt = binaryCont.end();
    for (typename _BinaryContainer::const_iterator it = binaryCont.begin();
         it != endIt;
         ++it)
        dict->emplace(LLBC_Variant(it->first), LLBC_Variant(it->second));
}

template <typename _Key, typename _Val, typename _BinaryContainer>
void LLBC_Variant::CpToBinaryCont(_BinaryContainer &binaryCont)
{
    const DictConstIter endIt = _holder.data.obj.dict->end();
    for (DictConstIter it = _holder.data.obj.dict->begin(); it != endIt; ++it)
        binaryCont.emplace(LLBC_Variant(it->first), LLBC_Variant(it->second));
}

inline bool LLBC_Variant::IsStrX() const
{
    return IsStr() && _holder.data.obj.str != nullptr;
}

inline bool LLBC_Variant::IsSeqX() const
{
    return IsSeq() && _holder.data.obj.seq != nullptr;
}

inline bool LLBC_Variant::IsDictX() const
{
    return IsDict() && _holder.data.obj.dict != nullptr;
}

inline LLBC_Variant &LLBC_Variant::BecomeStrX()
{
    BecomeStr();
    if (!_holder.data.obj.str)
        _holder.data.obj.str = new Str;

    return *this;
}

inline LLBC_Variant &LLBC_Variant::BecomeSeqX()
{
    BecomeSeq();
    if (!_holder.data.obj.seq)
        _holder.data.obj.seq = new Seq;

    return *this;
}

inline LLBC_Variant &LLBC_Variant::BecomeDictX()
{
    BecomeDict();
    if (!_holder.data.obj.dict)
        _holder.data.obj.dict = new Dict;

    return *this;
}

inline void LLBC_Variant::SeqPushBack()
{
}

inline LLBC_Variant::Dict::size_type LLBC_Variant::DictErase()
{
    return 0;
}

__LLBC_NS_END

namespace std
{

/**
 * \brief The explicit specialization of std::hash<LLBC_Variant> impl.
 * 
 */
template <>
struct hash<LLBC_NS LLBC_Variant>
{
    #if LLBC_CUR_COMP == LLBC_COMP_GCC || LLBC_CUR_COMP == LLBC_COMP_CLANG
    __attribute__((pure)) 
    #endif // Comp == Gcc or Comp == Clang
    size_t operator()(const LLBC_NS LLBC_Variant &var) const noexcept
    {
        if (var.IsRaw())
        {
            const LLBC_NS LLBC_Variant::Holder &holder = var.GetHolder();
            if (var.IsFloat() || var.IsDouble())
                #if LLBC_TARGET_PLATFORM_WIN32
                return ::std::_Hash_representation(
                    holder.data.raw.doubleVal == 0.0f ? 0.0f : holder.data.raw.doubleVal);
                #else // Non-win32 platform.
                    #if LLBC_CUR_COMP == LLBC_COMP_GCC
                        return ::std::_Hash_impl::hash(holder.data.raw.doubleVal);
                    #else // Non-GCC comp
                        return std::hash<double>()(holder.data.raw.doubleVal);
                    #endif
                #endif
            else
                return static_cast<size_t>(holder.data.raw.uint64Val);
        }
        else if (var.IsStr())
        {
            const LLBC_NS LLBC_Variant::Str * const &str = var.GetHolder().data.obj.str;
            return str && !str->empty() ? LLBC_Hash(str->data(), str->size()) : LLBC_Hash(nullptr, 0);
        }
        else if (var.IsSeq())
        {
            size_t hashVal = 10000;
            const LLBC_NS LLBC_Variant::Seq * const &seq = var.GetHolder().data.obj.seq;
            if (!seq || seq->empty())
                return hashVal;

            const LLBC_NS LLBC_Variant::SeqConstIter endIt = seq->end();
            for (LLBC_NS LLBC_Variant::SeqConstIter it = seq->begin();
                 it != endIt;
                 ++it)
                 hashVal += (*this)(*it);

            return hashVal;
        }
        else if (var.IsDict())
        {
            size_t hashVal = 20000;
            const LLBC_NS LLBC_Variant::Dict * const &dict = var.GetHolder().data.obj.dict;
            if (!dict || dict->empty())
                return hashVal;

            const LLBC_NS LLBC_Variant::DictConstIter endIt = dict->end();
            for (LLBC_NS LLBC_Variant::DictConstIter it = dict->begin();
                 it != endIt;
                 ++it)
                hashVal += (*this)(it->first);

            return hashVal;
        }
        else // Nil
        {
            return 0;
        }
    }
};

}


