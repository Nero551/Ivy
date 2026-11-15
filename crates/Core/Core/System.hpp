#pragma once

namespace Ivy::C
{
struct Engine;

struct System
{

    System(Engine& engine) : m_Engine(engine) {}
    virtual ~System() = default;

  protected:
    virtual void Start() {}

    virtual void Update(double dt) {}

    virtual void FixedUpdate(double fdt) {}

    virtual void Stop() {}

    virtual void BeginFrame(double dt) {}

    virtual void Render() {}

    virtual void EndFrame(double dt) {}

    Engine& m_Engine;
    friend struct Engine;
};
} // namespace Ivy::C
