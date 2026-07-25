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

#include <atomic>
#include <chrono>
#include <map>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/include/llbc/core/objpool/ObjPool.h
// @coverage-target: llbc/include/llbc/core/objpool/ObjPoolInl.h
// @coverage-target: llbc/include/llbc/core/objpool/ThreadSpecObjPool.h
// @coverage-target: llbc/src/core/objpool/ThreadSpecObjPool.cpp

namespace
{

struct PlainObject
{
    int value = 7;
};

struct LowerClearObject
{
    void clear() { ++calls; }

    int calls = 0;
};

struct UpperClearObject
{
    void Clear() { ++calls; }

    int calls = 0;
};

struct LowerResetObject
{
    void reset() { ++calls; }

    int calls = 0;
};

struct UpperResetObject
{
    void Reset() { ++calls; }

    int calls = 0;
};

struct LowerReuseObject
{
    void reuse() { ++calls; }

    int calls = 0;
};

struct UpperReuseObject
{
    void Reuse() { ++calls; }

    int calls = 0;
};

class NonReusableObject final
{
public:
    NonReusableObject()
    {
        ++constructions;
    }

    ~NonReusableObject()
    {
        ++destructions;
    }

    static inline int constructions = 0;
    static inline int destructions = 0;

    int value = 0;
};

class SmallStripeReusableObject final
{
public:
    SmallStripeReusableObject()
    {
        ++constructions;
    }

    ~SmallStripeReusableObject()
    {
        ++destructions;
    }

    static size_t GetStripeCapacity()
    {
        ++stripeCapacityRequests;
        return 2;
    }

    void Reuse()
    {
        ++reuses;
        value = 0;
    }

    static inline int constructions = 0;
    static inline int destructions = 0;
    static inline int reuses = 0;
    static inline int stripeCapacityRequests = 0;

    int value = 0;
};

class DeepCollectObject final
{
public:
    DeepCollectObject()
    {
        ++constructions;
    }

    ~DeepCollectObject()
    {
        ++destructions;
    }

    static size_t GetStripeCapacity()
    {
        return 2;
    }

    void Reuse()
    {
        ++reuses;
    }

    static inline int constructions = 0;
    static inline int destructions = 0;
    static inline int reuses = 0;
};

class MultiStripeCollectObject final
{
public:
    static size_t GetStripeCapacity()
    {
        return 2;
    }

    void Reuse()
    {
    }
};

class OutstandingReusableObject final
{
public:
    ~OutstandingReusableObject()
    {
        ++destructions;
    }

    void Reuse()
    {
        ++reuses;
    }

    static inline int reuses = 0;
    static inline int destructions = 0;
};

class GuardedObject final
{
public:
    void Reuse()
    {
        ++reuses;
        value = 0;
    }

    static inline int reuses = 0;

    int value = 0;
};

class PoolOnlyObject final : public LLBC_PoolObj
{
public:
    ~PoolOnlyObject() override
    {
        ++destructions;
    }

    static inline int destructions = 0;
};

class ReflectivePoolObject final
{
public:
    ~ReflectivePoolObject()
    {
        ++destructions;
    }

    static size_t GetStripeCapacity()
    {
        return 1;
    }

    LLBC_TypedObjPool<ReflectivePoolObject> *GetTypedObjPool() const
    {
        return _typedObjPool;
    }

    void SetTypedObjPool(LLBC_TypedObjPool<ReflectivePoolObject> *typedObjPool)
    {
        _typedObjPool = typedObjPool;
    }

    static void OnTypedObjPoolCreated(LLBC_TypedObjPool<ReflectivePoolObject> *typedObjPool)
    {
        ++createdEvents;
        createdPool = typedObjPool;
    }

    void Reuse()
    {
        ++reuses;
    }

    static inline int createdEvents = 0;
    static inline int destructions = 0;
    static inline int reuses = 0;
    static inline LLBC_TypedObjPool<ReflectivePoolObject> *createdPool = nullptr;

private:
    LLBC_TypedObjPool<ReflectivePoolObject> *_typedObjPool = nullptr;
};

class RecycledObject final
{
public:
    ~RecycledObject()
    {
        ++destructions;
    }

    static inline int destructions = 0;
};

class AutoReleasedObject final : public LLBC_Object
{
public:
    ~AutoReleasedObject() override
    {
        ++destructions;
    }

    static inline int destructions = 0;
};

class DispatchBase
{
public:
    virtual ~DispatchBase() = default;
};

class DispatchDerived final : public DispatchBase
{
};

class OrderedA final
{
public:
    ~OrderedA()
    {
        destructionOrder.push_back('A');
    }

    void Reuse() {}

    static inline std::vector<char> destructionOrder;
};

class OrderedB final
{
public:
    ~OrderedB()
    {
        OrderedA::destructionOrder.push_back('B');
    }

    void Reuse() {}
};

class OrderedC final
{
public:
    ~OrderedC()
    {
        OrderedA::destructionOrder.push_back('C');
    }

    void Reuse() {}
};

class OrderedD final
{
public:
    void Reuse() {}
};

class OrderedE final
{
public:
    void Reuse() {}
};

class OrderedF final
{
public:
    void Reuse() {}
};

class ThreadSafeObject final
{
public:
    void Reuse()
    {
        reuses.fetch_add(1, std::memory_order_relaxed);
        value = 0;
    }

