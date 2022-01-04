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

// replace rapidjson with LLBC_Json
namespace LLBC_Json = ::llbc_rapidjson;
// rapidjson's value
typedef LLBC_Json::Value LLBC_JsonValue;
// iterator of rapidjson's value
typedef LLBC_JsonValue::ValueIterator LLBC_JsonValueIter;
// const iterator of rapidjson's value
typedef LLBC_JsonValue::ConstValueIterator LLBC_JsonValueCIter;
// iterator of rapidjson's member
typedef LLBC_JsonValue::MemberIterator LLBC_JsonMemberIter;
// const iterator of rapidjson's member
typedef LLBC_JsonValue::ConstMemberIterator LLBC_JsonMemberCIter;

/**
 * LLBC_JsonValue convert to LLBC_String
 * @param[in] value - LLBC_JsonValue
 * @param[out] outStr - output string
 * @param[in] isPretty - when it is true, then json output a pretty string.
 */
LLBC_EXPORT void LLBC_JsonToString(const LLBC_JsonValue &value, LLBC_String &outStr, bool isPretty = false);

/**
 * LLBC_JsonValue output to stream operator function(in global ns).
 */
LLBC_EXPORT std::ostream &operator <<(std::ostream &o, const LLBC_JsonValue &value);

__LLBC_NS_END

#endif // JSON_RAPIDJSON_H_INCLUDED
