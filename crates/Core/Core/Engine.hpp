#pragma once
#include "Core/State.hpp"
#include "Core/System.hpp"
#include "Core/SystemManager.hpp"
#include "Core/TimeHandler.hpp"
#include "Core/World/World.hpp"
#include "Diagnostics/Log.hpp"
#include "ECS/EntityComponentSystem.hpp"
#include "Graphics/Window.hpp"
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

template <typename T>
concept IsState = std::derived_from<T, C::State>;

/** @brief Main Ivy engine and game loop. Manages the window, world, systems, ECS, and frame lifecycle. */
struct Engine
{
    G::Window Window{};
    World World{*this};
    ECS::EntityComponentSystem ECS{};
    TimeHandler TimeHandler{};
    SystemManager SM;

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
    U::TypedVector<std::unique_ptr<State>> m_States;

    const std::function<void(Engine&)> m_Configuration;
    bool m_Running = false;
};
} // namespace Ivy::C