    static inline std::atomic<int> reuses {0};

    int value = 0;
};

class ThreadSpecObject final
{
public:
    void Reuse()
    {
        ++reuses;
        value = 0;
    }

    static inline int reuses = 0;

    int value = 0;
};

class ThreadSpecProbeTask final : public LLBC_Task
{
public:
    void Svc() override
    {
        safePoolAvailable.store(LLBC_ThreadSpecObjPool::GetSafeObjPool() != nullptr,
                                std::memory_order_release);
        unsafePoolAvailable.store(LLBC_ThreadSpecObjPool::GetUnsafeObjPool() != nullptr,
                                  std::memory_order_release);

        LLBC_ThreadSpecObjPool::Finalize();
        safePoolAvailableAfterFinalize.store(LLBC_ThreadSpecObjPool::GetSafeObjPool() != nullptr,
                                             std::memory_order_release);

        initializeResult.store(LLBC_ThreadSpecObjPool::Initialize(), std::memory_order_release);
        initializeError.store(LLBC_GetLastError(), std::memory_order_release);
    }

    void Cleanup() override
    {
    }

    std::atomic<bool> safePoolAvailable {false};
    std::atomic<bool> unsafePoolAvailable {false};
    std::atomic<bool> safePoolAvailableAfterFinalize {false};
    std::atomic<int> initializeResult {LLBC_OK};
    std::atomic<int> initializeError {LLBC_ERROR_SUCCESS};
};

std::string ToStdString(const LLBC_String &value)
{
    return std::string(value.c_str(), value.size());
}

const LLBC_Json::Value &GetOnlyTypedPoolStatistics(
    const LLBC_String &statistics,
    LLBC_Json::Document &document)
{
    document.Parse(statistics.c_str());
    EXPECT_FALSE(document.HasParseError()) << statistics;
    EXPECT_TRUE(document.IsObject());
    EXPECT_TRUE(document.HasMember("typed_obj_pools"));
    EXPECT_TRUE(document["typed_obj_pools"].IsArray());
    EXPECT_EQ(document["typed_obj_pools"].Size(), 1u);

    return document["typed_obj_pools"][0];
}

template <typename Obj>
void CopyAssignGuard(LLBC_GuardedPoolObj<Obj> &target,
                     LLBC_GuardedPoolObj<Obj> &source)
{
    target = source;
}

template <typename Obj>
void MoveAssignGuard(LLBC_GuardedPoolObj<Obj> &target,
                     LLBC_GuardedPoolObj<Obj> &&source)
{
    target = std::move(source);
}

} // namespace

// The reflector recognizes six reset conventions plus the STL-container
// specializations. Reuse invokes the detected method while unsupported types
// remain untouched.
TEST(ObjPoolReflectorTest, DetectsReuseConventionsAndContainerSpecializations)
{
    EXPECT_FALSE(LLBC_ObjReflector::IsReusable<PlainObject>());
    EXPECT_TRUE(LLBC_ObjReflector::IsReusable<LowerClearObject>());
    EXPECT_TRUE(LLBC_ObjReflector::IsReusable<UpperClearObject>());
    EXPECT_TRUE(LLBC_ObjReflector::IsReusable<LowerResetObject>());
    EXPECT_TRUE(LLBC_ObjReflector::IsReusable<UpperResetObject>());
    EXPECT_TRUE(LLBC_ObjReflector::IsReusable<LowerReuseObject>());
    EXPECT_TRUE(LLBC_ObjReflector::IsReusable<UpperReuseObject>());
    EXPECT_TRUE(LLBC_ObjReflector::IsReusable<std::vector<int>>());
    EXPECT_TRUE((LLBC_ObjReflector::IsReusable<std::map<int, int>>()));
    EXPECT_TRUE((LLBC_ObjReflector::IsReusable<std::unordered_map<int, int>>()));

    PlainObject plain;
    LLBC_ObjReflector::Reuse<PlainObject>(&plain);
    EXPECT_EQ(plain.value, 7);

    LowerClearObject lowerClear;
    UpperClearObject upperClear;
    LowerResetObject lowerReset;
    UpperResetObject upperReset;
    LowerReuseObject lowerReuse;
    UpperReuseObject upperReuse;
    LLBC_ObjReflector::Reuse<LowerClearObject>(&lowerClear);
    LLBC_ObjReflector::Reuse<UpperClearObject>(&upperClear);
    LLBC_ObjReflector::Reuse<LowerResetObject>(&lowerReset);
    LLBC_ObjReflector::Reuse<UpperResetObject>(&upperReset);
    LLBC_ObjReflector::Reuse<LowerReuseObject>(&lowerReuse);
    LLBC_ObjReflector::Reuse<UpperReuseObject>(&upperReuse);
    EXPECT_EQ(lowerClear.calls, 1);
    EXPECT_EQ(upperClear.calls, 1);
    EXPECT_EQ(lowerReset.calls, 1);
    EXPECT_EQ(upperReset.calls, 1);
    EXPECT_EQ(lowerReuse.calls, 1);
    EXPECT_EQ(upperReuse.calls, 1);

    std::vector<int> vector {1, 2};
    std::map<int, int> map {{1, 2}};
    std::unordered_map<int, int> unorderedMap {{1, 2}};
    LLBC_ObjReflector::Reuse<std::vector<int>>(&vector);
    LLBC_ObjReflector::Reuse<std::map<int, int>>(&map);
    LLBC_ObjReflector::Reuse<std::unordered_map<int, int>>(&unorderedMap);
    EXPECT_TRUE(vector.empty());
    EXPECT_TRUE(map.empty());
    EXPECT_TRUE(unorderedMap.empty());
}

