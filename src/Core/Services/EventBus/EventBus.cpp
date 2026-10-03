#include "EventBus.hpp"

namespace N::C
{
void EventBus::EmptyFireQueue()
{
    for (auto& event : m_FireQueue)
    {
        auto listeners = m_Listeners.find(typeid(*event));
        if (listeners == m_Listeners.end())
        {
            continue;
        }

        for (auto& listener : listeners->second)
        {
            listener.Callback(*event);
        }
    }
    m_FireQueue.clear();
}

void EventBus::EndFrame()
{
    EmptyFireQueue();
}
} // namespace N::C
