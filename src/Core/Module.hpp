#pragma once

#include "SystemOwner.hpp"

namespace N::C
{
/**
 * @brief Base class for all engine modules.
 * Modules provide engine-specific functionality and participate in the
 * engine's lifecycle through protected callback methods.
 * Lifecycle callbacks are invoked by Engine and may be overridden by
 * derived modules to implement initialization, updating, rendering,
 * and shutdown behavior.
 * @remark Order: OnStart -> OnBeginFrame -> OnFixedUpdate -> OnUpdate -> OnRender ->
 * OnEndFrame -> OnStop
 */
struct Module : SystemOwner
{
    Module() = default;

    Module(const Module&) = delete;

    Module& operator=(const Module&) = delete;

    Module(Module&&) = default;

    Module& operator=(Module&&) = default;

  protected:
    /**
     * @brief Called once when the module is started.
     * Override this method to perform initialization.
     * @note Called by Engine. Do not call this method directly.
     */
    virtual void Start() {}

    /**
     * @brief Called once per variable-timestep frame.
     * Use this callback for logic that should run every frame and does
     * not require a fixed timestep.
     * @param dt Time elapsed since the previous frame, in seconds.
     * @note Called by Engine. Do not call this method directly.
     */
    virtual void Update(double dt) {}

    /**
     * @brief Called once per fixed-timestep update.
     * Use this callback for deterministic or physics-related logic that
     * should run at a fixed timestep.
     * @param fdt Fixed time step between updates, in seconds.
     * @note Called by Engine. Do not call this method directly.
     */
    virtual void FixedUpdate(double fdt) {}

    /**
     * @brief Called when the module's rendering stage is executed.
     * Use this callback to issue rendering commands or perform other
     * render-related work.
     * @note Called by Engine. Do not call this method directly.
     */
    virtual void Render() {}

    /**
     * @brief Called at the beginning of a frame.
     * Use this callback for per-frame preparation that must occur before
     * the main update and rendering stages.
     * @param dt Time elapsed since the previous frame, in seconds.
     * @note Called by Engine. Do not call this method directly.
     */
    virtual void BeginFrame(double dt) {}

    /**
     * @brief Called at the end of a frame.
     * Use this callback for operations that should occur after the main
     * update and rendering stages of the frame.
     * @param dt Time elapsed since the previous frame, in seconds.
     * @note Called by Engine. Do not call this method directly.
     */
    virtual void EndFrame(double dt) {}

    /**
     * @brief Called once when the module is stopped.
     * Override this method to perform cleanup.
     * @note Called by Engine. Do not call this method directly.
     */
    virtual void Stop() {}

    friend struct Engine;
};
} // namespace N::C
