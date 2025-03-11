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

#include "core/utils/TestCase_Core_Utils_Base64.h"

TestCase_Core_Utils_Base64::TestCase_Core_Utils_Base64()
{
}

TestCase_Core_Utils_Base64::~TestCase_Core_Utils_Base64()
{
}

int TestCase_Core_Utils_Base64::Run(int argc, char *argv[])
{
    LLBC_PrintLn("core/utils/base64 test: ");

    std::string encodedText;
    std::string decodedText;
    std::string plainText = "hello, world!@#$%^&*()_+`";

    std::string emptyPlainText;
    std::string encodedEmptyText;
    std::string decodedEmptyText;

    // Test encode.
    LLBC_Base64::Encode(plainText, encodedText);
    LLBC_PrintLn("Text: [%s]", plainText.c_str());
    LLBC_PrintLn("Encoded: [%s]", encodedText.c_str());
    LLBC_Base64::Encode(emptyPlainText, encodedEmptyText);
    LLBC_PrintLn("Empty Text Encoded: [%s]", encodedEmptyText.c_str());

    // Test decode.
    LLBC_Base64::Decode(encodedText, decodedText);
    LLBC_PrintLn("Decoded: [%s]", decodedText.c_str());
    LLBC_Base64::Decode(encodedEmptyText, decodedEmptyText);
    LLBC_PrintLn("Decoded Empty Text: [%s]", decodedEmptyText.c_str());

    LLBC_PrintLn("Press any key to continue...");
    getchar();

    return 0;
}
