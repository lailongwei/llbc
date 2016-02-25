/**
 * @file    csService.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/01/25
 * @version 1.0
 *
 * @brief
 */
#ifndef __CSLLBC_COMM_CSSERVICE_H__
#define __CSLLBC_COMM_CSSERVICE_H__

#include "csllbc/common/Common.h"
#include "csllbc/core/Core.h"

/**
 * Create new service.
 * @param[in] svcType - the service type, see LLBC_IService::Type enumeration.
 * @return LLBC_IService * - the created service, if failed, return NULL.
 */
LLBC_EXTERN_C CSLLBC_EXPORT LLBC_IService *csllbc_Service_Create(int svcType);

/**
 * Delete service.
 * @param[in] svc - will delete service.
 */
LLBC_EXTERN_C CSLLBC_EXPORT void csllbc_Service_Delete(LLBC_IService *svc);

/**
 * Get service Id.
 * @param[in] svc - the service.
 * @return int - the service Id.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_GetId(LLBC_IService *svc);
/**
 * Set service Id.
 * @param[in] svc   - the service.
 * @param[in] svcId - the service Id.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_SetId(LLBC_IService *svc, int svcId);

/**
 * Get service type.
 * @param[in] svc - the service.
 * @return int - the service type, see LLBC_IService::Type enumerations.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_GetType(LLBC_IService *svc);

/**
 * Set service drive mode.
 * @param[in] svc       - the service.
 * @param[in] deiveMode - the service drive mode, see LLBC_IService::DriveMode enumerations.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int SetDriveMode(int driveMode);

/**
 * Startup service.
 * @param[in] svc         - the service.
 * @param[in] pollerCount - the worker poller thread count.
 * @param[in] int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_Start(LLBC_IService *svc, int pollerCount);

/**
 * Stop service.
 * @param[in] svc - the service.
 */
LLBC_EXTERN_C CSLLBC_EXPORT void csllbc_Service_Stop(LLBC_IService *svc);

/**
 * Check service is started or not.
 * @param[in] svc - the service.
 * @return bool - return ture if service started, otherwise return false.
 */
LLBC_EXTERN_C CSLLBC_EXPORT bool csllbc_Service_IsStarted(LLBC_IService *svc);

/**
 * Get service FPS.
 * @param[in] svc - the service.
 * @return int - the service FPS.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_GetFPS(LLBC_IService *svc);

/**
 * Set service FPS.
 * @param[in] svc - the service.
 * @param[in] fps - the new FPS.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_SetFPS(LLBC_IService *svc, int fps);

/**
 * Get service frame interval, in milliseconds.
 * @param[in] svc - the service.
 * @return int - the service frame interval.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_GetFrameInterval(LLBC_IService *svc);

/**
 * Create a session and listening.
 * @param[in] ip   - the ip address.
 * @param[in] port - the port number.
 * @return int - the new session Id, if return 0, means failed.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_Listen(LLBC_IService *svc, const char *ip, int port);

/**
 * Establishes a connection to a specified address.
 * @param[in] svc  - the service.
 * @param[in] ip   - the ip address.
 * @param[in] port - the port number.
 * @return int - the session Id, if failed, return 0.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_Connect(LLBC_IService *svc, const char *ip, int port);

/**
 * Asynchronous establishes a connection to a specified address.
 * @param[in] svc  - the service.
 * @param[in] ip   - the ip address.
 * @param[in] port - the port number.
 * @return int - return 0 if post success, otherwise return -1.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Service_AsyncConn(LLBC_IService *svc, const char *ip, int port);

#endif // !__CSLLBC_COMM_CSSERVICE_H__
