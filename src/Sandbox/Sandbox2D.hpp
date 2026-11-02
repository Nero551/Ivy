#pragma once
#include "Core/ECS/EntityID.hpp"
#include "Core/Module.hpp"
namespace Ivy::Sandbox
{
struct Sandbox2D : C::Module
{
    using Module::Module;
    void Start() override;
    void FixedUpdate(double fdt) override;

  private:
    C::EntityID CreateCircle();
    C::EntityID CreateSquare();
    void PlayWithBall(double fdt);
};

} // namespace Ivy::Sandbox
