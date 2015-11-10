/**
 * @file    JVHelper.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/05/04
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/helper/JVHelper.h"

__LLBC_NS_BEGIN

LLBC_Variant LLBC_JVHelper::J2V(const LLBC_JsonValue &json)
{
    LLBC_Variant var;
    switch(json.type())
    {
    case Json::nullValue:
        return var;
    case Json::intValue:
        return (var = json.asInt());
    case Json::uintValue:
        return (var = json.asUInt());
    case Json::longValue:
        return (var = json.asLong());
    case Json::ulongValue:
        return (var = json.asULong());
    case Json::realValue:
        return (var = json.asDouble());
    case Json::booleanValue:
        return (var = json.asBool());

    case Json::stringValue:
        return (var = json.asCString());

    case Json::arrayValue:
        {
            uint32 idx = 0;
            for (LLBC_JsonValueCIter it = json.begin();
                it != json.end();
                it++)
                var.Insert(idx++, J2V(*it));
        }
        return var;
    case Json::objectValue:
        {
            for (LLBC_JsonValueCIter it = json.begin();
                it != json.end();
                it++)
                var.Insert(it.key().asCString(), J2V(*it));
        }
        return var;

    default:
        break;
    }

    return var;
}

LLBC_JsonValue LLBC_JVHelper::V2J(const LLBC_Variant &v)
{
    switch (v.GetType())
    {
    case LLBC_Variant::NIL:
        return LLBC_JsonValue();

    case LLBC_Variant::BOOL:
    case LLBC_Variant::SINT8:
    case LLBC_Variant::SINT16:
    case LLBC_Variant::SINT32:
        return LLBC_JsonValue(v.AsInt32());
    
    case LLBC_Variant::UINT8:
    case LLBC_Variant::UINT16:
    case LLBC_Variant::UINT32:
        return LLBC_JsonValue(v.AsUInt32());

    case LLBC_Variant::LONG:
        return (sizeof(long) == sizeof(sint32)) ?
            LLBC_JsonValue(v.AsInt32()) : LLBC_JsonValue(v.AsInt64());
    case LLBC_Variant::ULONG:
        return (sizeof(LLBC_NS ulong) == sizeof(uint32)) ?
            LLBC_JsonValue(v.AsUInt32()) : LLBC_JsonValue(v.AsUInt64());

    case LLBC_Variant::SINT64:
        return LLBC_JsonValue(v.AsInt64());
    case LLBC_Variant::UINT64:
        return LLBC_JsonValue(v.AsUInt64());

    case LLBC_Variant::FLOAT:
    case LLBC_Variant::DOUBLE:
        return LLBC_JsonValue(v.AsDouble());

    case LLBC_Variant::STR:
        return LLBC_JsonValue(v.AsCStr());

    case LLBC_Variant::DICT:
        {
            const LLBC_Variant::Dict &dict = v.AsDict();
            if (dict.empty())
            {
                return LLBC_JsonValue(Json::objectValue);
            }

            LLBC_Variant::Dict::const_iterator dictIt = dict.begin();
            LLBC_JsonValue json(Json::objectValue);
            for (; dictIt != dict.end(); dictIt++)
                json[dictIt->first.AsCStr()] = V2J(dictIt->second);

            return json;
        }
        break;

    default:
        break;
    }

    return LLBC_JsonValue(Json::nullValue);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
