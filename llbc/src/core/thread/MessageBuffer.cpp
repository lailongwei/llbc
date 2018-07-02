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

#include "llbc/core/thread/MessageBlock.h"
#include "llbc/core/thread/MessageBuffer.h"

__LLBC_NS_BEGIN

LLBC_MessageBuffer::LLBC_MessageBuffer()
: _head(NULL)
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
    while (needReadLen > 0)
    {
        size_t availableSize = _head->GetWritePos() - _head->GetReadPos();
        if (availableSize >= needReadLen)
        {
            _head->Read(reinterpret_cast<char *>(buf) + (len - needReadLen), needReadLen);
            _head->SetReadPos(_head->GetReadPos() + needReadLen);

            if (availableSize == needReadLen)
            {
                LLBC_MessageBlock *block = _head;
                _head = _head->GetNext();
                LLBC_Delete(block);
            }

            needReadLen = 0;
            break;
        }

        _head->Read(reinterpret_cast<char *>(buf) + (len - needReadLen), availableSize);
        needReadLen -= availableSize;

        LLBC_MessageBlock *block = _head;
        _head = _head->GetNext();
        LLBC_Delete(block);
    }

    if (needReadLen > 0)
        LLBC_SetLastError(LLBC_ERROR_NO_SUCH);
    else
        LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    return len - needReadLen;
}

int LLBC_MessageBuffer::Write(const char *buf, size_t len)
{
    if (!buf)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }
    else if (len == 0)
    {
        return LLBC_OK;
    }

    LLBC_MessageBlock *block = LLBC_New1(LLBC_MessageBlock, len);
    block->Write(buf, len);
    if (Append(block) != LLBC_OK)
    {
        LLBC_Delete(block);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

LLBC_MessageBlock *LLBC_MessageBuffer::FirstBlock() const
{
    return _head;
}

LLBC_MessageBlock *LLBC_MessageBuffer::MergeBuffersAndDetach()
{
    if (!_head)
        return NULL;

    LLBC_MessageBlock *mergedBlock = _head;
    LLBC_MessageBlock *curBlock = _head->GetNext();
    while (curBlock)
    {
        mergedBlock->Write(
            curBlock->GetDataStartWithReadPos(), curBlock->GetWritePos() - curBlock->GetReadPos());
        LLBC_MessageBlock *next = curBlock->GetNext();

        LLBC_Delete(curBlock);
        curBlock = next;
    }

    _head = NULL;
    mergedBlock->SetNext(NULL);

    return mergedBlock;
}

int LLBC_MessageBuffer::Append(LLBC_MessageBlock *block)
{
    if (UNLIKELY(!block))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    if (UNLIKELY(block->GetReadableSize() == 0))
    {
        LLBC_Delete(block);
        return LLBC_OK;
    }

    block->SetNext(NULL);

    if (!_head)
    {
        _head = block;
        return LLBC_OK;
    }

    LLBC_MessageBlock *curBlock = _head;
    while (curBlock->GetNext())
        curBlock = curBlock->GetNext();

    curBlock->SetNext(block);

    return LLBC_OK;
}

size_t LLBC_MessageBuffer::Remove(size_t length)
{
    if (UNLIKELY(length < 0))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return 0;
    }

    size_t needRemoveLength = length;
    while (needRemoveLength > 0)
    {
        size_t availableSize = _head->GetWritePos() - _head->GetReadPos();
        if (availableSize >= needRemoveLength)
        {
            _head->SetReadPos(_head->GetReadPos() + needRemoveLength);

            if (availableSize == needRemoveLength)
            {
                LLBC_MessageBlock *block = _head;
                _head = _head->GetNext();
                LLBC_Delete(block);
            }

            needRemoveLength = 0;
            break;
        }

        needRemoveLength -= availableSize;

        LLBC_MessageBlock *block = _head;
        _head = _head->GetNext();
        LLBC_Delete(block);
    }

    if (needRemoveLength > 0)
        LLBC_SetLastError(LLBC_ERROR_NO_SUCH);
    else
        LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    return length - needRemoveLength;
}

void LLBC_MessageBuffer::Cleanup()
{
    LLBC_MessageBlock *block = NULL;
    while (_head)
    {
        block = _head;
        _head = _head->GetNext();
        LLBC_Delete(block);
    }
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
