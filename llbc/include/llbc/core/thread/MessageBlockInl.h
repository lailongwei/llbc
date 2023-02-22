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

__LLBC_NS_BEGIN

inline LLBC_MessageBlock::LLBC_MessageBlock(size_t size)
: _attach(false)
, _buf(nullptr)
, _size(size)
, _readPos(0)
, _writePos(0)
, _prev(nullptr)
, _next(nullptr)
, _poolInst(nullptr)
{
    if (LIKELY(size > 0))
        _buf = LLBC_Malloc(char, size);
}

inline LLBC_MessageBlock::LLBC_MessageBlock(void *buf, size_t size, bool attach)
: _attach(attach)
, _buf(reinterpret_cast<char *>(buf))
, _size(size)
, _readPos(0)
, _writePos(0)
, _prev(nullptr)
, _next(nullptr)

, _poolInst(nullptr)
{
}

inline LLBC_MessageBlock::~LLBC_MessageBlock()
{
    if (_buf && !_attach)
        free(_buf);
}

inline int LLBC_MessageBlock::Allocate(size_t size)
{
    if (_attach)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    Resize(_size + size);

    return LLBC_OK;
}

inline int LLBC_MessageBlock::Read(void *buf, size_t len)
{
    if (!buf || len > GetReadableSize())
    {
        LLBC_SetLastError(LLBC_ERROR_LIMIT);
        return LLBC_FAILED;
    }

    ASSERT((char *)buf + len <= _buf || _buf + _size <= buf);

    memcpy(buf, _buf + _readPos, len);
    _readPos += len;

    return LLBC_OK;
}

inline int LLBC_MessageBlock::Write(const void *buf, size_t len)
{
    if (len == 0)
    {
        return LLBC_OK;
    }
    else if (UNLIKELY(!buf))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    if (_writePos + len > _size)
    {
        Resize(MAX(_writePos + len, _size * 2));
    }

    ASSERT((char *)(buf) + len <= _buf || _buf + _size <= buf);

    memcpy(_buf + _writePos, buf, len);
    _writePos += len;

    return LLBC_OK;
}

inline void LLBC_MessageBlock::Release()
{
    if (!_buf)
        return;

    if (!_attach)
        free(_buf);
    _buf = nullptr;

    _size = 0;
    _readPos = _writePos = 0;
}

inline void LLBC_MessageBlock::MarkPoolObject(LLBC_IObjectPoolInst &poolInst)
{
    _poolInst = &poolInst;
}

inline LLBC_IObjectPoolInst * LLBC_MessageBlock::GetPoolInst()
{
    return _poolInst;
}

inline size_t LLBC_MessageBlock::GetPoolInstPerBlockUnitsNum()
{
    return LLBC_CFG_CORE_OBJECT_POOL_MESSAGE_BLOCK_UNITS_NUMBER;
}

inline void LLBC_MessageBlock::Clear()
{
    _readPos = _writePos = 0;
    if (_attach)
    {
        _buf = 0;
        _size = 0;
        _attach = false;
    }

    _prev = _next = nullptr;
}

inline bool LLBC_MessageBlock::IsAttach() const
{
    return _attach;
}

inline void *LLBC_MessageBlock::GetData() const
{
    return _buf;
}

inline void *LLBC_MessageBlock::GetDataStartWithReadPos() const
{
    return const_cast<char *>(reinterpret_cast<const char *>(_buf) + _readPos);
}

inline void *LLBC_MessageBlock::GetDataStartWithWritePos() const
{
    return const_cast<char *>((reinterpret_cast<const char *>(_buf) + _writePos));
}

inline size_t LLBC_MessageBlock::GetSize() const
{
    return _size;
}

inline size_t LLBC_MessageBlock::GetReadableSize() const
{
    if (_readPos >= _writePos)
        return 0;

    return _writePos - _readPos;
}

inline size_t LLBC_MessageBlock::GetWritableSize() const
{
    return _size - _writePos;
}

inline size_t LLBC_MessageBlock::GetReadPos() const
{
    return _readPos;
}

inline int LLBC_MessageBlock::SetReadPos(size_t pos)
{
    if (pos == npos || pos > _size)
    {
        LLBC_SetLastError(LLBC_ERROR_RANGE);
        return LLBC_FAILED;
    }

    _readPos = pos;

    return LLBC_OK;
}

inline int LLBC_MessageBlock::ShiftReadPos(long off)
{
    long pos = static_cast<long>(_readPos) + off;
    if (pos < 0 || pos > static_cast<long>(_size))
    {
        LLBC_SetLastError(LLBC_ERROR_RANGE);
        return LLBC_FAILED;
    }

    _readPos = static_cast<size_t>(pos);

    return LLBC_OK;
}

inline size_t LLBC_MessageBlock::GetWritePos() const
{
    return _writePos;
}

inline int LLBC_MessageBlock::SetWritePos(size_t pos)
{
    if (pos == npos || pos > _size)
    {
        LLBC_SetLastError(LLBC_ERROR_RANGE);
        return LLBC_FAILED;
    }

    _writePos = pos;

    return LLBC_OK;
}

inline int LLBC_MessageBlock::ShiftWritePos(long off)
{
    long pos = static_cast<long>(_writePos) + off;
    if (UNLIKELY(pos < 0 || pos > static_cast<long>(_size)))
    {
        LLBC_SetLastError(LLBC_ERROR_RANGE);
        return LLBC_FAILED;
    }

    _writePos = static_cast<size_t>(pos);

    return LLBC_OK;
}

inline void LLBC_MessageBlock::Swap(LLBC_MessageBlock *another)
{
    LLBC_Swap(_attach, another->_attach);

    LLBC_Swap(_buf, another->_buf);
    LLBC_Swap(_size, another->_size);

    LLBC_Swap(_readPos, another->_readPos);
    LLBC_Swap(_writePos, another->_writePos);

    LLBC_Swap(_prev, another->_prev);
    LLBC_Swap(_next, another->_next);
}

inline LLBC_MessageBlock *LLBC_MessageBlock::Clone() const
{
    LLBC_MessageBlock *clone;
    if (IsAttach())
    {
        clone = new LLBC_MessageBlock(_buf, _size);
    }
    else
    {
        clone = new LLBC_MessageBlock(_size);
        memcpy(clone->_buf, _buf, _size);
    }

    clone->_readPos = _readPos;
    clone->_writePos = _writePos;
    clone->_prev = _prev;
    clone->_next = _next;

    return clone;
}

inline LLBC_MessageBlock *LLBC_MessageBlock::GetPrev() const
{
    return _prev;
}

inline void LLBC_MessageBlock::SetPrev(LLBC_MessageBlock *prev)
{
    _prev = prev;
}

inline LLBC_MessageBlock *LLBC_MessageBlock::GetNext() const
{
    return _next;
}

inline void LLBC_MessageBlock::SetNext(LLBC_MessageBlock *next)
{
    _next = next;
}

inline void LLBC_MessageBlock::Resize(size_t newSize)
{
    ASSERT(!_attach && newSize > _size);

    _buf = LLBC_Realloc(char, _buf, newSize);
    _size = newSize;
}

__LLBC_NS_END