// A non-reusable object is destructed on each Release(), but the stripe slot is
// retained and reconstructed in place the next time the type is acquired.
TEST(ObjPoolTest, ReconstructsNonReusableObjectsInRetainedStripeMemory)
{
    NonReusableObject::constructions = 0;
    NonReusableObject::destructions = 0;

    LLBC_ObjPool pool;
    auto *first = pool.Acquire<NonReusableObject>();
    ASSERT_NE(first, nullptr);
    first->value = 42;
    EXPECT_EQ(NonReusableObject::constructions, 1);
    EXPECT_EQ(pool.GetTypedObjPool<NonReusableObject>()->GetObjPool(), &pool);
    pool.Release(first);
    EXPECT_EQ(NonReusableObject::destructions, 1);

    auto *second = pool.Acquire<NonReusableObject>();
    ASSERT_NE(second, nullptr);
    #if !LLBC_CFG_CORE_OBJPOOL_USE_MALLOC_INSTEAD
    EXPECT_EQ(second, first);
    #endif
    EXPECT_EQ(second->value, 0);
    EXPECT_EQ(NonReusableObject::constructions, 2);
    pool.Release(second);
    EXPECT_EQ(NonReusableObject::destructions, 2);
}

// Stripe capacity reflection, statistics, shallow collection, and deep
// collection together describe the object pool's memory-pool semantics.
TEST(ObjPoolTest, ReportsReusableStateAndCollectsStripes)
{
    SmallStripeReusableObject::constructions = 0;
    SmallStripeReusableObject::destructions = 0;
    SmallStripeReusableObject::reuses = 0;
    SmallStripeReusableObject::stripeCapacityRequests = 0;

    LLBC_ObjPool pool;
    EXPECT_NE(pool.GetName().find("ObjPool_"), static_cast<LLBC_String::size_type>(-1));
    pool.SetName("small-stripe-unit-pool");
    EXPECT_EQ(pool.GetName(), "small-stripe-unit-pool");

    LLBC_Json::Document emptyStatistics;
    const LLBC_String emptyJson = pool.GetStatistics(LLBC_ObjPoolStatFormat::Json);
    emptyStatistics.Parse(emptyJson.c_str());
    ASSERT_FALSE(emptyStatistics.HasParseError());
    ASSERT_TRUE(emptyStatistics["typed_obj_pools"].IsArray());
    EXPECT_EQ(emptyStatistics["typed_obj_pools"].Size(), 0u);

    auto *first = pool.Acquire<SmallStripeReusableObject>();
    auto *second = pool.Acquire<SmallStripeReusableObject>();
    auto *third = pool.Acquire<SmallStripeReusableObject>();
    ASSERT_NE(first, nullptr);
    ASSERT_NE(second, nullptr);
    ASSERT_NE(third, nullptr);
    EXPECT_NE(first, second);
    EXPECT_NE(first, third);
    EXPECT_NE(second, third);
    first->value = 1;
    second->value = 2;
    third->value = 3;

    LLBC_Json::Document statistics;
    const auto &typedStatistics = GetOnlyTypedPoolStatistics(
        pool.GetStatistics(LLBC_ObjPoolStatFormat::Json), statistics);
    EXPECT_TRUE(typedStatistics["reusable"].GetBool());
    EXPECT_EQ(typedStatistics["obj_count_per_stripe"].GetUint(), 2u);
    EXPECT_EQ(typedStatistics["stripe_count"].GetUint(), 2u);
    EXPECT_EQ(typedStatistics["obj_count"].GetUint(), 4u);
    EXPECT_EQ(typedStatistics["using_obj_count"].GetUint(), 3u);
    EXPECT_EQ(typedStatistics["reusable_obj_count"].GetUint(), 0u);

    const std::string csv = ToStdString(pool.GetStatistics(LLBC_ObjPoolStatFormat::CSV));
    EXPECT_NE(csv.find("pool_name;name;reusable;"), std::string::npos);
    EXPECT_NE(csv.find("small-stripe-unit-pool"), std::string::npos);
    const std::string csvWithoutHead =
        ToStdString(pool.GetStatistics(LLBC_ObjPoolStatFormat::CSVWithoutHead));
    EXPECT_EQ(csvWithoutHead.find("pool_name;name;reusable;"), std::string::npos);
    EXPECT_NE(csvWithoutHead.find("small-stripe-unit-pool"), std::string::npos);
    EXPECT_NE(ToStdString(pool.GetStatistics(LLBC_ObjPoolStatFormat::PrettyJson)).find('\n'),
              std::string::npos);

    pool.Release(first);
    pool.Release(second);
    pool.Release(third);
    EXPECT_EQ(SmallStripeReusableObject::reuses, 3);

    auto *reissued = pool.Acquire<SmallStripeReusableObject>();
    ASSERT_NE(reissued, nullptr);
    EXPECT_EQ(reissued->value, 0);
    pool.Release(reissued);
    EXPECT_EQ(SmallStripeReusableObject::reuses, 4);

    pool.Collect<SmallStripeReusableObject>(false);
    LLBC_Json::Document afterShallowCollect;
    const auto &shallowStatistics = GetOnlyTypedPoolStatistics(
        pool.GetStatistics(LLBC_ObjPoolStatFormat::Json), afterShallowCollect);
    EXPECT_EQ(shallowStatistics["stripe_count"].GetUint(), 2u);
    EXPECT_EQ(shallowStatistics["obj_count"].GetUint(), 4u);
    EXPECT_EQ(shallowStatistics["using_obj_count"].GetUint(), 0u);
    EXPECT_EQ(shallowStatistics["reusable_obj_count"].GetUint(), 0u);
    EXPECT_EQ(shallowStatistics["free_obj_count"].GetUint(), 4u);
    EXPECT_EQ(SmallStripeReusableObject::constructions, 3);
    EXPECT_EQ(SmallStripeReusableObject::destructions, 3);

    pool.Collect(true);
    LLBC_Json::Document afterDeepCollect;
    const auto &deepStatistics = GetOnlyTypedPoolStatistics(
        pool.GetStatistics(LLBC_ObjPoolStatFormat::Json), afterDeepCollect);
    EXPECT_EQ(deepStatistics["stripe_count"].GetUint(), 0u);
    EXPECT_EQ(deepStatistics["obj_count"].GetUint(), 0u);
    EXPECT_EQ(deepStatistics["free_obj_count"].GetUint(), 0u);
    EXPECT_GE(SmallStripeReusableObject::stripeCapacityRequests, 2);
}

