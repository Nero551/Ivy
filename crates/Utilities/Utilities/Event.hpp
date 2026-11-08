#pragma once
#include "Utilities/DataStructures/GIndexPool.hpp"
#include "Utilities/DataStructures/SparseSetSoA.hpp"
#include "Utilities/Debug/Log.hpp"
#include "Utilities/ScopeGuard.hpp"
#include <concepts>
#include <functional>
namespace Ivy::U
{
template <typename... Args> struct Event
{
    using Callback = std::move_only_function<void(Args...)>;
    using Subscription = GIndexPool<>::Handle;

    template <typename F>
    Subscription Subscribe(F&& callback)
        requires std::constructible_from<Callback, F> && std::invocable<Callback&, Args...>
    {
        if (m_Firing)
        {
            U::Log::Error("[Event] Attempted to subscribe while firing");
        }
        Subscription sub = m_Subscriptions.Acquire();
        m_Subscribers.Emplace(sub.Index, std::forward<F>(callback));
        return sub;
    }

    void Unsubscribe(Subscription subscription)
    {
        if (m_Firing)
        {
            U::Log::Error("[Event] Attempted to unsubscribe while firing");
        }

        if (m_Subscriptions.IsAcquired(subscription))
        {
            m_Subscriptions.Release(subscription);
            m_Subscribers.Erase(subscription.Index);
        }
    }

    void Fire(Args... args)
    {
        if (m_Firing)
        {
            U::Log::Error("[Event] Attempted to fire while firing");
        }

        m_Firing = true;
        U::ScopeGuard guard{[&]() noexcept { m_Firing = false; }};

        for (Callback& callback : m_Subscribers)
        {
            callback(args...);
        }
    }

  private:
    U::SparseSetSoA<Callback, decltype(Subscription::Index)> m_Subscribers{};
    U::GIndexPool<decltype(Subscription::Index), decltype(Subscription::Generation)> m_Subscriptions{};
    bool m_Firing = false;
};
} // namespace Ivy::U
