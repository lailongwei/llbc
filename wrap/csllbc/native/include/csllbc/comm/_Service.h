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

#pragma once

#include "csllbc/common/Common.h"
#include "csllbc/core/Core.h"

/**
 * Previous declare some classes.
 */
class csllbc_Service;

/**
 * Create new service.
 * @param[in] svcName                  - the service name.
 * @param[in] useNormalProtocolFactory - the service type, see csllbc_Service::Type enumeration.
 * @param[in] fullStack                - the full-stack option.
 * @param[in] ...                      - the delegates, internal used.
 * @return csllbc_Service * - the created service, if failed, return nullptr.
 */
LLBC_EXTERN_C CSLLBC_EXPORT csllbc_Service *csllbc_Service_Create(const char *svcName,
                                                                  bool useNormalProtocolFactory,
                                                                  bool fullStack,
                                                                  csllbc_Delegates::Deleg_Service_EncodePacket encodeDeleg,
                                                                  csllbc_Delegates::Deleg_Service_DecodePacket decodeDeleg,
                                                                  csllbc_Delegates::Deleg_Service_PacketHandler handlerDeleg,
                                                                  csllbc_Delegates::Deleg_Service_PacketPreHandler preHandlerDeleg,
                                                                  csllbc_Delegates::Deleg_Service_PacketUnifyPreHandler unifyPreHandlerDeleg,
                                                                  csllbc_Delegates::Deleg_Service_NativeCouldNotFoundDecoderReport notFoundDecoderDeleg);

/**
 * Delete service.
 * @param[in] svc - will delete service.
 */
LLBC_EXTERN_C CSLLBC_EXPORT void csllbc_Service_Delete(csllbc_Service *svc);

/**
 * Get service Id.
 * @param[in] svc - the service.
 * @return int - the service Id, process uniqueue.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_GetId(csllbc_Service *svc);

/**
 * Get service full-stack option.
 * @param[in] svc - the service.
 * @return int - the full-stack option, 0 if disabled, 1 if enabled.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_IsFullStack(csllbc_Service *svc);

/**
 * Get service drive mode.
 * @param[in] svc - the service.
 * @return int - the service drive mode, see csllbc_Service::DriveMode enumerations.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_GetDriveMode(csllbc_Service *svc);

/**
 * Set service drive mode.
 * @param[in] svc       - the service.
 * @param[in] deiveMode - the service drive mode, see csllbc_Service::DriveMode enumerations.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_SetDriveMode(csllbc_Service *svc, int driveMode);

/**
 * Startup service.
 * @param[in] svc         - the service.
 * @param[in] pollerCount - the worker poller thread count.
 * @param[in] int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_Start(csllbc_Service *svc, int pollerCount);

/**
 * Stop service.
 * @param[in] svc - the service.
 */
LLBC_EXTERN_C CSLLBC_EXPORT void csllbc_Service_Stop(csllbc_Service *svc);

