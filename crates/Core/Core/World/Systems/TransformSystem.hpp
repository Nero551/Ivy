#pragma once

#include "Core/System.hpp"
#include "Core/World/Components.hpp"
#include "ECS/Entity.hpp"
#include "Utilities/CheckedPtr.hpp"
#include <cstddef>
#include <vector>
namespace Ivy::C
{
struct TransformSystem : System
{
    using System::System;
    void Update(double dt) override;

  private:
    void Do2D(U::CheckedPtr<CMP::Transform2D> transform2D, U::CheckedPtr<CMP::Transform2D> parentTransform2D);
    void Do3D(U::CheckedPtr<CMP::Transform3D> transform3D, U::CheckedPtr<CMP::Transform3D> parentTransform3D);

    std::vector<U::CheckedPtr<ECS::Entity>> descendants;
    size_t hierarchyVersion = 0;
    size_t ecsVersion = 0;
};
} // namespace Ivy::C
