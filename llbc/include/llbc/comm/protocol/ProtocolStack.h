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

#include "llbc/comm/protocol/IProtocol.h"
#include "llbc/comm/protocol/ProtocolLayer.h"

__LLBC_NS_BEGIN

/**
 * Previous declare some classes.
 */
class LLBC_Packet;
class LLBC_IProtocolFilter;
class LLBC_CoderFactory;
class LLBC_Session;
class LLBC_Service;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The protocol layer class encapsulation.
 */
class LLBC_EXPORT LLBC_ProtocolStack
{
public:
    /**
     * The stack type enumeration.
     */
    enum StackType
    {
        PackStack,
        CodecStack,

        FullStack
    };

public:
    /**
     * Create protocol stack by given type.
     * @param[in] type - the stack type.
     */
    LLBC_ProtocolStack(StackType type);
    /**
     * Destructor.
     */
    ~LLBC_ProtocolStack();

public:
    /**
     * Get service.
     * @return LLBC_Service * - the service.
     */
    LLBC_Service *GetService();

    /**
     * Set service to protocol stack, use to report something to service.
     * @param[in] svc - the service.
     */
    void SetService(LLBC_Service *svc);

    /**
     * Get session.
     * @return LLBC_Session * - the session.
     */
    LLBC_Session *GetSession();

    /**
     * Set session to protocol stack, use to report something to service.
     * @param[in] session - the session.
     */
    void SetSession(LLBC_Session *session);

    /**
     * Check is suppressed coder not found warning or not.
     * @return bool - the suppressed coder not found warning flag.
     */
    bool GetIsSuppressedCoderNotFoundWarning() const;

    /**
     * Set suppressed coder not found warning option to protocol-stack.
     * @param[in] suppressed - the suppressed flag.
     */
    void SetIsSuppressedCoderNotFoundWarning(bool suppressed);

public:
    /**
     * Set protocol to protocol stack.
     * @param[in] proto - the protocol.
     * @return int - return 0 if success, otherwise reutrn -1.
     */
    int AddProtocol(LLBC_IProtocol *proto);

    /**
     * Set coder factories to protocol stack.
     * @param[in] coders - the coder factories.
     *                    Warn: The protocol stack will not delete coder object,
     *                          Just simple hold it pointer.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetCoders(const LLBC_IProtocol::Coders *coders);

    /**
    * Get CodecLayer protocol instance.
    * @return LLBC_IProtocol * - return CodecLayer instance ptr.
    */
    const LLBC_IProtocol *GetCoderProtocol() const;

    /**
     * Set protocol filter to specified layer protocol.
     * @param[in] filter  - the protocol filter.
     * @param[in] toProto - protocol layer.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetFilter(LLBC_IProtocolFilter *filter, int toProto);

public:
    /**
     * When packet send, will use this protocol stack method to filter and encode packet.
     * @param[in] willEncode     - the will send packet.
     * @param[in] encoded        - the filtered and encoded packet. 
     * @param[out] removeSession - when error occurred, this out param determine remove session or not.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SendCodec(LLBC_Packet *willEncode, LLBC_Packet *&encoded, bool &removeSession);
    
    /**
     * When packet send, will use this protocol stack method to compress packet and convert to message-block to send.
     * @param[in] packet         - the will compress and convert packet.
     * @param[in] block          - the message block.
     * @param[out] removeSession - when error occurred, this out param determine remove session or not.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SendRaw(LLBC_Packet *packet, LLBC_MessageBlock *&block, bool &removeSession);

    /**
     * When packet send, will use this protocol stack method to convert packet to message-block type to send.
     * @param[in] packet         - the packet.
     * @param[in] block          - the message block.
     * @param[out] removeSession - when error occurred, this out param determine remove session or not.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Send(LLBC_Packet *packet, LLBC_MessageBlock *&block, bool &removeSession);

    /**
     * When message receive, will use this protocol stack method to convert message-block to undecoded.
     * @param[in] block          - the message block.
     * @param[in] packets        - the packets.
     * @param[out] removeSession - when error occurred, this out param determine remove session or not.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RecvRaw(LLBC_MessageBlock *block, std::vector<LLBC_Packet *> &packets, bool &removeSession);

    /**
     * When packet receive, will use this protocol stack method to decode and filter.
     * @param[in] willDecode     - the will decode and filter packet.
     * @param[in] decoded        - the decoded and filtered packet.
     * @param[out] removeSession - when error occurred, this out param determine remove session or not.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RecvCodec(LLBC_Packet *willDecode, LLBC_Packet *&decoded, bool &removeSession);

    /**
     * When packet recv, will use this protocol stack method to convert message-block type to packets.
     * @param[in] block          - the message block.
     * @param[in] packets        - the converted packet list.
     * @param[out] removeSession - when error occurred, this out param determine remove session or not.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Recv(LLBC_MessageBlock *block, std::vector<LLBC_Packet *> &packets, bool &removeSession);

public:
    /**
     * Report error, call by protocol.
     * @param[in] sessionId - the session Id.
     * @param[in] opcode    - the opcode.
     * @param[in] proto     - the reporter.
     * @param[in] level     - the report level.
     * @param[in] msg       - the report message.
     */
    void Report(LLBC_IProtocol *proto, int level, const LLBC_String &msg);
    void Report(int sessionId, LLBC_IProtocol *proto, int level, const LLBC_String &msg);
    void Report(int sessionId, int opcode, LLBC_IProtocol *proto, int level, const LLBC_String &msg);

public:
    /**
     * Control protocol stack.
     * @param[in] cmd           - the stack control command(user defined).
     * @param[in] ctrlData      - the stack control data(user defined).
     * @param[in] removeSession - when error occurred, this out param determine remove session or not,
     *                            only used when return false.
     */
    void CtrlStack(int cmd, const LLBC_Variant &ctrlData, bool &removeSession);
    
    /**
     * Control raw-part protocol stack.
     * @param[in] cmd           - the stack control command(user defined).
     * @param[in] ctrlData      - the stack control data(user defined).
     * @param[in] removeSession - when error occurred, this out param determine remove session or not,
     *                            only used when return false.
     */
    bool CtrlStackRaw(int cmd, const LLBC_Variant &ctrlData, bool &removeSession);

    /**
     * Control codec-part protocol stack.
     * @param[in] cmd           - the stack control command(user defined).
     * @param[in] ctrlData      - the stack control data(user defined).
     * @param[in] removeSession - when error occurred, this out param determine remove session or not,
     *                            only used when return false.
     */
    bool CtrlStackCodec(int cmd, const LLBC_Variant &ctrlData, bool &removeSession);

private:
    StackType _type;

    LLBC_Service *_svc;
    LLBC_Session *_session;
    bool _suppressCoderNotFoundError;

    LLBC_IProtocol *_protos[LLBC_ProtocolLayer::End];

    std::vector<LLBC_Packet *> _rawPackets;
};

__LLBC_NS_END
