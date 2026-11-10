#pragma once
#include "Core/Service.hpp"
#include "Core/TimeHandler.hpp"
#include "Core/World/World.hpp"
#include "Diagnostics/Log.hpp"
#include "ECS/EntityComponentSystem.hpp"
#include "Graphics/Window.hpp"
#include "Module.hpp"
#include "Utilities/DataStructures/TypedVector.hpp"
#include <concepts>
#include <format>
#include <functional>
#include <memory>

/** @brief Root namespace for Ivy engine functionality. */
namespace Ivy::C
{
template <typename T>
concept IsModule = std::derived_from<T, Module>;
template <typename T>
concept IsService = std::derived_from<T, Service>;

/** @brief Main Ivy engine and game loop. Manages the window, world, modules, ECS, and frame lifecycle. */
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

    /** @brief Returns a registered module by type. */
    template <IsModule T> T& GetModule()
    {
        auto module = m_Modules.Find<T>();
        if (module == m_Modules.end())
        {
            D::Log::Error(std::format("Module {} not found", typeid(T).name()));
        }
        return static_cast<T&>(**module);
    }

    template <IsService T> T& GetService()
    {
        auto service = m_Services.Find<T>();
        if (service == m_Services.end())
        {
            D::Log::Error(std::format("Service Not Found: {}", typeid(T).name()));
        }
        return static_cast<T&>(**service);
    }

    const U::TypedVector<std::unique_ptr<Module>>& GetAllModules() const
    {
        return m_Modules;
    }

    const U::TypedVector<std::unique_ptr<Service>>& GetAllServices() const
    {
        return m_Services;
    }

    /** @brief Creates and registers a module. */
    template <IsModule T> T& AddModule()
    {
        m_Modules.Emplace<T>(std::make_unique<T>(*this));
        return static_cast<T&>(*m_Modules.Get<T>());
    }

    template <IsService T> T& AddService()
    {
        if (m_Services.Contains<T>())
        {
            D::Log::Error(std::format(" Service {} Already Added", typeid(T).name()));
            return static_cast<T&>(*m_Services.At<T>());
        }

        m_Services.Emplace<T>(std::make_unique<T>());

        return static_cast<T&>(*m_Services.Get<T>());
    }

  private:
    /** @brief Registered engine modules. */
    U::TypedVector<std::unique_ptr<Module>> m_Modules;
    U::TypedVector<std::unique_ptr<Service>> m_Services;
    const std::function<void(Engine&)> m_Configuration;
    bool m_Running = false;
};
} // namespace Ivy::C
