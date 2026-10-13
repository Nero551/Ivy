#pragma once

namespace Ivy::C
{
/**
 * @brief Base class for globally accessible services.
 * Services aren't owned by the Engine, primarily to avoid cyclic dependencies.
 * services do get destroyed ,created & updated in the engine loop though.
 */
struct Service
{
    virtual ~Service() = default;

  protected:
    friend struct Engine;
    virtual void Start() {}
    virtual void BeginFrame(double dt) {}
    virtual void FixedUpdate(double fdt) {}
    virtual void Update(double dt) {}
    virtual void Render() {}
    virtual void EndFrame(double dt) {}
    virtual void Stop() {}
};
} // namespace Ivy::C
