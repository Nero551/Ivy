#pragma once

namespace Ivy::C
{
struct Engine;

struct System
{
    virtual ~System() {}

    System(Engine& engine) : m_Engine(engine) {}

    virtual void Start() {}

    virtual void Update(double dt) {}

    virtual void FixedUpdate(double fdt) {}

    virtual void Stop() {}

    virtual void BeginFrame(double dt) {}

    virtual void Render() {}

    virtual void EndFrame(double dt) {}

  protected:
    Engine& m_Engine;
};
} // namespace Ivy::C
