/**
 * @file    AsyncConnInfo.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/09/15
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_ASYNC_CONN_INFO_H__
#define __LLBC_COMM_ASYNC_CONN_INFO_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

__LLBC_NS_BEGIN

/**
 * Previous-declare Socket class.
 */
class LLBC_Socket;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The asynchronous connect info structure encapsulation.
 */
struct LLBC_HIDDEN LLBC_AsyncConnInfo
{
    LLBC_Socket *socket;
    LLBC_SockAddr_IN peerAddr;
    int sessionId;
};

__LLBC_NS_END

#endif // !__LLBC_COMM_ASYNC_CONN_INFO_H__
