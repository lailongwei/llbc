// The MIT License (MIT)
//
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
// @coverage-target: llbc/src/core/entity/Entity.cpp
// @coverage-target: llbc/include/llbc/core/entity/EntityInl.h
// @coverage-target: llbc/include/llbc/core/entity/HolderInl.h

namespace
{

class TrackedBehavior
{
public:
    explicit TrackedBehavior(int value)
    : value(value)
    {
    }

    ~TrackedBehavior()
    {
        ++destructions;
    }

    static inline int destructions = 0;
    int value;
};

} // namespace

// Holders are the entity's behavior-ownership primitive. They must expose a
// null payload safely and delete a non-null payload exactly once at scope exit.
TEST(EntityTest, HolderSafelyOwnsNullAndNonNullPayloads)
{
    TrackedBehavior::destructions = 0;
    {
        LLBC_Holder<TrackedBehavior> emptyHolder(nullptr);
        EXPECT_EQ(emptyHolder.GetValue(), nullptr);
    }

    {
        auto *behavior = new TrackedBehavior(7);
        LLBC_Holder<TrackedBehavior> holder(behavior);
        EXPECT_EQ(static_cast<TrackedBehavior *>(holder.GetValue())->value, 7);
    }
    EXPECT_EQ(TrackedBehavior::destructions, 1);
}

