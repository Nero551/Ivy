#pragma once
#include "Core/DependencyGraph.hpp"
#include "Utilities/DataStructures/TypedVector.hpp"
#include <array>
#include <tuple>
#include <type_traits>
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

template <typename T> struct Fetch
{
    static_assert("Parameter is Unfetchable");
    static decltype(auto) Get(Engine& e) {}
};

template <typename> struct SystemSig;

template <typename R, typename... A, bool NE> struct SystemSig<R (*)(A...) noexcept(NE)>
{
    using Args = std::tuple<A...>;

    template <auto F> static void Invoke(Engine& e)
    {
        F(Fetch<std::remove_cvref_t<A>>::Get(e)...);
    }
};

struct SystemManager
{
    template <auto Function> struct Key
    {
    };

    template <Phase P, auto F> void AddSystem()
    {
        auto& systems = m_Systems[static_cast<unsigned>(P)];
        systems.Emplace<Key<F>>(&SystemSig<decltype(F)>::template Invoke<F>);
        m_Scheduler[static_cast<unsigned>(P)].Ensure(systems.GetIndex<Key<F>>());
    }

    template <Phase P, auto... Fs> void AddSystems()
    {
        (AddSystem<P, Fs>(), ...);
    }

    template <Phase P, auto First, auto Second, auto... Rest> void Chain()
    {
        m_Scheduler[static_cast<unsigned>(P)].Attach(GetIndex<P, First>(), GetIndex<P, Second>());
        if constexpr (sizeof...(Rest) > 0)
        {
            Chain<P, Second, Rest...>();
        }
    }

    template <Phase P> void LoopOver(Engine& engine)
    {
        auto& graph = m_Scheduler[static_cast<unsigned>(P)];
        auto& systems = m_Systems[static_cast<unsigned>(P)];

        for (unsigned src : graph.ViewSources())
        {
            systems[src](engine);
            graph.ForEachDescendant(src, [&](unsigned i) { systems[i](engine); });
        }
    }

  private:
    template <Phase P, auto F> unsigned GetIndex()
    {
        return m_Systems[static_cast<unsigned>(P)].GetIndex<Key<F>>();
    }

    std::array<DependencyGraph<unsigned>, static_cast<unsigned>(Phase::Count)> m_Scheduler;
    std::array<U::TypedVector<void (*)(Engine&)>, static_cast<unsigned>(Phase::Count)> m_Systems;
};
} // namespace Ivy::C
