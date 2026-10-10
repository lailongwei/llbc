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

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/objbase/Object.cpp
// @coverage-target: llbc/src/core/objbase/ObjectFactory.cpp
// @coverage-target: llbc/src/core/objbase/AutoReleasePool.cpp
// @coverage-target: llbc/src/core/objbase/AutoReleasePoolStack.cpp

namespace
{

class CountingObject final : public LLBC_Object
{
public:
    ~CountingObject() override
    {
        ++destructionCount;
    }

    static inline int destructionCount = 0;
};

} // namespace

// LLBC_Object is intrusive-reference-counted. Both normal and atomic reference
// APIs must preserve the count and destroy exactly once at the final release.
TEST(ObjectBaseTest, RetainReleaseAndSafeRetainReleaseManageLifetime)
{
    CountingObject::destructionCount = 0;
    auto *object = new CountingObject;

    EXPECT_EQ(object->GetRefCount(), 1);
    EXPECT_EQ(object->GetAutoRefCount(), 0);
    object->Retain();
    EXPECT_EQ(object->GetRefCount(), 2);
    object->Release();
    EXPECT_EQ(object->GetRefCount(), 1);

    object->SafeRetain();
    EXPECT_EQ(object->GetRefCount(), 2);
    object->SafeRelease();
    EXPECT_EQ(object->GetRefCount(), 1);
    EXPECT_EQ(CountingObject::destructionCount, 0);

    object->Release();
    EXPECT_EQ(CountingObject::destructionCount, 1);
}

// SafeRelease() must delete an object when it consumes the final reference,
// matching the normal intrusive Release() lifecycle.
TEST(ObjectBaseTest, SafeReleaseDestroysFinalReference)
{
    CountingObject::destructionCount = 0;
    auto *object = new CountingObject;
    object->SafeRelease();
    EXPECT_EQ(CountingObject::destructionCount, 1);
}

// Base object cloning and serialization provide safe default behavior for
// framework object hierarchies.
TEST(ObjectBaseTest, BaseCloneStringAndSerializationDefaults)
{
    LLBC_Object object;
    EXPECT_NE(object.ToString().find("Object["), static_cast<LLBC_String::size_type>(-1));
    EXPECT_NE(object.ToString().find("ref:1"), static_cast<LLBC_String::size_type>(-1));

    LLBC_Stream stream;
    object.Serialize(stream);
    EXPECT_EQ(stream.GetWritePos(), 0lu);
    EXPECT_TRUE(object.Deserialize(stream));

    LLBC_Object *cloned = object.Clone();
    ASSERT_NE(cloned, nullptr);
    EXPECT_NE(cloned, &object);
    EXPECT_EQ(cloned->GetRefCount(), 1);
    cloned->Release();
}

// Factories create a generic object by default and clone their own factory type,
// allowing applications to override CreateObject() for custom construction.
TEST(ObjectBaseTest, ObjectFactoryCreatesAndClonesExpectedTypes)
{
    LLBC_ObjectFactory factory;

    LLBC_Object *created = factory.CreateObject();
    ASSERT_NE(created, nullptr);
    EXPECT_EQ(created->GetRefCount(), 1);
    EXPECT_EQ(dynamic_cast<LLBC_ObjectFactory *>(created), nullptr);
    created->Release();

    LLBC_Object *cloned = factory.Clone();
    ASSERT_NE(cloned, nullptr);
    EXPECT_NE(cloned, &factory);
    EXPECT_NE(dynamic_cast<LLBC_ObjectFactory *>(cloned), nullptr);
    cloned->Release();
}

// Auto-release pools retain objects until purge/scope exit, then release them
// once. They also reject object-like operations that would be invalid on a pool.
TEST(ObjectBaseTest, AutoReleasePoolOwnsAndPurgesScheduledObjects)
{
    CountingObject::destructionCount = 0;
    auto *stack = LLBC_AutoReleasePoolStack::GetCurrentThreadReleasePoolStack();
    ASSERT_NE(stack, nullptr);

    {
        LLBC_AutoReleasePool pool;
        EXPECT_EQ(stack->GetCurrentReleasePool(), &pool);
        EXPECT_EQ(pool.AddObject(nullptr), LLBC_FAILED);
        EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
        EXPECT_EQ(pool.RemoveObject(nullptr), LLBC_FAILED);
        EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
        EXPECT_EQ(pool.AutoRelease(), LLBC_FAILED);
        EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_PERM);
        EXPECT_EQ(pool.Clone(), nullptr);
        EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_PERM);

        auto *object = new CountingObject;
        EXPECT_EQ(object->AutoRelease(), LLBC_OK);
        EXPECT_EQ(object->GetRefCount(), 1);
        EXPECT_EQ(object->GetAutoRefCount(), 1);
        EXPECT_EQ(CountingObject::destructionCount, 0);

        pool.Purge();
        EXPECT_EQ(CountingObject::destructionCount, 1);
    }

}

