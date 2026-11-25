#pragma once
#include "Diagnostics/Log.hpp"
#include "Utilities/DataStructures/GIndexPool.hpp"
#include "Utilities/DataStructures/SparseSetSoA.hpp"
#include "Utilities/ScopeGuard.hpp"
#include <concepts>
#include <functional>
namespace Ivy::U
{
template <typename... Args> struct Event
{
    using Subscription = unsigned long;
    using Callback = std::move_only_function<void(Args...)>;

    template <typename F>
    Subscription Subscribe(F&& callback)
        requires std::constructible_from<Callback, F> && std::invocable<Callback&, Args...>
    {
        if (m_Firing)
        {
            D::Log::Error("[Event] Attempted to subscribe while firing");
        }
        Subscription sub = m_NextSubscription++;
        m_Subscribers.Emplace(sub, std::forward<F>(callback));
        return sub;
    }

    void Unsubscribe(Subscription subscription)
    {
        if (m_Firing)
        {
            D::Log::Error("[Event] Attempted to unsubscribe while firing");
        }

        m_Subscribers.Erase(subscription);
    }

    void Fire(Args... args)
    {

        //prevents the callback itself from running Fire().
        if (m_Firing)
        {
            D::Log::Error("[Event] Attempted to fire while firing");
        }

        m_Firing = true;
        U::ScopeGuard guard{[&]() noexcept { m_Firing = false; }};

        for (Callback& callback : m_Subscribers)
        {
            callback(args...);
        }
    }

  private:
    U::SparseSetSoA<Callback, unsigned long> m_Subscribers{};
    Subscription m_NextSubscription = 0;
    bool m_Firing = false;
};
} // namespace Ivy::U
