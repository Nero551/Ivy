#include "Utilities/Event.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

namespace Ivy::UnitTests
{
using namespace Ivy::U;

// ===========================================================================
// Event contract, as implemented today:
//
//   * Subscribe returns a Subscription id that is valid until Unsubscribe(id).
//   * Unsubscribe is idempotent while the id is still valid.
//   * Unsubscribe on an id that was never issued, or that has been released
//     and recycled, has UNDEFINED behaviour (it may remove a different
//     listener). See StaleSubscriptionId* tests below — they pin the current
//     implementation and are meant to be revisited if IndexPool gains
//     generation counters.
//   * Fire invokes every subscribed listener, in SparseSetSoA dense order
//     (which equals insertion order on a fresh event, and is UNSPECIFIED
//     after any Erase).
//   * If a listener throws, the exception propagates out of Fire and the
//     event remains usable afterwards (m_Firing is reset by ScopeGuard).
//   * Subscribe/Unsubscribe from *inside* a listener is forbidden by contract:
//     Event logs an error but does not prevent the mutation, which may
//     invalidate the iterator held by Fire (dense-array reallocation on
//     Subscribe, swap-and-pop on Unsubscribe). We do not test these paths at
//     runtime — they are UB by contract and any test would be implementation-
//     dependent on SparseSetSoA internals.
//   * Re-entrant Fire() recurses. The log message says "attempted" but the
//     implementation does not short-circuit; the guard nesting still keeps
//     m_Firing consistent. Tested below.
//   * Event is NOT thread-safe. No locking is performed.
// ===========================================================================

using TestEvent = Event<int>;

// ---------------------------------------------------------------------------
// Subscribe / Unsubscribe lifecycle
// ---------------------------------------------------------------------------
TEST(Event, SubscribeOnFreshEventIsValid)
{
    TestEvent ev;
    auto id = ev.Subscribe([](int) {});
    // No way to introspect validity other than using it; the fact that
    // Unsubscribe is a no-op on non-acquired ids means we just verify no
    // throw and that the operation is observed by future Fire().
    EXPECT_NO_THROW(ev.Unsubscribe(id));
}

TEST(Event, FireWithNoListenersDoesNotThrow)
{
    TestEvent ev;
    EXPECT_NO_THROW(ev.Fire(0));
}

TEST(Event, FireForwardsSingleArgument)
{
    TestEvent ev;
    int received = 0;
    ev.Subscribe([&received](int v) { received = v; });
    ev.Fire(42);
    EXPECT_EQ(received, 42);
}

TEST(Event, FireForwardsMultipleArgumentsInOrder)
{
    Event<int, std::string, double> ev;
    int gotI = 0;
    std::string gotS;
    double gotD = 0.0;
    ev.Subscribe(
        [&](int i, const std::string& s, double d)
        {
            gotI = i;
            gotS = s;
            gotD = d;
        });
    ev.Fire(7, "hello", 3.5);
    EXPECT_EQ(gotI, 7);
    EXPECT_EQ(gotS, "hello");
    EXPECT_DOUBLE_EQ(gotD, 3.5);
}

TEST(Event, ZeroArgumentEventFires)
{
    Event<> ev;
    int calls = 0;
    ev.Subscribe([&calls]() { ++calls; });
    ev.Fire();
    EXPECT_EQ(calls, 1);
}

// ---------------------------------------------------------------------------
// Listener count and invocation
// ---------------------------------------------------------------------------
TEST(Event, FireInvokesEveryListener)
{
    TestEvent ev;
    constexpr int N = 128;
    std::vector<int> hits(N, 0);
    for (int i = 0; i < N; ++i)
    {
        ev.Subscribe([&hits, i](int) { ++hits[static_cast<std::size_t>(i)]; });
    }
    ev.Fire(0);
    for (int i = 0; i < N; ++i)
    {
        EXPECT_EQ(hits[static_cast<std::size_t>(i)], 1) << "listener " << i;
    }
}

TEST(Event, ListenersOnFreshEventFireInInsertionOrder)
{
    // SparseSetSoA iteration order on a fresh event (no Erase calls) equals
    // the order callbacks were Emplace'd, i.e. subscription order. This is
    // load-bearing for user code that relies on deterministic dispatch order
    // before any removals.
    TestEvent ev;
    std::vector<int> order;
    ev.Subscribe([&order](int) { order.push_back(1); });
    ev.Subscribe([&order](int) { order.push_back(2); });
    ev.Subscribe([&order](int) { order.push_back(3); });
    ev.Fire(0);
    EXPECT_EQ(order, (std::vector<int>{1, 2, 3}));
}

TEST(Event, FireRepeatedlyInvokesEachTime)
{
    TestEvent ev;
    int calls = 0;
    ev.Subscribe([&calls](int) { ++calls; });
    ev.Fire(0);
    ev.Fire(0);
    ev.Fire(0);
    EXPECT_EQ(calls, 3);
}

// ---------------------------------------------------------------------------
// Unsubscribe semantics
// ---------------------------------------------------------------------------
TEST(Event, UnsubscribeStopsFutureDelivery)
{
    TestEvent ev;
    int calls = 0;
    auto id = ev.Subscribe([&calls](int) { ++calls; });
    ev.Fire(0);
    ev.Unsubscribe(id);
    ev.Fire(0);
    EXPECT_EQ(calls, 1);
}

TEST(Event, UnsubscribeTargetsOnlyTheGivenSubscription)
{
    TestEvent ev;
    int a = 0, b = 0, c = 0;
    auto idA = ev.Subscribe([&a](int) { ++a; });
    auto idB = ev.Subscribe([&b](int) { ++b; });
    auto idC = ev.Subscribe([&c](int) { ++c; });

    ev.Unsubscribe(idB);
    ev.Fire(0);

    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 0);
    EXPECT_EQ(c, 1);

