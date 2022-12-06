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

#include "llbc/common/Export.h"

#include "llbc/core/file/File.h"
#include "llbc/core/helper/STLHelper.h"
#include "llbc/core/utils/Util_Text.h"

#include "llbc/core/config/Property.h"

namespace
{
    typedef LLBC_NS LLBC_Property::Properties::iterator _It;
    typedef LLBC_NS LLBC_Property::Properties::const_iterator _CIt;
}

__LLBC_NS_BEGIN

const char LLBC_Property::NameSeparator = '.';
const char LLBC_Property::NameValueSeparator = '=';
const char LLBC_Property::CommentBeg = '#';

const char LLBC_Property::EscapeChar = '\\';
const char * const LLBC_Property::EscapeChars = ".=#";

LLBC_Property::LLBC_Property(const LLBC_Property &another)
{
    _parent = nullptr;

    if (another._value)
        _value = new LLBC_Variant(*another._value);
    else
        _value = nullptr;

    if (another._properties)
    {
        _properties = new Properties;
        for (Properties::const_iterator it = another._properties->begin();
             it != another._properties->end();
             it++)
        {
            LLBC_Property *childProp = new LLBC_Property(*it->second);
            childProp->_parent = this;

            _properties->insert(std::make_pair(it->first, childProp));
        }
    }
    else
    {
        _properties = nullptr;
    }

    _comments = another._comments;
    _errMsg = another._errMsg;
}

LLBC_Property::LLBC_Property(LLBC_Property *parent)
: _parent(parent)

, _value(nullptr)
, _properties(nullptr)

, _comments()

, _errMsg()
{
}

LLBC_Property::~LLBC_Property()
{
    Cleanup();
}

int LLBC_Property::LoadFromFile(const LLBC_String &file)
{
    LLBC_File f(file, LLBC_FileMode::BinaryRead);
    if (!f.IsOpened())
        return LLBC_FAILED;

    return LoadFromContent(f.ReadToEnd());
}

int LLBC_Property::LoadFromContent(const LLBC_String &content)
{
    // Backup all data members & set to nullptr.
    LLBC_Property *backParent = _parent;
    LLBC_Variant *backValue = _value;
    Properties *backProperties = _properties;
    LLBC_String backComments = _comments;

    _parent = nullptr;
    _value = nullptr;
    _properties = nullptr;
    _comments.clear();

    // Clear error message.
    _errMsg.clear();

    // Begin parse.
    bool done = true;
    LLBC_Strings contents = NormalizeContent(content);
    for (size_t i = 0; i < contents.size(); ++i)
    {
        const LLBC_String &line = contents[i];
        if (ParseLine(line, static_cast<int>(i) + 1) != LLBC_OK)
        {
            done = false;
            break;
        }
    }

    // If not done, restore above backet data members.
    if (!done)
    {
        Cleanup();

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
        delete backProperties;
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

    std::vector<const LLBC_Property *> properties;
    CollectValueProperties(properties);
    for (size_t i = 0; i < properties.size(); ++i)
    {
        const LLBC_Property *& prop = properties[i];
        content.append_format("%s %c %s\n",
                              prop->GetName().c_str(),
                              NameValueSeparator,
                              prop->SerializeValue().c_str());
    }

    return LLBC_OK;
}

int LLBC_Property::SaveToFile(const LLBC_String &file) const
{
    LLBC_File f;
    if (f.Open(file, LLBC_FileMode::BinaryWrite) != LLBC_OK)
        return LLBC_FAILED;

    LLBC_String content;
    if (SaveToContent(content) != LLBC_OK)
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

    const LLBC_Property *child = this;
    const LLBC_Property *parent = _parent;
    while(parent)
    {
        if (!name.empty())
            name.insert(0, 1, NameSeparator);

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

size_t LLBC_Property::GetPropertiesCount() const
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
        for (_CIt it = _properties->begin(); it != _properties->end(); ++it)
            names.push_back(it->first);
    }
    else
    {
        std::vector<const LLBC_Property *> valueProps;
        CollectValueProperties(valueProps);
        for (size_t i = 0; i < valueProps.size(); ++i)
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
        const LLBC_Property *prop = GetProperty(name);
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
    return !!GetProperty(name);
}

const LLBC_Property *LLBC_Property::GetProperty(const LLBC_String &name) const
{
    if (!_properties)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return nullptr;
    }
    else if (!CheckName(name))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return nullptr;
    }

    const LLBC_Strings names = name.split(NameSeparator, 1);
    const LLBC_String &topName = names[0];

    _CIt it = _properties->find(topName);
    if (it == _properties->end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return nullptr;
    }

    return (names.size() == 1 ? 
            it->second : it->second->GetProperty(names[1]));
}

