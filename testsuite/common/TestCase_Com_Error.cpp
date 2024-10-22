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


#include "common/TestCase_Com_Error.h"

#if LLBC_TARGET_PLATFORM_NON_WIN32
 #include <netdb.h>
#endif // Non-Win32

TestCase_Com_Error::TestCase_Com_Error()
{
}

TestCase_Com_Error::~TestCase_Com_Error()
{
}

int TestCase_Com_Error::Run(int argc, char *argv[])
{
    LLBC_PrintLn("common/error test:\n");

    // Test CLib type error.
    auto fmtCLibErr = [](int errNo)
    {
        errno = errNo;
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        LLBC_PrintLn("CLib type error: %s", LLBC_FormatLastError());
    };

    fmtCLibErr(EINTR);
    fmtCLibErr(EINVAL);
    fmtCLibErr(EPIPE);
    fmtCLibErr(33333);

    // Test OSApi type error(WIN32 specific).
#if LLBC_TARGET_PLATFORM_WIN32
    //Code Description Name 
//      0 The operation completed successfully.  ERROR_SUCCESS 
//      1 Incorrect function.  ERROR_INVALID_FUNCTION 
//      2 The system cannot find the file specified.  ERROR_FILE_NOT_FOUND 
//      3 The system cannot find the path specified.  ERROR_PATH_NOT_FOUND 
//      4 The system cannot open the file.  ERROR_TOO_MANY_OPEN_FILES 
//      5 Access is denied.  ERROR_ACCESS_DENIED 
//      6 The handle is invalid.  ERROR_INVALID_HANDLE 
//      7 The storage control blocks were destroyed.  ERROR_ARENA_TRASHED 
//      8 Not enough storage is available to process this command.  ERROR_NOT_ENOUGH_MEMORY 
//      9 The storage control block address is invalid.  ERROR_INVALID_BLOCK 
//      10 The environment is incorrect.  ERROR_BAD_ENVIRONMENT 
//      11 An attempt was made to load a program with an incorrect format.  ERROR_BAD_FORMAT 
//      ... ...
//      ... ... Other error codes.
//      ... ...
//      11022 An object with an invalid ObjectLength field was specified in the QOS provider-specific buffer. WSA_QOS_EOBJLENGTH 
//      11023 An incorrect number of flow descriptors was specified in the QOS structure. WSA_QOS_EFLOWCOUNT 
//      11024 An unrecognized object was found in the QOS provider-specific buffer. WSA_QOS_EUNKNOWNPSOBJ 
//      11025 An invalid policy object was found in the QOS provider-specific buffer. WSA_QOS_EPOLICYOBJ 
//      11026 An invalid QOS flow descriptor was found in the flow descriptor list. WSA_QOS_EFLOWDESC 
//      11027 An invalid or inconsistent flowspec was found in the QOS provider-specific buffer. WSA_QOS_EPSFLOWSPEC 
//      11028 An invalid FILTERSPEC was found in the QOS provider-specific buffer. WSA_QOS_EPSFILTERSPEC 
//      11029 An invalid shape discard mode object was found in the QOS provider-specific buffer. WSA_QOS_ESDMODEOBJ 
//      11030 An invalid shaping rate object was found in the QOS provider-specific buffer. WSA_QOS_ESHAPERATEOBJ 
//      11031 A reserved policy element was found in the QOS provider-specific buffer. WSA_QOS_RESERVED_PETYPE 
    ::SetLastError(ERROR_BAD_ENVIRONMENT);
    LLBC_SetLastError(LLBC_ERROR_OSAPI);

    LLBC_PrintLn("OSAPIError: err desc:%s", LLBC_FormatLastError());
#endif

    // Test Net API type error(WIN32 specific).
#if LLBC_TARGET_PLATFORM_WIN32
    ::WSASetLastError(WSAENAMETOOLONG);
    LLBC_SetLastError(LLBC_ERROR_NETAPI);

    LLBC_PrintLn("NetAPI error: err desc: %s", LLBC_FormatLastError());
#endif

    // Test GAI type error.
    LLBC_SetLastError(LLBC_ERROR_GAI);
    LLBC_SetSubErrorNo(EAI_NODATA);
    LLBC_PrintLn("GAI error: err desc: %s", LLBC_FormatLastError());
#if LLBC_TARGET_PLATFORM_NON_WIN32
    LLBC_SetSubErrorNo(EAI_ADDRFAMILY);
    LLBC_PrintLn("GAI error: err desc: %s", LLBC_FormatLastError());
#endif

    // LLBC library type error.
    LLBC_PrintLn("LLBC library type error test:");
    LLBC_PrintLn("\tLLBC_ERROR_UNKNOWN: %s", LLBC_StrError(LLBC_ERROR_UNKNOWN));
    LLBC_PrintLn("\tLLBC_ERROR_RANGE: %s", LLBC_StrError(LLBC_ERROR_RANGE));
    LLBC_PrintLn("\tLLBC_ERROR_EXIST: %s", LLBC_StrError(LLBC_ERROR_EXIST));
    LLBC_PrintLn("\tLLBC_ERROR_NOT_FOUND: %s", LLBC_StrError(LLBC_ERROR_NOT_FOUND));
    LLBC_PrintLn("\tLLBC_ERROR_END: %s", LLBC_StrError(LLBC_ERROR_END));
    LLBC_PrintLn("\tLLBC_ERROR_LIMIT: %s", LLBC_StrError(LLBC_ERROR_LIMIT));
    LLBC_PrintLn("\tLLBC_ERROR_NOT_INIT: %s", LLBC_StrError(LLBC_ERROR_NOT_INIT));
    LLBC_PrintLn("\tLLBC_ERROR_NOT_OPEN: %s", LLBC_StrError(LLBC_ERROR_NOT_OPEN));
    LLBC_PrintLn("\tLLBC_ERROR_INITED: %s", LLBC_StrError(LLBC_ERROR_INITED));
    LLBC_PrintLn("\tLLBC_ERROR_OPENED: %s", LLBC_StrError(LLBC_ERROR_OPENED));
    LLBC_PrintLn("\tLLBC_ERROR_PERM: %s", LLBC_StrError(LLBC_ERROR_PERM));
    LLBC_PrintLn("\tLLBC_ERROR_TRUNCATED: %s", LLBC_StrError(LLBC_ERROR_TRUNCATED));
    LLBC_PrintLn("\tLLBC_ERROR_INVALID: %s", LLBC_StrError(LLBC_ERROR_INVALID));
    LLBC_PrintLn("\tLLBC_ERROR_FORMAT: %s", LLBC_StrError(LLBC_ERROR_FORMAT));
    LLBC_PrintLn("\tLLBC_ERROR_TIMEOUTED: %s", LLBC_StrError(LLBC_ERROR_TIMEOUTED));
    LLBC_PrintLn("\tLLBC_ERROR_BUSY: %s", LLBC_StrError(LLBC_ERROR_BUSY));
    LLBC_PrintLn("\tLLBC_ERROR_REENTRY: %s", LLBC_StrError(LLBC_ERROR_REENTRY));
    LLBC_PrintLn("\tLLBC_ERROR_NO_SUCH: %s", LLBC_StrError(LLBC_ERROR_NO_SUCH));
    LLBC_PrintLn("\tLLBC_ERROR_WBLOCK: %s", LLBC_StrError(LLBC_ERROR_WBLOCK));
    LLBC_PrintLn("\tLLBC_ERROR_AGAIN: %s", LLBC_StrError(LLBC_ERROR_AGAIN));
    LLBC_PrintLn("\tLLBC_ERROR_PENDING: %s", LLBC_StrError(LLBC_ERROR_PENDING));
    LLBC_PrintLn("\tLLBC_ERROR_ILLEGAL: %s", LLBC_StrError(LLBC_ERROR_ILLEGAL));
    LLBC_PrintLn("\tLLBC_ERROR_ACCESS_DENY: %s", LLBC_StrError(LLBC_ERROR_ACCESS_DENY));
    LLBC_PrintLn("\tLLBC_ERROR_REPEAT: %s", LLBC_StrError(LLBC_ERROR_REPEAT));
    LLBC_PrintLn("\tLLBC_ERROR_ENCODE: %s", LLBC_StrError(LLBC_ERROR_ENCODE));
    LLBC_PrintLn("\tLLBC_ERROR_DECODE: %s", LLBC_StrError(LLBC_ERROR_DECODE));
    LLBC_PrintLn("\tLLBC_ERROR_COMPRESS: %s", LLBC_StrError(LLBC_ERROR_COMPRESS));
    LLBC_PrintLn("\tLLBC_ERROR_DECOMPRESS: %s", LLBC_StrError(LLBC_ERROR_DECOMPRESS));
    LLBC_PrintLn("\tLLBC_ERROR_PACK: %s", LLBC_StrError(LLBC_ERROR_PACK));
    LLBC_PrintLn("\tLLBC_ERROR_UNPACK: %s", LLBC_StrError(LLBC_ERROR_UNPACK));
    LLBC_PrintLn("\tLLBC_ERROR_NOT_ALLOW: %s", LLBC_StrError(LLBC_ERROR_NOT_ALLOW));
    LLBC_PrintLn("\tLLBC_ERROR_COMP_INIT: %s", LLBC_StrError(LLBC_ERROR_COMP_INIT));
    LLBC_PrintLn("\tLLBC_ERROR_COMP_START: %s", LLBC_StrError(LLBC_ERROR_COMP_START));
    LLBC_PrintLn("\tLLBC_ERROR_WSA_SYSNOTREADY: %s", LLBC_StrError(LLBC_ERROR_WSA_SYSNOTREADY));
    LLBC_PrintLn("\tLLBC_ERROR_WSA_VERNOTSUPPORTED: %s", LLBC_StrError(LLBC_ERROR_WSA_VERNOTSUPPORTED));
    LLBC_PrintLn("\tLLBC_ERROR_WSA_EPROCLIM: %s", LLBC_StrError(LLBC_ERROR_WSA_EPROCLIM));
    LLBC_PrintLn("\tLLBC_ERROR_IS_LISTEN_SOCKET: %s", LLBC_StrError(LLBC_ERROR_IS_LISTEN_SOCKET));
    LLBC_PrintLn("\tLLBC_ERROR_NOT_POOL_OBJECT: %s", LLBC_StrError(LLBC_ERROR_NOT_POOL_OBJECT));
    LLBC_PrintLn("\tLLBC_ERROR_SESSION_SND_BUF_LIMIT: %s", LLBC_StrError(LLBC_ERROR_SESSION_SND_BUF_LIMIT));
    LLBC_PrintLn("\tLLBC_ERROR_NOT_SUPPORT: %s", LLBC_StrError(LLBC_ERROR_NOT_SUPPORT));
    LLBC_PrintLn("\tLLBC_ERROR_CANCELLED: %s", LLBC_StrError(LLBC_ERROR_CANCELLED));

    // Custom erorr test.
    LLBC_PrintLn("LLBC library custom error test:");

    const int customErr1 = LLBC_BuildCustomErrno(1);
    const int customErr2 = LLBC_BuildCustomErrno(2);
    LLBC_AddCustomErrno(customErr1, "The custom error 1");
    LLBC_AddCustomErrno(customErr2, "The custom error 2");

    LLBC_PrintLn("Custom error 0x%x desc: %s", customErr1, LLBC_StrError(customErr1));
    LLBC_PrintLn("Custom error 0x%x desc: %s", customErr2, LLBC_StrError(customErr2));

    LLBC_PrintLn("Remove custom error: 0x%x", customErr1);
    LLBC_RemoveCustomErrno(customErr1);

    LLBC_PrintLn("Custom error 0x%x desc: %s", customErr1, LLBC_StrError(customErr1));

    // Custom error string test.
    LLBC_PrintLn("Custom error string test:");
    LLBC_SetLastError(LLBC_ERROR_FORMAT, "The custom error string for LLBC_ERROR_FORMAT");
    LLBC_PrintLn("\tSet LLBC_ERROR_FORMAT error string to: \"The custom error string for LLBC_ERROR_FORMAT\"");
    LLBC_PrintLn("\tLLBC_FormatLastError(LLBC_ERROR_FORMAT):%s", LLBC_FormatLastError());
    LLBC_PrintLn("\tReset last eror:LLBC_ERROR_FORMAT:");
    LLBC_SetLastError(LLBC_ERROR_FORMAT);
    LLBC_PrintLn("\tLLBC_FormatLastError(LLBC_ERROR_FORMAT):%s", LLBC_FormatLastError());

    LLBC_Print("\n");
    LLBC_PrintLn("Press any key to continue ...");
    getchar();

    return 0;
}
