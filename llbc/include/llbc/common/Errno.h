/**
 * @file    Errno.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/05
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COM_ERRNO_H__
#define __LLBC_COM_ERRNO_H__

#include "llbc/common/Macro.h"

#include "llbc/common/OSHeader.h"
#include "llbc/common/BasicDataType.h"

__LLBC_NS_BEGIN

// LLBC library error number.
extern LLBC_EXPORT int *__LLBC_Errno();
#define LLBC_Errno (*__LLBC_Errno())
// LLBC library sub error number of OS-API errno.
extern LLBC_EXPORT int *__LLBC_SubErrno();
#define LLBC_SubErrno (*__LLBC_SubErrno())

//
// Values are 32 bit values laid out as follow:

//  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +---+-+-----------------+-------+-------------------------------+
// |Sev|C|    Reserved     |  Type  |         Errno no              |
// +---+-+-----------------+-------+-------------------------------+
// 
// Where
// 
//      Sev - is the severity code
//            00 - Success
//            01 - Informational
//            10 - Warning
//            11 - Error
//
//      C   - is the customer code flag.
//
//      Reserved - reserved area, must set to 0.
//
//      Type - error no type.
//             0000 - llbc library error.
//             0001 - c/c++ library error.
//             0002 - OS library error(WIN32 specific).
//             0003 - Net library error(WIN32 WSA specific).
//             0004 - gai error(getaddrinfo).
//             others - undefined.
//
//      Error no - error number.
//

// Error area mask value define.
//
#define LLBC_ERROR_MASK_SEVERITY       0xc0000000
#define LLBC_ERROR_MASK_CUSTOM         0x20000000
#define LLBC_ERROR_MASK_RESERVED       0x1ff00000
#define LLBC_ERROR_MASK_TYPE           0x000f0000
#define LLBC_ERROR_MASK_NO             0x0000ffff

// Error area right shift value define.
//
#define LLBC_ERROR_SEV_RSHIFT          30
#define LLBC_ERROR_CUSTOM_RSHIFT       29
#define LLBC_ERROR_RESERVED_RSHIFT     20
#define LLBC_ERROR_TYPE_RSHIFT         16
#define LLBC_ERROR_NO_RSHIFT            0

// Error severity level define.
//
#define LLBC_ERROR_SEV_SUCCESS    0x00
#define LLBC_ERROR_SEV_INFO       0x01
#define LLBC_ERROR_SEV_WARN       0x02
#define LLBC_ERROR_SEV_ERROR      0x03

// Error custom flag define.
//
#define LLBC_ERROR_NON_CUSTOM     0x00
#define LLBC_ERROR_CUSTOM         0x01

// Error type define.
//
#define LLBC_ERROR_TYPE_LIB       0x0000
#define LLBC_ERROR_TYPE_CLIB      0x0001
#define LLBC_ERROR_TYPE_OSAPI     0x0002
#define LLBC_ERROR_TYPE_NETAPI    0x0003
#define LLBC_ERROR_TYPE_GAI       0x0004

// Error part getter helper functions(implement with macro).
// Get errno severity part value.
#define LLBC_GetErrnoSeverityPart(no) ((((uint32)(no)) & LLBC_ERROR_MASK_SEVERITY) >> LLBC_ERROR_SEV_RSHIFT)
// Get errno custom flag value.
#define LLBC_GetErrnoCustomPart(no) ((((uint32)(no)) & LLBC_ERROR_MASK_CUSTOM) >> LLBC_ERROR_CUSTOM_RSHIFT)
// Get errno reversed part value.
#define LLBC_GetErrnoReservedPart(no) ((((uint32)(no)) & LLBC_ERROR_MASK_RESERVED) >> LLBC_ERROR_RESERVED_RSHIFT)
// Get errno type part value.
#define LLBC_GetErrnoTypePart(no) ((((uint32)(no)) & LLBC_ERROR_MASK_TYPE) >> LLBC_ERROR_TYPE_RSHIFT)
// Get errno value part value.
#define LLBC_GetErrnoNoPart(no) ((((uint32)(no)) & LLBC_ERROR_MASK_NO) >> LLBC_ERROR_NO_RSHIFT)

// Build custom error number(error value part | severity part value(error level) | custom flag).
#define LLBC_BuildCustomErrno(customErrNo) ( LLBC_GetErrnoNoPart(customErrNo) | \
                                            (LLBC_ERROR_SEV_ERROR << LLBC_ERROR_SEV_RSHIFT) | \
                                            (LLBC_ERROR_CUSTOM << LLBC_ERROR_CUSTOM_RSHIFT) )

// error type getter helper macro define.
//
#define LLBC_ERROR_TYPE_IS_LIBRARY(no) (LLBC_GetErrnoTypePart(no) == LLBC_ERROR_TYPE_LIB)
#define LLBC_ERROR_TYPE_IS_CLIB(no) (LLBC_GetErrnoTypePart(no) == LLBC_ERROR_TYPE_CLIB)
#define LLBC_ERROR_TYPE_IS_OSAPI(no) (LLBC_GetErrnoTypePart(no) == LLBC_ERROR_TYPE_OSAPI)
#define LLBC_ERROR_TYPE_IS_NETAPI(no) (LLBC_GetErrnoTypePart(no) == LLBC_ERROR_TYPE_NETAPI)
#define LLBC_ERROR_TYPE_IS_GAI(no) (LLBC_GetErrnoTypePart(no) == LLBC_ERROR_TYPE_GAI)

//
// Message Id: LLBC_ERROR_SUCCESS
//
// MessageText:
//
// Successfully.
//
#define LLBC_ERROR_SUCCESS      0

//
// Message Id: LLBC_ERROR_UNKNOWN
//
// MessageText:
//
// Unknown error.
//
#define __LLBC_ERROR_UNKNOWN     ((int)(0x0000ffff))
#define LLBC_ERROR_UNKNOWN       ((int)(0xc000ffff))

//
// Message Id: LLBC_ERROR_CLIB
//
// MessageText:
//
// C Lib error: %s.
//
#define __LLBC_ERROR_CLIB        ((int)(0x00000001))
#define LLBC_ERROR_CLIB          ((int)(0xc0010001))

//
// Message Id: LLBC_ERROR_OSAPI 
//
// MessageText:
//
// OS Api call error: %s.
//
#define __LLBC_ERROR_OSAPI       ((int)(0x00000002))
#define LLBC_ERROR_OSAPI         ((int)(0xc0020002))

//
// Message Id: LLBC_ERROR_NETAPI
//
// MessageText:
//
// Net API call error: %d.
//
#define __LLBC_ERROR_NETAPI      ((int)(0x00000003))
#define LLBC_ERROR_NETAPI        ((int)(0xc0030003))

//
// Message Id: LLBC_ERROR_GAI
//
// MessageText:
//
// getaddrinfo() call error: %d:%s
//
#define __LLBC_ERROR_GAI         ((int)(0x00000004))
#define LLBC_ERROR_GAI           ((int)(0xc0040004))

//
// Message Id: LLBC_ERROR_ARG
//
// MessageText:
//
// Argument error.
//
#define __LLBC_ERROR_ARG         ((int)(0x00000005))
#define LLBC_ERROR_ARG           ((int)(0xc0000005))

//
// Message Id: LLBC_ERROR_NOT_IMPL
//
// MessageText:
//
// Not implement.
//
#define __LLBC_ERROR_NOT_IMPL    ((int)(0x00000006))
#define LLBC_ERROR_NOT_IMPL      ((int)(0xc0000006))

//
// Message Id: LLBC_ERROR_RANGE
//
// MessageText:
//
// Range error.
//
#define __LLBC_ERROR_RANGE       ((int)(0x00000007))
#define LLBC_ERROR_RANGE         ((int)(0xc0000007))

//
// Message Id: LLBC_ERROR_EXIST
//
// MessageText:
//
// exist.
//
#define __LLBC_ERROR_EXIST       ((int)(0x00000008))
#define LLBC_ERROR_EXIST         ((int)(0xc0000008))

//
// Message Id: LLBC_ERROR_NOT_FOUND
//
// MessageText:
//
// not found.
//
#define __LLBC_ERROR_NOT_FOUND   ((int)(0x00000009))
#define LLBC_ERROR_NOT_FOUND     ((int)(0xc0000009))

//
// Message Id: LLBC_ERROR_END
//
// MessageText:
//
// end.
//
#define __LLBC_ERROR_END         ((int)(0x0000000a))
#define LLBC_ERROR_END           ((int)(0xc000000a))

//
// Message Id: LLBC_ERROR_END
//
// MessageText:
//
// limit.
//
#define __LLBC_ERROR_LIMIT       ((int)(0x0000000b))
#define LLBC_ERROR_LIMIT         ((int)(0xc000000b))

//
// Message Id: LLBC_ERROR_NOT_INIT
//
// MessageText:
//
// not init.
//
#define __LLBC_ERROR_NOT_INIT    ((int)(0x0000000c))
#define LLBC_ERROR_NOT_INIT      ((int)(0xc000000c))

//
// Message Id: LLBC_ERROR_NOT_OPEN
//
// MessageText:
//
// not open.
//
#define __LLBC_ERROR_NOT_OPEN    ((int)(0x0000000d))
#define LLBC_ERROR_NOT_OPEN      ((int)(0xc000000d))

//
// Message Id: LLBC_ERROR_INITED
//
// MessageText:
//
// initialized.
//
#define __LLBC_ERROR_INITED      ((int)(0x0000000e))
#define LLBC_ERROR_INITED        ((int)(0xc000000e))

//
// Message Id: LLBC_ERROR_OPENED
//
// MessageText:
//
// opened.
//
#define __LLBC_ERROR_OPENED      ((int)(0x0000000f))
#define LLBC_ERROR_OPENED        ((int)(0xc000000f))

//
// Message Id: LLBC_ERROR_PERM
//
// MessageText:
//
// permission denied.
//
#define __LLBC_ERROR_PERM        ((int)(0x00000010))
#define LLBC_ERROR_PERM          ((int)(0xc0000010))

//
// Message Id: LLBC_ERROR_TRUNCATED
//
// MessageText:
//
// truncated.
//
#define __LLBC_ERROR_TRUNCATED   ((int)(0x00000011))
#define LLBC_ERROR_TRUNCATED     ((int)(0xc0000011))

//
// Message Id: LLBC_ERROR_INVALID
//
// MessageText:
//
// invalidate.
//
#define __LLBC_ERROR_INVALID     ((int)(0x00000012))
#define LLBC_ERROR_INVALID       ((int)(0xc0000012))

//
// Message Id: LLBC_ERROR_FORMAT
//
// MessageText:
//
// format error.
//
#define __LLBC_ERROR_FORMAT      ((int)(0x00000013))
#define LLBC_ERROR_FORMAT        ((int)(0xc0000013))

//
// Message Id: LLBC_ERROR_TIMEOUT
//
// MessageText:
//
// timeout.
//
#define __LLBC_ERROR_TIMEOUT     ((int)(0x00000014))
#define LLBC_ERROR_TIMEOUT       ((int)(0xc0000014))

//
// Message Id: LLBC_ERROR_BUSY
//
// MessageText:
//
// busy.
//
#define __LLBC_ERROR_BUSY        ((int)(0x00000015))
#define LLBC_ERROR_BUSY          ((int)(0xc0000015))

//
// Message Id: LLBC_ERROR_REENTRY
//
// MessageText:
//
// reentry.
//
#define __LLBC_ERROR_REENTRY     ((int)(0x00000016))
#define LLBC_ERROR_REENTRY       ((int)(0xc0000016))

//
// Message Id: LLBC_ERROR_NO_SUCH
//
// MessageText:
//
// no such memory/data/resource.
//
#define __LLBC_ERROR_NO_SUCH     ((int)(0x00000017))
#define LLBC_ERROR_NO_SUCH       ((int)(0xc0000017))

//
// Message Id: LLBC_ERROR_WOULD_BLOCK
//
// MessageText:
//
// would block.
//
#define __LLBC_ERROR_WBLOCK      ((int)(0x00000018))
#define LLBC_ERROR_WBLOCK        ((int)(0xc0000018))

//
// Message Id: LLBC_ERROR_AGAIN
//
// MessageText:
//
// again.
//
#define __LLBC_ERROR_AGAIN       ((int)(0x00000019))
#define LLBC_ERROR_AGAIN         ((int)(0xc0000019))

//
// Message Id: LLBC_ERROR_PENDING
//
// MessageText:
//
// pending.
//
#define __LLBC_ERROR_PENDING     ((int)(0x0000001a))
#define LLBC_ERROR_PENDING       ((int)(0xc000001a))

//
// Message Id: LLBC_ERROR_ILLEGAL
//
// MessageText:
//
// illegal.
//
#define __LLBC_ERROR_ILLEGAL     ((int)(0x0000001b))
#define LLBC_ERROR_ILLEGAL       ((int)(0xc000001b))

//
// Message Id: LLBC_ERROR_ACCESS_DENY
//
// MessageText:
//
// access deny.
//
#define __LLBC_ERROR_ACCESS_DENY ((int)(0x0000001c))
#define LLBC_ERROR_ACCESS_DENY   ((int)(0xc000001c))

//
// Message Id: LLBC_ERROR_REPEAT
//
// MessageText:
//
// repeat
//
#define __LLBC_ERROR_REPEAT      ((int)(0x0000001d))
#define LLBC_ERROR_REPEAT        ((int)(0xc000001d))

//
//!! Sentinel error no.
//
#define __LLBC_ERROR_SENTINEL    ((int)(0x000000ff))

__LLBC_NS_END

#endif // !__LLBC_COM_ERRNO_H__
