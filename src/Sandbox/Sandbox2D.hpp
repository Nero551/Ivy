#pragma once
#include "Core/System.hpp"
#include "ECS/EntityID.hpp"
namespace Ivy::Sandbox
{
struct Sandbox2D : C::System
{
    using C::System::System;
    void Start() override;
    void FixedUpdate(double fdt) override;

  private:
    ECS::EntityID CreateCircle();
    ECS::EntityID CreateSquare();
    void PlayWithBall(double fdt);
};

} // namespace Ivy::Sandbox
