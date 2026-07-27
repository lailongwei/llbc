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

#include "common/Example_Common_String.h"

int Example_Common_String::Run(int argc, char *argv[])
{
    // ========================= Construct =========================
    // LLBC_String derives from std::basic_string<char>, is compatible with all std::string usages, and adds many convenience methods.
    LLBC_PrintLn("Construct:");
    // Construct from a C string
    LLBC_String s1("Hello");
    // Copy construct
    LLBC_String s2(s1);
    // Construct by repeating a character n times
    LLBC_String s3(3, '=');
    LLBC_PrintLn("- s1: \"%s\", s2: \"%s\", s3: \"%s\"", s1.c_str(), s2.c_str(), s3.c_str());

    // ========================= Concat: append / operator+ / operator+= =========================
    LLBC_PrintLn("\nAppend & concat:");
    LLBC_String greet("Hello");
    greet.append(", ").append("World");   // Chained append
    greet += '!';                          // operator+= appends a single character
    LLBC_String greet2 = greet + " (llbc)"; // operator+ produces a new string
    LLBC_PrintLn("- greet: \"%s\"", greet.c_str());
    LLBC_PrintLn("- greet2: \"%s\"", greet2.c_str());
    // operator* repeats the string
    LLBC_PrintLn("- \"ab\" * 3 = \"%s\"", (LLBC_String("ab") * 3).c_str());

    // ========================= Format: format / append_format =========================
    // format is like sprintf, returns a reference to itself, and can be chained; only effective for the char element type.
    LLBC_PrintLn("\nFormat:");
    LLBC_String fmt;
    fmt.format("player[id=%d, name=%s, hp=%.1f]", 9527, "Judy", 99.5f);
    LLBC_PrintLn("- format: \"%s\"", fmt.c_str());
    // append_format appends formatted text after the existing content
    fmt.append_format(" lv=%d", 30);
    LLBC_PrintLn("- append_format: \"%s\"", fmt.c_str());

    // ========================= Case: tolower / toupper =========================
    // tolower/toupper return a new string without changing the original (only handles ASCII A-Z / a-z).
    LLBC_PrintLn("\nCase:");
    LLBC_String mixed("Hello LLBC");
    LLBC_PrintLn("- tolower: \"%s\"", mixed.tolower().c_str());
    LLBC_PrintLn("- toupper: \"%s\"", mixed.toupper().c_str());

    // ========================= Trim whitespace / trim given chars: strip / lstrip / rstrip =========================
    // strip with no args removes leading/trailing whitespace (space/tab/newline, etc.); you can also specify the char set to remove.
    LLBC_PrintLn("\nStrip:");
    LLBC_String padded("   trim me   ");
    LLBC_PrintLn("- strip: \"%s\"", padded.strip().c_str());
    LLBC_PrintLn("- strip('*') of \"**abc**\": \"%s\"", LLBC_String("**abc**").strip("*").c_str());

    // ========================= Split: split =========================
    // split splits by a delimiter into LLBC_Strings (i.e. std::vector<LLBC_String>).
    LLBC_PrintLn("\nSplit:");
    LLBC_String csv("a,b,c,,d");
    LLBC_Strings parts = csv.split(',');
    LLBC_PrintLn("- split \"%s\" by ',' => %d parts:", csv.c_str(), static_cast<int>(parts.size()));
    for (size_t i = 0; i < parts.size(); ++i)
        LLBC_PrintLn("  - [%d] = \"%s\"", static_cast<int>(i), parts[i].c_str());

    // ========================= Find / substr / prefix & suffix =========================
    LLBC_PrintLn("\nFind & substr & startswith/endswith:");
    LLBC_String path("/usr/local/bin/llbc");
    LLBC_String::size_type slash = path.rfind('/'); // Reverse-find the last '/'
    LLBC_PrintLn("- rfind('/') pos: %d", static_cast<int>(slash));
    LLBC_PrintLn("- substr after last '/': \"%s\"", path.substr(slash + 1).c_str());
    LLBC_PrintLn("- startswith(\"/usr\"): %s", path.startswith("/usr") ? "true" : "false");
    LLBC_PrintLn("- endswith(\"llbc\"): %s", path.endswith("llbc") ? "true" : "false");

    // ========================= Replace: findreplace =========================
    // findreplace replaces all occurrences of s1 with s2 in the string (modifies in place and returns itself).
    LLBC_PrintLn("\nfindreplace:");
    LLBC_String tmpl("a-b-c-d");
    tmpl.findreplace(LLBC_String("-"), LLBC_String("/"));
    LLBC_PrintLn("- after replace '-'->'/':\"%s\"", tmpl.c_str());

    // ========================= LLBC_CString: lightweight read-only string view =========================
    // LLBC_CString only holds a pointer + length without copying the underlying data, suitable for read-only keys / passing constants.
    LLBC_PrintLn("\nLLBC_CString (lightweight const string):");
    LLBC_CString cstr("const-view");
    LLBC_PrintLn("- c_str: \"%s\", size: %d, empty: %s",
                 cstr.c_str(), static_cast<int>(cstr.size()), cstr.empty() ? "true" : "false");
    // Supports comparison operators
    LLBC_CString cstr2("const-view");
    LLBC_PrintLn("- cstr == cstr2: %s", (cstr == cstr2) ? "true" : "false");
    // Can be converted to/from LLBC_String
    LLBC_String fromC(cstr);
    LLBC_PrintLn("- LLBC_String(cstr): \"%s\"", fromC.c_str());

    return LLBC_OK;
}
