#pragma once
#include "Core/Module.hpp"
#include "ECS/EntityID.hpp"
namespace Ivy::Sandbox
{
struct Sandbox2D : C::Module
{
    using Module::Module;
    void Start() override;
    void FixedUpdate(double fdt) override;

  private:
    ECS::EntityID CreateCircle();
    ECS::EntityID CreateSquare();
    void PlayWithBall(double fdt);
};

} // namespace Ivy::Sandbox
