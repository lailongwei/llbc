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

#include "core/Example_Core_Variant.h"

int Example_Core_Variant::Run(int argc, char *argv[])
{
    // LLBC_Variant is a general-purpose variant type that can hold nil (NIL) / number (RAW) / string (STR) / sequence (SEQ) / dictionary (DICT).

    // ========================= 1. Construct from different types =========================
    LLBC_PrintLn("Construct variants from different types:");
    LLBC_Variant nilVar;                      // default-constructed as NIL
    LLBC_Variant intVar = 42;                 // integer (RAW)
    LLBC_Variant dblVar = 3.14;               // floating-point (RAW)
    LLBC_Variant strVar = "hello llbc";       // string (STR)

    // Dictionary (DICT): assigning by subscript automatically Becomes a DICT
    LLBC_Variant dictVar;
    dictVar["name"] = "Judy";
    dictVar["level"] = 99;
    dictVar["vip"] = true;

    // Sequence (SEQ): append elements via SeqPushBack, automatically Becomes a SEQ
    LLBC_Variant seqVar;
    seqVar.SeqPushBack(10);
    seqVar.SeqPushBack("mixed");
    seqVar.SeqPushBack(2.5);

    LLBC_PrintLn("- nilVar : %s", nilVar.ValueToString().c_str());
    LLBC_PrintLn("- intVar : %s", intVar.ValueToString().c_str());
    LLBC_PrintLn("- dblVar : %s", dblVar.ValueToString().c_str());
    LLBC_PrintLn("- strVar : %s", strVar.ValueToString().c_str());
    LLBC_PrintLn("- dictVar: %s", dictVar.ValueToString().c_str());
    LLBC_PrintLn("- seqVar : %s", seqVar.ValueToString().c_str());

    // ========================= 2. Type queries =========================
    LLBC_PrintLn("\nType queries:");
    // Is<void>() checks nil; IsRaw() checks number; Is<Str/Seq/Dict>() checks a specific composite type; TypeToString() gets the type name
    LLBC_PrintLn("- nilVar.Is<void>()             : %s", nilVar.Is<void>() ? "true" : "false");
    LLBC_PrintLn("- intVar.IsRaw()                : %s", intVar.IsRaw() ? "true" : "false");
    LLBC_PrintLn("- strVar.Is<LLBC_Variant::Str>(): %s", strVar.Is<LLBC_Variant::Str>() ? "true" : "false");
    LLBC_PrintLn("- seqVar.Is<LLBC_Variant::Seq>(): %s", seqVar.Is<LLBC_Variant::Seq>() ? "true" : "false");
    LLBC_PrintLn("- dictVar type name             : %s", dictVar.TypeToString().c_str());

    // ========================= 3. Fetch value (As<T>) =========================
    LLBC_PrintLn("\nFetch value by As<T>():");
    LLBC_PrintLn("- intVar.As<int>()        : %d", intVar.As<int>());
    LLBC_PrintLn("- dblVar.As<double>()     : %f", dblVar.As<double>());
    LLBC_PrintLn("- strVar.As<LLBC_String>(): %s", strVar.As<LLBC_String>().c_str());

    // ========================= 4. Arithmetic operations =========================
    LLBC_PrintLn("\nArithmetic operations:");
    LLBC_Variant a = 10;
    LLBC_Variant b = 3;
    LLBC_PrintLn("- a + b = %s", (a + b).ValueToString().c_str());
    LLBC_PrintLn("- a - b = %s", (a - b).ValueToString().c_str());
    LLBC_PrintLn("- a * b = %s", (a * b).ValueToString().c_str());
    LLBC_PrintLn("- a / b = %s", (a / b).ValueToString().c_str());

    // ========================= 5. Iterate dict =========================
    LLBC_PrintLn("\nIterate dict:");
    for (auto it = dictVar.DictBegin(); it != dictVar.DictEnd(); ++it)
        LLBC_PrintLn("- %s => %s", it->first.ValueToString().c_str(), it->second.ValueToString().c_str());

    // ========================= 6. Iterate seq =========================
    LLBC_PrintLn("\nIterate seq:");
    for (auto it = seqVar.SeqBegin(); it != seqVar.SeqEnd(); ++it)
        LLBC_PrintLn("- %s", it->ValueToString().c_str());

    return LLBC_OK;
}
