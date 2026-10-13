#pragma once

#include "Core/ECS/Entity.hpp"
#include "Core/Module.hpp"
#include "Math/Vector/Vector4.hpp"

namespace Ivy
{
struct Physics : C::Module
{
    using C::Module::Module;

  protected:
    void Start() override;
    void FixedUpdate(double fdt) override;
    C::Entity& CreatePoint(M::Vector<4> col);
    void Plot(const M::Vector<3> vec3, const M::Vector<4> col = {1, 1, 1, 1});
};
} // namespace Ivy
