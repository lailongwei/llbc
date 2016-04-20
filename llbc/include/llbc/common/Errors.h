/**
 * @file    Errors.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/05
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COM_ERRORS_H__
#define __LLBC_COM_ERRORS_H__

#include "llbc/common/Macro.h"

__LLBC_NS_BEGIN

/**
 * Get llbc library last error.
 * @return int - last error no.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_GetLastError();

/**
 * Set llbc library last error.
 * @param[in] no - error no.
 */
LLBC_EXTERN LLBC_EXPORT void LLBC_SetLastError(int no);

/**
 * Format error message.
 * @param[in] no - error number.
 * @return const char *error describe.
 */
LLBC_EXTERN LLBC_EXPORT const char *LLBC_StrError(int no);


/**
 * Format error message(extend version).
 * @param[in] no        - error number.
 * @param[in] subErrrno - the sub error number.
 */
LLBC_EXTERN LLBC_EXPORT const char *LLBC_StrErrorEx(int no, int subErrno);

/**
 * Check given error no has sub error no or not.
 * @param[in] no = error number.
 * @return bool - return true if has sub error number, otherwise return false.
 */
LLBC_EXTERN LLBC_EXPORT bool LLBC_HasSubErrorNo(int no);

/**
 * Format last error.
 * @return const char *last error describe.
 */
LLBC_EXTERN LLBC_EXPORT const char *LLBC_FormatLastError();

/**
 * Get Sub error no.
 * @return int - sub error no.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_GetSubErrorNo();

/**
 * Set sub-error no.
 * @param[in] subErrorNo - sub error no.
 */
LLBC_EXTERN LLBC_EXPORT void LLBC_SetSubErrorNo(int subErrorNo);

/**
 * Add custom error number, and error describe.
 * @param[in] no   - custom error number.
 * @param[in] desc - custom error describe.
 * @return int - return 0 if successed, otherwise return -1.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_AddCustomErrno(int no, const char *desc);

/**
 * Remove custom error number.
 * @param[in] no - custom error number.
 * @return int - return 0 if successed, otherwise return -1.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_RemoveCustomErrno(int no);

/**
 * Remove all custom error numbers.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_RemoveAllCustomErrnos();

/**
 * Initialize errors part.
 */
LLBC_EXTERN LLBC_HIDDEN void __LLBC_InitErrors();

/**
 * Destroy errors part.
 */
LLBC_EXTERN LLBC_HIDDEN void __LLBC_DestroyErrors();

__LLBC_NS_END

#endif // !__LLBC_COM_ERRORS_H__