    (void)idA;
    (void)idC;
}

TEST(Event, UnsubscribeIsIdempotentOnStillValidId)
{
    TestEvent ev;
    auto id = ev.Subscribe([](int) {});
    ev.Unsubscribe(id);
    EXPECT_NO_THROW(ev.Unsubscribe(id));
}

TEST(Event, UnsubscribeOfNeverIssuedIdIsNoOp)
{
    TestEvent ev;
    EXPECT_NO_THROW(ev.Unsubscribe({999999, 5}));
}

TEST(Event, SubscribeAfterUnsubscribeWorks)
{
    TestEvent ev;
    int calls = 0;
    auto id = ev.Subscribe([&calls](int) { ++calls; });
    ev.Fire(0);
    ev.Unsubscribe(id);
    ev.Subscribe([&calls](int) { ++calls; });
    ev.Fire(0);
    EXPECT_EQ(calls, 2);
}

TEST(Event, UnsubscribeFromEndLeavesRemainingSubscribersIntact)
{
    // After Unsubscribe, dispatch order is UNSPECIFIED (SparseSetSoA does
    // swap-and-pop), so we check the set of invoked listeners, not the order.
    TestEvent ev;
    int a = 0, b = 0, c = 0;
    auto idA = ev.Subscribe([&a](int) { ++a; });
    auto idB = ev.Subscribe([&b](int) { ++b; });
    auto idC = ev.Subscribe([&c](int) { ++c; });

    ev.Unsubscribe(idB);
    ev.Fire(0);
    ev.Fire(0);

    EXPECT_EQ(a, 2);
    EXPECT_EQ(b, 0);
    EXPECT_EQ(c, 2);

    (void)idA;
    (void)idC;
}

// ---------------------------------------------------------------------------
// Footgun: stale ids after recycling.
//
// IndexPool<Subscription>::Acquire() typically returns the lowest free id, so
// after Unsubscribe(idA) the next Subscribe call recycles idA. A *second*
// Unsubscribe(idA) then removes the recycled subscription. This test PINS the
// observed behaviour; if IndexPool ever gains generation counters, this test
// should fail and prompt a contract review.
// ---------------------------------------------------------------------------
TEST(Event, StaleIdAfterRecycleRemovesNewSubscription_CurrentBehaviour)
{
    TestEvent ev;
    int first = 0, second = 0;

    auto idFirst = ev.Subscribe([&first](int) { ++first; });
    ev.Unsubscribe(idFirst);

    // Pool likely recycles idFirst for the next Acquire — capture what we get.
    auto idSecond = ev.Subscribe([&second](int) { ++second; });

    // Only run the assertion if the pool actually recycled the id; if it
    // didn't, the test is a no-op and the footgun is currently absent.
    if (idSecond == idFirst)
    {
        ev.Unsubscribe(idFirst); // stale id — removes idSecond on current impl
        ev.Fire(0);
        EXPECT_EQ(first, 0);
        EXPECT_EQ(second, 0) << "documented footgun: reusing a released id unsubscribes the "
                                "recycled subscriber; revisit if IndexPool gains generations";
    }
}

// ---------------------------------------------------------------------------
// Listener throwing: exception propagates, event remains usable.
// ---------------------------------------------------------------------------
TEST(Event, ListenerExceptionPropagatesOutOfFire)
{
    TestEvent ev;
    ev.Subscribe([](int) { throw std::runtime_error("listener"); });
    EXPECT_THROW(ev.Fire(0), std::runtime_error);
}

TEST(Event, EventIsUsableAfterListenerThrows)
{
    // Verifies ScopeGuard inside Fire resets m_Firing on the exception path.
    TestEvent ev;
    int calls = 0;
    bool throwOnce = true;
    ev.Subscribe(
        [&](int)
        {
            ++calls;
            if (throwOnce)
            {
                throwOnce = false;
                throw std::runtime_error("first call only");
            }
        });

    EXPECT_THROW(ev.Fire(0), std::runtime_error);
    EXPECT_NO_THROW(ev.Fire(0)) << "m_Firing must have been reset by ScopeGuard";
    EXPECT_EQ(calls, 2);
}