// Deep collection must preserve a stripe with a live object while unlinking an
// older free stripe from the middle of the reusable-stripe chain.
TEST(ObjPoolTest, DeepCollectionKeepsLiveStripeAndUnlinksOlderFreeStripe)
{
    DeepCollectObject::constructions = 0;
    DeepCollectObject::destructions = 0;
    DeepCollectObject::reuses = 0;

    LLBC_ObjPool pool;
    auto *a = pool.Acquire<DeepCollectObject>();
    auto *b = pool.Acquire<DeepCollectObject>();
    auto *c = pool.Acquire<DeepCollectObject>();
    auto *d = pool.Acquire<DeepCollectObject>();
    ASSERT_NE(a, nullptr);
    ASSERT_NE(b, nullptr);
    ASSERT_NE(c, nullptr);
    ASSERT_NE(d, nullptr);

    pool.Release(a);
    pool.Release(b);
    pool.Release(c);
    auto *e = pool.Acquire<DeepCollectObject>();
    ASSERT_NE(e, nullptr);
    #if !LLBC_CFG_CORE_OBJPOOL_USE_MALLOC_INSTEAD
    EXPECT_EQ(e, c);
    #endif
    pool.Release(d);

    pool.Collect(true);
    LLBC_Json::Document afterFirstCollect;
    const auto &firstStatistics = GetOnlyTypedPoolStatistics(
        pool.GetStatistics(LLBC_ObjPoolStatFormat::Json), afterFirstCollect);
    EXPECT_EQ(firstStatistics["stripe_count"].GetUint(), 1u);
    EXPECT_EQ(firstStatistics["obj_count"].GetUint(), 2u);
    EXPECT_EQ(firstStatistics["using_obj_count"].GetUint(), 1u);
    EXPECT_EQ(firstStatistics["reusable_obj_count"].GetUint(), 0u);
    EXPECT_EQ(DeepCollectObject::constructions, 4);
    EXPECT_EQ(DeepCollectObject::destructions, 3);

    pool.Release(e);
    pool.Collect(true);
    LLBC_Json::Document afterSecondCollect;
    const auto &secondStatistics = GetOnlyTypedPoolStatistics(
        pool.GetStatistics(LLBC_ObjPoolStatFormat::Json), afterSecondCollect);
    EXPECT_EQ(secondStatistics["stripe_count"].GetUint(), 0u);
    EXPECT_EQ(secondStatistics["obj_count"].GetUint(), 0u);
    EXPECT_EQ(DeepCollectObject::reuses, 5);
    EXPECT_EQ(DeepCollectObject::destructions, 4);
}

