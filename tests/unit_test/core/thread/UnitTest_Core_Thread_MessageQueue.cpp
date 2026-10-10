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

#include <llbc.h>
using namespace llbc;

#include <atomic>
#include <chrono>
#include <string>
#include <thread>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/thread/MessageQueue.cpp
// @coverage-target: llbc/include/llbc/core/thread/MessageQueueInl.h
// @coverage-target: llbc/include/llbc/core/thread/MessageBlockInl.h
// @coverage-target: llbc/src/core/thread/MessageBuffer.cpp

namespace
{

LLBC_MessageBlock *MakeBlock(const std::string &text)
{
    auto *block = new LLBC_MessageBlock(text.size());
    EXPECT_EQ(block->Write(text.data(), text.size()), LLBC_OK);
    return block;
}

std::string ReadBlock(LLBC_MessageBlock *block)
{
    std::string result(block->GetReadableSize(), '\0');
    EXPECT_EQ(block->Read(result.data(), result.size()), LLBC_OK);
    return result;
}

void DeleteChain(LLBC_MessageBlock *block)
{
    while (block)
    {
        auto *next = block->GetNext();
        delete block;
        block = next;
    }
}

} // namespace

// Message blocks own or attach buffers, grow on write, expose positional APIs,
// and clone payload state without aliasing owned data.
TEST(MessageQueueTest, MessageBlockReadsWritesResizesAndClones)
{
    LLBC_MessageBlock block(2);
    EXPECT_NE(block.GetData(), nullptr);
    EXPECT_EQ(block.GetReadableSize(), 0lu);
    EXPECT_EQ(block.GetWritableSize(), 2lu);
    EXPECT_EQ(block.Write(nullptr, 0), LLBC_OK);
    EXPECT_EQ(block.Write(nullptr, 1), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(block.Write("abcd", 4), LLBC_OK);
    EXPECT_GE(block.GetSize(), 4lu);
    EXPECT_EQ(block.GetReadableSize(), 4lu);
    EXPECT_NE(block.GetDataStartWithReadPos(), nullptr);
    EXPECT_NE(block.GetDataStartWithWritePos(), nullptr);

    char read[3] {};
    EXPECT_EQ(block.Read(read, 2), LLBC_OK);
    EXPECT_EQ(std::string(read, 2), "ab");
    EXPECT_EQ(block.GetReadableSize(), 2lu);
    EXPECT_EQ(block.Read(nullptr, 1), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_LIMIT);
    EXPECT_EQ(block.Read(read, 3), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_LIMIT);
    EXPECT_EQ(block.SetReadPos(LLBC_MessageBlock::npos), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_RANGE);
    EXPECT_EQ(block.SetReadPos(1), LLBC_OK);
    EXPECT_EQ(block.ShiftReadPos(1), LLBC_OK);
    EXPECT_EQ(block.GetReadPos(), 2lu);
    EXPECT_EQ(block.ShiftWritePos(-100), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_RANGE);
    EXPECT_EQ(block.SetWritePos(4), LLBC_OK);
    EXPECT_EQ(block.ShiftWritePos(-1), LLBC_OK);
    EXPECT_EQ(block.GetWritePos(), 3lu);
    EXPECT_EQ(block.Allocate(4), LLBC_OK);
    EXPECT_GE(block.GetSize(), 8lu);

    LLBC_MessageBlock *clone = block.Clone();
    ASSERT_NE(clone, nullptr);
    EXPECT_EQ(clone->GetReadableSize(), block.GetReadableSize());
    EXPECT_EQ(ReadBlock(clone), "c");
    delete clone;

    char external[] = "xyz";
    LLBC_MessageBlock attached(external, sizeof(external) - 1, true);
    EXPECT_TRUE(attached.IsAttach());
    EXPECT_EQ(attached.SetWritePos(3), LLBC_OK);
    EXPECT_EQ(attached.Allocate(1), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_INVALID);
    LLBC_MessageBlock *attachedClone = attached.Clone();
    ASSERT_NE(attachedClone, nullptr);
    EXPECT_TRUE(attachedClone->IsAttach());
    delete attachedClone;
    attached.Release();
    EXPECT_EQ(attached.GetSize(), 0lu);
    EXPECT_EQ(std::string(external), "xyz");

    LLBC_MessageBlock swapLeft;
    LLBC_MessageBlock swapRight;
    ASSERT_EQ(swapLeft.Write("left", 4), LLBC_OK);
    ASSERT_EQ(swapRight.Write("right", 5), LLBC_OK);
    swapLeft.Swap(&swapRight);
    EXPECT_EQ(ReadBlock(&swapLeft), "right");
    EXPECT_EQ(ReadBlock(&swapRight), "left");
}

// Reused blocks must detach attached storage without freeing caller-owned
// memory, while owned storage can be released explicitly. Cursor setters are
// intentionally range checked because queue/buffer code uses them for partial
// consumption and direct writes.
TEST(MessageQueueTest, MessageBlockReleasesOwnedBuffersAndValidatesCursorRanges)
{
    LLBC_MessageBlock owned(4);
    owned.SetTypedObjPool(nullptr);
    EXPECT_EQ(owned.GetTypedObjPool(), nullptr);
    ASSERT_EQ(owned.Write("xy", 2), LLBC_OK);
    EXPECT_EQ(owned.SetReadPos(5), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_RANGE);
    EXPECT_EQ(owned.ShiftReadPos(-1), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_RANGE);
    EXPECT_EQ(owned.ShiftReadPos(5), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_RANGE);
    EXPECT_EQ(owned.SetWritePos(LLBC_MessageBlock::npos), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_RANGE);
    EXPECT_EQ(owned.SetWritePos(5), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_RANGE);

    owned.Release();
    EXPECT_EQ(owned.GetData(), nullptr);
    EXPECT_EQ(owned.GetSize(), 0lu);
    EXPECT_EQ(owned.GetReadPos(), 0lu);
    EXPECT_EQ(owned.GetWritePos(), 0lu);
    owned.Release();

    char external[] = "abc";
    LLBC_MessageBlock attached(external, sizeof(external) - 1, true);
    LLBC_MessageBlock previous;
    LLBC_MessageBlock next;
    attached.SetPrev(&previous);
    attached.SetNext(&next);
    ASSERT_EQ(attached.SetReadPos(1), LLBC_OK);
    ASSERT_EQ(attached.SetWritePos(3), LLBC_OK);
    attached.Clear();

    EXPECT_FALSE(attached.IsAttach());
    EXPECT_EQ(attached.GetData(), nullptr);
    EXPECT_EQ(attached.GetSize(), 0lu);
    EXPECT_EQ(attached.GetReadPos(), 0lu);
    EXPECT_EQ(attached.GetWritePos(), 0lu);
    EXPECT_EQ(attached.GetPrev(), nullptr);
    EXPECT_EQ(attached.GetNext(), nullptr);
    EXPECT_STREQ(external, "abc");
}

// Queue operations preserve front/back ordering, return a linked chain for
// PopAll, and clean up unconsumed blocks when requested.
TEST(MessageQueueTest, PushPopOrderPopAllAndCleanup)
{
    LLBC_MessageQueue queue;
    LLBC_MessageBlock *block = nullptr;
    EXPECT_FALSE(queue.TryPopFront(block));
    EXPECT_FALSE(queue.TryPopBack(block));

    queue.PushBack(MakeBlock("middle"));
    queue.PushFront(MakeBlock("front"));
    queue.PushBack(MakeBlock("back"));
    EXPECT_EQ(queue.GetSize(), 3lu);

    ASSERT_TRUE(queue.TryPopFront(block));
    EXPECT_EQ(ReadBlock(block), "front");
    delete block;
    ASSERT_TRUE(queue.TryPopBack(block));
    EXPECT_EQ(ReadBlock(block), "back");
    delete block;
    EXPECT_EQ(queue.GetSize(), 1lu);

    LLBC_MessageBlock *all = nullptr;
    ASSERT_TRUE(queue.PopAll(all));
    ASSERT_NE(all, nullptr);
    EXPECT_EQ(ReadBlock(all), "middle");
    EXPECT_EQ(all->GetNext(), nullptr);
    DeleteChain(all);
    EXPECT_EQ(queue.GetSize(), 0lu);
    EXPECT_FALSE(queue.PopAll(all));

    queue.PushBack(MakeBlock("cleanup-one"));
    queue.PushBack(MakeBlock("cleanup-two"));
    queue.Cleanup();
    EXPECT_EQ(queue.GetSize(), 0lu);
}

// Timed and blocking pops are the producer/consumer path used by task queues.
TEST(MessageQueueTest, TimedAndBlockingPopsSynchronizeWithProducer)
{
    LLBC_MessageQueue queue;
    LLBC_MessageBlock *block = nullptr;
    EXPECT_FALSE(queue.TimedPopFront(block, 20));

    std::atomic<bool> consumerStarted {false};
    std::atomic<bool> readSucceeded {false};
    std::string consumed;
    std::thread consumer([&] {
        consumerStarted.store(true, std::memory_order_release);
        LLBC_MessageBlock *received = nullptr;
        queue.PopFront(received);
        if (received)
        {
            consumed.assign(received->GetReadableSize(), '\0');
            readSucceeded.store(
                received->Read(consumed.data(), consumed.size()) == LLBC_OK,
                std::memory_order_release);
        }
        delete received;
    });

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while (!consumerStarted.load(std::memory_order_acquire))
    {
        if (std::chrono::steady_clock::now() >= deadline)
        {
            queue.PushBack(MakeBlock("abort"));
            consumer.join();
            FAIL() << "consumer did not start before deadline";
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    queue.PushBack(MakeBlock("from-producer"));
    consumer.join();
    EXPECT_TRUE(readSucceeded.load(std::memory_order_acquire));
    EXPECT_EQ(consumed, "from-producer");
}

// Back-oriented helpers share the queue's blocking core but must preserve the
// same single-element and producer/consumer semantics as front-oriented calls.
TEST(MessageQueueTest, SupportsBackOrientedTimedAndBlockingPops)
{
    LLBC_MessageQueue queue;
    LLBC_MessageBlock *block = nullptr;
    EXPECT_FALSE(queue.TimedPopBack(block, 10));

    queue.PushFront(MakeBlock("only"));
    ASSERT_TRUE(queue.TryPopBack(block));
    ASSERT_NE(block, nullptr);
    EXPECT_EQ(ReadBlock(block), "only");
    delete block;
    EXPECT_EQ(queue.GetSize(), 0lu);

    std::atomic<bool> consumerStarted {false};
    std::string consumed;
    std::thread consumer([&] {
        consumerStarted.store(true, std::memory_order_release);
        LLBC_MessageBlock *received = nullptr;
        queue.PopBack(received);
        if (received)
        {
            consumed.assign(received->GetReadableSize(), '\0');
            received->Read(consumed.data(), consumed.size());
            delete received;
        }
    });

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while (!consumerStarted.load(std::memory_order_acquire))
    {
        if (std::chrono::steady_clock::now() >= deadline)
        {
            queue.PushBack(MakeBlock("abort"));
            consumer.join();
            FAIL() << "back-oriented consumer did not start before deadline";
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    queue.PushFront(MakeBlock("from-front"));
    consumer.join();
    EXPECT_EQ(consumed, "from-front");
}

// Message buffers coalesce writes into blocks, allow partial consumption and
// removal, and can detach either the first block or a merged contiguous block.
TEST(MessageQueueTest, MessageBufferReadsWritesAppendsDetachesAndMerges)
{
    LLBC_MessageBuffer buffer;
    EXPECT_EQ(buffer.Write(nullptr, 1), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(buffer.Write("unused", 0), LLBC_OK);
    EXPECT_EQ(buffer.GetSize(), 0lu);

    ASSERT_EQ(buffer.Write("abc", 3), LLBC_OK);
    char partial[3] {};
    EXPECT_EQ(buffer.Read(partial, 2), 2lu);
    EXPECT_EQ(std::string(partial, 2), "ab");
    EXPECT_EQ(buffer.GetSize(), 1lu);
    EXPECT_EQ(buffer.Remove(0), 0lu);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);

    ASSERT_EQ(buffer.Write("def", 3), LLBC_OK);
    ASSERT_EQ(buffer.Append(MakeBlock("ghi")), LLBC_OK);
    EXPECT_EQ(buffer.GetSize(), 7lu);
    EXPECT_NE(buffer.FirstBlock(), nullptr);

    char all[8] {};
    EXPECT_EQ(buffer.Read(all, 7), 7lu);
    EXPECT_EQ(std::string(all, 7), "cdefghi");
    EXPECT_EQ(buffer.GetSize(), 0lu);
    EXPECT_EQ(buffer.Read(all, 1), 0lu);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NO_SUCH);

    ASSERT_EQ(buffer.Append(MakeBlock("first")), LLBC_OK);
    ASSERT_EQ(buffer.Append(MakeBlock("second")), LLBC_OK);
    LLBC_MessageBlock *first = buffer.DetachFirstBlock();
    ASSERT_NE(first, nullptr);
    EXPECT_EQ(ReadBlock(first), "first");
    delete first;
    EXPECT_EQ(buffer.GetSize(), 6lu);

    LLBC_MessageBlock *merged = buffer.MergeBlocksAndDetach();
    ASSERT_NE(merged, nullptr);
    EXPECT_EQ(ReadBlock(merged), "second");
    delete merged;
    EXPECT_EQ(buffer.GetSize(), 0lu);
    EXPECT_EQ(buffer.DetachFirstBlock(), nullptr);
    EXPECT_EQ(buffer.MergeBlocksAndDetach(), nullptr);

    ASSERT_EQ(buffer.Append(nullptr), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    buffer.Cleanup();
}

// Message buffers coalesce into spare tail capacity where possible, preserve
// chained blocks once the resize limit is reached, and support partial/full
// removal without exposing stale ownership to callers.
TEST(MessageQueueTest, MessageBufferCoversCapacityChainAndRemovalEdgeCases)
{
    LLBC_MessageBuffer buffer;
    char byte = '\0';
    EXPECT_EQ(buffer.Read(nullptr, 1), 0lu);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(buffer.Read(&byte, 0), 0lu);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_SUCCESS);

    auto *spacious = new LLBC_MessageBlock(16);
    ASSERT_EQ(spacious->Write("abc", 3), LLBC_OK);
    ASSERT_EQ(buffer.Append(spacious), LLBC_OK);
    EXPECT_EQ(buffer.Write("de", 2), LLBC_OK);
    auto *coalesced = MakeBlock("fg");
    ASSERT_EQ(buffer.Append(coalesced), LLBC_OK);
    EXPECT_EQ(buffer.GetSize(), 7lu);

    char compact[8] {};
    EXPECT_EQ(buffer.Read(compact, 7), 7lu);
    EXPECT_EQ(std::string(compact, 7), "abcdefg");
    EXPECT_EQ(buffer.GetSize(), 0lu);

    auto *empty = new LLBC_MessageBlock(0);
    EXPECT_EQ(buffer.Append(empty), LLBC_OK);
    EXPECT_EQ(buffer.GetSize(), 0lu);

    LLBC_MessageBuffer removal;
    ASSERT_EQ(removal.Write("abcdef", 6), LLBC_OK);
    EXPECT_EQ(removal.Remove(2), 2lu);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_SUCCESS);
    EXPECT_EQ(removal.GetSize(), 4lu);
    EXPECT_EQ(removal.Remove(4), 4lu);
    EXPECT_EQ(removal.GetSize(), 0lu);
    EXPECT_EQ(removal.Remove(1), 0lu);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NO_SUCH);

    LLBC_MessageBuffer crossBlockRemoval;
    ASSERT_EQ(crossBlockRemoval.Append(MakeBlock("abc")), LLBC_OK);
    ASSERT_EQ(crossBlockRemoval.Append(MakeBlock("def")), LLBC_OK);
    EXPECT_EQ(crossBlockRemoval.Remove(4), 4lu);
    EXPECT_EQ(crossBlockRemoval.GetSize(), 2lu);
    char remaining[3] {};
    EXPECT_EQ(crossBlockRemoval.Read(remaining, 2), 2lu);
    EXPECT_EQ(std::string(remaining, 2), "ef");

    ASSERT_EQ(removal.Write("cleanup", 7), LLBC_OK);
    removal.Cleanup();
    EXPECT_EQ(removal.GetSize(), 0lu);
    EXPECT_EQ(removal.FirstBlock(), nullptr);

    LLBC_MessageBuffer detachedBuffer;
    ASSERT_EQ(detachedBuffer.Write("z", 1), LLBC_OK);
    LLBC_MessageBlock *detached = detachedBuffer.DetachFirstBlock();
    ASSERT_NE(detached, nullptr);
    EXPECT_EQ(detachedBuffer.GetSize(), 0lu);
    EXPECT_EQ(detachedBuffer.FirstBlock(), nullptr);
    delete detached;

    LLBC_MessageBuffer chained;
    const std::string fullBlock(LLBC_CFG_COMM_MSG_BUFFER_ELEM_RESIZE_LIMIT, 'a');
    ASSERT_EQ(chained.Write(fullBlock.data(), fullBlock.size()), LLBC_OK);
    ASSERT_EQ(chained.Write("b", 1), LLBC_OK);
    ASSERT_NE(chained.FirstBlock(), nullptr);
    ASSERT_NE(chained.FirstBlock()->GetNext(), nullptr);
    LLBC_MessageBlock *merged = chained.MergeBlocksAndDetach();
    ASSERT_NE(merged, nullptr);
    EXPECT_EQ(merged->GetReadableSize(), fullBlock.size() + 1);
    delete merged;
}
