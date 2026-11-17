#pragma once
#include "Core/DependencyGraph.hpp"
#include "Utilities/DataStructures/TypedVector.hpp"
#include <array>
#include <functional>
#include <utility>
namespace Ivy::C
{
struct Engine;
enum class Phase : unsigned int
{
    Start,
    BeginFrame,
    FixedUpdate,
    Update,
    Render,
    EndFrame,
    Stop,
    Count,
};

struct SystemManager
{
    template <auto Function> struct Key
    {
    };

    template <Phase P, typename... Args> void LoopOver(Args&&... args)
    {
        auto& graph = m_Scheduler[static_cast<unsigned int>(P)];

        for (unsigned int sourceIndex : graph.ViewSources())
        {
            if constexpr (P == Phase::Start)
            {
                m_Starts[sourceIndex](args...);
                graph.ForEachDescendant(
                    sourceIndex, [&args..., this](unsigned int index) { m_Starts[index](args...); });
            }
            else if constexpr (P == Phase::BeginFrame)
            {
                m_Begins[sourceIndex](args...);
                graph.ForEachDescendant(
                    sourceIndex, [&args..., this](unsigned int index) { m_Begins[index](args...); });
            }
            else if constexpr (P == Phase::FixedUpdate)
            {
                m_FixedUpdates[sourceIndex](args...);
                graph.ForEachDescendant(
                    sourceIndex, [&args..., this](unsigned int index) { m_FixedUpdates[index](args...); });
            }
            else if constexpr (P == Phase::Update)
            {
                m_Updates[sourceIndex](args...);
                graph.ForEachDescendant(
                    sourceIndex, [&args..., this](unsigned int index) { m_Updates[index](args...); });
            }
            else if constexpr (P == Phase::Render)
            {
                m_Renders[sourceIndex](args...);
                graph.ForEachDescendant(
                    sourceIndex, [&args..., this](unsigned int index) { m_Renders[index](args...); });
            }
            else if constexpr (P == Phase::EndFrame)
            {
                m_Ends[sourceIndex](args...);
                graph.ForEachDescendant(
                    sourceIndex, [&args..., this](unsigned int index) { m_Ends[index](args...); });
            }
            else if constexpr (P == Phase::Stop)
            {
                m_Stops[sourceIndex](args...);
                graph.ForEachDescendant(
                    sourceIndex, [&args..., this](unsigned int index) { m_Stops[index](args...); });
            }
        }
    }

    template <Phase P, auto F> void AddSystems()
    {
        if constexpr (P == Phase::Start)
        {
            m_Starts.Emplace<Key<F>>(F);
        }
        else if constexpr (P == Phase::BeginFrame)
        {
            m_Begins.Emplace<Key<F>>(F);
        }
        else if constexpr (P == Phase::FixedUpdate)
        {
            m_FixedUpdates.Emplace<Key<F>>(F);
        }
        else if constexpr (P == Phase::Update)
        {
            m_Updates.Emplace<Key<F>>(F);
        }
        else if constexpr (P == Phase::Render)
        {
            m_Renders.Emplace<Key<F>>(F);
        }
        else if constexpr (P == Phase::EndFrame)
        {
            m_Ends.Emplace<Key<F>>(F);
        }
        else if constexpr (P == Phase::Stop)
        {
            m_Stops.Emplace<Key<F>>(F);
        }
        m_Scheduler[static_cast<unsigned int>(P)].Ensure(GetIndex<P, F>());
    }

    template <Phase P, auto... Fs> void AddSystems() requires(sizeof...(Fs) != 1)
    {
        (AddSystems<P, Fs>(), ...);
    }

    template <Phase P, auto First, auto Second, auto... Args> void Chain()
    {
        auto& graph = m_Scheduler[static_cast<unsigned int>(P)];

        graph.Attach(GetIndex<P, First>(), GetIndex<P, Second>());

        if constexpr (sizeof...(Args) > 0)
        {
            Chain<P, Second, Args...>();
        }
    }

  private:
    std::array<Ivy::C::DependencyGraph<unsigned int>, static_cast<unsigned int>(Phase::Count)> m_Scheduler;

    U::TypedVector<std::move_only_function<void(C::Engine&)>> m_Starts;
    U::TypedVector<std::move_only_function<void(C::Engine&, double)>> m_Begins;
    U::TypedVector<std::move_only_function<void(C::Engine&, double)>> m_FixedUpdates;
    U::TypedVector<std::move_only_function<void(C::Engine&, double)>> m_Updates;
    U::TypedVector<std::move_only_function<void(C::Engine&)>> m_Renders;
    U::TypedVector<std::move_only_function<void(C::Engine&, double)>> m_Ends;
    U::TypedVector<std::move_only_function<void(C::Engine&)>> m_Stops;

    template <Phase P, auto F> unsigned int GetIndex()
    {
        if constexpr (P == Phase::Start)
        {
            return m_Starts.GetIndex<Key<F>>();
        }
        else if constexpr (P == Phase::BeginFrame)
        {
            return m_Begins.GetIndex<Key<F>>();
        }
        else if constexpr (P == Phase::FixedUpdate)
        {
            return m_FixedUpdates.GetIndex<Key<F>>();
        }
        else if constexpr (P == Phase::Update)
        {
            return m_Updates.GetIndex<Key<F>>();
        }
        else if constexpr (P == Phase::Render)
        {
            return m_Renders.GetIndex<Key<F>>();
        }
        else if constexpr (P == Phase::EndFrame)
        {
            return m_Ends.GetIndex<Key<F>>();
        }
        else if constexpr (P == Phase::Stop)
        {
            return m_Stops.GetIndex<Key<F>>();
        }
        else
        {
            static_assert(P != P, "Invalid execution phase");
        }
    }
};

} // namespace Ivy::C
