#pragma once
#include "Core/Plugin.hpp"
#include "Core/State.hpp"

#include "Core/SystemManager.hpp"
#include "Core/TimeHandler.hpp"
#include "Core/World.hpp"
#include "Diagnostics/Log.hpp"
#include "ECS/EntityComponentSystem.hpp"
#include "ECS/Query.hpp"
#include "Platform/Window.hpp"
#include "Utilities/DataStructures/TypedVector.hpp"
#include <concepts>
#include <format>
#include <functional>
#include <memory>
#include <tuple>
#include <utility>

/** @brief Root namespace for Ivy engine functionality. */
namespace Ivy::C
{

/** @brief Main Ivy engine and game loop. Manages the window, world, systems, ECS, and frame lifecycle. */
struct Engine
{
    Platform::Window Window{};
    ECS::EntityComponentSystem ECS{};
    World World{ECS};
    TimeHandler Time{};
    SystemManager SM{};
    const std::function<void(Engine&)> Configuration;

    template <typename F>
    Engine(F&& configure) requires(std::invocable<F, Engine&>)
        : World(ECS), Configuration(std::forward<F>(configure))
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

    template <IsPlugin P> void AddPlugins()
    {
        P::Setup(*this);
    }

    template <IsPlugin... Ps> void AddPlugins() requires(sizeof...(Ps) != 1)
    {
        (AddPlugins<Ps>(), ...);
    }

  private:
    U::TypedVector<std::unique_ptr<State>> m_States;
    bool m_Running = false;
};

template <typename T> requires(IsState<T>)
struct Fetch<T>
{
    static T& Get(Engine& e)
    {
        return e.GetStates<T>();
    };
};

template <> struct Fetch<ECS::EntityComponentSystem>
{
    static ECS::EntityComponentSystem& Get(Engine& e)
    {
        return e.ECS;
    };
};
template <> struct Fetch<ECS::Query>
{
    static ECS::Query& Get(Engine& e)
    {
        return e.ECS.Query;
    };
};

template <> struct Fetch<World>
{
    static World& Get(Engine& e)
    {
        return e.World;
    };
};
template <> struct Fetch<Platform::Window>
{
    static Platform::Window& Get(Engine& e)
    {
        return e.Window;
    };
};

template <> struct Fetch<Engine>
{
    static Engine& Get(Engine& e)
    {
        return e;
    };
};
template <> struct Fetch<TimeHandler>
{
    static const TimeHandler& Get(Engine& e)
    {
        return e.Time;
    };
};
template <> struct Fetch<double>
{
    static decltype(auto) Get(Engine& e)
    {
        return e.Time.GetDelta();
    };
};
} // namespace Ivy::C
