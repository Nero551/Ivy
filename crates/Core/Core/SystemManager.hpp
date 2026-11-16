#pragma once
#include "Core/DependencyGraph.hpp"
#include "Utilities/DataStructures/TypedVector.hpp"
#include <array>
#include <functional>
#include <utility>
namespace Ivy::C
{
struct Engine;
enum class Execution : unsigned int
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

    template <Execution Exec, typename... Args> void LoopOver(Args&&... args)
    {
        auto& graph = m_Scheduler[static_cast<unsigned int>(Exec)];

        for (unsigned int sourceIndex : graph.ViewSources())
        {
            if constexpr (Exec == Execution::Start)
            {
                m_Starts[sourceIndex](args...);
                graph.ForEachDescendant(
                    sourceIndex, [&args..., this](unsigned int index) { m_Starts[index](args...); });
            }
            else if constexpr (Exec == Execution::BeginFrame)
            {
                m_Begins[sourceIndex](args...);
                graph.ForEachDescendant(
                    sourceIndex, [&args..., this](unsigned int index) { m_Begins[index](args...); });
            }
            else if constexpr (Exec == Execution::FixedUpdate)
            {
                m_FixedUpdates[sourceIndex](args...);
                graph.ForEachDescendant(
                    sourceIndex, [&args..., this](unsigned int index) { m_FixedUpdates[index](args...); });
            }
            else if constexpr (Exec == Execution::Update)
            {
                m_Updates[sourceIndex](args...);
                graph.ForEachDescendant(
                    sourceIndex, [&args..., this](unsigned int index) { m_Updates[index](args...); });
            }
            else if constexpr (Exec == Execution::Render)
            {
                m_Renders[sourceIndex](args...);
                graph.ForEachDescendant(
                    sourceIndex, [&args..., this](unsigned int index) { m_Renders[index](args...); });
            }
            else if constexpr (Exec == Execution::EndFrame)
            {
                m_Ends[sourceIndex](args...);
                graph.ForEachDescendant(
                    sourceIndex, [&args..., this](unsigned int index) { m_Ends[index](args...); });
            }
            else if constexpr (Exec == Execution::Stop)
            {
                m_Stops[sourceIndex](args...);
                graph.ForEachDescendant(
                    sourceIndex, [&args..., this](unsigned int index) { m_Stops[index](args...); });
            }
        }
    }

    template <Execution Exec, auto F> void AddSystems()
    {
        if constexpr (Exec == Execution::Start)
        {
            m_Starts.Emplace<Key<F>>(F);
        }
        else if constexpr (Exec == Execution::BeginFrame)
        {
            m_Begins.Emplace<Key<F>>(F);
        }
        else if constexpr (Exec == Execution::FixedUpdate)
        {
            m_FixedUpdates.Emplace<Key<F>>(F);
        }
        else if constexpr (Exec == Execution::Update)
        {
            m_Updates.Emplace<Key<F>>(F);
        }
        else if constexpr (Exec == Execution::Render)
        {
            m_Renders.Emplace<Key<F>>(F);
        }
        else if constexpr (Exec == Execution::EndFrame)
        {
            m_Ends.Emplace<Key<F>>(F);
        }
        else if constexpr (Exec == Execution::Stop)
        {
            m_Stops.Emplace<Key<F>>(F);
        }
        m_Scheduler[static_cast<unsigned int>(Exec)].Ensure(GetIndex<Exec, F>());
    }

    template <Execution Exec, auto... Fs> void AddSystems() requires(sizeof...(Fs) != 1)
    {
        (AddSystems<Exec, Fs>(), ...);
    }

    template <Execution Exec, auto First, auto Second, auto... Args> void Chain()
    {
        auto& graph = m_Scheduler[static_cast<unsigned int>(Exec)];

        graph.Attach(GetIndex<Exec, First>(), GetIndex<Exec, Second>());

        if constexpr (sizeof...(Args) > 0)
        {
            Chain<Exec, Second, Args...>();
        }
    }

  private:
    std::array<Ivy::C::DependencyGraph<unsigned int>, static_cast<unsigned int>(Execution::Count)>
        m_Scheduler;

    U::TypedVector<std::move_only_function<void(C::Engine&)>> m_Starts;
    U::TypedVector<std::move_only_function<void(C::Engine&, double)>> m_Begins;
    U::TypedVector<std::move_only_function<void(C::Engine&, double)>> m_FixedUpdates;
    U::TypedVector<std::move_only_function<void(C::Engine&, double)>> m_Updates;
    U::TypedVector<std::move_only_function<void(C::Engine&)>> m_Renders;
    U::TypedVector<std::move_only_function<void(C::Engine&, double)>> m_Ends;
    U::TypedVector<std::move_only_function<void(C::Engine&)>> m_Stops;

    template <Execution Exec, auto F> unsigned int GetIndex()
    {
        if constexpr (Exec == Execution::Start)
        {
            return m_Starts.GetIndex<Key<F>>();
        }
        else if constexpr (Exec == Execution::BeginFrame)
        {
            return m_Begins.GetIndex<Key<F>>();
        }
        else if constexpr (Exec == Execution::FixedUpdate)
        {
            return m_FixedUpdates.GetIndex<Key<F>>();
        }
        else if constexpr (Exec == Execution::Update)
        {
            return m_Updates.GetIndex<Key<F>>();
        }
        else if constexpr (Exec == Execution::Render)
        {
            return m_Renders.GetIndex<Key<F>>();
        }
        else if constexpr (Exec == Execution::EndFrame)
        {
            return m_Ends.GetIndex<Key<F>>();
        }
        else if constexpr (Exec == Execution::Stop)
        {
            return m_Stops.GetIndex<Key<F>>();
        }
        else
        {
            static_assert(Exec != Exec, "Invalid execution phase");
        }
    }
};

} // namespace Ivy::C
