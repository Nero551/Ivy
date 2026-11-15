#pragma once
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
#include <vector>

/** @brief Root namespace for Ivy engine functionality. */
namespace Ivy::C
{
template <typename T>
concept IsSystem = std::derived_from<T, C::System>;

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

    template <IsSystem T> T& GetSystems()
    {
        auto system = m_Systems.Find<T>();
        if (system == m_Systems.end())
        {
            D::Log::Error(std::format("System {} not found", typeid(T).name()));
        }
        return static_cast<T&>(**system);
    }

    template <IsSystem... Args> std::tuple<Args&...> GetSystems() requires(sizeof...(Args) != 1)
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
        return static_cast<T&>(*m_Systems.Get<T>());
    }

    template <IsSystem... Args> std::tuple<Args&...> AddSystems() requires(sizeof...(Args) != 1)
    {
        return {AddSystems<Args>()...};
    }

    template <typename Tuple> decltype(auto) AddSystems() requires(U::IsTSpec<Tuple, std::tuple>)
    {
        return U::UnpackAndRun<Tuple>(std::make_index_sequence<std::tuple_size_v<Tuple>>{},
            [this]<IsSystem... Args>() { return AddSystems<Args...>(); });
    }

  private:
    U::TypedVector<std::unique_ptr<System>> m_Systems;
    const std::function<void(Engine&)> m_Configuration;
    bool m_Running = false;
};
} // namespace Ivy::C