// A longer free-stripe chain exercises deep collection's internal unlink walk:
// only the oldest stripe is completely idle, while newer stripes retain one
// live object each.
TEST(ObjPoolTest, DeepCollectionUnlinksAnOlderStripePastMultipleLiveStripes)
{
    LLBC_ObjPool pool;
    auto *a = pool.Acquire<MultiStripeCollectObject>();
    auto *b = pool.Acquire<MultiStripeCollectObject>();
    auto *c = pool.Acquire<MultiStripeCollectObject>();
    auto *d = pool.Acquire<MultiStripeCollectObject>();
    auto *e = pool.Acquire<MultiStripeCollectObject>();
    auto *f = pool.Acquire<MultiStripeCollectObject>();
    ASSERT_NE(a, nullptr);
    ASSERT_NE(b, nullptr);
    ASSERT_NE(c, nullptr);
    ASSERT_NE(d, nullptr);
    ASSERT_NE(e, nullptr);
    ASSERT_NE(f, nullptr);

    pool.Release(a);
    pool.Release(b);
    pool.Release(c);
    pool.Release(e);
    pool.Collect(true);

    LLBC_Json::Document afterFirstCollect;
    const auto &firstStatistics = GetOnlyTypedPoolStatistics(
        pool.GetStatistics(LLBC_ObjPoolStatFormat::Json), afterFirstCollect);
    EXPECT_EQ(firstStatistics["stripe_count"].GetUint(), 2u);
    EXPECT_EQ(firstStatistics["using_obj_count"].GetUint(), 2u);

    pool.Release(d);
    pool.Release(f);
    pool.Collect(true);
    LLBC_Json::Document afterSecondCollect;
    const auto &secondStatistics = GetOnlyTypedPoolStatistics(
        pool.GetStatistics(LLBC_ObjPoolStatFormat::Json), afterSecondCollect);
    EXPECT_EQ(secondStatistics["stripe_count"].GetUint(), 0u);
}

// Pool destruction also cleans up an outstanding reusable object. Reuse runs
// before destruction so reusable resources receive the same reset callback at
// shutdown that they receive on ordinary Release().
TEST(ObjPoolTest, DestructionReusesAndDestroysOutstandingReusableObjects)
{
    OutstandingReusableObject::reuses = 0;
    OutstandingReusableObject::destructions = 0;
    {
        LLBC_ObjPool pool;
        ASSERT_NE(pool.Acquire<OutstandingReusableObject>(), nullptr);
    }

    EXPECT_EQ(OutstandingReusableObject::reuses, 1);
    EXPECT_EQ(OutstandingReusableObject::destructions, 1);
}

// Guarded handles transfer sole release ownership through their intentionally
// move-like copy operations, while the pool can dispatch a base pointer to its
// owning typed pool even when Release() is called on another pool instance.
TEST(ObjPoolTest, GuardsTransferOwnershipAndReleaseDispatchesToOwningPool)
{
    GuardedObject::reuses = 0;

    LLBC_ObjPool pool;
    auto *typedPool = pool.GetTypedObjPool<GuardedObject>();
    {
        auto first = pool.AcquireGuarded<GuardedObject>();
        ASSERT_TRUE(first);
        ASSERT_EQ(first.GetTypedObjPool(), typedPool);
        first->value = 42;
        EXPECT_EQ((*first).value, 42);

        auto copied = first;
        EXPECT_FALSE(first);
        ASSERT_TRUE(copied);

        auto moved = std::move(copied);
        EXPECT_FALSE(copied);
        ASSERT_TRUE(moved);

        auto replacement = pool.AcquireGuarded<GuardedObject>();
        ASSERT_TRUE(replacement);
        replacement = moved;
        EXPECT_FALSE(moved);
        ASSERT_TRUE(replacement);
        CopyAssignGuard(replacement, replacement);
        MoveAssignGuard(replacement, std::move(replacement));
        ASSERT_TRUE(replacement);

        auto moveTarget = pool.AcquireGuarded<GuardedObject>();
        auto moveSource = pool.AcquireGuarded<GuardedObject>();
        ASSERT_TRUE(moveTarget);
        ASSERT_TRUE(moveSource);
        MoveAssignGuard(moveTarget, std::move(moveSource));
        ASSERT_TRUE(moveTarget);
        EXPECT_FALSE(moveSource);

        auto detachedGuard = pool.AcquireGuarded<GuardedObject>();
        auto *detached = detachedGuard.Detach();
        EXPECT_FALSE(detachedGuard);
        EXPECT_EQ(detachedGuard.GetTypedObjPool(), nullptr);
        ASSERT_NE(detached, nullptr);
        pool.Release(detached);

        auto resetGuard = pool.AcquireGuarded<GuardedObject>();
        resetGuard.Reset();
        EXPECT_FALSE(resetGuard);
        resetGuard.Reset();
        EXPECT_FALSE(resetGuard);
    }
    EXPECT_GE(GuardedObject::reuses, 4);

    LLBC_ObjPool ownerPool;
    LLBC_ObjPool dispatchPool;
    DispatchBase *base = ownerPool.Acquire<DispatchDerived>();
    ASSERT_NE(base, nullptr);
    dispatchPool.Release(base);
    auto *reacquired = ownerPool.Acquire<DispatchDerived>();
    ASSERT_NE(reacquired, nullptr);
    #if !LLBC_CFG_CORE_OBJPOOL_USE_MALLOC_INSTEAD
    EXPECT_EQ(reacquired, base);
    #endif
    ownerPool.Release(reacquired);
}

