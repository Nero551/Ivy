#pragma once

#include "Core/Service.hpp"
#include "Core/Services/EventBus/Event.hpp"

namespace Ivy::C
{
/** @brief Concept constraining types that can be dispatched through the event system. */
template <typename T>
concept EventType = std::derived_from<T, IEvent>;

/** @brief Global event dispatcher and deferred event queue. */
struct EventBus : Service
{
    /** @brief Constructs and immediately dispatches a global event. */
    template <EventType T, typename... Args>
    void Fire(Args&&... args) requires(std::constructible_from<T, Args...>)
    {
        auto listeners = m_Listeners.find(typeid(T));
        if (listeners == m_Listeners.end())
        {
            return;
        }

        T event{std::forward<Args>(args)...};
        for (auto& callback : listeners->second)
        {
            callback.Callback(event);
        }
    }

    /** @brief Queues a global event for deferred dispatch at the end of the frame. */
    template <EventType T, typename... Args>
    void Queue(Args&&... args) requires(std::constructible_from<T, Args...>)
    {
        auto event = std::make_unique<T>(std::forward<Args>(args)...);
        m_FireQueue.emplace_back(std::move(event));
    }

    /** @brief Dispatches all events currently waiting in the fire queue. */
    void EmptyFireQueue();

    /** @brief Subscribes a callback to a global event type and returns its subscription
     * ID. */
    template <EventType T, typename F> requires std::invocable<F, const T&>
    std::size_t Sub(F&& callback)
    {
        auto method = [callback = std::forward<F>(callback)](IEvent& e) mutable
        { callback(static_cast<T&>(e)); };

        const auto subscription = ++m_NextSubscription;
        m_Listeners[typeid(T)].push_back({.Subscription = subscription, .Callback = method});
        return subscription;
    }

    /** @brief Removes a global event subscription by its subscription ID. */
    template <EventType T> void Unsub(const std::size_t subscription)
    {
        auto it = m_Listeners.find(typeid(T));
        if (it != m_Listeners.end())
        {
            size_t i = 0;
            while (i < it->second.size() && it->second.at(i).Subscription != subscription)
            {
                ++i;
            }

            if (i < it->second.size())
            {
                it->second.erase(it->second.begin() + i);

                if (it->second.empty())
                {
                    m_Listeners.erase(it);
                }
            }
        }
    }

  private:
    /** @brief Stores a global event callback and its subscription ID. */
    struct Entry
    {
        std::size_t Subscription;
        std::function<void(IEvent&)> Callback;
    };

    /** @brief Global event listeners grouped by event type. */
    std::unordered_map<std::type_index, std::vector<Entry>> m_Listeners;

    /** @brief Events waiting for deferred dispatch. */
    std::vector<std::unique_ptr<IEvent>> m_FireQueue;

    /** @brief Generates unique subscription IDs. */
    size_t m_NextSubscription = 0;

  protected:
    void EndFrame() override;
};
} // namespace Ivy::C