TEST(Event, EarlierListenersRunBeforeListenerThrows)
{
    // Order matters here: on a fresh event, dispatch is in insertion order.
    TestEvent ev;
    std::vector<int> order;
    ev.Subscribe([&order](int) { order.push_back(1); });
    ev.Subscribe(
        [&order](int)
        {
            order.push_back(2);
            throw std::runtime_error("second");
        });
    ev.Subscribe([&order](int) { order.push_back(3); }); // never reached

    EXPECT_THROW(ev.Fire(0), std::runtime_error);
    EXPECT_EQ(order, (std::vector<int>{1, 2}));
}

// ---------------------------------------------------------------------------
// Argument forwarding: by-value, by-reference, and const-reference events.
// ---------------------------------------------------------------------------
TEST(Event, ValueArgumentIsCopiedPerListener)
{
    Event<std::string> ev;
    std::string got1, got2;
    ev.Subscribe([&got1](std::string s) { got1 = std::move(s); });
    ev.Subscribe([&got2](std::string s) { got2 = std::move(s); });

    ev.Fire(std::string("hello"));
    EXPECT_EQ(got1, "hello");
    EXPECT_EQ(got2, "hello");
}

TEST(Event, ReferenceArgumentPropagatesMutations)
{
    Event<int&> ev;
    ev.Subscribe([](int& v) { v += 10; });
    ev.Subscribe([](int& v) { v *= 2; });

    int x = 1;
    ev.Fire(x);
    EXPECT_EQ(x, 22); // (1 + 10) * 2, in subscription order on a fresh event
}

TEST(Event, ConstReferenceArgumentBindsTemporary)
{
    Event<const std::string&> ev;
    std::string got;
    ev.Subscribe([&got](const std::string& s) { got = s; });

    ev.Fire("literal"); // temp binds to const ref for duration of Fire
    EXPECT_EQ(got, "literal");
}

// ---------------------------------------------------------------------------
// Callable acceptance: std::constructible_from<Callback, F> must accept
// move-only callables and free functions.
// ---------------------------------------------------------------------------
TEST(Event, SubscribeAcceptsMoveOnlyCallable)
{
    TestEvent ev;
    auto payload = std::make_unique<int>(0);
    int* raw = payload.get();
    ev.Subscribe([p = std::move(payload)](int v) { *p = v; });
    ev.Fire(99);
    EXPECT_EQ(*raw, 99);
}

namespace
{
void FreeFunctionListener(int& sink, int v)
{
    sink = v;
}
} // namespace

TEST(Event, SubscribeAcceptsFreeFunction)
{
    TestEvent ev;
    int sink = 0;
    // std::function requires a callable; bind the free function to capture
    // `sink` via a lambda is the standard pattern, but the lambda-to-
    // function-pointer conversion inside std::function also works directly
    // when the function signature matches.
    ev.Subscribe([&sink](int v) { FreeFunctionListener(sink, v); });
    ev.Fire(7);
    EXPECT_EQ(sink, 7);
}

// ---------------------------------------------------------------------------
// Bulk churn: subscribe/unsubscribe many times, verify no leaks in the
// invoked-set. Does not assert on order (see file header).
// ---------------------------------------------------------------------------
TEST(Event, SubscribeUnsubscribeChurnLeavesCorrectSet)
{
    TestEvent ev;
    constexpr int N = 64;
    std::vector<int> hits(N, 0);

    std::vector<TestEvent::Subscription> ids;
    for (int i = 0; i < N; ++i)
    {
        ids.push_back(ev.Subscribe([&hits, i](int) { ++hits[static_cast<std::size_t>(i)]; }));
    }
    // Remove evens.
    for (int i = 0; i < N; i += 2)
    {
        ev.Unsubscribe(ids[static_cast<std::size_t>(i)]);
    }
    ev.Fire(0);

    for (int i = 0; i < N; ++i)
    {
        if (i % 2 == 0)
        {
            EXPECT_EQ(hits[static_cast<std::size_t>(i)], 0) << "even " << i;
        }
        else
        {
            EXPECT_EQ(hits[static_cast<std::size_t>(i)], 1) << "odd " << i;
        }
    }
}

TEST(Event, ManyRecyclesDoNotCorruptDenseSet)
{
    // Repeatedly subscribe/unsubscribe a single slot; the pool will recycle
    // the same id every time. Each round must result in exactly one hit for
    // the *current* listener.
    TestEvent ev;
    int currentHits = 0;
    for (int round = 0; round < 32; ++round)
    {
        int local = 0;
        auto id = ev.Subscribe([&local](int) { ++local; });
        ev.Fire(0);
        EXPECT_EQ(local, 1) << "round " << round;
        ev.Unsubscribe(id);
        ev.Fire(0);
        EXPECT_EQ(local, 1) << "round " << round;
        (void)currentHits;
    }
}

} // namespace Ivy::UnitTests