// Pool reflection supports both LLBC_PoolObj and compatible user types. The
// latter verifies that GetTypedObjPool() returns the typed sub-pool correctly,
// and Recycle() returns pooled objects or deletes ordinary heap objects.
TEST(ObjPoolReflectorTest, ReflectsCustomPoolOwnershipAndRecyclesObjects)
{
    ReflectivePoolObject::createdEvents = 0;
    ReflectivePoolObject::createdPool = nullptr;
    ReflectivePoolObject::destructions = 0;
    ReflectivePoolObject::reuses = 0;
    PoolOnlyObject::destructions = 0;
    RecycledObject::destructions = 0;
    AutoReleasedObject::destructions = 0;

    EXPECT_TRUE(LLBC_ObjReflector::IsSupportedObjPoolReflection<PoolOnlyObject>());
    EXPECT_TRUE(LLBC_ObjReflector::IsSupportedObjPoolReflection<ReflectivePoolObject>());
    EXPECT_FALSE(LLBC_ObjReflector::IsSupportedObjPoolReflection<RecycledObject>());

    PoolOnlyObject *nullPoolObject = nullptr;
    RecycledObject *nullRecycledObject = nullptr;
    LLBC_ObjReflector::Recycle(nullPoolObject);
    LLBC_ObjReflector::Recycle(nullRecycledObject);
    LLBC_ObjReflector::RecycleX(nullRecycledObject);
    EXPECT_EQ(nullPoolObject, nullptr);
    EXPECT_EQ(nullRecycledObject, nullptr);

    LLBC_ObjPool pool;
    auto *typedPool = pool.GetTypedObjPool<ReflectivePoolObject>();
    ASSERT_NE(typedPool, nullptr);
    EXPECT_EQ(ReflectivePoolObject::createdEvents, 1);
    EXPECT_EQ(ReflectivePoolObject::createdPool, typedPool);
    EXPECT_EQ(typedPool->GetObjPool(), &pool);

    auto *reflective = pool.Acquire<ReflectivePoolObject>();
    ASSERT_NE(reflective, nullptr);
    EXPECT_EQ(reflective->GetTypedObjPool(), typedPool);
    EXPECT_EQ(LLBC_ObjReflector::GetTypedObjPool<ReflectivePoolObject>(reflective), typedPool);
    LLBC_ObjReflector::Recycle(reflective);
    EXPECT_EQ(ReflectivePoolObject::reuses, 1);
    auto *reused = pool.Acquire<ReflectivePoolObject>();
    #if !LLBC_CFG_CORE_OBJPOOL_USE_MALLOC_INSTEAD
    EXPECT_EQ(reused, reflective);
    #endif
    LLBC_ObjReflector::RecycleX(reused);
    EXPECT_EQ(reused, nullptr);

    auto *standaloneReflective = new ReflectivePoolObject;
    LLBC_ObjReflector::Recycle(standaloneReflective);
    EXPECT_EQ(ReflectivePoolObject::destructions, 1);

    auto *pooledBase = pool.Acquire<PoolOnlyObject>();
    ASSERT_NE(pooledBase, nullptr);
    EXPECT_EQ(LLBC_ObjReflector::GetTypedObjPool<PoolOnlyObject>(pooledBase),
              pool.GetTypedObjPool<PoolOnlyObject>());
    LLBC_ObjReflector::Recycle(pooledBase);
    EXPECT_EQ(PoolOnlyObject::destructions, 1);
    LLBC_ObjReflector::Recycle(new PoolOnlyObject);
    EXPECT_EQ(PoolOnlyObject::destructions, 2);

    LLBC_ObjReflector::Recycle(new RecycledObject);
    EXPECT_EQ(RecycledObject::destructions, 1);

    {
        LLBC_AutoReleasePool autoReleasePool;
        auto *autoReleased = new AutoReleasedObject;
        LLBC_ObjReflector::Recycle(autoReleased);
        EXPECT_EQ(autoReleased->GetAutoRefCount(), 1);
        EXPECT_EQ(AutoReleasedObject::destructions, 0);
        autoReleasePool.Purge();
    }
    EXPECT_EQ(AutoReleasedObject::destructions, 1);

    LLBC_PoolObj source;
    source.SetTypedObjPool(&pool);
    LLBC_PoolObj copied(source);
    EXPECT_EQ(copied.GetTypedObjPool(), nullptr);
    LLBC_PoolObj assigned;
    assigned.SetTypedObjPool(&pool);
    assigned = source;
    EXPECT_EQ(assigned.GetTypedObjPool(), &pool);
    LLBC_PoolObj moved(std::move(source));
    EXPECT_EQ(moved.GetTypedObjPool(), nullptr);
    assigned = std::move(moved);
    EXPECT_EQ(assigned.GetTypedObjPool(), &pool);
}

