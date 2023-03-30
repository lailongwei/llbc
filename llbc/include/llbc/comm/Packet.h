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

#include "llbc/core/Core.h"

/**
 * Pre-declare some classes.
 */
__LLBC_NS_BEGIN
class LLBC_Coder;
class LLBC_Session;
__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The packet class encapsulation.
 */
class LLBC_EXPORT LLBC_Packet
{
public:
    LLBC_Packet();
    virtual ~LLBC_Packet();

public:
    /**
     * Get packet length.
     * @return size_t - the packet length(included header).
     */
    size_t GetLength() const;
    /**
     * Set packet length.
     * @param[in] length - the packet length.
     */
    void SetLength(size_t length);

    /**
     * Get sender service Id.
     * @return int - sender service Id.
     */
    int GetSenderServiceId() const;
    /**
     * Set sender service Id.
     * @param[in] senderServiceId - sender service Id.
     */
    void SetSenderServiceId(int senderServiceId);

    /**
     * Get receiver service Id.
     * @return int - receiver service Id.
     */
    int GetRecverServiceId() const;
    /**
     * Set receiver service Id.
     * @param[in] recverServiceId - receiver service Id.
     */
    void SetRecverServiceId(int recverServiceId);

    /**
     * Get session Id.
     * @return int - the session Id.
     */
    int GetSessionId() const;
    /**
     * Set session Id.
     * @param[in] sessionId - the session Id.
     */
    void SetSessionId(int sessionId);

    /**
     * Get accept session Id.
     * @return int - the accept session Id.
     */
    int GetAcceptSessionId() const;

    /**
     * Set accept session Id.
     * @param[in] sessionId - the accept session Id.
     */
    void SetAcceptSessionId(int sessionId);

    /**
     * Get local address.
     * @return const LLBC_SockAddr_IN & - the local address.
     */
    const LLBC_SockAddr_IN &GetLocalAddr() const;
    /**
     * Set local address.
     * @param[in] addr - the local address.
     */
    void SetLocalAddr(const LLBC_SockAddr_IN &addr);

    /**
     * Get peer address.
     * @return const LLBC_SockAddr_IN & - the peer address.
     */
    const LLBC_SockAddr_IN &GetPeerAddr() const;
    /**
     * Set peer address.
     * @param[in] addr - the peer address.
     */
    void SetPeerAddr(const LLBC_SockAddr_IN &addr);

public:
    /**
     * Get opcode.
     * @return int - opcode.
     */
    int GetOpcode() const;
    /**
     * Set opcode.
     * @param[in] opcode - opcode.
     */
    void SetOpcode(int opcode);

    /**
     * Get status code.
     * @return int - status code.
     */
    int GetStatus() const;
    /**
     * Set status code.
     * @param[in] status - status code.
     */
    void SetStatus(int status);

#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
    /**
     * Get the status describe.
     * @return const LLBC_String & - the status describe.
     */
    const LLBC_String &GetStatusDesc() const;

    /**
     * Set the status describe.
     * @param[in] desc - the status describe.
     */
    void SetStatusDesc(const LLBC_String &desc);
#endif // LLBC_CFG_COMM_ENABLE_STATUS_DESC

public:
    /**
     * Get packet flags.
     * @return int - the packet flags.
     */
    int GetFlags() const;
    /**
     * Set packet flags.
     * @param[in] flags - the packet falgs.
     */
    void SetFlags(int flags);

    /**
     * Check this packet already set specified flags or not.
     * @param[in] flags - the packet flags.
     * @return bool - return true if has specified flags, otherwise return return false.
     */
    bool HasFlags(int flags) const;

    /**
     * Add flags to packet.
     * @param[in] flags - the packet flags.
     */
    void AddFlags(int flags);

    /**
     * Remove flags from packet.
     * @param[in] flags - the packet flags.
     */
    void RemoveFlags(int flags);

public:
    /**
     * Get packet first extention data.
     * @return sint64 - the first extension data.
     */
    const sint64 &GetExtData1() const;
    /**
     * Set packet first extension data.
     * @param[in] extData1 - the first extension data.
     */
    void SetExtData1(const sint64 &extData1);

    /**
     * Get packet second extension data.
     * @return sint64 - the second extension data.
     */
    const sint64 &GetExtData2() const;
    /**
     * Set packet second extension data.
     * @param[in] extData2 - the second extension data.
     */
    void SetExtData2(const sint64 &extData2);

    /**
     * Get packet third extension data.
     * @return sint64 - the third extension data.
     */
    const sint64 &GetExtData3() const;
    /**
     * Set packet third extension data.
     * @param[in] extData3 - the third extension data.
     */
    void SetExtData3(const sint64 &extData3);
public:
    /**
     * Set packet header.
     * @param[in] sessionId - session Id.
     * @param[in] opcode    - the opcode.
     * @param[in] status    - the status code.
     */
    void SetHeader(int sessionId, int opcode, int status);
    void SetHeader(int svcId, int sessionId, int opcode, int status);

