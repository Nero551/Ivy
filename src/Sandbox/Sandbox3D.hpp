#pragma once
#include "Core/Module.hpp"
namespace Ivy::Sandbox
{
struct Sandbox3D : C::Module
{
    using Module::Module;
    void Start() override;
    void FixedUpdate(double dt) override;
};

} // namespace Ivy::Sandbox
