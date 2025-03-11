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

#pragma once

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief The base64 coder encapsulation.
 */
class LLBC_EXPORT LLBC_Base64
{
public:
    /**
     * Encode a byte array of characters as base64 format.
     * @param[in] input         - the input byte array, not null.
     * @param[in] inputLen      - the input byte array length.
     * @param[in] output        - the output buffer of the encoding, stores the base64 encoded string.
     * @param[in/out] outputLen - the output buffer length, when encoded, this parameter stores the encoded string length(not included \0).
     * @return int - return 0 if success, otherwise return -1.
     * @return std::string/LLBC_String - the encoded string.
     */
    static std::string Encode(const std::string &input);
    static LLBC_String Encode(const LLBC_String &input);
    static LLBC_String Encode(const char *input, size_t inputLen);
    static int Encode(const std::string &input, std::string &output);
    static int Encode(const LLBC_String &input, LLBC_String &output);
    static int Encode(const char *input, size_t inputLen, std::string &output);
    static int Encode(const char *input, size_t inputLen, LLBC_String &output);
    static int Encode(const char *input, size_t inputLen, char *output, size_t &outputLen);

    /**
     * Decode a base64 encoded string into bytes.
     * @param[in] input         - the input buffer for decode string, base64 encoded format.
     * @param[in] inputLen      - the input buffer size(not included \n).
     * @param[in] output        - the decoded byte array.
     * @param[in/out] outputLen - the decoded bytes length, when decoded, this parameter store the decoded byte array length.
     * @return int - return 0 if success, otherwise return -1.
     * @return std::string/LLBC_String - the decoded string.
     */
    static std::string Decode(const std::string &input);
    static LLBC_String Decode(const LLBC_String &output);
    static LLBC_String Decode(const char *input, size_t inputLen);
    static int Decode(const std::string &input, std::string &output);
    static int Decode(const LLBC_String &input, LLBC_String &output);
    static int Decode(const char *input, size_t inputLen, std::string &output);
    static int Decode(const char *input, size_t inputLen, LLBC_String &output);
    static int Decode(const char *input, size_t inputLen, char *output, size_t &outputLen);

public:
    /**
     * Calcuate encoded length.
     * @param[in] bytesLen - the bytes length.
     * @return - size_t - the encoded string length.
     */
    static size_t CalcEncodeLen(size_t bytesLen);

    /**
     * Calculate decoded length.
     * @param[in] input    - the input characters, b64 encoded.
     * @param[in] inputLen - the input characters length(not included \0).
     * @return size_t the decoded length.
     */
    static size_t CalcDecodedLen(const char *input, size_t inputLen);
};

__LLBC_NS_END

#include "llbc/core/utils/Util_Base64Inl.h"