const LLBC_Property::Properties &LLBC_Property::GetAllProperties() const
{
    static const Properties emptyProperties;
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
    if (!CheckName(name))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }
    else if (!_properties)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    const LLBC_Strings names = name.split(NameSeparator, 1);
    const LLBC_String &topName = names[0];

    _It it = _properties->find(topName);
    if (it == _properties->end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    LLBC_Property *prop = it->second;
    if (names.size() == 1 || removeAll)
    {
        delete prop;
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

    const LLBC_Property *prop = GetProperty(name);
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

    LLBC_Property *prop = const_cast<LLBC_Property *>(GetProperty(name));
    if (!prop)
        return LLBC_FAILED;

    return prop->SetComments("", comments);
}

LLBC_Property &LLBC_Property::operator =(const LLBC_Property &another)
{
    _parent = nullptr;

    Cleanup();
    if (another._value)
        _value = new LLBC_Variant(*another._value);

    if (another._properties)
    {
        _properties = new Properties;
        for (Properties::const_iterator it = another._properties->begin();
             it != another._properties->end();
             it++)
            _properties->insert(std::make_pair(it->first, new LLBC_Property(*it->second)));
    }

    _comments = another._comments;
    _errMsg = another._errMsg;

    return *this;
}

void LLBC_Property::Cleanup()
{
    CleanupValue();
    RemoveAllProperties();

    CleanupComments();

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
        _properties = new Properties;

    const LLBC_Strings names = name.split(NameSeparator, 1);
    const LLBC_String &topName = names[0]; 

    _It it = _properties->find(topName);
    if (it == _properties->end())
    {
        LLBC_Property *prop = new LLBC_Property(this);
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
        if (ch == NameSeparator)
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

    const LLBC_Strings names = name.split(NameSeparator);
    for (size_t i = 0; i < names.size(); ++i)
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
    size_t lineEndLen = strlen(winLineEnd);

    LLBC_String::size_type pos;
    LLBC_String normalizedContent(content);
    while ((pos = normalizedContent.find(winLineEnd)) != LLBC_String::npos)
        normalizedContent.replace(pos, lineEndLen, unixLineEnd);

    // Replace MAC style line-end to UNIX style.
    const char *macLineEnd = "\r";
    lineEndLen = strlen(macLineEnd);
    while ((pos = normalizedContent.find(macLineEnd)) != LLBC_String::npos)
        normalizedContent.replace(pos, lineEndLen, unixLineEnd);

    // Split lines and strip the lines.
    LLBC_Strings normalizedContents = normalizedContent.split(unixLineEnd);
    for (size_t i = 0; i < normalizedContents.size(); ++i)
        normalizedContents[i].strip();

    return normalizedContents;
}

int LLBC_Property::ParseLine(const LLBC_String &line, int lineNo)
{
    if (line.empty()) // Empty line.
        return LLBC_OK;
    else if (line[0] == CommentBeg) // Comments line.
        return LLBC_OK;

    typedef LLBC_Strings _Pair;
    _Pair pair = line.split(NameValueSeparator, 1);
    if (pair.size() != 2)
    {
        _errMsg.format("line %d: could not find name-value separator '%c'", lineNo, NameValueSeparator);
        LLBC_SetLastError(LLBC_ERROR_FORMAT);

        return LLBC_FAILED;
    }

    const LLBC_String name = pair[0].strip();
    if (!CheckName(name))
    {
        _errMsg.format("line %d: property name format error", lineNo);
        LLBC_SetLastError(LLBC_ERROR_FORMAT);

        return LLBC_FAILED;
    }

    const LLBC_String valueComments = pair[1].strip();

    LLBC_String::size_type pos = 0;
    while ((pos = valueComments.find(CommentBeg, pos)) != LLBC_String::npos)
    {
        if (pos == 0 || valueComments[pos - 1] != EscapeChar)
            break;
        else
            pos += 1;
    }

    LLBC_String value, comments;
    if (pos != LLBC_String::npos)
    {
        value = valueComments.substr(0, pos)
            .rstrip().unescape(EscapeChar);
        comments = valueComments.substr(pos + 1).lstrip();
    }
    else
    {
        value = valueComments.strip().unescape(EscapeChar);
    }

    if (SetValue(name, value) == LLBC_OK)
    {
        LLBC_Property *prop = const_cast<LLBC_Property *>(GetProperty(name));
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
                .escape(EscapeChars, EscapeChar).c_str());

    if (!_comments.empty())
    {
        if (valueStr.empty())
            valueStr.append_format("%c %s", CommentBeg, _comments.c_str());
        else
            valueStr.append_format(" %c %s", CommentBeg, _comments.c_str());
    }

    return valueStr;
}

void LLBC_Property::CollectValueProperties(std::vector<const LLBC_Property *> &properties) const
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
