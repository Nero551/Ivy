#pragma once
#include "Core/TimeHandler.hpp"
#include "Graphics/Window.hpp"
#include "Module.hpp"
#include "Utilities/CheckedPtr.hpp"
#include "World/World.hpp"

/** @brief Root namespace for Ivy engine functionality. */
namespace Ivy::C
{
template <typename T>
concept IsModule = std::derived_from<T, Module>;

/**
 * @brief Main Ivy engine and game loop. Manages the window, world, modules, and frame lifecycle.
 */
struct Engine
{
    G::Window Window;
    std::optional<World> World;

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;

    /** @brief Constructs the engine and registers it as the global instance. */
    Engine();

    /** @brief Returns the global engine instance. */
    static Engine& Get();

    /**
     * @brief Runs the engine's main loop.
     * @remark Order: Start -> BeginFrame -> FixedUpdate -> Update -> Render -> EndFrame
     * -> Stop
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

    const U::TypedVector<std::unique_ptr<Module>>& GetAllModules() const
    {
        return m_Modules;
    }

  private:
    /** @brief Global engine instance. */
    inline static U::CheckedPtr<Engine> Instance = nullptr;

    /** @brief Registered engine modules. */
    U::TypedVector<std::unique_ptr<Module>> m_Modules;
    TimeHandler m_TimeHandler;

    bool m_Running = false;

    /** @brief Initializes engine services, modules & custom configurations. */
    void Configure();

    /** @brief Creates and registers a module. */
    template <IsModule T> T& AddModule()
    {
        auto module = std::make_unique<T>();
        m_Modules.Emplace<T>(std::move(module));
        return static_cast<T&>(*m_Modules.Get<T>());
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
