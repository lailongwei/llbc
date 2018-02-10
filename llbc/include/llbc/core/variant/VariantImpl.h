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