// The framework's frequently allocated Event, Stream, MessageBlock, Packet,
// and LogData types opt into reflection/reuse. Pooling them together verifies
// that their typed sub-pools coexist and can be collected as one unit.
TEST(ObjPoolTest, PoolsBuiltInReusableFrameworkTypes)
{
    EXPECT_TRUE(LLBC_ObjReflector::IsReusable<LLBC_Event>());
    EXPECT_TRUE(LLBC_ObjReflector::IsReusable<LLBC_Stream>());
    EXPECT_TRUE(LLBC_ObjReflector::IsReusable<LLBC_MessageBlock>());
    EXPECT_TRUE(LLBC_ObjReflector::IsReusable<LLBC_Packet>());
    EXPECT_TRUE(LLBC_ObjReflector::IsReusable<LLBC_LogData>());

    LLBC_ObjPool pool;
    auto *event = pool.Acquire<LLBC_Event>();
    auto *stream = pool.Acquire<LLBC_Stream>();
    auto *messageBlock = pool.Acquire<LLBC_MessageBlock>();
    auto *packet = pool.Acquire<LLBC_Packet>();
    auto *logData = pool.Acquire<LLBC_LogData>();
    ASSERT_NE(event, nullptr);
    ASSERT_NE(stream, nullptr);
    ASSERT_NE(messageBlock, nullptr);
    ASSERT_NE(packet, nullptr);
    ASSERT_NE(logData, nullptr);

    event->SetId(9);
    *stream << 42;
    const char payload[] = "msg";
    ASSERT_EQ(messageBlock->Write(payload, sizeof(payload) - 1), LLBC_OK);
    packet->SetOpcode(18);
    logData->level = LLBC_LogLevel::Info;

    pool.Release(event);
    pool.Release(stream);
    pool.Release(messageBlock);
    pool.Release(packet);
    pool.Release(logData);

    LLBC_Json::Document statistics;
    statistics.Parse(pool.GetStatistics(LLBC_ObjPoolStatFormat::Json).c_str());
    ASSERT_FALSE(statistics.HasParseError());
    ASSERT_TRUE(statistics["typed_obj_pools"].IsArray());
    EXPECT_EQ(statistics["typed_obj_pools"].Size(), 5u);
    pool.Collect(true);
}

// Ordered-deletion constraints form a forest of type dependencies. They reject
// self/cyclic constraints, normalize merged trees, and apply the declared
// pre-order when collecting constructed reusable objects.
TEST(ObjPoolTest, OrdersCollectionAndDestructionByTypeDependencies)
{
    LLBC_ObjPool graphPool;
    EXPECT_EQ(graphPool.GetOrderedDeleteTree(), "[]");
    EXPECT_EQ((graphPool.EnsureDeletionBefore<OrderedA, OrderedA>()), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_ALLOW);
    EXPECT_EQ((graphPool.EnsureDeletionBefore<OrderedA, OrderedB>()), LLBC_OK);
    EXPECT_EQ((graphPool.EnsureDeletionBefore<OrderedA, OrderedC>()), LLBC_OK);
    EXPECT_EQ((graphPool.EnsureDeletionBefore<OrderedD, OrderedA>()), LLBC_OK);
    EXPECT_EQ((graphPool.EnsureDeletionBefore<OrderedE, OrderedF>()), LLBC_OK);
    EXPECT_EQ((graphPool.EnsureDeletionBefore<OrderedB, OrderedE>()), LLBC_OK);
    EXPECT_EQ((graphPool.EnsureDeletionBefore<OrderedB, OrderedE>()), LLBC_OK);
    EXPECT_EQ((graphPool.EnsureDeletionBefore<OrderedF, OrderedD>()), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_ALLOW);

    const std::string graph = ToStdString(graphPool.GetOrderedDeleteTree());
    EXPECT_NE(graph.find(typeid(OrderedA).name()), std::string::npos);
    EXPECT_NE(graph.find(typeid(OrderedF).name()), std::string::npos);
    EXPECT_NE(ToStdString(graphPool.GetOrderedDeleteTree(true)).find('\n'), std::string::npos);

    // Insert a new predecessor before a non-root node that already has a
    // successor. This drives the reparenting path used to maintain the tree.
    LLBC_ObjPool reparentPool;
    ASSERT_EQ((reparentPool.EnsureDeletionBefore<OrderedA, OrderedB>()), LLBC_OK);
    ASSERT_EQ((reparentPool.EnsureDeletionBefore<OrderedB, OrderedC>()), LLBC_OK);
    ASSERT_EQ((reparentPool.EnsureDeletionBefore<OrderedD, OrderedB>()), LLBC_OK);
    const std::string reparentedTree = ToStdString(reparentPool.GetOrderedDeleteTree());
    EXPECT_NE(reparentedTree.find(typeid(OrderedA).name()), std::string::npos);
    EXPECT_NE(reparentedTree.find(typeid(OrderedB).name()), std::string::npos);
    EXPECT_NE(reparentedTree.find(typeid(OrderedC).name()), std::string::npos);
    EXPECT_NE(reparentedTree.find(typeid(OrderedD).name()), std::string::npos);

    // Merge two trees through a non-root destination, then reorder siblings in
    // one tree. These operations cover both root-merge and same-root paths.
    LLBC_ObjPool mergePool;
    ASSERT_EQ((mergePool.EnsureDeletionBefore<OrderedA, OrderedB>()), LLBC_OK);
    ASSERT_EQ((mergePool.EnsureDeletionBefore<OrderedB, OrderedC>()), LLBC_OK);
    ASSERT_EQ((mergePool.EnsureDeletionBefore<OrderedD, OrderedE>()), LLBC_OK);
    ASSERT_EQ((mergePool.EnsureDeletionBefore<OrderedB, OrderedE>()), LLBC_OK);
    EXPECT_NE(ToStdString(mergePool.GetOrderedDeleteTree()).find(typeid(OrderedE).name()),
              std::string::npos);

    LLBC_ObjPool sameRootPool;
    ASSERT_EQ((sameRootPool.EnsureDeletionBefore<OrderedA, OrderedB>()), LLBC_OK);
    ASSERT_EQ((sameRootPool.EnsureDeletionBefore<OrderedA, OrderedC>()), LLBC_OK);
    ASSERT_EQ((sameRootPool.EnsureDeletionBefore<OrderedB, OrderedC>()), LLBC_OK);
    EXPECT_NE(ToStdString(sameRootPool.GetOrderedDeleteTree()).find(typeid(OrderedC).name()),
              std::string::npos);

    OrderedA::destructionOrder.clear();
    {
        LLBC_ObjPool pool;
        ASSERT_EQ((pool.EnsureDeletionBefore<OrderedA, OrderedB>()), LLBC_OK);
        ASSERT_EQ((pool.EnsureDeletionBefore<OrderedB, OrderedC>()), LLBC_OK);
        pool.Release(pool.Acquire<OrderedA>());
        pool.Release(pool.Acquire<OrderedB>());
        pool.Release(pool.Acquire<OrderedC>());
        pool.Collect(false);
    }
    EXPECT_EQ(OrderedA::destructionOrder, std::vector<char>({'A', 'B', 'C'}));
}

