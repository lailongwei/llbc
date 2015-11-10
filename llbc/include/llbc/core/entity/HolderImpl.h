/**
 * @file    HolderImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2103/06/13
 * @version 1.0
 *
 * @brief
 */
#ifdef __LLBC_CORE_ENTITY_HOLDER_H__

__LLBC_NS_BEGIN

template <typename ValueType>
LLBC_Holder<ValueType>::LLBC_Holder(ValueType *value)
: _value(value)
{
}

template <typename ValueType>
LLBC_Holder<ValueType>::~LLBC_Holder()
{
    LLBC_XDelete(_value);
}

template <typename ValueType>
void *LLBC_Holder<ValueType>::GetValue() const
{
    return _value;
}

__LLBC_NS_END

#endif // __LLBC_CORE_ENTITY_HOLDER_H__
