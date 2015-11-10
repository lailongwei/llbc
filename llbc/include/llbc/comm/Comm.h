/**
 * @file    Comm.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    214/11/24
 * @version 1.0
 * 
 * @brief
 */
#ifndef __LLBC_COMM_COMM_H__
#define __LLBC_COMM_COMM_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

#include "llbc/comm/Socket.h"
#include "llbc/comm/Session.h"
#include "llbc/comm/Packet.h"
#include "llbc/comm/ICoder.h"
#include "llbc/comm/IFacade.h"
#include "llbc/comm/PollerType.h"
#include "llbc/comm/BasePoller.h"
#include "llbc/comm/IService.h"
#include "llbc/comm/ServiceMgr.h"
#include "llbc/comm/PacketHeaderParts.h"
#include "llbc/comm/LibPacketHeaderDescFactory.h"
#include "llbc/comm/protocol/ProtocolLayer.h"
#include "llbc/comm/protocol/ProtoReportLevel.h"
#include "llbc/comm/protocol/IProtocol.h"
#include "llbc/comm/protocol/IProtocolFilter.h"
#include "llbc/comm/headerdesc/PacketHeaderDesc.h"

__LLBC_NS_BEGIN

/**
 * LLBC library Communication-Module startup function, before use any
 * this module's functions or classes, must call this function to initialize
 * the module.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN LLBC_HIDDEN int __LLBC_CommStartup();

/**
 * LLBC library Communication-Module cleanup function.
 */
LLBC_EXTERN LLBC_HIDDEN void __LLBC_CommCleanup();

__LLBC_NS_END

#endif // !__LLBC_COMM_COMM_H__

