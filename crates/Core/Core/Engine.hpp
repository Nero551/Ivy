#pragma once
#include "Core/DependencyGraph.hpp"
#include "Core/State.hpp"
#include "Core/System.hpp"
#include "Core/TimeHandler.hpp"
#include "Core/World/World.hpp"
#include "Diagnostics/Log.hpp"
#include "ECS/EntityComponentSystem.hpp"
#include "Graphics/Window.hpp"
#include "Utilities/DataStructures/TypedVector.hpp"
#include "Utilities/IsSpecialization.hpp"
#include "Utilities/TupleUtils.hpp"
#include <array>
#include <concepts>
#include <format>
#include <functional>
#include <memory>
#include <tuple>
#include <utility>

/** @brief Root namespace for Ivy engine functionality. */
namespace Ivy::C
{

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

template <typename T>
concept IsSystem = std::derived_from<T, C::System>;
template <typename T>
concept IsState = std::derived_from<T, C::State>;

/** @brief Main Ivy engine and game loop. Manages the window, world, systems, ECS, and frame lifecycle. */
struct Engine
{
    G::Window Window{};
    World World{*this};
    ECS::EntityComponentSystem ECS{};
    TimeHandler TimeHandler{};

    template <typename F>
    Engine(F&& configure) requires(std::invocable<F, Engine&>)
        : World(*this), m_Configuration(std::forward<F>(configure))
    {
    }
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;

    void Start();
    void BeginFrame();
    void FixedUpdate();
    void Update();
    void Render();
    void EndFrame();
    void Stop();

    /**
     * @brief Runs the engine's main loop.
     * @remark Order: Start -> BeginFrame -> FixedUpdate -> Update -> Render -> EndFrame -> Stop
     */
    void Run();

    /** @brief Requests the engine to shut down. */
    void Shutdown();

    /** @brief Returns elapsed engine time in seconds. */
    double GetTime() const;

    template <IsSystem T> T& GetSystems() const
    {
        auto system = m_Systems.Find<T>();
        if (system == m_Systems.end())
        {
            D::Log::Error(std::format("System {} not found", typeid(T).name()));
        }
        return static_cast<T&>(**system);
    }

    template <IsSystem... Args> std::tuple<Args&...> GetSystems() const requires(sizeof...(Args) != 1)
    {
        return {GetSystems<Args>()...};
    }

    const U::TypedVector<std::unique_ptr<System>>& GetAllSystems() const
    {
        return m_Systems;
    }

    template <IsSystem T> T& AddSystems()
    {
        m_Systems.Emplace<T>(std::make_unique<T>(*this));
        for (auto& execution : m_SystemScheduler)
        {
            execution.Ensure(m_Systems.GetIndex<T>());
        }
        return static_cast<T&>(*m_Systems.Get<T>());
    }

    template <IsSystem... Args> std::tuple<Args&...> AddSystems() requires(sizeof...(Args) != 1)
    {
        return {AddSystems<Args>()...};
    }

    template <typename Tuple> void AddPlugins() requires(U::IsTSpec<Tuple, std::tuple>)
    {
        return U::UnpackAndRun<Tuple>(std::make_index_sequence<std::tuple_size_v<Tuple>>{},
            [this]<typename... Args>()
            {
                (
                    [&]<typename T>()
                    {
                        if constexpr (IsSystem<T>)
                        {
                            AddSystems<T>();
                        }
                        else if constexpr (IsState<T>)
                        {
                            AddStates<T>();
                        }
                    }.template operator()<Args>(),
                    ...);
            });
    }

    template <typename... Tuples>
    void AddPlugins() requires(sizeof...(Tuples) != 1 && (U::IsTSpec<Tuples, std::tuple> && ...))
    {
        (AddPlugins<Tuples>(), ...);
    }

    template <typename First, typename Second, typename... Args> void Chain(Execution execution)
    {
        auto& graph = m_SystemScheduler[static_cast<unsigned int>(execution)];

        graph.Attach(m_Systems.GetIndex<First>(), m_Systems.GetIndex<Second>());

        if constexpr (sizeof...(Args) > 0)
        {
            Chain<Second, Args...>(execution);
        }
    }

    template <IsState T> T& GetStates() const
    {
        auto state = m_States.Find<T>();
        if (state == m_States.end())
        {
            D::Log::Error(std::format("State {} not found", typeid(T).name()));
        }
        return static_cast<T&>(**state);
    }

    template <IsState... Args> std::tuple<Args&...> GetStates() const requires(sizeof...(Args) != 1)
    {
        return {GetStates<Args>()...};
    }

    const U::TypedVector<std::unique_ptr<State>>& GetAllStates() const
    {
        return m_States;
    }

    template <IsState T> T& AddStates()
    {
        m_States.Emplace<T>(std::make_unique<T>());
        return static_cast<T&>(*m_States.Get<T>());
    }

    template <IsState... Args> std::tuple<Args&...> AddStates() requires(sizeof...(Args) != 1)
    {
        return {AddStates<Args>()...};
    }

  private:
    std::array<DependencyGraph<unsigned int>, static_cast<unsigned int>(Execution::Count)> m_SystemScheduler;
    U::TypedVector<std::unique_ptr<System>> m_Systems;
    U::TypedVector<std::unique_ptr<State>> m_States;

    const std::function<void(Engine&)> m_Configuration;
    bool m_Running = false;
};
} // namespace Ivy::C
