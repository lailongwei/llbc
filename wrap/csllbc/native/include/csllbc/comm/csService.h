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
 * Predeclare some classes.
 */
class csllbc_Coder;
class csllbc_Facade;
class csllbc_PacketHandler;
 
/**
 * \brief The csharp wrapped service class encapsulation.
 */
class CSLLBC_HIDDEN csllbc_Service
{
    typedef csllbc_Delegates _D;

public:
    typedef LLBC_IService::Type Type;
    typedef LLBC_IService::DriveMode DriveMode;

public:
    /**
     * Constructor & Destructor.
     */
    csllbc_Service(Type type, 
                   _D::Deleg_Service_EncodePacket encodeDeleg,
                   _D::Deleg_Service_DecodePacket decodeDeleg,
                   _D::Deleg_Service_PacketHandler handlerDeleg,
                   _D::Deleg_Service_PacketPreHandler preHandlerDeleg,
                   _D::Deleg_Service_PacketUnifyPreHandler unifyPreHandlerDeleg,
                   _D::Deleg_Service_NativeCouldNotFoundDecoderReport notFoundDecoderDeleg);
    ~csllbc_Service();

public:
    /**
     * Start service.
     */
    int Start(int pollerCount = 1);
    /**
     * Stop service.
     */
    void Stop();
    /**
     * Check service started or not.
     */
    bool IsStarted() const;

public:
    /**
     * Get service type.
     */
    Type GetType() const;

    /**
     * Get service Id.
     */
    int GetId() const;

    /**
     * Get service FPS.
     */
    int GetFPS() const;
    /**
     * Set service FPS.
     */
    int SetFPS(int fps);
    /**
     * Get service frame interval.
     */
    int GetFrameInterval() const;

public:
    /**
     * Get service drive mode.
     */
    DriveMode GetDriveMode() const;
    /**
     * Set service drive mode.
     */
    int SetDriveMode(DriveMode mode);

public:
    /**
     * Get the csharp layer packet encode delegage.
     */
    _D::Deleg_Service_EncodePacket GetEncodePacketDeleg() const;

public:
    /**
     * Create a new listen session in ip:port.
     */
    int Listen(const char *ip, uint16 port);
    /**
     * Create new session to connect to specified remote endpoint.
     */
    int Connect(const char *ip, uint16 port);
    /**
     * Create new session to asynchronous connect to specified remote endpoint.
     */
    int AsyncConn(const char *ip, uint16 port);

    /**
     * Check given session Id is validate or not.
     */
    bool IsSessionValidate(int sessionId);

    /**
     * Remove specific session.
     */
    int RemoveSession(int sessionId, const char *reason = NULL);

public:
    /**
     * Send data.
     */
    int Send(int sessionId, int opcode, csllbc_Coder *coder, int status = 0);
    int Send(int sessionId, int opcode, const void *bytes, size_t len, int status = 0);
    /**
     * Multicast data.
     */
    int Multicast(const LLBC_SessionIdList &sessionIds, int opcode, const void *bytes, size_t len, int status = 0);
    /**
     * Broadcast data.
     */
    int Broadcast(int opcode, const void *bytes, size_t len, int status = 0);

public:
    /**
     * Register csharp layer facades.
     */
    int RegisterFacade(csllbc_Facade *facade);

    /**
     * Register csharp layer coder.
     */
    int RegisterCoder(int opcode);

    /**
     * Subscribe/PreSubscribe/Unify-PreSubscribe specific opcode's packet.
     */
    int Subscribe(int opcode);
    int PreSubscribe(int opcode);
    int UnifyPreSubscribe();

public:
    /**
     * per-frame service method, call when drivemove == ExternalDrive.
     */
    void OnSvc(bool fullFrame);

public:
    /**
     * \brief The packet decode embedded class encapsulation.
     */
    struct PacketDecodeDelegates
    {
        _D::Deleg_Service_DecodePacket decodeDeleg;
        _D::Deleg_Service_PacketHandler handlerDeleg;
        _D::Deleg_Service_PacketPreHandler preHandlerDeleg;
        _D::Deleg_Service_PacketUnifyPreHandler unifyPreHandlerDeleg;
    };

    /**
     * Add csharp layer all packet decode delegates.
     * @param[in] svcId     - the service Id.
     * @param[in] delegates - the csharp layer all packet decode delegates.
     */
    static void AddPacketDecodeDelegates(int svcId, PacketDecodeDelegates *delegates);

    /**
     * Get all csharp layer packet decode delegates.
     * @param[in] svcId - the service Id.
     * @return PacketDecodeDelegates - the all csharp layer packet decode delegates.
     */
    static PacketDecodeDelegates *GetPacketDecodeDelegates(int svcId);

    /**
     * Remove all csharp layer packet decode delegates.
     * @param[in] svcId - the service Id.
     */
    static void RemovePacketDecodeDelegates(int svcId);

private:
    LLBC_IService *_llbcSvc;
    static int _maxSvcId;
    csllbc_PacketHandler *_packetHandler;

    static LLBC_SpinLock _packetDelegatesLock;
    _D::Deleg_Service_EncodePacket _packetEncodeDeleg;
    typedef std::map<int, PacketDecodeDelegates *> _PacketDecodeDelegs;
    static _PacketDecodeDelegs _packetDecodeDelegs;
};

#endif // !__CSLLBC_COMM_CSSERVICE_H__
