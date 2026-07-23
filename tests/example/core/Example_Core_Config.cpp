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

#include "core/Example_Core_Config.h"

int Example_Core_Config::Run(int argc, char *argv[])
{
    // ========================= LLBC_Ini (INI config) =========================
    // LoadFromContent() parses directly from an in-memory string, with no external file needed.
    LLBC_PrintLn("LLBC_Ini parsing from a string:");
    const LLBC_String iniContent =
        "[Server]\n"
        "name = GameSvr\n"
        "port = 8001\n"
        "maxConn = 10000\n"
        "\n"
        "[Log]\n"
        "level = Debug\n";

    LLBC_Ini ini;
    if (ini.LoadFromContent(iniContent) != LLBC_OK)
    {
        // On parse failure, GetLoadError() gives error details.
        LLBC_PrintLn("- Failed to parse INI: %s", ini.GetLoadError().c_str());
        return LLBC_FAILED;
    }

    // GetValue<T>(section, key, default) reads a config value by type.
    LLBC_PrintLn("- [Server] name = %s",
                 ini.GetValue<LLBC_String>("Server", "name").c_str());
    LLBC_PrintLn("- [Server] port = %d", ini.GetValue<int>("Server", "port"));
    LLBC_PrintLn("- [Server] maxConn = %d", ini.GetValue<int>("Server", "maxConn"));
    LLBC_PrintLn("- [Log] level = %s",
                 ini.GetValue<LLBC_String>("Log", "level").c_str());

    // Config items can also be modified/added programmatically.
    ini.SetValue<int>("Server", "port", 9001);
    LLBC_PrintLn("- After modification, [Server] port = %d", ini.GetValue<int>("Server", "port"));

    // ========================= LLBC_Properties (properties config) =========================
    // properties uses '.'-separated hierarchical keys; the parse result is stored in an LLBC_Variant and read by level.
    LLBC_PrintLn("LLBC_Properties parsing from a string:");
    const LLBC_String propContent =
        "llbc.test.intVal = 30\n"
        "llbc.test.boolVal = true\n"
        "llbc.test.strVal = Hello World\n";

    LLBC_Variant properties;
    LLBC_String errMsg;
    if (LLBC_Properties::LoadFromString(propContent, properties, &errMsg) != LLBC_OK)
    {
        LLBC_PrintLn("- Failed to parse Properties: %s", errMsg.c_str());
        return LLBC_FAILED;
    }

    // Drill down level by level; ValueToString() prints a leaf node's value.
    LLBC_PrintLn("- llbc.test.intVal  = %s",
                 properties["llbc"]["test"]["intVal"].ValueToString().c_str());
    LLBC_PrintLn("- llbc.test.boolVal = %s",
                 properties["llbc"]["test"]["boolVal"].ValueToString().c_str());
    LLBC_PrintLn("- llbc.test.strVal  = %s",
                 properties["llbc"]["test"]["strVal"].ValueToString().c_str());

    // A leaf node can be used with type conversion.
    LLBC_PrintLn("- llbc.test.intVal as int = %d",
                 properties["llbc"]["test"]["intVal"].As<int>());

    return LLBC_OK;
}