/**
 * Check service is started or not.
 * @param[in] svc - the service.
 * @return int - return non-zero if service started, otherwise return zero.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_IsStarted(csllbc_Service *svc);

/**
 * Get service FPS.
 * @param[in] svc - the service.
 * @return int - the service FPS.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_GetFPS(csllbc_Service *svc);

/**
 * Set service FPS.
 * @param[in] svc - the service.
 * @param[in] fps - the new FPS.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_SetFPS(csllbc_Service *svc, int fps);

/**
 * Get service frame interval, in milliseconds.
 * @param[in] svc - the service.
 * @return int - the service frame interval.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_GetFrameInterval(csllbc_Service *svc);

/**
 * Create a session and listening.
 * @param[in] ip   - the ip address.
 * @param[in] port - the port number.
 * @return int - the new session Id, if return 0, means failed.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_Listen(csllbc_Service *svc, const char *ip, int port);

/**
 * Establishes a connection to a specified address.
 * @param[in] svc  - the service.
 * @param[in] ip   - the ip address.
 * @param[in] port - the port number.
 * @return int - the session Id, if failed, return 0.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_Connect(csllbc_Service *svc, const char *ip, int port);

/**
 * Asynchronous establishes a connection to a specified address.
 * @param[in] svc  - the service.
 * @param[in] ip   - the ip address.
 * @param[in] port - the port number.
 * @return int - return session Id(not yet connected), otherwise return 0.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_AsyncConn(csllbc_Service *svc, const char *ip, int port);

/**
 * Remove session.
 * @param[in] svc       - the service.
 * @param[in] sessionId - will remove sessionId.
 * @param[in] reason    - reason.
 * @param[in] reasonLen - reason length.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_RemoveSession(csllbc_Service *svc, int sessionId, const char *reason, int reasonLen);

/**
 * Check given session Id validate or not.
 * @param[in] svc       - the service.
 * @param[in] sessionId - the session Id.
 * @return int - return non-zero if validate, otherwise return zero.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_IsSessionValidate(csllbc_Service *svc, int sessionId);

/**
 * Common send data function.
 * @param[in] svc       - the service.
 * @param[in] sessionId - the packet opcode.
 * @param[in] opcode    - the packet opcode.
 * @param[in] data      - the will send data pointer.
 * @param[in] dataLen   - the will send data length.
 * @param[in] status    - the status code.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_SendBytes(csllbc_Service *svc,
                                                         int sessionId,
                                                         int opcode,
                                                         const void *data,
                                                         int dataLen,
                                                         int status);

/**
 * Send csharp layer packet.
 * @param[in] svc       - the service.
 * @param[in] sessionId - the packet opcode.
 * @param[in] opcode    - the packet opcode.
 * @param[in] packetId  - the csharp layer created packet Id.
 * @param[in] status    - the status code.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_SendPacket(csllbc_Service *svc,
                                                          int sessionId,
                                                          int opcode,
                                                          sint64 packetId,
                                                          int status);

/**
 * Common multicast data function.
 * @param[in] svc            - the service.
 * @param[in] sessionIds     - the session Ids, this function will Free this memory.
 * @param[in] sessionIdCount - the session Id count.
 * @param[in] opcode         - the packet opcode.
 * @param[in] data           - the will send data pointer.
 * @param[in] dataLen        - the will send data length.
 * @param[in] status         - the status code.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_Multicast(csllbc_Service *svc,
                                                         int *sessionIds,
                                                         int sessionIdCount,
                                                         int opcode,
                                                         const void *data,
                                                         int dataLen,
                                                         int status);

/**
 * Common broadcast data function.
 * @param[in] svc     - the service.
 * @param[in] opcode  - the packet opcode.
 * @param[in] data    - the will send data pointer.
 * @param[in] dataLen - the will send data length.
 * @param[in] status  - the status code.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_Broadcast(csllbc_Service *svc,
                                                         int opcode,
                                                         const void *data,
                                                         int dataLen,
                                                         int status);

/**
 * Add all csharp comp delegates.
 * @param[in] svc       - the service.
 * @param[in] delegates - all comp event handler delegates.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_AddComponent(csllbc_Service *svc,
                                                            csllbc_Delegates::Deleg_Comp_OnInit initDeleg,
                                                            csllbc_Delegates::Deleg_Comp_OnDestroy destroyDeleg,
                                                            csllbc_Delegates::Deleg_Comp_OnStart startDeleg,
                                                            csllbc_Delegates::Deleg_Comp_OnStop stopDeleg,
                                                            csllbc_Delegates::Deleg_Comp_OnUpdate updateDeleg,
                                                            csllbc_Delegates::Deleg_Comp_OnIdle idleDeleg,
                                                            csllbc_Delegates::Deleg_Comp_OnSessionCreate sessionCreateDeleg,
                                                            csllbc_Delegates::Deleg_Comp_OnSessionDestroy sessionDestroyDeleg,
                                                            csllbc_Delegates::Deleg_Comp_OnAsyncConnResult asyncConnResultDeleg,
                                                            csllbc_Delegates::Deleg_Comp_OnProtoReport protoReportDeleg,
                                                            csllbc_Delegates::Deleg_Comp_OnUnHandledPacket unHandledPacketDeleg);

/**
 * Register csharp layer coder.
 * @param[in] svc    - the service.
 * @param[in] opcode - the opcode.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_AddCoder(csllbc_Service *svc, int opcode);

/**
 * Subscribe specific opcode's packet.
 * @param[in] svc    - the service.
 * @param[in] opcode - the opcode.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_Subscribe(csllbc_Service *svc, int opcode);

/**
 * PreSubscribe specific opcode's packet.
 * @param[in] svc    - the service.
 * @param[in] opcode - the opcode.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_PreSubscribe(csllbc_Service *svc, int opcode);

/**
 * Unify presubscribe specific opcode's packet.
 * @param[in] svc    - the service.
 * @param[in] opcode - the opcode.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_UnifyPreSubscribe(csllbc_Service *svc);

/**
 * Service per frame service function.
 * @param[in] svc       - the service.
 * @param[in] fullFrame - full stack flag.
 */
LLBC_EXTERN_C CSLLBC_EXPORT void csllbc_Service_OnSvc(csllbc_Service *svc, bool fullFrame);