    /**
     * Set packet header.
     * @param[in] packet - packet.
     * @param[in] opcode - the opcode.
     * @param[in] status - the status code.
     */
    void SetHeader(const LLBC_Packet &packet, int opcode, int status);

public:
    /**
     * Get payload data.
     * @return const void * - payload data pointer.
     */
    const void *GetPayload() const;

    /**
     * Get payload length.
     * @return size_t - payload length.
     */
    size_t GetPayloadLength() const;

    /**
     * Get mutable payload(message block object pointer).
     * @return LLBC_MessageBlock * - the mutable payload.
     */
    LLBC_MessageBlock *GetMutablePayload();

    /**
     * Detach payload.
     * @return LLBC_MessageBlock * - the payload.
     */
    LLBC_MessageBlock *DetachPayload();

    /**
    * Set payload data.
    * @param[in] payload - the new payload.
    */
    void SetPayload(LLBC_MessageBlock *payload);

    /**
     * Set payload delete delegate.
     * @param[in] deleg - the payload delete delegate.
     */
    void SetPayloadDeleteDeleg(const LLBC_Delegate<void(LLBC_MessageBlock *)> &deleg);

    /**
     * Reset packet payload.
     */
    void ResetPayload();

public:
    /**
     * Object-Pool reflection support: Mark pool object.
     */
    void MarkPoolObject(LLBC_IObjectPoolInst &poolInst);

    /**
     * Object-Pool reflection support: Get pool instance.
     */
    LLBC_IObjectPoolInst *GetPoolInst();

    /**
     * Object-Pool reflection support, get user-defined per-block units number.
     */
    size_t GetPoolInstPerBlockUnitsNum();

    /**
     * Object-Pool reflection support: pool instance create event callback.
     */
    void OnPoolInstCreate(LLBC_IObjectPoolInst &poolInst);

    /**
     * Object-Pool reflection support: Clear message block, this operation will clear read&write position information.
     */
    void Clear();

public:
    /**
     * Raw data type read methods.
     * @param[out] val - storage location for data.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Read(bool &val);
    int Read(sint8 &val);
    int Read(uint8 &val);
    int Read(sint16 &val);
    int Read(uint16 &val);
    int Read(sint32 &val);
    int Read(uint32 &val);
    int Read(long &val);
    int Read(ulong &val);
    int Read(sint64 &val);
    int Read(uint64 &val);
    int Read(float &val);
    int Read(double &val);

    /**
     * Read data from packet.
     * @param[out] buf - storage location for data.
     * @param[in]  len - buffer len.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Read(void *buf, size_t len);

    /**
     * STL container adapt read functions.
     * @param[out] val - container.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename _Ty>
    int Read(std::vector<_Ty> &val);
    template <typename _Ty>
    int Read(std::list<_Ty> &val);
    template <typename _Ty>
    int Read(std::deque<_Ty> &val);
    template <typename _Kty>
    int Read(std::set<_Kty> &val);
    template <typename _Kty, typename _Ty>
    int Read(std::map<_Kty, _Ty> &val);

    /**
     * Read object from packet.
     * @param[out] val - the object reference.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename _Ty>
    int Read(_Ty &val);

    /**
     * Raw data type write methods.
     * @param[in] val - will write to packet's object.
     * @return int - return 0 if success, otherwise retrun -1.
     */
    int Write(bool val);
    int Write(sint8 val);
    int Write(uint8 val);
    int Write(sint16 val);
    int Write(uint16 val);
    int Write(sint32 val);
    int Write(uint32 val);
    int Write(long val);
    int Write(ulong val);
    int Write(sint64 val);
    int Write(uint64 val);
    int Write(float val);
    int Write(double val);
    int Write(const char *val);

    /**
     * Write data to packet.
     * @param[in] buf - buffer to write from.
     * @param[in] len - number of bytes to write.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Write(const void *buf, size_t len);

    /**
     * Write stream type data to packet.
     * @param[in] stream - stream.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Write(const LLBC_Stream &stream);

    /**
     * STL container adapt write functions.
     * @param[in] val - container.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename _Ty>
    int Write(const std::vector<_Ty> &val);
    template <typename _Ty>
    int Write(const std::list<_Ty> &val);
    template <typename _Ty>
    int Write(const std::deque<_Ty> &val);
    template <typename _Kty>
    int Write(const std::set<_Kty> &val);
    template <typename _Kty, typename _Ty>
    int Write(const std::map<_Kty, _Ty> &val);

    /**
     * Write object to packet.
     * @param[in] obj - the object.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename _Ty>
    int Write(const _Ty &obj);

public:
    /**
     * stream output operations.
     */
    template <typename _Ty>
    LLBC_Packet &operator <<(const _Ty &val);

