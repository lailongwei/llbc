/**
 * @file    OS_Select.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/17
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_OS_OS_SELECT_H__
#define __LLBC_CORE_OS_OS_SELECT_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Clear fd set.
 * @param[in] fd     - file descriptor.
 * @param[in] setPtr - file descriptor set.
 */
#define LLBC_ClrFd(fd, setPtr) FD_CLR(fd, setPtr)

/**
 * Check specific fd is in set.
 * @param[in] fd     - file descriptor.
 * @param[in] setPtr - file descriptor set.
 */
#define LLBC_FdIsSet(fd, setPtr) FD_ISSET(fd, setPtr)

/**
 * Set specific fd to set.
 * @param[in] fd     - file descriptor.
 * @param[in] setPtr - file descriptor set.
 */
#define LLBC_SetFd(fd, setPtr) FD_SET(fd, setPtr)

/**
 * Reset the file descriptor set.
 * @param[in] setPtr - file descriptor set.
 */
#define LLBC_ZeroFdSet(setPtr) FD_ZERO(setPtr)

/**
 * The select function determines the status of one or more sockets, warning if necessary, to perform synchronous I/O.
 * @param[in] nfds      - highest-numbered file descriptor in any of the three sets, plus 1, in WIN32 platform, ignored.
 * @param[in] readfds   - optional pointer to a set of sockets to be checked for readability.
 * @param[in] writefds  - optional pointer to a set of sockets to be checked for writability.
 * @param[in] exceptfds - optional pointer to a set of sockets to be checked for errors.
 * @param[in] interval  - maximum time for select to wait.
 * @return int - return the total number of socket handles that are ready and contained in the fd_set structures, zero if
 *               timeout, if error occurs, return -1.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_Select(int nfds, LLBC_FdSet *readfds, LLBC_FdSet *writefds, LLBC_FdSet *exceptfds, int interval);

__LLBC_NS_END

#endif // !__LLBC_CORE_OS_OS_SELECT_H__