// Entities combine Variant properties with owned behavior objects. Property
// lookup failures return a reusable Nil sentinel without leaking stale values
// between distinct missing-key requests.
TEST(EntityTest, ManagesIdentityPropertiesAndInvalidLookupSentinel)
{
    LLBC_Entity entity;
    EXPECT_EQ(entity.GetId(), 0);
    EXPECT_TRUE(entity.GetName().empty());
    entity.SetId(101);
    entity.SetName("player");
    EXPECT_EQ(entity.GetId(), 101);
    EXPECT_EQ(entity.GetName(), "player");

    EXPECT_EQ(entity.AddProperty("", 1), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    ASSERT_EQ(entity.AddProperty("health", 100), LLBC_OK);
    EXPECT_EQ(entity.AddProperty("health", 200), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_EXIST);
    EXPECT_TRUE(entity.IsExistProperty("health"));
    EXPECT_FALSE(entity.IsExistProperty("missing"));
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    EXPECT_FALSE(entity.IsExistProperty(""));
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(entity.GetProperty("health").As<int>(), 100);

    LLBC_Variant &missing = entity.GetProperty("missing");
    EXPECT_TRUE(missing.Is<void>());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    missing = 123;
    EXPECT_TRUE(entity.GetProperty("another-missing").Is<void>());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    const LLBC_Entity &constEntity = entity;
    EXPECT_TRUE(constEntity.GetProperty("const-missing").Is<void>());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);

    EXPECT_EQ(entity.RemmoveProperty("missing"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    EXPECT_EQ(entity.RemmoveProperty(""), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(entity.RemmoveProperty("health"), LLBC_OK);
    EXPECT_FALSE(entity.IsExistProperty("health"));
    entity.AddProperty("score", 9);
    entity.RemoveAllProperties();
    EXPECT_FALSE(entity.IsExistProperty("score"));
}

// Behavior values are owned by the entity through LLBC_Holder. Validate the
// id/name uniqueness rules, lookup overloads, map views, and destruction paths.
TEST(EntityTest, OwnsBehaviorsByIdAndName)
{
    TrackedBehavior::destructions = 0;
    LLBC_Entity entity;
    EXPECT_EQ(entity.AddBehavior<TrackedBehavior>(1, nullptr), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);

    ASSERT_EQ(entity.AddBehavior(1, new TrackedBehavior(10), "primary"), LLBC_OK);
    ASSERT_EQ(entity.AddBehavior(2, new TrackedBehavior(20), "secondary"), LLBC_OK);
    EXPECT_EQ(entity.GetIdBehaviorMap().size(), 2lu);
    EXPECT_EQ(entity.GetNameBehaviorMap().size(), 2lu);
    EXPECT_TRUE(entity.IsExistBehavior(1));
    EXPECT_TRUE(entity.IsExistBehavior("secondary"));
    EXPECT_FALSE(entity.IsExistBehavior(999));
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    EXPECT_FALSE(entity.IsExistBehavior(""));
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);

    EXPECT_EQ(entity.GetBehavior<TrackedBehavior>(1)->value, 10);
    EXPECT_EQ(entity.GetBehavior<TrackedBehavior>("secondary")->value, 20);
    const LLBC_Entity &constEntity = entity;
    EXPECT_EQ(constEntity.GetBehavior<TrackedBehavior>(2)->value, 20);
    EXPECT_EQ(constEntity.GetBehavior<TrackedBehavior>("primary")->value, 10);
    EXPECT_EQ(entity.GetBehavior<TrackedBehavior>(999), nullptr);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    EXPECT_EQ(entity.GetBehavior<TrackedBehavior>(""), nullptr);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(entity.GetBehavior<TrackedBehavior>("missing"), nullptr);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);

    auto *duplicateId = new TrackedBehavior(30);
    EXPECT_EQ(entity.AddBehavior(1, duplicateId, "third"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_EXIST);
    delete duplicateId;
    auto *duplicateName = new TrackedBehavior(40);
    EXPECT_EQ(entity.AddBehavior(3, duplicateName, "primary"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_EXIST);
    delete duplicateName;
    EXPECT_EQ(TrackedBehavior::destructions, 2);

    EXPECT_EQ(entity.RemoveBehavior("primary"), LLBC_OK);
    EXPECT_EQ(TrackedBehavior::destructions, 3);
    EXPECT_FALSE(entity.IsExistBehavior(1));
    EXPECT_EQ(entity.RemoveBehavior(999), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    EXPECT_EQ(entity.RemoveBehavior(""), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);

    entity.RemoveAllBehaviors();
    EXPECT_EQ(TrackedBehavior::destructions, 4);
    EXPECT_TRUE(entity.GetIdBehaviorMap().empty());
    EXPECT_TRUE(entity.GetNameBehaviorMap().empty());
}

// Const and mutable accessors must report the same property/behavior errors,
// while id-based behavior removal must unlink an optional name index before
// destroying the holder-owned behavior object.
TEST(EntityTest, HandlesConstLookupErrorsAndIdBasedBehaviorRemoval)
{
    TrackedBehavior::destructions = 0;
    LLBC_Entity entity;
    ASSERT_EQ(entity.AddProperty("value", 42), LLBC_OK);

    EXPECT_TRUE(entity.GetProperty("").Is<void>());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    const LLBC_Entity &constEntity = entity;
    EXPECT_TRUE(constEntity.GetProperty("").Is<void>());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(constEntity.GetProperty("value").As<int>(), 42);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_SUCCESS);

    ASSERT_EQ(entity.AddBehavior(10, new TrackedBehavior(10)), LLBC_OK);
    ASSERT_EQ(entity.AddBehavior(11, new TrackedBehavior(11), "named"), LLBC_OK);
    EXPECT_EQ(constEntity.GetBehavior<TrackedBehavior>(999), nullptr);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    EXPECT_EQ(constEntity.GetBehavior<TrackedBehavior>(""), nullptr);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(constEntity.GetBehavior<TrackedBehavior>("missing"), nullptr);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);

    EXPECT_EQ(entity.RemoveBehavior(10), LLBC_OK);
    EXPECT_EQ(TrackedBehavior::destructions, 1);
    EXPECT_EQ(entity.RemoveBehavior(11), LLBC_OK);
    EXPECT_EQ(TrackedBehavior::destructions, 2);
    EXPECT_FALSE(entity.IsExistBehavior("named"));
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    EXPECT_EQ(entity.RemoveBehavior("missing"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
}
