/**
 * @file    Property.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/09/18
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/file/File.h"
#include "llbc/core/helper/STLHelper.h"
#include "llbc/core/utils/Util_Text.h"

#include "llbc/core/config/Property.h"

namespace
{
	typedef LLBC_NS LLBC_Property This;

    typedef LLBC_NS LLBC_Property::Properties::iterator _It;
    typedef LLBC_NS LLBC_Property::Properties::const_iterator _CIt;
}

__LLBC_NS_BEGIN

const char This::NameSeparator = '.';
const char This::NameValueSeparator = '=';
const char This::CommentBeg = '#';

const char This::EscapeChar = '\\';
const char * const LLBC_Property::EscapeChars = ".=#";

LLBC_Property::LLBC_Property(const This &another)
{
    _parent = NULL;

    if (another._value)
        _value = LLBC_New1(LLBC_Variant, *another._value);
    else
        _value = NULL;

    if (another._properties)
    {
        _properties = LLBC_New0(This::Properties);
        for (This::Properties::const_iterator it = another._properties->begin();
             it != another._properties->end();
             it++)
            _properties->insert(std::make_pair(it->first, LLBC_New1(This, *it->second)));
    }
    else
    {
        _properties = NULL;
    }

    _comments = another._comments;
    _errMsg = another._errMsg;
}

LLBC_Property::LLBC_Property(LLBC_Property *parent)
: _parent(parent)

, _value(NULL)
, _properties(NULL)

, _comments()

, _errMsg()
{
}

LLBC_Property::~LLBC_Property()
{
    this->Cleanup();
}

int LLBC_Property::LoadFromFile(const LLBC_String &file)
{
    LLBC_File f(file, LLBC_FileMode::BinaryRead);
    if (!f.IsOpened())
        return LLBC_FAILED;

    return this->LoadFromContent(f.ReadToEnd());
}

int LLBC_Property::LoadFromContent(const LLBC_String &content)
{
    // Backup all data members & set to NULL.
    This *backParent = _parent;
    LLBC_Variant *backValue = _value;
	This::Properties *backProperties = _properties;
    LLBC_String backComments = _comments;

    _parent = NULL;
    _value = NULL;
    _properties = NULL;
    _comments.clear();

    // Clear error message.
    _errMsg.clear();

    // Begin parse.
    bool done = true;
    LLBC_Strings contents = this->NormalizeContent(content);
    for (size_t i = 0; i < contents.size(); i++)
    {
        const LLBC_String &line = contents[i];
        if (this->ParseLine(line, static_cast<int>(i) + 1) != LLBC_OK)
        {
            done = false;
            break;
        }
    }

    // If not done, restore above backet data members.
    if (!done)
    {
        this->Cleanup();

        _parent = backParent;
        _value = backValue;
        _properties = backProperties;
        _comments = backComments;

        return LLBC_FAILED;
    }
    // Done, Delete all backet old data members.
    LLBC_XDelete(backValue);
    if (backProperties)
    {
        LLBC_STLHelper::DeleteContainer(*backProperties);
        LLBC_Delete(backProperties);
    }

    return LLBC_OK;
}

int LLBC_Property::SaveToContent(LLBC_String &content) const
{
    if (!_properties)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

	content.clear();

    std::vector<const This *> properties;
    this->CollectValueProperties(properties);
    for (size_t i = 0; i < properties.size(); i++)
    {
        const This *& prop = properties[i];
        content.append_format("%s %c %s\n", prop->GetName().c_str(), 
                This::NameValueSeparator, prop->SerializeValue().c_str());
    }

    return LLBC_OK;
}

int LLBC_Property::SaveToFile(const LLBC_String &file) const
{
    LLBC_File f;
    if (f.Open(file, LLBC_FileMode::BinaryWrite) != LLBC_OK)
        return LLBC_FAILED;

    LLBC_String content;
    if (this->SaveToContent(content) != LLBC_OK)
        return LLBC_FAILED;

    if (f.Write(content) != LLBC_OK)
        return LLBC_FAILED;

    f.Close();

    return LLBC_OK;
}

const LLBC_String &LLBC_Property::GetLoadErrorDesc() const
{
    return _errMsg;
}

const LLBC_Property *LLBC_Property::GetParent() const
{
    return _parent;
}

LLBC_String LLBC_Property::GetName() const
{
    LLBC_String name;

    const This *child = this;
    const This *parent = _parent;
    while(parent)
    {
        if (!name.empty())
            name.insert(0, 1, This::NameSeparator);

        for (_CIt it = parent->_properties->begin();
             it != parent->_properties->end();
             it++)
            if (it->second == child)
            {
                name.insert(0, it->first);
                break;
            }

        child = parent;
        parent = parent->_parent;
    }

    return name;
}

size_t LLBC_Property::GetPropertyCount() const
{
    return (_properties ? _properties->size() : 0);
}

LLBC_Strings LLBC_Property::GetPropertyNames(bool nest) const
{
	LLBC_Strings names;
	if (!_properties)
		return names;

	if (!nest)
	{
		for (_CIt it = _properties->begin(); it != _properties->end(); it++)
			names.push_back(it->first);
	}
	else
	{
		std::vector<const This *> valueProps;
		this->CollectValueProperties(valueProps);
		for (size_t i = 0; i < valueProps.size(); i++)
			names.push_back(valueProps[i]->GetName());
	}

	return names;
}

LLBC_Variant LLBC_Property::GetValue(const LLBC_String &name,
                                     const LLBC_Variant &dft) const
{
    if (name.empty())
    {
        if (!_value)
        {
            LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
            return dft;
        }
        else
        {
            LLBC_SetLastError(LLBC_ERROR_SUCCESS);
            return *_value;
        }
    }
    else
    {
        const LLBC_Property *prop = this->GetProperty(name);
        if (!prop)
        {
            LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
            return dft;
        }
        else
        {
            LLBC_SetLastError(LLBC_ERROR_SUCCESS);
            return prop->GetValue("", dft);
        }
    }
}

bool LLBC_Property::HasProperty(const LLBC_String &name) const
{
    return !!this->GetProperty(name);
}

const LLBC_Property *LLBC_Property::GetProperty(const LLBC_String &name) const
{
    if (!_properties)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return NULL;
    }
    else if (!this->CheckName(name))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return NULL;
    }

    const LLBC_Strings names = name.split(This::NameSeparator, 1);
    const LLBC_String &topName = names[0];

    _CIt it = _properties->find(topName);
    if (it == _properties->end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return NULL;
    }

    return (names.size() == 1 ? 
            it->second : it->second->GetProperty(names[1]));
}

const LLBC_Property::Properties &LLBC_Property::GetAllProperties() const
{
    static const This::Properties emptyProperties;
    if (!_properties)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return emptyProperties;
    }

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    return *_properties;
}

int LLBC_Property::RemoveProperty(const LLBC_String &name, bool removeAll)
{
    if (!this->CheckName(name))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }
    else if (!_properties)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    const LLBC_Strings names = name.split(This::NameSeparator, 1);
    const LLBC_String &topName = names[0];

    _It it = _properties->find(topName);
    if (it == _properties->end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    This *prop = it->second;
    if (names.size() == 1 || removeAll)
    {
        LLBC_Delete(prop);
        _properties->erase(it);

		if (_properties->size() == 0)
			LLBC_XDelete(_properties);

        return LLBC_OK;
    }
    else
    {
        return prop->RemoveProperty(names[1], removeAll);
    }
}

void LLBC_Property::RemoveAllProperties()
{
    if (_properties)
    {
        LLBC_STLHelper::DeleteContainer(*_properties);
        LLBC_XDelete(_properties);
    }
}

LLBC_String LLBC_Property::GetComments(const LLBC_String &name) const
{
    if (name.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_SUCCESS);
        return _comments;
    }

    const This *prop = this->GetProperty(name);
    if (!prop)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return "";
    }

    return prop->GetComments("");
}

int LLBC_Property::SetComments(const LLBC_String &name, const LLBC_String &comments)
{
    if (name.empty())
    {
        _comments = comments;
        return LLBC_OK;
    }

    This *prop = const_cast<This *>(this->GetProperty(name));
    if (!prop)
        return LLBC_FAILED;

    return prop->SetComments("", comments);
}

This &LLBC_Property::operator =(const This &another)
{
    _parent = NULL;

    this->Cleanup();
    if (another._value)
        _value = LLBC_New1(LLBC_Variant, *another._value);

    if (another._properties)
    {
        _properties = LLBC_New0(This::Properties);
        for (This::Properties::const_iterator it = another._properties->begin();
             it != another._properties->end();
             it++)
            _properties->insert(std::make_pair(it->first, LLBC_New1(This, *it->second)));
    }

    _comments = another._comments;
    _errMsg = another._errMsg;

    return *this;
}

void LLBC_Property::Cleanup()
{
    this->CleanupValue();
    this->RemoveAllProperties();

    this->CleanupComments();

	_comments.clear();
}

void LLBC_Property::CleanupValue()
{
    LLBC_XDelete(_value);
}

void LLBC_Property::CleanupComments()
{
    _comments.clear();
}

void LLBC_Property::ExpandProperties(const LLBC_String &name)
{
    if (!_properties)
        _properties = LLBC_New(This::Properties);

    const LLBC_Strings names = name.split(This::NameSeparator, 1);
    const LLBC_String &topName = names[0]; 

    _It it = _properties->find(topName);
    if (it == _properties->end())
    {
        LLBC_Property *prop = LLBC_New1(LLBC_Property, this);
        it = _properties->insert(std::make_pair(topName, prop)).first;
    }
	else
	{
        it->second->CleanupValue();
        it->second->CleanupComments();
	}

    if (names.size() == 2)
        it->second->ExpandProperties(names[1]);
}

bool LLBC_Property::CheckName(const LLBC_String &name, bool allowSeparator) const
{
    if (name.empty())
        return false;

    for (LLBC_String::const_iterator it = name.begin();
         it != name.end();
         it++)
    {
        const char &ch = *it;
        if (ch == This::NameSeparator)
		{
			if (!allowSeparator)
				return false;
			else
				continue;
		}

        if (!((ch >= 'a' && ch <= 'z') ||
			(ch >= 'A' && ch <= 'Z') ||
			(ch >= '0' && ch <= '9') ||
			ch == '_'))
            return false;
    }

    const LLBC_Strings names = name.split(This::NameSeparator);
    for (size_t i = 0; i < names.size(); i++)
	{
        if (names[i].empty())
            return false;
	}

    return true;
}

LLBC_Strings LLBC_Property::NormalizeContent(const LLBC_String &content)
{
    // Replace WIN style line-end to UNIX style.
    const char *unixLineEnd = "\n";

    const char *winLineEnd = "\r\n";
    size_t lineEndLen = LLBC_StrLenA(winLineEnd);

    LLBC_String::size_type pos;
    LLBC_String normalizedContent(content);
    while ((pos = normalizedContent.find(winLineEnd)) != LLBC_String::npos)
        normalizedContent.replace(pos, lineEndLen, unixLineEnd);

    // Replace MAC style line-end to UNIX style.
    const char *macLineEnd = "\r";
    lineEndLen = LLBC_StrLenA(macLineEnd);
    while ((pos = normalizedContent.find(macLineEnd)) != LLBC_String::npos)
        normalizedContent.replace(pos, lineEndLen, unixLineEnd);

    // Split lines and strip the lines.
    LLBC_Strings normalizedContents = normalizedContent.split(unixLineEnd);
    for (size_t i = 0; i < normalizedContents.size(); i++)
        normalizedContents[i].strip();

    return normalizedContents;
}

int LLBC_Property::ParseLine(const LLBC_String &line, int lineNo)
{
    if (line.empty()) // Empty line.
        return LLBC_OK;
    else if (line[0] == This::CommentBeg) // Comments line.
        return LLBC_OK;

    typedef LLBC_Strings _Pair;
    _Pair pair = line.split(This::NameValueSeparator, 1);
    if (pair.size() != 2)
    {
        _errMsg.format("line %d: could not find name-value separator '%c'", lineNo, This::NameValueSeparator);
        LLBC_SetLastError(LLBC_ERROR_FORMAT);

        return LLBC_FAILED;
    }

    const LLBC_String name = pair[0].strip();
    if (!this->CheckName(name))
    {
        _errMsg.format("line %d: property name format error", lineNo);
        LLBC_SetLastError(LLBC_ERROR_FORMAT);

        return LLBC_FAILED;
    }

    const LLBC_String valueComments = pair[1].strip();

    LLBC_String::size_type pos = 0;
    while ((pos = valueComments.find(This::CommentBeg, pos)) != LLBC_String::npos)
    {
        if (pos == 0 || valueComments[pos - 1] != This::EscapeChar)
            break;
        else
            pos += 1;
    }

    LLBC_String value, comments;
    if (pos != LLBC_String::npos)
    {
        pair = valueComments.split(This::CommentBeg, 1);

        value = valueComments.substr(0, pos)
            .rstrip().unescape(This::EscapeChar);
        comments = valueComments.substr(pos + 1).lstrip();
    }
    else
    {
        value = valueComments.strip();
    }

    if (this->SetValue(name, value) == LLBC_OK)
    {
        This *prop = const_cast<This *>(this->GetProperty(name));
        prop->SetComments("", comments);
    }

    return LLBC_OK;
}

LLBC_String LLBC_Property::SerializeValue() const
{
    if (_properties)
        return "";

    static const LLBC_Variant nilValue;
    const LLBC_Variant &value = (_value ? *_value : nilValue);

	LLBC_String valueStr;
    if (!value.IsNil())
        valueStr.append_format("%s", value.ValueToString()
                .escape(This::EscapeChars, This::EscapeChar).c_str());

    if (!_comments.empty())
    {
        if (valueStr.empty())
            valueStr.append_format("%c %s", This::CommentBeg, _comments.c_str());
        else
            valueStr.append_format(" %c %s", This::CommentBeg, _comments.c_str());
    }

    return valueStr;
}

void LLBC_Property::CollectValueProperties(std::vector<const This *> &properties) const
{
    if (!_properties)
	{
        properties.push_back(this);
		return;
	}

    for (_CIt it = _properties->begin();
         it != _properties->end();
         it++)
        it->second->CollectValueProperties(properties);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