// A thread-local pool stack routes autoreleased objects to its newest pool and
// supports explicit removal when ownership must return to the caller.
TEST(ObjectBaseTest, AutoReleasePoolStackHandlesNestingAndExplicitRemoval)
{
    CountingObject::destructionCount = 0;
    auto *stack = LLBC_AutoReleasePoolStack::GetCurrentThreadReleasePoolStack();
    ASSERT_NE(stack, nullptr);

    EXPECT_EQ(stack->RemoveObject(nullptr), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(stack->PushPool(nullptr), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(stack->PopPool(nullptr), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);

    {
        LLBC_AutoReleasePool outer;
        EXPECT_EQ(stack->GetCurrentReleasePool(), &outer);

        {
            LLBC_AutoReleasePool inner;
            EXPECT_EQ(stack->GetCurrentReleasePool(), &inner);
        }
        EXPECT_EQ(stack->GetCurrentReleasePool(), &outer);

        auto *removable = new CountingObject;
        EXPECT_EQ(stack->AddObject(removable), LLBC_OK);
        EXPECT_EQ(removable->GetAutoRefCount(), 1);
        EXPECT_EQ(stack->RemoveObject(removable), LLBC_OK);
        EXPECT_EQ(removable->GetAutoRefCount(), 0);
        removable->Release();
    }

    EXPECT_EQ(CountingObject::destructionCount, 1);
}

// Standalone stacks have no current pool until one is pushed. They must reject
// additions safely instead of dereferencing a null head; pools also reject an
// object that has already been scheduled for auto-release.
TEST(ObjectBaseTest, RejectsUnpooledAndDuplicateAutoReleaseOperations)
{
    CountingObject::destructionCount = 0;
    LLBC_AutoReleasePoolStack detachedStack;
    auto *unpooled = new CountingObject;
    EXPECT_EQ(detachedStack.AddObject(unpooled), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    EXPECT_EQ(detachedStack.RemoveObject(unpooled), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    {
        LLBC_AutoReleasePool unrelatedPool;
        EXPECT_EQ(detachedStack.PopPool(&unrelatedPool), LLBC_FAILED);
        EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    }
    unpooled->Release();
    EXPECT_EQ(CountingObject::destructionCount, 1);

    {
        LLBC_AutoReleasePool pool;
        auto *scheduled = new CountingObject;
        ASSERT_EQ(scheduled->AutoRelease(), LLBC_OK);
        EXPECT_EQ(pool.AddObject(scheduled), LLBC_FAILED);
        EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_INVALID);
        pool.Purge();
    }

    EXPECT_EQ(CountingObject::destructionCount, 2);
}

// Leaving a pool scope without an explicit Purge() still releases the scheduled
// objects. The destructor must first detach their pool-stack marker so a later
// lifecycle operation cannot retain a stale pool pointer.
TEST(ObjectBaseTest, PoolDestructionPurgesOutstandingAutoReleasedObjects)
{
    CountingObject::destructionCount = 0;
    {
        LLBC_AutoReleasePool pool;
        auto *scheduled = new CountingObject;
        ASSERT_EQ(scheduled->AutoRelease(), LLBC_OK);
        EXPECT_EQ(scheduled->GetAutoRefCount(), 1);
        EXPECT_EQ(CountingObject::destructionCount, 0);
    }

    EXPECT_EQ(CountingObject::destructionCount, 1);
}

// A caller may release an autoreleased object before its pool is purged. The
// object destructor must remove its stale entry from the owning pool so a later
// Purge() never touches freed memory.
TEST(ObjectBaseTest, ManualReleaseRemovesObjectFromAutoReleasePool)
{
    CountingObject::destructionCount = 0;
    {
        LLBC_AutoReleasePool pool;
        auto *scheduled = new CountingObject;
        ASSERT_EQ(scheduled->AutoRelease(), LLBC_OK);
        EXPECT_EQ(scheduled->GetAutoRefCount(), 1);

        scheduled->Release();
        EXPECT_EQ(CountingObject::destructionCount, 1);
        pool.Purge();
    }

    EXPECT_EQ(CountingObject::destructionCount, 1);
}

// A stack may remove a non-head pool when lifecycle ownership changes. It must
// preserve the active head, report repeated removal as not-found, and Purge()
// must traverse every remaining nested pool.
TEST(ObjectBaseTest, StackRemovesNonHeadPoolsAndPurgesNestedPools)
{
    CountingObject::destructionCount = 0;
    auto *stack = LLBC_AutoReleasePoolStack::GetCurrentThreadReleasePoolStack();
    ASSERT_NE(stack, nullptr);

    {
        LLBC_AutoReleasePool outer;
        auto *outerObject = new CountingObject;
        ASSERT_EQ(outerObject->AutoRelease(), LLBC_OK);

        {
            LLBC_AutoReleasePool inner;
            auto *innerObject = new CountingObject;
            ASSERT_EQ(innerObject->AutoRelease(), LLBC_OK);

            auto *unscheduled = new CountingObject;
            EXPECT_EQ(stack->RemoveObject(unscheduled), LLBC_FAILED);
            EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
            unscheduled->Release();
            EXPECT_EQ(CountingObject::destructionCount, 1);

            stack->Purge();
            EXPECT_EQ(CountingObject::destructionCount, 3);

            EXPECT_EQ(stack->PopPool(&outer), LLBC_OK);
            EXPECT_EQ(stack->GetCurrentReleasePool(), &inner);
            EXPECT_EQ(stack->PopPool(&outer), LLBC_FAILED);
            EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
        }
    }

    EXPECT_EQ(CountingObject::destructionCount, 3);
}
