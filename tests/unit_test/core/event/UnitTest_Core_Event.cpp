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

#include <sstream>
#include <string>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/include/llbc/core/event/EventInl.h
// @coverage-target: llbc/include/llbc/core/event/EventFirerInl.h
// @coverage-target: llbc/src/core/event/EventMgr.cpp

namespace
{

constexpr LLBC_ListenerStub InvalidListenerStub = 0;

class MemberReceiver
{
public:
    void OnEvent(LLBC_Event &event)
    {
        ++calls;
        lastValue = event["value"].As<int>();
    }

    int calls = 0;
    int lastValue = 0;
};

class CountingListener final : public LLBC_EventListener
{
public:
    ~CountingListener() override
    {
        ++destructions;
    }

    void Invoke(LLBC_Event &event) override
    {
        ++calls;
        lastValue = event["value"].As<int>();
    }

    static inline int destructions = 0;
    int calls = 0;
    int lastValue = 0;
};

class TestableEvent final : public LLBC_Event
{
public:
    using LLBC_Event::LLBC_Event;
};

} // namespace

// Events are reusable parameter carriers. Validate mixed key ownership,
// const indexed access, copy/move behavior, and external-data cleanup.
TEST(EventTest, CarriesParametersAndCleansExternalDataCorrectly)
{
    TestableEvent event(7, true);
    EXPECT_EQ(event.GetId(), 7);
    EXPECT_TRUE(event.IsDontDelAfterFire());
    event.SetId(8);
    event.SetDontDelAfterFire(false);
    EXPECT_EQ(event.GetId(), 8);
    EXPECT_FALSE(event.IsDontDelAfterFire());

    event.SetParam("number", 42);
    event.SetParam(std::string("owned-key"), LLBC_String("value"));
    event.SetParam(std::string("owned-key"), LLBC_String("updated"));
    event["level"] = 9;
    EXPECT_EQ(event.GetParam("number").As<int>(), 42);
    EXPECT_EQ(event["owned-key"].As<LLBC_String>(), "updated");
    EXPECT_EQ(event["level"].As<int>(), 9);
    event.GetParam("number") = 43;
    EXPECT_EQ(event.GetParam("number").As<int>(), 43);
    event.GetParam("number") = 42;
    const LLBC_Event &constEvent = event;
    EXPECT_TRUE(constEvent.GetParam("missing").Is<void>());
    EXPECT_TRUE(constEvent["missing"].Is<void>());
    EXPECT_EQ(event.GetParams().size(), 3lu);
    event.GetMutableParams()["level"] = 10;
    EXPECT_EQ(event["level"].As<int>(), 10);

    int clearCount = 0;
    const auto clearInt = [&](void *data) {
        ++clearCount;
        delete static_cast<int *>(data);
    };
    event.SetExtData(new int(1), clearInt);
    EXPECT_NE(event.GetExtData(), nullptr);
    event.SetExtData(new int(2), clearInt);
    EXPECT_EQ(clearCount, 1);
    event.ClearExtData(true);
    EXPECT_EQ(clearCount, 2);
    EXPECT_EQ(event.GetExtData(), nullptr);

    event.SetExtData(new int(3), clearInt);
    int *withoutDeleg = new int(4);
    event.SetExtData(withoutDeleg);
    EXPECT_EQ(clearCount, 3);
    event.ClearExtData(true);
    EXPECT_EQ(clearCount, 3);
    delete withoutDeleg;

    LLBC_Event copied(event);
    EXPECT_EQ(copied.GetId(), event.GetId());
    EXPECT_EQ(copied["owned-key"].As<LLBC_String>(), "updated");
    LLBC_Event moved(std::move(copied));
    EXPECT_EQ(moved["number"].As<int>(), 42);
    LLBC_Event assigned;
    assigned = moved;
    EXPECT_EQ(assigned["level"].As<int>(), 10);
    LLBC_Event moveAssigned;
    moveAssigned = std::move(assigned);
    EXPECT_EQ(moveAssigned["owned-key"].As<LLBC_String>(), "updated");
    LLBC_Event *selfAlias = &moveAssigned;
    moveAssigned = *selfAlias;
    moveAssigned = std::move(*selfAlias);
    EXPECT_EQ(moveAssigned["number"].As<int>(), 42);

    std::ostringstream output;
    output << moveAssigned;
    EXPECT_NE(output.str().find("LLBC_Event(id:8"), std::string::npos);

    moveAssigned.Reuse();
    EXPECT_EQ(moveAssigned.GetId(), 0);
    EXPECT_FALSE(moveAssigned.IsDontDelAfterFire());
    EXPECT_TRUE(moveAssigned.GetParams().empty());
}

