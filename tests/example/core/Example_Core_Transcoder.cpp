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

#include "core/Example_Core_Transcoder.h"

int Example_Core_Transcoder::Run(int argc, char *argv[])
{
    // LLBC_Transcoder provides encoding conversion between multibyte<->multibyte and multibyte<->wide char;
    // all methods are static and return LLBC_OK / LLBC_FAILED.
    LLBC_PrintLn("LLBC_Transcoder demo (UTF-8 <-> GBK <-> UTF-16):");
    LLBC_PrintLn("\n");

    // GBK bytes for the Chinese "你好" (hello): C4 E3 BA C3
    const LLBC_String gbkStr = "\xC4\xE3\xBA\xC3";
    LLBC_PrintLn("Source GBK bytes  : %s",
                 LLBC_Byte2Hex(gbkStr.c_str(), gbkStr.length()).c_str());
    LLBC_PrintLn("\n");

    // ========================= GBK -> UTF-8 =========================
    // MultiByteToMultiByte(fromCode, src, toCode, dest): multibyte to multibyte
    LLBC_PrintLn("GBK -> UTF-8:");
    LLBC_String utf8Str;
    if (LLBC_Transcoder::MultiByteToMultiByte("GBK", gbkStr, "UTF-8", utf8Str) != LLBC_OK)
    {
        // Some platforms may lack support for the corresponding codepage; print and still return normally
        LLBC_PrintLn("- convert failed: %s (skipped)", LLBC_FormatLastError());
        return LLBC_OK;
    }
    LLBC_PrintLn("- UTF-8 bytes     : %s",
                 LLBC_Byte2Hex(utf8Str.c_str(), utf8Str.length()).c_str());
    LLBC_PrintLn("\n");

    // ========================= UTF-8 -> GBK (round-trip check) =========================
    LLBC_PrintLn("UTF-8 -> GBK (round-trip):");
    LLBC_String gbkStr2;
    if (LLBC_Transcoder::MultiByteToMultiByte("UTF-8", utf8Str, "GBK", gbkStr2) != LLBC_OK)
    {
        LLBC_PrintLn("- convert failed: %s (skipped)", LLBC_FormatLastError());
        return LLBC_OK;
    }
    LLBC_PrintLn("- GBK bytes       : %s",
                 LLBC_Byte2Hex(gbkStr2.c_str(), gbkStr2.length()).c_str());
    LLBC_PrintLn("- round-trip match: %s", gbkStr2 == gbkStr ? "true" : "false");
    LLBC_PrintLn("\n");

    // ========================= UTF-8 -> UTF-16 (wide char) =========================
    // MultiByteToWideChar(fromCode, src, dest): multibyte to wide char (Unicode)
    LLBC_PrintLn("UTF-8 -> UTF-16(wide char):");
    LLBC_WString wStr;
    if (LLBC_Transcoder::MultiByteToWideChar("UTF-8", utf8Str, wStr) != LLBC_OK)
    {
        LLBC_PrintLn("- convert failed: %s (skipped)", LLBC_FormatLastError());
        return LLBC_OK;
    }
    LLBC_PrintLn("- wide char bytes : %s",
                 LLBC_Byte2Hex(wStr.data(), wStr.size() * sizeof(wchar)).c_str());

    // WideCharToMultiByte(toCode, src, dest): wide char back to multibyte
    LLBC_String utf8Str2;
    if (LLBC_Transcoder::WideCharToMultiByte("UTF-8", wStr, utf8Str2) == LLBC_OK)
        LLBC_PrintLn("- back to UTF-8   : %s (match: %s)",
                     LLBC_Byte2Hex(utf8Str2.c_str(), utf8Str2.length()).c_str(),
                     utf8Str2 == utf8Str ? "true" : "false");

    return LLBC_OK;
}
