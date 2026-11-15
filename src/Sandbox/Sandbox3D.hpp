#pragma once
#include "Core/System.hpp"
namespace Ivy::Sandbox
{
struct Sandbox3D : C::System
{
    using C::System::System;
    void Start() override;
    void FixedUpdate(double dt) override;
};

} // namespace Ivy::Sandbox
