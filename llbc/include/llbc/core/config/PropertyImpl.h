/**
 * @file	PropertyImpl.h
 * @author	Longwei Lai<lailongwei@126.com>
 * @date	2015/09/19
 * @version	1.0
 *
 * @brief
 */
#ifdef __LLBC_CORE_CONFIG_PROPERTY_H__

__LLBC_NS_BEGIN

template <>
inline int LLBC_Property::SetValue(const LLBC_String &name, const LLBC_Variant &value, const LLBC_String &comments)
{
    if (!name.empty() && !this->CheckName(name))
    {
        LLBC_SetLastError(LLBC_ERROR_FORMAT);
        return LLBC_RTN_FAILED;
    }

    if (name.empty())
    {
        this->Cleanup();
		if (!value.IsStr())
			_value = LLBC_New1(LLBC_Variant, value.ValueToString());
		else
			_value = LLBC_New1(LLBC_Variant, value);

		_comments = comments;

        return LLBC_RTN_OK;
    }
    else
    {
        this->ExpandProperties(name);

        This *prop = const_cast<This *>(this->GetProperty(name));
        prop->SetValue("", value, comments);

        return LLBC_RTN_OK;
    }
}

template <typename _ValueType>
inline int LLBC_Property::SetValue(const LLBC_String &name, const _ValueType &value, const LLBC_String &comments)
{
	return this->SetValue(name, LLBC_Variant(value), comments);
}

__LLBC_NS_END

#endif // __LLBC_CORE_CONFIG_PROPERTY_H__
