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

#include "core/Example_Core_Utils.h"

int Example_Core_Utils::Run(int argc, char *argv[])
{
    // The core/utils module provides a set of standalone utility functions/classes; this sample picks a few common ones:
    //   LLBC_Base64 / LLBC_MD5 / Util_Text / Util_Math / Util_Algorithm

    // ========================= LLBC_Base64: encode/decode =========================
    LLBC_PrintLn("LLBC_Base64:");
    const LLBC_String plain = "Hello, llbc utils!";
    LLBC_String encoded;
    LLBC_Base64::Encode(plain, encoded);
    LLBC_PrintLn("- Encode(\"%s\") = %s", plain.c_str(), encoded.c_str());

    LLBC_String decoded;
    LLBC_Base64::Decode(encoded, decoded);
    LLBC_PrintLn("- Decode back  = %s (match: %s)",
                 decoded.c_str(), decoded == plain ? "true" : "false");
    LLBC_PrintLn("\n");

    // ========================= LLBC_MD5: digest =========================
    LLBC_PrintLn("LLBC_MD5:");
    // HexDigest: returns a 32-character hex digest string
    LLBC_PrintLn("- HexDigest(\"\")      = %s", LLBC_MD5::HexDigest(LLBC_String("")).c_str());
    LLBC_PrintLn("- HexDigest(\"abc\")   = %s", LLBC_MD5::HexDigest(LLBC_String("abc")).c_str());
    LLBC_PrintLn("\n");

    // ========================= Util_Text: num <-> str =========================
    LLBC_PrintLn("Util_Text (num <-> str):");
    // LLBC_Num2Str<T>(num): number to string, never fails
    LLBC_PrintLn("- Num2Str(123456)      = %s", LLBC_Num2Str(123456).c_str());
    LLBC_PrintLn("- Num2Str(3.14)        = %s", LLBC_Num2Str(3.14).c_str());
    // Hex format (template second parameter _HexFormat = true)
    LLBC_PrintLn("- Num2Str<hex>(255)    = %s", (LLBC_Num2Str<int, true>(255)).c_str());
    // LLBC_Str2Num<T>(str, base): string to number
    LLBC_PrintLn("- Str2Num<sint32>(\"42\")= %d", LLBC_Str2Num<sint32>("42"));
    // Str2LooseBool: loose boolean parsing, recognizes true/yes/non-zero
    LLBC_PrintLn("- Str2LooseBool(\"yes\") = %s", LLBC_Str2LooseBool("yes") ? "true" : "false");
    LLBC_PrintLn("\n");

    // ========================= Util_Math: absolute value =========================
    LLBC_PrintLn("Util_Math:");
    LLBC_PrintLn("- Abs(-100)   = %d", LLBC_Abs(-100));
    LLBC_PrintLn("- Abs(-2.5)   = %f", LLBC_Abs(-2.5));
    LLBC_PrintLn("\n");

    // ========================= Util_Algorithm: string escaping =========================
    LLBC_PrintLn("Util_Algorithm (escape):");
    LLBC_String toEscape = "a,b,c";
    // Escape ',' with '\\'
    LLBC_StringEscape(toEscape, ",", '\\');
    LLBC_PrintLn("- Escape   \"a,b,c\" => %s", toEscape.c_str());
    // Un-escape to restore
    LLBC_StringUnEscape(toEscape, '\\');
    LLBC_PrintLn("- UnEscape back      => %s", toEscape.c_str());

    return LLBC_OK;
}
