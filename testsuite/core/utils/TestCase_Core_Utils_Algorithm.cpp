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


#include "core/utils/TestCase_Core_Utils_Algorithm.h"

int TestCase_Core_Utils_Algorithm::Run(int argc, char *argv[])
{
    LLBC_PrintLn("llbc library core/utils/Util_Algorithm test");

    LLBC_PrintLn("flow check test:");
    LLBC_PrintLn("3 + 4: %s", 
        LLBC_FlowType::Type2Str(LLBC_CheckFlowUseAdd(3, 4)));
    LLBC_PrintLn("(sint16)32767 + (sint16)2: %s",
        LLBC_FlowType::Type2Str(LLBC_CheckFlowUseAdd((sint16)32767, (sint16)2)));
    LLBC_PrintLn("(sint16)-32768 + (sint16)-2: %s",
        LLBC_FlowType::Type2Str(LLBC_CheckFlowUseAdd((sint16)-32768, (sint16)-2)));

    LLBC_PrintLn("2 - 1: %s", 
        LLBC_FlowType::Type2Str(LLBC_CheckFlowUseSub(2, 1)));
    LLBC_PrintLn("(sint16)-32768 - (sing16)1: %s",
        LLBC_FlowType::Type2Str(LLBC_CheckFlowUseSub((sint16)-32768, (sint16)1)));
    LLBC_PrintLn("(sint16)32767 - (sint16)-2: %s",
        LLBC_FlowType::Type2Str(LLBC_CheckFlowUseSub((sint16)32767, (sint16)-2)));
    LLBC_Print("\n");

    LLBC_PrintLn("NOFLOW string: %s", LLBC_FlowType::Type2Str(LLBC_FlowType::NoFlow));
    LLBC_PrintLn("UNDERFLOW string: %s", LLBC_FlowType::Type2Str(LLBC_FlowType::UnderFlow));
    LLBC_PrintLn("OVERFLOW string: %s", LLBC_FlowType::Type2Str(LLBC_FlowType::OverFlow));
    LLBC_PrintLn("UNKNOWN string: %s", LLBC_FlowType::Type2Str(LLBC_FlowType::Unknown));
    LLBC_PrintLn("0xff3d string: %s", LLBC_FlowType::Type2Str(0xff3d));

	LLBC_PrintLn("\nLLBC_StringEscape test:");

    LLBC_String willEscape = ".#=";
    //Ä©Î²±àÂë²âÊÔ
    LLBC_String escapeTail1 = "\\abcdefghijklmn.abcdefghijklmn=abcdefghijklmn#abcdefghijklmn.";
    LLBC_String escapeTail2 = "\\abcdefghijklmn.abcdefghijklmn=abcdefghijklmn#abcdefghijklmn.";
    escapeTail1.escape(willEscape, '\\');
    LLBC_StringEscape(escapeTail2, willEscape, '\\');
    ASSERT(escapeTail1 == escapeTail2);
    LLBC_PrintLn("Tail escape test: %s", (escapeTail1 == escapeTail2) ? "true" : "false");

    //·ÇÄ©±àÂë²âÊÔ
    LLBC_String escapeNotTail1 = "\\abcdefghijklmn.abcdefghijklmn=abcdefghijklmn#abcdefghijklmn";
    LLBC_String escapeNotTail2 = "\\abcdefghijklmn.abcdefghijklmn=abcdefghijklmn#abcdefghijklmn";
    escapeNotTail1.escape(willEscape, '\\');
    LLBC_StringEscape(escapeNotTail2, willEscape, '\\');
    ASSERT(escapeNotTail1 == escapeNotTail2);
    LLBC_PrintLn("Not Tail escape test: %s", (escapeNotTail1 == escapeNotTail2) ? "true" : "false");

    //¶à×Ö·û²âÊÔ
    LLBC_String speEscape2 = "!.#=$:?<>_^[]@+-{}~/|&*`";
    LLBC_String speStr1 = "!.#=$:?<>_^[]@+-{}~/|&*`";
    LLBC_String speStr2 = "!.#=$:?<>_^[]@+-{}~/|&*`";
    speStr1.escape(speEscape2, '\\');
    LLBC_StringEscape(speStr2, speEscape2, '\\');
    ASSERT(speStr1 == speStr2);
    LLBC_PrintLn("Special character escape test: %s", (speStr1 == speStr2) ? "true": "false");

    const static int nEscapeTestNum = 100000;
    std::vector<LLBC_String> t1;
    std::vector<LLBC_String> t2;
    t1.resize(nEscapeTestNum,
              "\\abcdefghijklmn.abcdefghijklmn=abcdefghijklmn#abcdefghijklmn.\\abcdefghijklmn.abcdefghijklmn="
              "abcdefghijklmn#abcdefghijklmn.");
    t2.resize(nEscapeTestNum,
              "\\abcdefghijklmn.abcdefghijklmn=abcdefghijklmn#abcdefghijklmn.\\abcdefghijklmn.abcdefghijklmn="
              "abcdefghijklmn#abcdefghijklmn.");

    LLBC_Time begTestTime = LLBC_Time::Now();
    for (int i = 0; i < nEscapeTestNum; ++i)
    {
        t1[i].escape(willEscape, '\\');
    }
    LLBC_PrintLn("LLBC_String escape test used time(ms): %lld",
                   (LLBC_Time::Now() - begTestTime).GetTotalMilliSeconds());

    begTestTime = LLBC_Time::Now();
    for (int i = 0; i < nEscapeTestNum; ++i)
    {
        LLBC_StringEscape(t2[i], willEscape, '\\');
    }
    LLBC_PrintLn("Algorithm LLBC_String escape test used time(ms): %lld",
                   (LLBC_Time::Now() - begTestTime).GetTotalMilliSeconds());

    ASSERT(t1[0] == t2[0]);
    LLBC_PrintLn("test completed, press any key to exit");
    getchar();

    return 0;
}

