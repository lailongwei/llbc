/**
 * @file    Packet.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2103/06/16
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/PacketHeaderDescAccessor.h"

#include "llbc/comm/ICoder.h"
#include "llbc/comm/Packet.h"

namespace
{
    typedef LLBC_NS LLBC_PacketHeaderDescAccessor _HDAccessor;
}

__LLBC_INTERNAL_NS_BEGIN

static const LLBC_NS LLBC_String __g_dftStatusDesc;

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_Packet::LLBC_Packet()
: _headerDesc(_HDAccessor::GetHeaderDesc())
, _lenSize(_HDAccessor::GetHeaderDesc()->GetLenPartLen())
, _lenOffset(_HDAccessor::GetHeaderDesc()->GetLenPartOffset())

, _sessionId(0)

, _encoder(NULL)
, _decoder(NULL)
#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
, _statusDesc(NULL)
#endif // LLBC_CFG_COMM_ENABLE_STATUS_DESC

, _preHandleResult(NULL)
, _resultClearDeleg(NULL)
{
    const size_t headerLen = _headerDesc->GetHeaderLen();
    _block = new LLBC_MessageBlock(headerLen);
    LLBC_MemSet(_block->GetData(), 0, headerLen);

    _block->SetReadPos(headerLen);
    _block->SetWritePos(headerLen);
}

LLBC_Packet::~LLBC_Packet()
{
    CleanupPreHandleResult();

    LLBC_XDelete(_encoder);
    LLBC_XDelete(_decoder);
#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
    LLBC_XDelete(_statusDesc);
#endif // LLBC_CFG_COMM_ENABLE_STATUS_DESC

    LLBC_XDelete(_block);
}

int LLBC_Packet::GetLength() const
{
    const char *lenBeg =
        reinterpret_cast<const char *>(_block->GetData()) + _lenOffset;

    int len;
    RawGetNonFloatTypeHeaderPartVal(lenBeg, _lenSize, len);

    return len;
}

int LLBC_Packet::GetOpcode() const
{
    if (!_headerDesc->IsHasOpcodePart())
        return 0;

    const size_t opcodeLen = _headerDesc->GetOpcodePartLen();
    const size_t opcodeOffset = _headerDesc->GetOpcodePartOffset();
    
    const char *opcodeBeg = 
        reinterpret_cast<const char *>(_block->GetData()) + opcodeOffset;

    int opcode;
    RawGetNonFloatTypeHeaderPartVal(opcodeBeg, opcodeLen, opcode);

    return opcode;
}

void LLBC_Packet::SetOpcode(int opcode)
{
    if (!_headerDesc->IsHasOpcodePart())
        return;

    const size_t opcodeLen = _headerDesc->GetOpcodePartLen();
    const size_t opcodeOffset = _headerDesc->GetOpcodePartOffset();

    char *opcodeBeg = reinterpret_cast<
        char *>(_block->GetData()) + opcodeOffset;

    RawSetNonFloatTypeHeaderPartVal(opcodeBeg, opcodeLen, opcode);
}

int LLBC_Packet::GetStatus() const
{
    if (!_headerDesc->IsHasStatusPart())
        return 0;

    const size_t statusLen = _headerDesc->GetStatusPartLen();
    const size_t statusOffset = _headerDesc->GetStatusPartOffset();

    const char *statusBeg = 
        reinterpret_cast<const char *>(_block->GetData()) + statusOffset;

    int status;
    RawGetNonFloatTypeHeaderPartVal(statusBeg, statusLen, status);

    return status;
}

void LLBC_Packet::SetStatus(int status)
{
    if (!_headerDesc->IsHasStatusPart())
        return;

    const size_t statusLen = _headerDesc->GetStatusPartLen();
    const size_t statusOffset = _headerDesc->GetStatusPartOffset();

    char *statusBeg = reinterpret_cast<
        char *>(_block->GetData()) + statusOffset;

    RawSetNonFloatTypeHeaderPartVal(statusBeg, statusLen, status);
}

#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
const LLBC_String &LLBC_Packet::GetStatusDesc() const
{
    return (_statusDesc) ? *_statusDesc : LLBC_INL_NS __g_dftStatusDesc;
}

void LLBC_Packet::SetStatusDesc(const LLBC_String &desc)
{
    if (_statusDesc)
        _statusDesc->assign(desc.data(), desc.size());
    else
        _statusDesc = LLBC_New2(LLBC_String, desc.data(), desc.size());
}
#endif // LLBC_CFG_COMM_ENABLE_STATUS_DESC

int LLBC_Packet::GetServiceId() const
{
    if (!_headerDesc->IsHasServiceIdPart())
        return 0;

    const size_t svcIdLen = _headerDesc->GetServiceIdPartLen();
    const size_t svcIdOffset = _headerDesc->GetServiceIdPartOffset();

    const char *svcIdBeg = 
        reinterpret_cast<const char *>(_block->GetData()) + svcIdOffset;

    int svcId;
    RawGetNonFloatTypeHeaderPartVal(svcIdBeg, svcIdLen, svcId);

    return svcId;
}

void LLBC_Packet::SetServiceId(int serviceId)
{
    if (!_headerDesc->IsHasServiceIdPart())
        return;

    const size_t svcIdLen = _headerDesc->GetServiceIdPartLen();
    const size_t svcIdOffset = _headerDesc->GetServiceIdPartOffset();

    char *svcBeg = reinterpret_cast<
        char *>(_block->GetData()) + svcIdOffset;

    RawSetNonFloatTypeHeaderPartVal(svcBeg, svcIdLen, serviceId);
}

int LLBC_Packet::GetFlags() const
{
    if (!_headerDesc->IsHasFlagsPart())
        return 0;

    const size_t flagsLen = _headerDesc->GetFlagsPartLen();
    const size_t flagsOffset = _headerDesc->GetFlagsPartOffset();

    const char *flagsBeg = 
        reinterpret_cast<const char *>(_block->GetData()) + flagsOffset;

    int flags;
    RawGetNonFloatTypeHeaderPartVal(flagsBeg, flagsLen, flags);

    return flags;
}

void LLBC_Packet::SetFlags(int flags)
{
    if (!_headerDesc->IsHasFlagsPart())
        return;

    const size_t flagsLen = _headerDesc->GetFlagsPartLen();
    const size_t flagsOffset = _headerDesc->GetFlagsPartOffset();

    char *flagsBeg = reinterpret_cast<
        char *>(_block->GetData()) + flagsOffset;

    RawSetNonFloatTypeHeaderPartVal(flagsBeg, flagsLen, flags);
}

bool LLBC_Packet::HasFlags(int flags) const
{
    const int packetFlags = GetFlags();
    return (packetFlags & flags) == flags;
}

void LLBC_Packet::AddFlags(int flags)
{
    const int oldFlags = GetFlags();
    SetFlags(oldFlags | flags);
}

void LLBC_Packet::RemoveFlags(int flags)
{
    const int oldFlags = GetFlags();
    SetFlags(oldFlags & (~flags));
}

sint8 LLBC_Packet::GetHeaderPartAsSInt8(int serialNo) const
{
    sint8 val;
    RawGetNonFloatTypeHeaderPartVal<sint8>(serialNo, val);

    return val;
}

uint8 LLBC_Packet::GetHeaderPartAsUInt8(int serialNo) const
{
    return static_cast<uint8>(GetHeaderPartAsSInt8(serialNo));
}

sint16 LLBC_Packet::GetHeaderPartAsSInt16(int serialNo) const
{
    sint16 val;
    RawGetNonFloatTypeHeaderPartVal<sint16>(serialNo, val);

    return val;
}

uint16 LLBC_Packet::GetHeaderPartAsUInt16(int serialNo) const
{
    return static_cast<uint16>(GetHeaderPartAsSInt16(serialNo));
}

sint32 LLBC_Packet::GetHeaderPartAsSInt32(int serialNo) const
{
    sint32 val;
    RawGetNonFloatTypeHeaderPartVal<sint32>(serialNo, val);

    return val;
}

uint32 LLBC_Packet::GetHeaderPartAsUInt32(int serialNo) const
{
    return static_cast<uint32>(GetHeaderPartAsSInt32(serialNo));
}

sint64 LLBC_Packet::GetHeaderPartAsSInt64(int serialNo) const
{
    sint64 val;
    RawGetNonFloatTypeHeaderPartVal<sint64>(serialNo, val);

    return val;
}

uint64 LLBC_Packet::GetHeaderPartAsUInt64(int serialNo) const
{
    return static_cast<uint64>(GetHeaderPartAsSInt64(serialNo));
}

float LLBC_Packet::GetHeaderPartAsFloat(int serialNo) const
{
    float val;
    RawGetFloatTypeHeaderPartVal(serialNo, val);

    return val;
}

double LLBC_Packet::GetHeaderPartAsDouble(int serialNo) const
{
    double val;
    RawGetFloatTypeHeaderPartVal(serialNo, val);

    return val;
}

LLBC_String LLBC_Packet::GetHeaderPartAsStr(int serialNo) const
{
    const LLBC_PacketHeaderPartDesc *
        partDesc = _headerDesc->GetPart(serialNo);
    if (!partDesc)
        return LLBC_String();

    LLBC_String str;
    str.append(reinterpret_cast<const char *>(_block->GetData()) + 
        _headerDesc->GetPartOffset(serialNo), partDesc->GetPartLen());

    return str;
}

const void *LLBC_Packet::GetHeaderPartVal(int serialNo) const
{
    const LLBC_PacketHeaderPartDesc *
        partDesc = _headerDesc->GetPart(serialNo);
    if (!partDesc)
        return NULL;

    return reinterpret_cast<const char *>(
        _block->GetData()) + _headerDesc->GetPartOffset(serialNo);
}

size_t LLBC_Packet::GetHeaderPartLen(int serialNo) const
{
    const LLBC_PacketHeaderPartDesc *
        partDesc = _headerDesc->GetPart(serialNo);

    return (partDesc ? partDesc->GetPartLen() : 0);
}

int LLBC_Packet::SetHeaderPartVal(int serialNo, const void *data, size_t len)
{
    const LLBC_PacketHeaderPartDesc *
        partDesc = _headerDesc->GetPart(serialNo);
    if (!serialNo)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    char *partBeg = const_cast<char *>(reinterpret_cast<
        const char *>(_block->GetData())) + _headerDesc->GetPartOffset(serialNo);

    const size_t partLen = partDesc->GetPartLen();
    if (partLen >= len)
    {
        ::memset(partBeg, 0, partLen);
        ::memcpy(partBeg, data, len);
    }
    else
    {
        ::memcpy(partBeg, data, partLen);
    }
    
    return LLBC_OK;
}

void LLBC_Packet::SetHeader(int sessionId, int opcode, int status)
{
    SetHeader(0, sessionId, opcode, status);
}

void LLBC_Packet::SetHeader(int svcId, int sessionId, int opcode, int status)
{
    SetOpcode(opcode);
    SetStatus(status);

    SetServiceId(svcId);
    SetSessionId(sessionId);
}

void LLBC_Packet::SetHeader(const LLBC_Packet &packet, int opcode, int status)
{
    SetOpcode(opcode);
    SetStatus(status);

    SetServiceId(packet.GetServiceId());
    SetSessionId(packet.GetSessionId());
}

int LLBC_Packet::Read(bool &val)
{
    return _block->Read(&val, sizeof(val));
}

int LLBC_Packet::Read(sint8 &val)
{
    return _block->Read(&val, sizeof(val));
}

int LLBC_Packet::Read(uint8 &val)
{
    return _block->Read(&val, sizeof(val));
}

int LLBC_Packet::Read(sint16 &val)
{
    return ReadRawType<>(val);
}

int LLBC_Packet::Read(uint16 &val)
{
    return ReadRawType<>(val);
}

int LLBC_Packet::Read(sint32 &val)
{
    return ReadRawType<>(val);
}

int LLBC_Packet::Read(uint32 &val)
{
    return ReadRawType<>(val);
}

int LLBC_Packet::Read(long &val)
{
    return ReadRawType<>(val);
}

int LLBC_Packet::Read(ulong &val)
{
    return ReadRawType<>(val);
}

int LLBC_Packet::Read(sint64 &val)
{
    return ReadRawType<>(val);
}

int LLBC_Packet::Read(uint64 &val)
{
    return ReadRawType<>(val);
}

int LLBC_Packet::Read(float &val)
{
    return ReadRawType<>(val);
}

int LLBC_Packet::Read(double &val)
{
    return ReadRawType<>(val);
}

int LLBC_Packet::Read(void *buf, size_t len)
{
    return _block->Read(buf, len);
}

int LLBC_Packet::Write(bool val)
{
    return _block->Write(&val, sizeof(val));
}

int LLBC_Packet::Write(sint8 val)
{
    return _block->Write(&val, sizeof(val));
}

int LLBC_Packet::Write(uint8 val)
{
    return _block->Write(&val, sizeof(val));
}

int LLBC_Packet::Write(sint16 val)
{
    return WriteRawType<>(val);
}

int LLBC_Packet::Write(uint16 val)
{
    return WriteRawType<>(val);
}

int LLBC_Packet::Write(sint32 val)
{
    return WriteRawType<>(val);
}

int LLBC_Packet::Write(uint32 val)
{
    return WriteRawType<>(val);
}

int LLBC_Packet::Write(long val)
{
    return WriteRawType<>(val);
}

int LLBC_Packet::Write(ulong val)
{
    return WriteRawType<>(val);
}

int LLBC_Packet::Write(sint64 val)
{
    return WriteRawType<>(val);
}

int LLBC_Packet::Write(uint64 val)
{
    return WriteRawType<>(val);
}

int LLBC_Packet::Write(float val)
{
    return WriteRawType<>(val);
}

int LLBC_Packet::Write(double val)
{
    return WriteRawType<>(val);
}

int LLBC_Packet::Write(const char *val)
{
    if (LIKELY(val))
    {
        while (*val != '\0')
        {
            Write(val, 1);
            val += 1;
        }
    }

    return Write("\0", 1);
}

int LLBC_Packet::Write(const void *buf, size_t len)
{
    return _block->Write(buf, len);
}

int LLBC_Packet::Write(const LLBC_Stream &stream)
{
    return _block->Write(stream.GetBuf(), stream.GetPos());
}

int LLBC_Packet::WriteHeader(const void *buf)
{
    if (!buf)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    memcpy(_block->GetData(), buf, _headerDesc->GetHeaderLen());

    return LLBC_OK;
}

void *LLBC_Packet::GetPayload() const
{
    return const_cast<char *>(reinterpret_cast<
        const char *>(_block->GetData()) + _headerDesc->GetHeaderLen());
}

size_t LLBC_Packet::GetPayloadLength() const
{
    return _block->GetWritePos() - _headerDesc->GetHeaderLen();
}

LLBC_ICoder *LLBC_Packet::GetEncoder() const
{
    return _encoder;
}

void LLBC_Packet::SetEncoder(LLBC_ICoder *encoder)
{
    LLBC_XDelete(_encoder);
    _encoder = encoder;
}

LLBC_ICoder *LLBC_Packet::GetDecoder() const
{
    return _decoder;
}

void LLBC_Packet::SetDecoder(LLBC_ICoder *decoder)
{
    LLBC_XDelete(_decoder);
    _decoder = decoder;
}

LLBC_MessageBlock *LLBC_Packet::GiveUp()
{
    Encode();

    LLBC_MessageBlock *block = _block;
    block->SetReadPos(0);

    _block = NULL;

    size_t length = block->GetWritePos();
    length -= _headerDesc->GetLenPartNotIncludedLen();

    char *lenBeg = reinterpret_cast<
        char *>(block->GetData()) + _lenOffset;

    RawSetNonFloatTypeHeaderPartVal(lenBeg, _lenSize, length);

    return block;
}

void LLBC_Packet::Encode()
{
    if (_encoder)
    {
        _encoder->Encode(*this);

        delete _encoder;
        _encoder = NULL;
    }
}

void LLBC_Packet::Decode()
{
    if (_decoder)
    {
        _decoder->Decode(*this);
    }
}

void LLBC_Packet::CleanupPreHandleResult()
{
    if (_preHandleResult)
    {
        if (_resultClearDeleg)
        {
            _resultClearDeleg->Invoke(_preHandleResult);
            LLBC_Delete(_resultClearDeleg);
            _resultClearDeleg = NULL;
        }

        _preHandleResult = NULL;
    }
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
