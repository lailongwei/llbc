#ifndef JSON_RAPIDJSON_H_INCLUDED
# define JSON_RAPIDJSON_H_INCLUDED

#include "llbc/common/Common.h"

# include "reader.h"
# include "writer.h"
# include "prettywriter.h"
# include "pointer.h"
# include "stream.h"
# include "document.h"
# include "stringbuffer.h"
#include "error/en.h"

__LLBC_NS_BEGIN

namespace LLBC_Json = ::rapidjson;
typedef LLBC_Json::Value LLBC_JsonValue;
typedef LLBC_Json::Document LLBC_JsonDocument;
typedef LLBC_Json::Value::ValueIterator LLBC_JsonValueIter;
typedef LLBC_Json::Value::ConstValueIterator LLBC_JsonValueCIter;
typedef LLBC_Json::Value::MemberIterator LLBC_JsonMemberIter;
typedef LLBC_Json::Value::ConstMemberIterator LLBC_JsonMemberCIter;

LLBC_EXTERN LLBC_EXPORT void LLBC_JsonToString(const LLBC_JsonValue &value, LLBC_String &outStr, bool isPretty = false);
LLBC_EXTERN LLBC_EXPORT std::ostream &operator <<(std::ostream &o, const LLBC_Json::Value &json);

__LLBC_NS_END

#endif // JSON_RAPIDJSON_H_INCLUDED
