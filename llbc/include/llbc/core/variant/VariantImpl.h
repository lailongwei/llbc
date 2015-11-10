/**
 * @file    VariantImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/05/03
 * @version 1.0
 *
 * @brief
 */
#ifdef __LLBC_CORE_VARIANT_VARIANT_H__

__LLBC_NS_BEGIN

template <typename _Kty, typename _Ty>
inline std::pair<LLBC_Variant::DictIter, bool> LLBC_Variant::Insert(const _Kty &key, const _Ty &val)
{
    return this->Insert(LLBC_Variant::Dict::key_type(key),
        LLBC_Variant::Dict::mapped_type(val));
}

template <typename _Kty>
inline LLBC_Variant::DictIter LLBC_Variant::Find(const _Kty &key)
{
    return this->Find(LLBC_Variant::Dict::key_type(key));
}

template <typename _Kty>
inline LLBC_Variant::DictConstIter LLBC_Variant::Find(const _Kty &key) const
{
    return this->Find(LLBC_Variant::Dict::key_type(key));
}

template <typename _Kty>
inline LLBC_Variant::Dict::size_type LLBC_Variant::Erase(const _Kty &key)
{
    return this->Erase(LLBC_Variant::Dict::key_type(key));
}

template <typename _Kty>
inline LLBC_Variant::Dict::mapped_type &LLBC_Variant::operator [](const _Kty &key)
{
    return this->operator [](LLBC_Variant::Dict::key_type(key));
}

template <typename _Kty>
inline const LLBC_Variant::Dict::mapped_type &LLBC_Variant::operator [](const _Kty &key) const
{
    return this->operator [](LLBC_Variant::Dict::key_type(key));
}

__LLBC_NS_END

#endif // __LLBC_CORE_VARIANT_VARIANT_H__
