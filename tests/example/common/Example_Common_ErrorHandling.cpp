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

#include "common/Example_Common_ErrorHandling.h"

// A demo business function that follows the llbc error convention:
// returns LLBC_OK(0) on success, LLBC_FAILED(-1) on failure with the error number set via LLBC_SetLastError.
static int DoDivide(int a, int b, int &result)
{
    if (b == 0)
    {
        // Invalid argument: set the error number and return failure; the caller retrieves details via LLBC_GetLastError()/LLBC_FormatLastError().
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    result = a / b;
    return LLBC_OK;
}

int Example_Common_ErrorHandling::Run(int argc, char *argv[])
{
    // ========================= LLBC_OK / LLBC_FAILED return convention =========================
    // Most llbc APIs use LLBC_OK(0) to indicate success and LLBC_FAILED(-1) to indicate failure;
    // failure details are not carried in the return value but obtained via the thread-local "last error".
    LLBC_PrintLn("Return convention (LLBC_OK / LLBC_FAILED):");
    int quotient = 0;
    if (DoDivide(10, 2, quotient) == LLBC_OK)
        LLBC_PrintLn("- 10 / 2 = %d (LLBC_OK)", quotient);

    if (DoDivide(10, 0, quotient) != LLBC_OK)
    {
        // Failure: read the error number + human-readable description.
        LLBC_PrintLn("- 10 / 0 failed, errNo: 0x%x, desc: %s",
                     LLBC_GetLastError(), LLBC_FormatLastError());
    }

    // ========================= SetLastError / GetLastError / FormatLastError =========================
    // Manually set a builtin error number, then retrieve and format it.
    LLBC_PrintLn("\nSetLastError / GetLastError / FormatLastError:");
    LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
    LLBC_PrintLn("- GetLastError: 0x%x", LLBC_GetLastError());
    LLBC_PrintLn("- FormatLastError: %s", LLBC_FormatLastError());

    // ========================= LLBC_StrError: get a description directly from an error number =========================
    // LLBC_StrError does not depend on the current last error; pass any error number to get its description string.
    LLBC_PrintLn("\nLLBC_StrError (describe an errNo directly):");
    LLBC_PrintLn("- LLBC_ERROR_TIMEOUTED: %s", LLBC_StrError(LLBC_ERROR_TIMEOUTED));
    LLBC_PrintLn("- LLBC_ERROR_ACCESS_DENY: %s", LLBC_StrError(LLBC_ERROR_ACCESS_DENY));
    LLBC_PrintLn("- LLBC_ERROR_NOT_SUPPORT: %s", LLBC_StrError(LLBC_ERROR_NOT_SUPPORT));

    // ========================= Builtin errNo carrying a custom error string =========================
    // The second argument of SetLastError can attach a custom description text to a builtin error number.
    LLBC_PrintLn("\nBuiltin errNo with custom error string:");
    LLBC_SetLastError(LLBC_ERROR_FORMAT, "bad packet header magic");
    LLBC_PrintLn("- FormatLastError: %s", LLBC_FormatLastError());
    // Set the same error number again without a custom string; the description reverts to the default text.
    LLBC_SetLastError(LLBC_ERROR_FORMAT);
    LLBC_PrintLn("- reset, FormatLastError: %s", LLBC_FormatLastError());

    // ========================= Register a custom errNo =========================
    // Build a custom error number via LLBC_BuildCustomErrno, then register its description with LLBC_AddCustomErrno.
    LLBC_PrintLn("\nCustom errNo (AddCustomErrno / RemoveCustomErrno):");
    const int myErr = LLBC_BuildCustomErrno(1);
    LLBC_AddCustomErrno(myErr, "My business logic error");
    LLBC_PrintLn("- custom errNo 0x%x desc: %s", myErr, LLBC_StrError(myErr));
    // Once set as the last error, it can be formatted as well.
    LLBC_SetLastError(myErr);
    LLBC_PrintLn("- as last error, FormatLastError: %s", LLBC_FormatLastError());
    // Unregister when done to avoid polluting the global error number table.
    LLBC_RemoveCustomErrno(myErr);
    LLBC_PrintLn("- after remove, desc: %s", LLBC_StrError(myErr));

    return LLBC_OK;
}