// A thread-safe pool serializes lazy typed-pool creation and repeated
// acquire/release operations across native threads without sharing user state.
TEST(ObjPoolTest, ThreadSafePoolHandlesConcurrentAcquireAndRelease)
{
    constexpr int threadCount = 4;
    constexpr int operationsPerThread = 128;
    ThreadSafeObject::reuses.store(0, std::memory_order_release);

    LLBC_ObjPool pool(true);
    std::atomic<int> ready {0};
    std::atomic<bool> start {false};
    std::vector<std::thread> workers;
    workers.reserve(threadCount);

    for (int threadIndex = 0; threadIndex < threadCount; ++threadIndex)
    {
        workers.emplace_back([&, threadIndex] {
            ready.fetch_add(1, std::memory_order_release);
            while (!start.load(std::memory_order_acquire))
                std::this_thread::yield();

            for (int operation = 0; operation < operationsPerThread; ++operation)
            {
                auto *object = pool.Acquire<ThreadSafeObject>();
                object->value = threadIndex * operationsPerThread + operation;
                pool.Release(object);
            }
        });
    }

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while (ready.load(std::memory_order_acquire) != threadCount &&
           std::chrono::steady_clock::now() < deadline)
        std::this_thread::yield();
    start.store(true, std::memory_order_release);
    for (auto &worker : workers)
        worker.join();

    EXPECT_EQ(ready.load(std::memory_order_acquire), threadCount);
    EXPECT_EQ(ThreadSafeObject::reuses.load(std::memory_order_acquire),
              threadCount * operationsPerThread);
    pool.Collect(true);
}

// The entry thread owns preinitialized safe/unsafe pools. A framework worker
// already has independent pools but cannot initialize entry-thread-only state.
TEST(ThreadSpecObjPoolTest, UsesEntryPoolsAndRejectsNonEntryInitialization)
{
    ThreadSpecObject::reuses = 0;
    auto *safePool = LLBC_ThreadSpecObjPool::GetSafeObjPool();
    auto *unsafePool = LLBC_ThreadSpecObjPool::GetUnsafeObjPool();
    ASSERT_NE(safePool, nullptr);
    ASSERT_NE(unsafePool, nullptr);
    EXPECT_NE(safePool, unsafePool);

    EXPECT_EQ(LLBC_ThreadSpecObjPool::Initialize(), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_REENTRY);

    auto *safe = LLBC_ThreadSpecObjPool::SafeAcquire<ThreadSpecObject>();
    auto *unsafe = LLBC_ThreadSpecObjPool::UnsafeAcquire<ThreadSpecObject>();
    ASSERT_NE(safe, nullptr);
    ASSERT_NE(unsafe, nullptr);
    EXPECT_NE(safePool->GetTypedObjPool<ThreadSpecObject>(),
              unsafePool->GetTypedObjPool<ThreadSpecObject>());
    safe->value = 1;
    unsafe->value = 2;
    LLBC_ThreadSpecObjPool::SafeRelease(safe);
    LLBC_ThreadSpecObjPool::UnsafeRelease(unsafe);

    {
        auto guardedSafe = LLBC_ThreadSpecObjPool::GuardedSafeAcquire<ThreadSpecObject>();
        auto guardedUnsafe = LLBC_ThreadSpecObjPool::GuardedUnsafeAcquire<ThreadSpecObject>();
        ASSERT_TRUE(guardedSafe);
        ASSERT_TRUE(guardedUnsafe);
        guardedSafe->value = 3;
        guardedUnsafe->value = 4;
    }
    EXPECT_EQ(ThreadSpecObject::reuses, 4);

    ThreadSpecProbeTask task;
    ASSERT_EQ(task.Activate(), LLBC_OK);
    ASSERT_EQ(task.Wait(), LLBC_OK);
    EXPECT_TRUE(task.safePoolAvailable.load(std::memory_order_acquire));
    EXPECT_TRUE(task.unsafePoolAvailable.load(std::memory_order_acquire));
    EXPECT_TRUE(task.safePoolAvailableAfterFinalize.load(std::memory_order_acquire));
    EXPECT_EQ(task.initializeResult.load(std::memory_order_acquire), LLBC_FAILED);
    EXPECT_EQ(task.initializeError.load(std::memory_order_acquire), LLBC_ERROR_NOT_ALLOW);
}
