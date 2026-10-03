#pragma once

namespace N::C
{
struct System
{
    virtual ~System() {}

    virtual void Start() {}

    virtual void Update(double dt) {}

    virtual void FixedUpdate(double fdt) {}

    virtual void Stop() {}

    virtual void BeginFrame(double dt) {}

    virtual void Render() {}

    virtual void EndFrame(double dt) {}
};
} // namespace N::C
