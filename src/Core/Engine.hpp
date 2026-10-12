#pragma once
#include "Core/Service.hpp"
#include "Core/TimeHandler.hpp"
#include "Core/World/World.hpp"
#include "ECS/ECS.hpp"
#include "Graphics/Window.hpp"
#include "Module.hpp"
#include "Utilities/CheckedPtr.hpp"
#include "Utilities/DataStructures/TypedVector.hpp"
#include "Utilities/Debug/Log.hpp"
#include <concepts>
#include <format>
#include <memory>

/** @brief Root namespace for Ivy engine functionality. */
namespace Ivy::C
{
template <typename T>
concept IsModule = std::derived_from<T, Module>;
template <typename T>
concept ServiceType = std::derived_from<T, Service>;

/**
 * @brief Main Ivy engine and game loop. Manages the window, world, modules, and frame lifecycle.
 */
struct Engine
{
    G::Window Window{};
    World World{*this};
    EntityComponentSystem ECS{};

    Engine() {}
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;

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
            U::Log::Fatal(std::format("Module {} not found", typeid(T).name()));
        }
        return static_cast<T&>(**module);
    }

    template <ServiceType T> T& GetService()
    {
        auto service = m_Services.Find<T>();
        if (service == m_Services.end())
        {
            U::Log::Fatal(std::format("Service Not Found: {}", typeid(T).name()));
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

  private:
    /** @brief Global engine instance. */
    inline static U::CheckedPtr<Engine> Instance = nullptr;

    /** @brief Registered engine modules. */
    U::TypedVector<std::unique_ptr<Module>> m_Modules;
    U::TypedVector<std::unique_ptr<Service>> m_Services;
    TimeHandler m_TimeHandler;

    bool m_Running = false;

    /** @brief Initializes engine services, modules & custom configurations. */
    void Configure();

    /** @brief Creates and registers a module. */
    template <IsModule T> T& AddModule()
    {
        m_Modules.Emplace<T>(std::make_unique<T>(*this));
        return static_cast<T&>(*m_Modules.Get<T>());
    }

    template <ServiceType T> T& AddService()
    {
        if (m_Services.Contains<T>())
        {
            U::Log::Error(std::format(" Service {} Already Added", typeid(T).name()));
            return static_cast<T&>(*m_Services.At<T>());
        }

        m_Services.Emplace<T>(std::make_unique<T>());

        return static_cast<T&>(*m_Services.Get<T>());
    }

    void Start();
    void BeginFrame();
    void FixedUpdate();
    void Update();
    void Render();
    void EndFrame();
    void Stop();
};
} // namespace Ivy::C
