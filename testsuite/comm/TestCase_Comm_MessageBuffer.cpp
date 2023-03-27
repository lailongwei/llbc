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


#include "comm/TestCase_Comm_MessageBuffer.h"

TestCase_Comm_MessageBuffer::TestCase_Comm_MessageBuffer()
{
}

TestCase_Comm_MessageBuffer::~TestCase_Comm_MessageBuffer()
{
}

int TestCase_Comm_MessageBuffer::Run(int argc, char *argv[])
{
    LLBC_PrintLn("MessageBuffer testcase:");

    LLBC_PrintLn("Message buffer read/write ptr test:");
    {
        LLBC_MessageBuffer msgBuffer;
        LLBC_PrintLn("Write 'hello world':");
        msgBuffer.Write("hello world", 12);
        LLBC_PrintLn("After wrote, size:%lu, first block readable size:%lu, first block data(const char *):%s",
                     msgBuffer.GetSize(),
                     msgBuffer.FirstBlock()->GetReadableSize(),
                     reinterpret_cast<const char *>(msgBuffer.FirstBlock()->GetDataStartWithReadPos()));

        char strBuf[16];
        size_t readRet = msgBuffer.Read(strBuf, 3);
        strBuf[3] = '\0';
        LLBC_PrintLn("After read 3 chars, size:%lu, first block readable size:%lu, "
                     "first block data(const char *):%s, read data:%s, read ret:%lu",
                     msgBuffer.GetSize(),
                     msgBuffer.FirstBlock()->GetReadableSize(),
                     reinterpret_cast<const char *>(msgBuffer.FirstBlock()->GetDataStartWithReadPos()),
                     strBuf,
                     readRet);

        readRet = msgBuffer.Read(strBuf, 13);
        strBuf[readRet] = '\0';
        LLBC_PrintLn("After read 13 chars ,size:%lu, first block?:%p, read data:%s, read ret:%lu",
                     msgBuffer.GetSize(),
                     msgBuffer.FirstBlock(),
                     strBuf,
                     readRet);

        LLBC_Print("\n");
    }

    LLBC_PrintLn("Message buffer read/write message block test:");
    {
        LLBC_MessageBuffer msgBuffer;
        LLBC_MessageBlock *block1 = new LLBC_MessageBlock(24);
        block1->Write("Hello World", 11);
        block1->Resize(25);
        LLBC_MessageBlock *block2 = new LLBC_MessageBlock();
        block2->Write("Hey Judy", 9);
        msgBuffer.Append(block1);
        msgBuffer.Append(block2);
        LLBC_PrintLn("After append 2 buffers, size:%lu, first block size:%lu, first block data(const char *):%s",
                     msgBuffer.GetSize(),
                     msgBuffer.FirstBlock()->GetReadableSize(),
                     reinterpret_cast<const char *>(msgBuffer.FirstBlock()->GetDataStartWithReadPos()));

        LLBC_MessageBlock *block3 = new LLBC_MessageBlock();
        block3->Write("Who's your dady!", 17);
        msgBuffer.Append(block3);
        LLBC_PrintLn("After append new buffer(17 bytes), size:%lu, first block size:%lu",
                     msgBuffer.GetSize(), msgBuffer.FirstBlock()->GetReadableSize());

        size_t removeRet = msgBuffer.Remove(20);
        LLBC_PrintLn("After remove 20 bytes, size:%lu, first block?:%p, remove ret:%lu",
                     msgBuffer.GetSize(), msgBuffer.FirstBlock(), removeRet);

        LLBC_Print("\n");
    }

    LLBC_PrintLn("Message buffer write large data test(ptr):");
    {
        LLBC_MessageBuffer msgBuffer;
        for (int i = 0; i < 4000; ++i)
            msgBuffer.Write("hello world", 11);

        LLBC_PrintLn("After write 4000 * 11 = %d bytes, size:%lu", 4000 * 11, msgBuffer.GetSize());
        int blockIdx = 0;
        const LLBC_MessageBlock *block = msgBuffer.FirstBlock();
        while(block)
        {
            LLBC_PrintLn("- block %02d[%p] size:%lu, cap:%lu",
                         blockIdx, block, block->GetReadableSize(), block->GetSize());
            block = block->GetNext();

            blockIdx += 1;
        }

        LLBC_Print("\n");
    }

    LLBC_PrintLn("Press any key to continue...");
    getchar();

    return 0;
}
