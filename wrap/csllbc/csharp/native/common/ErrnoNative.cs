using System;

namespace llbc
{
    public class Errno
    {

        static public uint LLBC_ERROR_MASK_SEVERITY          = 0xc0000000;
        static public uint LLBC_ERROR_MASK_CUSTOM            = 0x20000000;
        static public uint LLBC_ERROR_MASK_RESERVED          = 0x1ff00000;
        static public uint LLBC_ERROR_MASK_TYPE              = 0x000f0000;
        static public uint LLBC_ERROR_MASK_NO                = 0x0000ffff;

        static public uint LLBC_ERROR_SEV_RSHIFT             = 30;
        static public uint LLBC_ERROR_CUSTOM_RSHIFT          = 29;
        static public uint LLBC_ERROR_RESERVED_RSHIFT        = 20;
        static public uint LLBC_ERROR_TYPE_RSHIFT            = 16;
        static public uint LLBC_ERROR_NO_RSHIFT              = 0;

        static public uint LLBC_ERROR_SEV_SUCCESS            = 0x00;
        static public uint LLBC_ERROR_SEV_INFO               = 0x01;
        static public uint LLBC_ERROR_SEV_WARN               = 0x02;
        static public uint LLBC_ERROR_SEV_ERROR              = 0x03;

        static public uint LLBC_ERROR_NON_CUSTOM             = 0x00;
        static public uint LLBC_ERROR_CUSTOM                 = 0x01;

        static public uint LLBC_ERROR_TYPE_LIB               = 0x0000;
        static public uint LLBC_ERROR_TYPE_CLIB              = 0x0001;
        static public uint LLBC_ERROR_TYPE_OSAPI             = 0x0002;
        static public uint LLBC_ERROR_TYPE_NETAPI            = 0x0003;
        static public uint LLBC_ERROR_TYPE_GAI               = 0x0004;

        static public uint LLBC_ERROR_SUCCESS                = 0;

        static public uint LLBC_ERROR_UNKNOWN                = 0xc000ffff;

        static public uint LLBC_ERROR_CLIB                   = 0xc0010001;

        static public uint LLBC_ERROR_OSAPI                  = 0xc0020002;

        static public uint LLBC_ERROR_NETAPI                 = 0xc0030003;

        static public uint LLBC_ERROR_GAI                    = 0xc0040004;

        static public uint LLBC_ERROR_ARG                    = 0xc0000005;

        static public uint LLBC_ERROR_NOT_IMPL               = 0xc0000006;

        static public uint LLBC_ERROR_RANGE                  = 0xc0000007;

        static public uint LLBC_ERROR_EXIST                  = 0xc0000008;

        static public uint LLBC_ERROR_NOT_FOUND              = 0xc0000009;

        static public uint LLBC_ERROR_END                    = 0xc000000a;

        static public uint LLBC_ERROR_LIMIT                  = 0xc000000b;

        static public uint LLBC_ERROR_NOT_INIT               = 0xc000000c;

        static public uint LLBC_ERROR_NOT_OPEN               = 0xc000000d;

        static public uint LLBC_ERROR_INITED                 = 0xc000000e;

        static public uint LLBC_ERROR_OPENED                 = 0xc000000f;

        static public uint LLBC_ERROR_PERM                   = 0xc0000010;

        static public uint LLBC_ERROR_TRUNCATED              = 0xc0000011;

        static public uint LLBC_ERROR_INVALID                = 0xc0000012;

        static public uint LLBC_ERROR_FORMAT                 = 0xc0000013;

        static public uint LLBC_ERROR_TIMEOUTED              = 0xc0000014;

        static public uint LLBC_ERROR_BUSY                   = 0xc0000015;

        static public uint LLBC_ERROR_REENTRY                = 0xc0000016;

        static public uint LLBC_ERROR_NO_SUCH                = 0xc0000017;

        static public uint LLBC_ERROR_WBLOCK                 = 0xc0000018;

        static public uint LLBC_ERROR_AGAIN                  = 0xc0000019;

        static public uint LLBC_ERROR_PENDING                = 0xc000001a;

        static public uint LLBC_ERROR_ILLEGAL                = 0xc000001b;

        static public uint LLBC_ERROR_ACCESS_DENY            = 0xc000001c;

        static public uint LLBC_ERROR_REPEAT                 = 0xc000001d;

        static public uint LLBC_ERROR_ENCODE                 = 0xc000001e;

        static public uint LLBC_ERROR_DECODE                 = 0xc000001f;

        static public uint LLBC_ERROR_COMPRESS               = 0xc0000020;

        static public uint LLBC_ERROR_DECOMPRESS             = 0xc0000021;

        static public uint LLBC_ERROR_PACK                   = 0xc0000022;

        static public uint LLBC_ERROR_UNPACK                 = 0xc0000023;

        static public uint LLBC_ERROR_NOT_ALLOW              = 0xc0000024;

        static public uint LLBC_ERROR_COMP_INIT              = 0xc0000025;

        static public uint LLBC_ERROR_COMP_START             = 0xc0000026;

        static public uint LLBC_ERROR_WSA_SYSNOTREADY        = 0xc0000027;

        static public uint LLBC_ERROR_WSA_VERNOTSUPPORTED    = 0xc0000028;

        static public uint LLBC_ERROR_WSA_EINPROGRESS        = 0xc0000029;

        static public uint LLBC_ERROR_WSA_EPROCLIM           = 0xc000002a;

        static public uint LLBC_ERROR_IS_LISTEN_SOCKET       = 0xc000002b;

        static public uint LLBC_ERROR_NOT_POOL_OBJECT        = 0xc000002c;

        static public uint LLBC_ERROR_SESSION_SND_BUF_LIMIT  = 0xc000002d;

        static public uint LLBC_ERROR_NOT_SUPPORT            = 0xc000002e;

        static public uint LLBC_ERROR_CANCELLED              = 0xc000002f;

    }
}
