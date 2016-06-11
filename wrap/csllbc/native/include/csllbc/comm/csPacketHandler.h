/**
 * @file    csPacketHandler.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/04/23
 * @version 1.0
 *
 * @brief
 */
#ifndef __CSLLBC_COMM_CSPACKET_HANDLER_H__
#define __CSLLBC_COMM_CSPACKET_HANDLER_H__

#include "csllbc/common/Common.h"
#include "csllbc/core/Core.h"

class CSLLBC_HIDDEN csllbc_PacketHandler
{
    typedef csllbc_Delegates::Deleg_Service_NativeCouldNotFoundDecoderReport _NotFoundDecoderReport;

    /**
     * The csharp packet handle phase enumerations.
     */
    enum
    {
        Phase_UnifyPreHandle, // packet in unify pre-handling
        Phase_PreHandle, // packet in pre-handling
        Phase_Handle, // packet in handling
    };

public:
    /**
     * Constructor.
     * @param[in] notFoundDecoderReport - not found decoder report delegate, use when decoder not found.
     */
    csllbc_PacketHandler(_NotFoundDecoderReport notFoundDecoderReport);

public:
    /**
    * The csharp packet handler.
    */
    void Handle(LLBC_Packet &packet);

    /**
    * The csharp packet pre-handler.
    */
    void *PreHandle(LLBC_Packet &packet);

    /**
    * The csharp packet unify-prehandler.
    */
    void *UnifyPreHandle(LLBC_Packet &packet);

private:
    /**
     * Report to csharp layer packet decoder not found.
     * @param[in] phase  - the phase.
     * @param[in] packet - the packet.
     */
    void ReportNotFoundDecoder(int phase, LLBC_Packet &packet);

private:
    _NotFoundDecoderReport _notFoundDecoderReport;
};

#endif // !__CSLLBC_COMM_CSPACKET_HANDLER_H__
