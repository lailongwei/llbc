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
static const char *__g_msgBlockTypeName = typeid(LLBC_NS LLBC_MessageBlock).name();

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_Packet::LLBC_Packet()
: _length(0)

, _sessionId(0)
, _acceptSessionId(0)
, _senderSvcId(0)
, _recverSvcId(0)

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
, _payloadDeleteDeleg(NULL)
, _deletePayloadDeleteDelegWhenDestroy(false)

, _selfPoolInst(NULL)
, _msgBlockPoolInst(NULL)
{
}

LLBC_Packet::~LLBC_Packet()
{
    CleanupPayload();
    CleanupPayloadDeleteDeleg();

    CleanupPreHandleResult();

    LLBC_XRecycle(_encoder);
    LLBC_XRecycle(_decoder);
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
        _statusDesc = LLBC_New(LLBC_String, desc.data(), desc.size());
}
#endif // LLBC_CFG_COMM_ENABLE_STATUS_DESC

void LLBC_Packet::SetPayloadDeleteDeleg(LLBC_IDelegate1<void, LLBC_MessageBlock *> *deleg, bool deleteWhenPacketDestroy)
{
    if (_payloadDeleteDeleg == deleg)
    {
        _deletePayloadDeleteDelegWhenDestroy = deleteWhenPacketDestroy;
        return;
    }

    if (_payloadDeleteDeleg && _deletePayloadDeleteDelegWhenDestroy)
    {
        LLBC_Delete(_payloadDeleteDeleg);
        _deletePayloadDeleteDelegWhenDestroy = false;
    }

    if ((_payloadDeleteDeleg = deleg))
        _deletePayloadDeleteDelegWhenDestroy = deleteWhenPacketDestroy;
}

void LLBC_Packet::MarkPoolObject(LLBC_IObjectPoolInst &poolInst)
{
    _selfPoolInst = &poolInst;
    _msgBlockPoolInst = poolInst.GetIObjectPool()->GetIPoolInst(LLBC_INL_NS __g_msgBlockTypeName);
}

size_t LLBC_Packet::GetPoolInstPerBlockUnitsNum()
{
    return LLBC_CFG_CORE_OBJECT_POOL_PACKET_UNITS_NUMBER;
}

void LLBC_Packet::OnPoolInstCreate(LLBC_IObjectPoolInst &poolInst)
{
    // Set delete order.
    LLBC_IObjectPool *objPool = poolInst.GetIObjectPool();
    objPool->AcquireOrderedDeletePoolInst(typeid(LLBC_Packet).name(), LLBC_INL_NS __g_msgBlockTypeName);

    // Force create dependented pool instances(MessageBlock/...)
    (void)objPool->GetIPoolInst(LLBC_INL_NS __g_msgBlockTypeName);
}

void LLBC_Packet::Clear()
{
    // Clear payload.
    if (_payload)
    {
        if (_payloadDeleteDeleg)
            _payloadDeleteDeleg->Invoke(_payload);

        if (_payload->IsPoolObject())
        {
            _payload->GiveBackToPool();
            _payload = NULL;
        }
        else
        {
            _payload->Clear();
        }
    }

    // Clear payload delete delegate.
    CleanupPayloadDeleteDeleg();

    // Clear encoder/decoder
    LLBC_XRecycle(_encoder);
    LLBC_XRecycle(_decoder);

    // Reset some normal members.
    _length = 0;
    _sessionId = 0;
    _acceptSessionId = 0;
    _senderSvcId = 0;
    _recverSvcId = 0;
    _localAddr.SetIp(0); _localAddr.SetPort(0);
    _peerAddr.SetIp(0); _peerAddr.SetPort(0);

    _opcode = 0;
    _status = 0;
#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
    if (_statusDesc)
        _statusDesc->clear();
#endif // LLBC_CFG_COMM_ENABLE_STATUS_DESC
    _flags = 0;
    _extData1 = 0;
    _extData2 = 0;
    _extData3 = 0;

    // Clear codec error.
    LLBC_XDelete(_codecError);

    // Clear pre-handle result.
    CleanupPreHandleResult();
}

LLBC_ICoder *LLBC_Packet::GetEncoder() const
{
    return _encoder;
}

void LLBC_Packet::SetEncoder(LLBC_ICoder *encoder)
{
    if (UNLIKELY(encoder == _encoder))
        return;

    LLBC_XRecycle(_encoder);
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
    if (UNLIKELY(decoder == _decoder))
        return;

    LLBC_XRecycle(_decoder);
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
        if (!_payload && _msgBlockPoolInst)
            _payload = reinterpret_cast<LLBC_MessageBlock *>(_msgBlockPoolInst->Get());

        if (!_encoder->Encode(*this))
            return false;

        LLBC_XRecycle(_encoder);
    }

    return true;
}

bool LLBC_Packet::Decode()
{
    if (_decoder)
        return _decoder->Decode(*this);

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
    {
        if (clearDeleg)
            _resultClearDeleg = clearDeleg;
    }
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

    _codecError = LLBC_New(LLBC_String, codecErr.c_str(), codecErr.length());
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

void LLBC_Packet::CleanupPayload()
{
    if (!_payload)
        return;

    if (_payloadDeleteDeleg)
        _payloadDeleteDeleg->Invoke(_payload);

    LLBC_Recycle(_payload);
    _payload = NULL;
}

void LLBC_Packet::CleanupPayloadDeleteDeleg()
{
    if (!_payloadDeleteDeleg)
        return;

    if (_deletePayloadDeleteDelegWhenDestroy)
    {
        LLBC_Delete(_payloadDeleteDeleg);
        _deletePayloadDeleteDelegWhenDestroy = false;
    }

    _payloadDeleteDeleg = NULL;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
