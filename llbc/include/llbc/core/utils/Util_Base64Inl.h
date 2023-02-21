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

__LLBC_NS_BEGIN

inline std::string LLBC_Base64::Encode(const std::string &input)
{
    std::string output;
    Encode(input, output);
    return output;
}

inline LLBC_String LLBC_Base64::Encode(const LLBC_String &input)
{
    LLBC_String output;
    Encode(input, output);
    return output;
}

inline LLBC_String LLBC_Base64::Encode(const char *input, size_t inputLen)
{
    LLBC_String output;
    Encode(input, inputLen, output);
    return output;
}

inline int LLBC_Base64::Encode(const std::string &input, std::string &output)
{
    return Encode(input.data(), input.size(), output);
}

inline int LLBC_Base64::Encode(const LLBC_String &input, LLBC_String &output)
{
    return Encode(input.data(), input.size(), output);
}

inline int LLBC_Base64::Encode(const char *input, size_t inputLen, std::string &output)
{
    size_t outputLen = CalcEncodeLen(inputLen);
    if (UNLIKELY(outputLen == 0))
    {
        output.clear();
        return LLBC_OK;
    }

    output.resize(outputLen);
    return Encode(input, inputLen, const_cast<char *>(output.data()), outputLen);
}

inline int LLBC_Base64::Encode(const char *input, size_t inputLen, LLBC_String &output)
{
    size_t outputLen = CalcEncodeLen(inputLen);
    if (UNLIKELY(outputLen == 0))
    {
        output.clear();
        return LLBC_OK;
    }

    output.resize(outputLen);
    return Encode(input, inputLen, const_cast<char *>(output.data()), outputLen);
}

inline std::string LLBC_Base64::Decode(const std::string &input)
{
    std::string output;
    Decode(input, output);
    return output;
}

inline LLBC_String LLBC_Base64::Decode(const LLBC_String &input)
{
    LLBC_String output;
    Decode(input, output);
    return output;
}

inline LLBC_String LLBC_Base64::Decode(const char *input, size_t inputLen)
{
    LLBC_String output;
    Decode(input, inputLen, output);
    return output;
}

inline int LLBC_Base64::Decode(const std::string &input, std::string &output)
{
    return Decode(input.data(), input.size(), output);
}

inline int LLBC_Base64::Decode(const LLBC_String &input, LLBC_String &output)
{
    return Decode(input.data(), input.size(), output);
}

inline int LLBC_Base64::Decode(const char *input, size_t inputLen, std::string &output)
{
    size_t outputLen = CalcDecodedLen(input, inputLen);
    if (UNLIKELY(outputLen == 0))
    {
        output.clear();
        return LLBC_OK;
    }

    output.resize(outputLen);
    return Decode(input, inputLen, const_cast<char *>(output.data()), outputLen);
}

inline int LLBC_Base64::Decode(const char *input, size_t inputLen, LLBC_String &output)
{
    size_t outputLen = CalcDecodedLen(input, inputLen);
    if (UNLIKELY(outputLen == 0))
    {
        output.clear();
        return LLBC_OK;
    }

    output.resize(outputLen);
    return Decode(input, inputLen, const_cast<char *>(output.data()), outputLen);
}

__LLBC_NS_END
