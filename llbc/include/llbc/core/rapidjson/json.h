#ifndef JSON_RAPIDJSON_H_INCLUDED
#define JSON_RAPIDJSON_H_INCLUDED

#include "llbc/common/Common.h"

#include "llbc/core/rapidjson/reader.h"
#include "llbc/core/rapidjson/writer.h"
#include "llbc/core/rapidjson/prettywriter.h"
#include "llbc/core/rapidjson/pointer.h"
#include "llbc/core/rapidjson/stream.h"
#include "llbc/core/rapidjson/document.h"
#include "llbc/core/rapidjson/stringbuffer.h"
#include "llbc/core/rapidjson/error/en.h"

__LLBC_NS_BEGIN

namespace LLBC_Json = ::rapidjson;
typedef LLBC_Json::Value LLBC_JsonValue;
typedef LLBC_Json::Document LLBC_JsonDocument;
typedef LLBC_Json::Value::ValueIterator LLBC_JsonValueIter;
typedef LLBC_Json::Value::ConstValueIterator LLBC_JsonValueCIter;
typedef LLBC_Json::Value::MemberIterator LLBC_JsonMemberIter;
typedef LLBC_Json::Value::ConstMemberIterator LLBC_JsonMemberCIter;

/**
 * LLBC_JsonValue convert to LLBC_String
 * @param[in] value - LLBC_JsonValue
 * @param[out] outStr - output string
 * @param[in] isPretty - when it is true, then json output a pretty string.
 */
LLBC_EXTERN LLBC_EXPORT void LLBC_JsonToString(const LLBC_JsonValue &value, LLBC_String &outStr, bool isPretty = false);

/**
 * LLBC_JsonValue output to stream operator function(in global ns).
 */
LLBC_EXTERN LLBC_EXPORT std::ostream &operator <<(std::ostream &o, const LLBC_JsonValue &json);

__LLBC_NS_END

#endif // JSON_RAPIDJSON_H_INCLUDED
