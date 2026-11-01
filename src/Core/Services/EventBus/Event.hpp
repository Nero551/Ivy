#pragma once
#include "stl.hpp"
#include <concepts>
#include <cstddef>
#include <functional>
#include <vector>

namespace Ivy::C
{
struct IEvent
{
    virtual ~IEvent() = default;
};

/** @brief Self-managing event that owns its listeners and dispatches them immediately after Fire(). */
template <typename T> struct Event : IEvent
{
    Event() = default;
    ~Event() override = default;

    /** @brief Dispatches the event to all registered listeners. */
    void Fire() const
    {

        for (auto& listener : m_Listeners)
        {
            listener.Callback(static_cast<const T&>(*this));
        }
    }

    /** @brief Registers a listener and returns its unique subscription ID. */
    template <typename F> requires std::invocable<F, const T&>
    std::size_t Sub(F&& callback)
    {
        const auto subscription = ++m_NextSubscription;

        m_Listeners.push_back({.Subscription = subscription, .Callback = std::forward<F>(callback)});

        return subscription;
    }

    /** @brief Removes a listener using its subscription ID. */
    void Unsub(std::size_t subscription)
    {
        int i = 0;
        while (i < m_Listeners.size() && m_Listeners.at(i).Subscription != subscription)
        {
            ++i;
        }
        if (i < m_Listeners.size())
        {
            m_Listeners.erase(m_Listeners.begin() + i);
        }
    }

  private:
    /** @brief Stores a listener callback and its subscription ID. */
    struct Entry
    {
        const std::size_t Subscription;
        const stl::function<void(const T&) const> Callback;
    };

    std::vector<Entry> m_Listeners;
    std::size_t m_NextSubscription = 0;
};
} // namespace Ivy::C
