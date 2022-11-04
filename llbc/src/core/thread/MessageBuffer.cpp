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

#include "llbc/core/objectpool/ObjectPool.h"
#include "llbc/core/objectpool/PoolObjectReflection.h"

#include "llbc/core/thread/MessageBlock.h"
#include "llbc/core/thread/MessageBuffer.h"

__LLBC_NS_BEGIN

LLBC_MessageBuffer::LLBC_MessageBuffer()
: _size(0)
, _head(nullptr)
, _tail(nullptr)
{
}

LLBC_MessageBuffer::~LLBC_MessageBuffer()
{
    Cleanup();
}

size_t LLBC_MessageBuffer::Read(void *buf, size_t len)
{
    if (!buf)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return 0;
    }

    if (len == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_SUCCESS);
        return len;
    }

    size_t needReadLen = len;
    while (needReadLen > 0 && _head)
    {
        size_t availableSize = _head->GetWritePos() - _head->GetReadPos();
        if (availableSize >= needReadLen)
        {
            _head->Read(reinterpret_cast<char *>(buf) + (len - needReadLen), needReadLen);
            if (availableSize == needReadLen)
                DeleteFirstBlock();

            needReadLen = 0;
            break;
        }

        _head->Read(reinterpret_cast<char *>(buf) + (len - needReadLen), availableSize);
        needReadLen -= availableSize;

        DeleteFirstBlock();
    }

    if (needReadLen > 0)
        LLBC_SetLastError(LLBC_ERROR_NO_SUCH);
    else
        LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    const size_t alreadyRead = len - needReadLen;
    ASSERT(alreadyRead <= _size && "llbc library internal error for LLBC_MessageBuffer");

    _size -= alreadyRead;

    return alreadyRead;
}

int LLBC_MessageBuffer::Write(const char *buf, size_t len)
{
    // Params check.
    if (!buf)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }
    else if (len == 0)
    {
        return LLBC_OK;
    }

    // If message buffer empty, execute fast write.
    if (!_tail)
    {
        _head = _tail = new LLBC_MessageBlock(len);
        _tail->Write(buf, len);

        _size = len;

        return LLBC_OK;
    }

    // If message buffer not empty and _tail block has writable size, try execute fast write.
    const size_t tailWritableSize = _tail->GetWritableSize();
    if (tailWritableSize >= len)

    {
        _tail->Write(buf, len);
        _size += len;

        return LLBC_OK;
    }
    else if (_tail->GetWritePos() + len < 
                 LLBC_CFG_COMM_MSG_BUFFER_ELEM_RESIZE_LIMIT) // writableSize >= len or (writableSize < len and tail msg block auto resize not reach to limit).
    {
        _tail->Resize(MIN(MAX(_tail->GetWritePos() + len, _tail->GetSize() * 2), LLBC_CFG_COMM_MSG_BUFFER_ELEM_RESIZE_LIMIT));
        _tail->Write(buf, len);
        _size += len;

        return LLBC_OK;
    }

    // Execute normal write(create new block and append to message buffer).
    LLBC_MessageBlock *block = new LLBC_MessageBlock(len);
    block->Write(buf, len);
    if (UNLIKELY(Append(block) != LLBC_OK))
    {
        LLBC_Recycle(block);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

size_t LLBC_MessageBuffer::GetSize() const
{
    return _size;
}

const LLBC_MessageBlock *LLBC_MessageBuffer::FirstBlock() const
{
    return _head;
}

LLBC_MessageBlock *LLBC_MessageBuffer::DetachFirstBlock()
{
    if (!_head)
        return nullptr;

    LLBC_MessageBlock *firstBlock = _head;
    _head = _head->GetNext();
    if (_head == nullptr)
        _tail = nullptr;

    _size -= firstBlock->GetReadableSize();

    firstBlock->SetNext(nullptr);

    return firstBlock;
}

LLBC_MessageBlock *LLBC_MessageBuffer::MergeBlocksAndDetach()
{
    if (!_head)
        return nullptr;

    LLBC_MessageBlock *mergedBlock = _head;
    LLBC_MessageBlock *curBlock = _head->GetNext();
    while (curBlock)
    {
        mergedBlock->Write(
            curBlock->GetDataStartWithReadPos(), curBlock->GetWritePos() - curBlock->GetReadPos());
        LLBC_MessageBlock *next = curBlock->GetNext();

        LLBC_Recycle(curBlock);
        curBlock = next;
    }

    _size = 0;
    _head = nullptr;
    _tail = nullptr;
    mergedBlock->SetNext(nullptr);

    return mergedBlock;
}

int LLBC_MessageBuffer::Append(LLBC_MessageBlock *block)
{
    // Block ptr empty check.
    if (UNLIKELY(!block))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    // Block readable size zero check.
    if (UNLIKELY(block->GetReadableSize() == 0))
    {
        LLBC_Recycle(block);
        return LLBC_OK;
    }

    // Force set block next to nullptr.
    block->SetNext(nullptr);

    // Update message-buffer size first.
    _size += block->GetReadableSize();

    // If message buffer is empty, execute fast append.
    if (!_head)
    {
        _head = _tail = block;
        return LLBC_OK;
    }

    // If tail block writable size >= will append block readable size, execute fast append.
    if (_tail->GetWritableSize() >= block->GetReadableSize())
    {
        _tail->Write(block->GetDataStartWithReadPos(), block->GetReadableSize());
        LLBC_Recycle(block);

        return LLBC_OK;
    }

    // Execute normal append.
    _tail->SetNext(block);
    _tail = block;

    return LLBC_OK;
}

size_t LLBC_MessageBuffer::Remove(size_t length)
{
    if (UNLIKELY(length <= 0))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return 0;
    }

    size_t needRemoveLength = length;
    while (needRemoveLength > 0 && _head)
    {
        size_t availableSize = _head->GetWritePos() - _head->GetReadPos();
        if (availableSize >= needRemoveLength)
        {
            _head->SetReadPos(_head->GetReadPos() + needRemoveLength);

            if (availableSize == needRemoveLength)
                DeleteFirstBlock();

            needRemoveLength = 0;
            break;
        }

        needRemoveLength -= availableSize;
        DeleteFirstBlock();
    }

    if (needRemoveLength > 0)
        LLBC_SetLastError(LLBC_ERROR_NO_SUCH);
    else
        LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    const size_t removedLen = length - needRemoveLength;
    ASSERT(removedLen <= _size && "llbc library internal error for LLBC_MessageBuffer");

    _size -= removedLen;
    
    return removedLen;
}

void LLBC_MessageBuffer::Cleanup()
{
    if (!_head)
        return;

    LLBC_MessageBlock *block = nullptr;
    while (_head)
    {
        block = _head;
        _head = _head->GetNext();
        LLBC_Recycle(block);
    }

    _tail = nullptr;
    _size = 0;
}

void LLBC_MessageBuffer::DeleteFirstBlock()
{
    if (LIKELY(_head))
    {
        LLBC_MessageBlock *block = _head;
        _head = _head->GetNext();
        LLBC_Recycle(block);

        if (_head == nullptr)
            _tail = nullptr;
    }
}

__LLBC_NS_END
