#pragma once
#include "Core/Plugin.hpp"
#include "Core/Res.hpp"

#include "Core/ResManager.hpp"
#include "Core/SystemManager.hpp"
#include "Core/TimeHandler.hpp"
#include "Core/World.hpp"
#include "ECS/EntityComponentSystem.hpp"
#include "ECS/Query.hpp"
#include "Platform/Window.hpp"
#include <concepts>
#include <functional>
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
    ResManager RM{};
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

    template <IsPlugin P> void AddPlugins()
    {
        P::Setup(*this);
    }

    template <IsPlugin... Ps> void AddPlugins() requires(sizeof...(Ps) != 1)
    {
        (AddPlugins<Ps>(), ...);
    }

  private:
    bool m_Running = false;
};

template <IsRes T> struct Fetch<T>
{
    static T& Get(Engine& e)
    {
        return e.RM.GetRes<typename T::Base>();
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

template <> struct Fetch<TimeHandler>
{
    static const TimeHandler& Get(Engine& e)
    {
        return e.Time;
    };
};
template <> struct Fetch<Engine>
{
    static Engine& Get(Engine& e)
    {
        return e;
    };
};
} // namespace Ivy::C