// Listener registration controls event ownership and deferred mutation. A
// listener removed during dispatch is skipped immediately and actually removed
// after the outer fire completes; listeners added during dispatch become active
// on the next event.
TEST(EventTest, DispatchesListenersAndDefersMutationsDuringFire)
{
    LLBC_EventMgr manager;
    EXPECT_EQ(manager.AddListener(0, [](LLBC_Event &) {}), InvalidListenerStub);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(manager.RemoveListener(0), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(manager.RemoveListener(999999), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);

    MemberReceiver receiver;
    const LLBC_ListenerStub memberStub =
        manager.AddListener(1, &receiver, &MemberReceiver::OnEvent, 100);
    ASSERT_EQ(memberStub, 100u);
    EXPECT_EQ(manager.AddListener(1, [](LLBC_Event &) {}, 100), InvalidListenerStub);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_REPEAT);

    int firstCalls = 0;
    int secondCalls = 0;
    int lateCalls = 0;
    bool addedLateListener = false;
    LLBC_ListenerStub secondStub = InvalidListenerStub;
    const LLBC_ListenerStub firstStub = manager.AddListener(1, [&](LLBC_Event &event) {
        ++firstCalls;
        EXPECT_TRUE(manager.IsFiring());
        if (!addedLateListener)
        {
            addedLateListener = true;
            const LLBC_ListenerStub lateStub = manager.AddListener(1, [&](LLBC_Event &) {
                ++lateCalls;
            });
            EXPECT_NE(lateStub, InvalidListenerStub);
            EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_PENDING);
        }

        EXPECT_EQ(manager.RemoveListener(secondStub), LLBC_FAILED);
        EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_PENDING);
        EXPECT_EQ(event["value"].As<int>(), 5);
    });
    ASSERT_NE(firstStub, InvalidListenerStub);
    secondStub = manager.AddListener(1, [&](LLBC_Event &) {
        ++secondCalls;
    });
    ASSERT_NE(secondStub, InvalidListenerStub);

    LLBC_Event event(1, true);
    event["value"] = 5;
    ASSERT_EQ(manager.Fire(&event), LLBC_OK);
    EXPECT_FALSE(manager.IsFiring());
    EXPECT_EQ(receiver.calls, 1);
    EXPECT_EQ(receiver.lastValue, 5);
    EXPECT_EQ(firstCalls, 1);
    EXPECT_EQ(secondCalls, 0);
    EXPECT_EQ(lateCalls, 0);

    ASSERT_EQ(manager.Fire(&event), LLBC_OK);
    EXPECT_EQ(receiver.calls, 2);
    EXPECT_EQ(firstCalls, 2);
    EXPECT_EQ(secondCalls, 0);
    EXPECT_EQ(lateCalls, 1);

    LLBC_ListenerStub removableStub = firstStub;
    EXPECT_EQ(manager.RemoveListenerX(removableStub), LLBC_OK);
    EXPECT_EQ(removableStub, InvalidListenerStub);
    EXPECT_EQ(manager.RemoveListener(memberStub), LLBC_OK);
    EXPECT_EQ(manager.RemoveAllListeners(), LLBC_OK);
}

