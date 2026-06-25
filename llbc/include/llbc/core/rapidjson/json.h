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
#include "llbc/core/rapidjson/istreamwrapper.h"
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
LLBC_EXPORT std::ostream &operator<<(std::ostream &o, const LLBC_JsonValue &value);

/**
 * Lenient JSON value accessors.
 *
 * These helpers fold the common rapidjson "HasMember + Is<T>() + Get<T>()"
 * three-step pattern into a single call:
 *  - Missing key, parent-not-object, or wrong-type are treated as a single
 *    silent-failure branch (returns default / a shared null value).
 *  - The shared null value returned by LLBC_JsonGetMember has every Is<T>()
 *    returning false, so it composes safely with the other accessors:
 *    e.g. LLBC_JsonGetStr(LLBC_JsonGetMember(obj, "name")).
 *
 * Intended for config-style "best effort" parsing, NOT for strict schemas
 * (callers cannot distinguish "missing" from "wrong type").
 */

/**
 * Get parent[key] if present, otherwise a shared static null value.
 * @param[in] parent - the parent json value (need not be an object).
 * @param[in] key    - the member key.
 * @return the member value, or a static null value if not found / parent not object.
 */
LLBC_EXPORT const LLBC_JsonValue &LLBC_JsonGetMember(
    const LLBC_JsonValue &parent, const char *key);

/**
 * Get v as a C-string if v is a string, otherwise def.
 * @param[in] v   - the json value.
 * @param[in] def - the default value when v is not a string (default: nullptr).
 * @return the C-string pointer, or def.
 */
LLBC_EXPORT const char *LLBC_JsonGetStr(
    const LLBC_JsonValue &v, const char *def = nullptr);

/**
 * Get v as an int if v is Int / Int64-fits-in-int, otherwise def.
 * @param[in] v   - the json value.
 * @param[in] def - the default value when v is not an int (default: 0).
 * @return the int value, or def.
 */
LLBC_EXPORT int LLBC_JsonGetInt(const LLBC_JsonValue &v, int def = 0);

__LLBC_NS_END

#endif // JSON_RAPIDJSON_H_INCLUDED
