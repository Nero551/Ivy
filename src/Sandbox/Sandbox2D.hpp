#pragma once
#include "Core/Module.hpp"
namespace Ivy::Sandbox
{
struct Sandbox2D : C::Module
{
    using Module::Module;
    void Start() override;
    void FixedUpdate(double fdt) override;
};

} // namespace Ivy::Sandbox