// Event managers accept polymorphic listeners, support nested dispatch, and
// defer whole-id/all-listener removal until the outermost fire returns.
TEST(EventTest, OwnsListenerObjectsAndHandlesNestedPendingRemovals)
{
    CountingListener::destructions = 0;
    LLBC_EventMgr listenerMgr;
    auto *listener = new CountingListener;
    const LLBC_ListenerStub listenerStub = listenerMgr.AddListener(2, listener);
    ASSERT_NE(listenerStub, InvalidListenerStub);

    LLBC_Event listenerEvent(2, true);
    listenerEvent["value"] = 21;
    ASSERT_EQ(listenerMgr.Fire(&listenerEvent), LLBC_OK);
    EXPECT_EQ(listener->calls, 1);
    EXPECT_EQ(listener->lastValue, 21);
    EXPECT_EQ(listenerMgr.RemoveListener(2), LLBC_OK);
    EXPECT_EQ(CountingListener::destructions, 1);

    LLBC_EventMgr nestedMgr;
    int eventOneCalls = 0;
    int eventTwoCalls = 0;
    ASSERT_NE(nestedMgr.AddListener(1, [&](LLBC_Event &) {
        ++eventOneCalls;
        LLBC_Event nested(2, true);
        EXPECT_EQ(nestedMgr.Fire(&nested), LLBC_OK);
    }), InvalidListenerStub);
    ASSERT_NE(nestedMgr.AddListener(2, [&](LLBC_Event &) {
        ++eventTwoCalls;
    }), InvalidListenerStub);
    LLBC_Event first(1, true);
    ASSERT_EQ(nestedMgr.Fire(&first), LLBC_OK);
    EXPECT_EQ(eventOneCalls, 1);
    EXPECT_EQ(eventTwoCalls, 1);

    LLBC_EventMgr removeByIdMgr;
    int removeByIdCalls = 0;
    ASSERT_NE(removeByIdMgr.AddListener(3, [&](LLBC_Event &) {
        ++removeByIdCalls;
        EXPECT_EQ(removeByIdMgr.RemoveListener(3), LLBC_FAILED);
        EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_PENDING);
        LLBC_Event suppressed(3, true);
        EXPECT_EQ(removeByIdMgr.Fire(&suppressed), LLBC_OK);
    }), InvalidListenerStub);
    LLBC_Event removeByIdEvent(3, true);
    ASSERT_EQ(removeByIdMgr.Fire(&removeByIdEvent), LLBC_OK);
    EXPECT_EQ(removeByIdCalls, 1);
    ASSERT_EQ(removeByIdMgr.Fire(&removeByIdEvent), LLBC_OK);
    EXPECT_EQ(removeByIdCalls, 1);

    LLBC_EventMgr removeAllMgr;
    int removeAllCalls = 0;
    ASSERT_NE(removeAllMgr.AddListener(4, [&](LLBC_Event &) {
        ++removeAllCalls;
        EXPECT_EQ(removeAllMgr.RemoveAllListeners(), LLBC_FAILED);
        EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_PENDING);
        LLBC_Event suppressed(4, true);
        EXPECT_EQ(removeAllMgr.Fire(&suppressed), LLBC_OK);
    }), InvalidListenerStub);
    LLBC_Event removeAllEvent(4, true);
    ASSERT_EQ(removeAllMgr.Fire(&removeAllEvent), LLBC_OK);
    EXPECT_EQ(removeAllCalls, 1);
    ASSERT_EQ(removeAllMgr.Fire(&removeAllEvent), LLBC_OK);
    EXPECT_EQ(removeAllCalls, 1);
}

