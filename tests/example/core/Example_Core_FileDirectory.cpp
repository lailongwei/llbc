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

#include "core/Example_Core_FileDirectory.h"

int Example_Core_FileDirectory::Run(int argc, char *argv[])
{
    // ========================= LLBC_Directory: directory query =========================
    LLBC_PrintLn("Directory query:");
    // Current working directory
    const LLBC_String curDir = LLBC_Directory::CurDir();
    LLBC_PrintLn("- CurDir : %s", curDir.c_str());
    // System temp directory (this sample uses it for temp files, to avoid polluting the working directory)
    const LLBC_String tmpDir = LLBC_Directory::TempDir();
    LLBC_PrintLn("- TempDir: %s", tmpDir.c_str());
    // Directory / file name of the current executable module
    LLBC_PrintLn("- ModuleFileDir : %s", LLBC_Directory::ModuleFileDir().c_str());
    LLBC_PrintLn("- ModuleFileName: %s", LLBC_Directory::ModuleFileName().c_str());

    // Join: intelligently joins path segments to form the full temp file path
    const LLBC_String filePath = LLBC_Directory::Join(tmpDir, "llbc_example_filedir.txt");
    LLBC_PrintLn("- Join   : %s", filePath.c_str());
    // SplitExt: splits off the extension; parts[0] is the path without extension, parts[1] is the extension
    const LLBC_Strings extParts = LLBC_Directory::SplitExt(filePath);
    LLBC_PrintLn("- SplitExt: base=[%s] ext=[%s]", extParts[0].c_str(), extParts[1].c_str());
    // BaseName / DirName
    LLBC_PrintLn("- BaseName: %s", LLBC_Directory::BaseName(filePath).c_str());
    LLBC_PrintLn("- DirName : %s", LLBC_Directory::DirName(filePath).c_str());
    LLBC_PrintLn("\n");

    // ========================= LLBC_File: create/write =========================
    LLBC_PrintLn("File create/write:");
    {
        // Open in text-write mode (create if the file doesn't exist, truncate if it does)
        LLBC_File file;
        if (file.Open(filePath, LLBC_FileMode::TextWrite) != LLBC_OK)
        {
            // Extreme sandbox environments may be unable to write the temp directory; print and still return normally
            LLBC_PrintLn("- Open for write failed: %s, skip file demo.", LLBC_FormatLastError());
            return LLBC_OK;
        }

        // Write line by line
        file.WriteLn("Hello, llbc file!");
        file.WriteLn("Second line.");
        // Write one formatted line
        file.FormatWrite("Formatted line: %d + %d = %d\n", 1, 2, 3);
        file.Flush();
        file.Close();
        LLBC_PrintLn("- Wrote 3 lines to: %s", filePath.c_str());
    }

    // Exists: check whether the file already exists
    LLBC_PrintLn("- File::Exists after write: %s", LLBC_File::Exists(filePath) ? "true" : "false");
    LLBC_PrintLn("\n");

    // ========================= LLBC_File: read =========================
    LLBC_PrintLn("File read:");
    {
        // Read the entire content at once
        const LLBC_String content = LLBC_File::ReadToEnd(filePath);
        LLBC_PrintLn("- ReadToEnd content:");
        LLBC_PrintLn("%s", content.c_str());

        // Read line by line
        LLBC_File file;
        if (file.Open(filePath, LLBC_FileMode::TextRead) == LLBC_OK)
        {
            const LLBC_Strings lines = file.ReadLns();
            LLBC_PrintLn("- ReadLns line count: %lu", static_cast<unsigned long>(lines.size()));
            file.Close();
        }
    }
    LLBC_PrintLn("\n");

    // ========================= LLBC_File: delete (clean up temp file) =========================
    LLBC_PrintLn("File delete(cleanup):");
    if (LLBC_File::DeleteFile(filePath) == LLBC_OK)
        LLBC_PrintLn("- Deleted temp file: %s", filePath.c_str());
    else
        LLBC_PrintLn("- Delete temp file failed: %s", LLBC_FormatLastError());
    // Confirm cleanup, leaving no leftovers
    LLBC_PrintLn("- File::Exists after delete: %s", LLBC_File::Exists(filePath) ? "true" : "false");

    return LLBC_OK;
}