    /**
     * Stream input operations.
     */
    template <typename _Ty>
    LLBC_Packet &operator >>(_Ty &val);

public:
    /**
     * Get encoder.
     * @return LLBC_Coder * - encoder.
     */
    LLBC_Coder *GetEncoder() const;
    template <typename CoderType>
    CoderType *GetEncoder() const;

    /**
     * Set encoder.
     * @param[in] encoder - encoder.
     */
    void SetEncoder(LLBC_Coder *encoder);

    /**
     * Give up encoder.
     * @return LLBC_Coder * - the already give up encoder object pointer.
     */
    LLBC_Coder *GiveUpEncoder();

    /**
     * Get decoder.
     * @return LLBC_Coder * - decoder.
     */
    LLBC_Coder *GetDecoder() const;
    template <typename CoderType>
    CoderType *GetDecoder() const;

    /**
     * Set decoder.
     * @param[in] decoder - decoder.
     */
    void SetDecoder(LLBC_Coder *decoder);

    /**
     * Give up decoder.
     * @return[in] LLBC_Coder * - the already give up decoder object pointer.
     */
    LLBC_Coder *GiveUpDecoder();

    /**
     * Encode packet data.
     */
    bool Encode();

    /**
     * Decode packet data.
     */
    bool Decode();

    /**
     * Giveup payload.
     * @return LLBC_MessageBlock * - payload.
     */
    LLBC_MessageBlock *GiveUpPayload();

public:
    /**
     * Get the pre-handle result in the packet.
     * @return void * - the packet pre-handle result.
     */
    void *GetPreHandleResult() const;
    template<typename RetType>
    RetType *GetPreHandleResult() const;

    /**
     * Set the pre-handle result into the packet.
     * @param[in] result     - the packet pre-handle result.
     * @param[in] clearDeleg - the result clear method, default is nullptr.
     */
    void SetPreHandleResult(void *result, void(*clearFunc)(void *));
    template <typename ObjType>
    void SetPreHandleResult(void *result, ObjType *obj, void (ObjType::*clearMethod)(void *));
    void SetPreHandleResult(void *result, const LLBC_Delegate<void(void *)> &clearDeleg = nullptr);

public:
    /**
     * Get packet codec error.
     * @return const LLBC_String & - the packet codec error.
     */
    const LLBC_String &GetCodecError() const;

    /**
     * Set packet codec error.
     * @param[in] codecErr - the codec error.
     */
    void SetCodecError(const LLBC_String &codecErr);

public:
    /**
     * Get packet string representation.
     * @return LLBC_String - the packet string representation.
     */
    LLBC_String ToString() const;

private:
    /**
     * Read raw type data from packet.
     * @param[out] val - the value reference.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename _RawTy>
    int ReadRawType(_RawTy &val);

    /**
     * Write raw type data to packet.
     * @param[in] val - value.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename _RawTy>
    int WriteRawType(_RawTy val);

private:
    /**
     * Check and create payload(if payload not exist).
     * @param[in] initSize - the messageBlock init size(if need create, will use this parameter).
     * @return LLBC_MessageBlock *& - the payload pointer reference.
     */
    LLBC_MessageBlock *&CheckAndCreatePayload(size_t initSize);

private:
    /**
     * Cleanup the pre-handle result data.
     */
    void CleanupPreHandleResult();

    /**
     * Cleanup payload.
     */
    void CleanupPayload();

private:
    size_t _length;

    int _sessionId;
    int _acceptSessionId;
    int _senderSvcId;
    int _recverSvcId;
    LLBC_SockAddr_IN _localAddr;
    LLBC_SockAddr_IN _peerAddr;

    int _opcode;
    int _status;
#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
    LLBC_String *_statusDesc;
#endif // LLBC_CFG_COMM_ENABLE_STATUS_DESC
    int _flags;
    sint64 _extData1;
    sint64 _extData2;
    sint64 _extData3;

    LLBC_Coder *_encoder;
    LLBC_Coder *_decoder;
    LLBC_String *_codecError;

    void *_preHandleResult;
    LLBC_Delegate<void(void *)> _resultClearDeleg;

    LLBC_MessageBlock *_payload;
    LLBC_Delegate<void(LLBC_MessageBlock *)> _payloadDeleteDeleg;

    LLBC_IObjectPoolInst *_selfPoolInst;
    LLBC_IObjectPoolInst *_msgBlockPoolInst;
};

__LLBC_NS_END

/**
 * Stream output operator support. 
 */
std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_Packet &packet);

#include "llbc/comm/PacketInl.h"