// BeginFire uses a pooled event and EventFirer owns it until Fire(). Its move
// and consumed-state behavior must avoid duplicate dispatch or leaks.
TEST(EventTest, FiresPooledEventsAndRejectsRepeatedFirerUse)
{
    LLBC_EventMgr manager;
    int received = 0;
    ASSERT_NE(manager.AddListener(9, [&](LLBC_Event &event) {
        received = event["value"].As<int>();
    }), InvalidListenerStub);

    LLBC_EventFirer firer = manager.BeginFire(9);
    EXPECT_EQ(firer.SetParam("value", 77).Fire(), LLBC_OK);
    EXPECT_EQ(received, 77);
    EXPECT_EQ(firer.Fire(), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_INVALID);

    LLBC_EventFirer source = manager.BeginFire(9);
    source.SetParam("value", 88);
    LLBC_EventFirer moved(std::move(source));
    EXPECT_EQ(moved.Fire(), LLBC_OK);
    EXPECT_EQ(received, 88);

    LLBC_EventFirer copiedSource = manager.BeginFire(9);
    copiedSource.SetParam("value", 99);
    LLBC_EventFirer copied(copiedSource);
    EXPECT_EQ(copiedSource.Fire(), LLBC_FAILED);
    EXPECT_EQ(copied.Fire(), LLBC_OK);
    EXPECT_EQ(received, 99);

    LLBC_EventFirer assignmentSource = manager.BeginFire(9);
    assignmentSource.SetParam("value", 100);
    LLBC_EventFirer copyAssigned = manager.BeginFire(9);
    copyAssigned = assignmentSource;
    EXPECT_EQ(assignmentSource.Fire(), LLBC_FAILED);
    EXPECT_EQ(copyAssigned.Fire(), LLBC_OK);
    EXPECT_EQ(received, 100);

    LLBC_EventFirer moveAssignmentSource = manager.BeginFire(9);
    moveAssignmentSource.SetParam("value", 101);
    LLBC_EventFirer moveAssigned = manager.BeginFire(9);
    moveAssigned = std::move(moveAssignmentSource);
    EXPECT_EQ(moveAssignmentSource.Fire(), LLBC_FAILED);
    EXPECT_EQ(moveAssigned.Fire(), LLBC_OK);
    EXPECT_EQ(received, 101);

    LLBC_EventFirer selfAssigned = manager.BeginFire(9);
    selfAssigned.SetParam("value", 102);
    LLBC_EventFirer *selfAssignedAlias = &selfAssigned;
    selfAssigned = *selfAssignedAlias;
    EXPECT_EQ(selfAssigned.Fire(), LLBC_OK);
    EXPECT_EQ(received, 102);

    LLBC_EventFirer selfMoveAssigned = manager.BeginFire(9);
    selfMoveAssigned.SetParam("value", 103);
    LLBC_EventFirer *selfMoveAssignedAlias = &selfMoveAssigned;
    selfMoveAssigned = std::move(*selfMoveAssignedAlias);
    EXPECT_EQ(selfMoveAssigned.Fire(), LLBC_OK);
    EXPECT_EQ(received, 103);

    {
        LLBC_EventFirer unused = manager.BeginFire(9);
        unused.SetParam("value", 999);
    }
    EXPECT_EQ(received, 103);
}

// Object listeners transfer ownership only after valid registration. Stub
// removal must also erase the final event-id bucket, so a later listener can
// safely reuse the same event ID.
TEST(EventTest, ValidatesObjectListenersAndRemovesFinalStubEntry)
{
    LLBC_EventMgr manager;
    EXPECT_EQ(manager.AddListener(1, static_cast<LLBC_EventListener *>(nullptr)),
              InvalidListenerStub);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(manager.RemoveListener(static_cast<LLBC_ListenerStub>(0)), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);

    const LLBC_ListenerStub boundStub = manager.AddListener(6, [](LLBC_Event &) {}, 600);
    ASSERT_EQ(boundStub, 600u);
    auto *rejectedObjectListener = new CountingListener;
    EXPECT_EQ(manager.AddListener(6, rejectedObjectListener, boundStub), InvalidListenerStub);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_REPEAT);
    delete rejectedObjectListener;
    EXPECT_EQ(manager.RemoveListener(boundStub), LLBC_OK);

    int calls = 0;
    const LLBC_ListenerStub stub = manager.AddListener(5, [&](LLBC_Event &) {
        ++calls;
    });
    ASSERT_NE(stub, InvalidListenerStub);
    EXPECT_EQ(manager.RemoveListener(stub), LLBC_OK);
    EXPECT_EQ(manager.RemoveListener(stub), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);

    ASSERT_NE(manager.AddListener(5, [&](LLBC_Event &) {
        ++calls;
    }), InvalidListenerStub);
    LLBC_Event event(5, true);
    EXPECT_EQ(manager.Fire(&event), LLBC_OK);
    EXPECT_EQ(calls, 1);
}
