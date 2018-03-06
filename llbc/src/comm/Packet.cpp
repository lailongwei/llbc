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


#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/ICoder.h"
#include "llbc/comm/Packet.h"

__LLBC_INTERNAL_NS_BEGIN

static const LLBC_NS LLBC_String __g_dftStatusDesc;

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_Packet::LLBC_Packet()
: _length(0)

, _sessionId(0)
, _senderSvcId(0)
, _recverSvcId(0)
, _localAddr()
, _peerAddr()

, _opcode(0)
, _status(0)
#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
, _statusDesc(NULL)
#endif // LLBC_CFG_COMM_ENABLE_STATUS_DESC
, _flags(0)
, _extData1(0)
, _extData2(0)
, _extData3(0)

, _encoder(NULL)
, _decoder(NULL)
, _codecError(NULL)

, _preHandleResult(NULL)
, _resultClearDeleg(NULL)

, _payload(NULL)
{
}

LLBC_Packet::~LLBC_Packet()
{
    CleanupPreHandleResult();

    LLBC_XDelete(_payload);

    LLBC_XDelete(_encoder);
    LLBC_XDelete(_decoder);
    LLBC_XDelete(_codecError);

#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
    LLBC_XDelete(_statusDesc);
#endif // LLBC_CFG_COMM_ENABLE_STATUS_DESC
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

const void *LLBC_Packet::GetPayload() const
{
    if (!_payload)
        return NULL;

    return _payload->GetData();
}

size_t LLBC_Packet::GetPayloadLength() const
{
    if (!_payload)
        return NULL;

    return _payload->GetWritePos();
}

int LLBC_Packet::Read(bool &val)
{
    return ReadRawType<>(val);
}

int LLBC_Packet::Read(sint8 &val)
{
    return ReadRawType<>(val);
}

int LLBC_Packet::Read(uint8 &val)
{
    return ReadRawType<>(val);
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
    if (!_payload)
    {
        LLBC_SetLastError(LLBC_ERROR_LIMIT);
        return LLBC_FAILED;
    }

    return _payload->Read(buf, len);
}

int LLBC_Packet::Write(bool val)
{
    return WriteRawType<>(val);
}

int LLBC_Packet::Write(sint8 val)
{
    return WriteRawType<>(val);
}

int LLBC_Packet::Write(uint8 val)
{
    return WriteRawType<>(val);
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
    if (!_payload)
        _payload = new LLBC_MessageBlock(len);
    return _payload->Write(buf, len);
}

int LLBC_Packet::Write(const LLBC_Stream &stream)
{
    if (!_payload)
        _payload = new LLBC_MessageBlock(stream.GetPos());

    return _payload->Write(stream.GetBuf(), stream.GetPos());
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

LLBC_ICoder *LLBC_Packet::GiveUpEncoder()
{
    LLBC_ICoder *encoder = _encoder;
    _encoder = NULL;

    return encoder;
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

LLBC_ICoder *LLBC_Packet::GiveUpDecoder()
{
    LLBC_ICoder *decoder = _decoder;
    _decoder = NULL;

    return decoder;
}

bool LLBC_Packet::Encode()
{
    if (_encoder)
    {
        if (!_encoder->Encode(*this))
            return false;

        delete _encoder;
        _encoder = NULL;
    }

    return true;
}

bool LLBC_Packet::Decode()
{
    if (_decoder)
    {
        return _decoder->Decode(*this);
    }

    return true;
}

LLBC_MessageBlock *LLBC_Packet::GiveUp()
{
    Encode();
    if (!_payload)
        return NULL;

    LLBC_MessageBlock *block = _payload;
    _payload = NULL;

    return block;
}

void *LLBC_Packet::GetPreHandleResult() const
{
    return _preHandleResult;
}

void LLBC_Packet::SetPreHandleResult(void *result, LLBC_IDelegate1<void, void *> *clearDeleg)
{
    this->CleanupPreHandleResult();
    if ((_preHandleResult = result))
        if (clearDeleg)
            _resultClearDeleg = clearDeleg;
}

const LLBC_String &LLBC_Packet::GetCodecError() const
{
    static const LLBC_String noError;
    return _codecError ? *_codecError : noError;
}

void LLBC_Packet::SetCodecError(const LLBC_String &codecErr)
{
    if (_codecError)
        LLBC_Delete(_codecError);

    _codecError = new LLBC_String(codecErr.c_str(), codecErr.length());
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
