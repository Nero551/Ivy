#pragma once

#include "Core/State.hpp"
#include "Core/System.hpp"
#include "ECS/Entity.hpp"
#include "Systems/Components.hpp"
#include "Utilities/CheckedPtr.hpp"
#include <cstddef>
#include <vector>

namespace Ivy::C
{
struct TransformSystemState : C::State
{
    std::vector<U::CheckedPtr<ECS::Entity>> Descendants;
    size_t HierarchyVersion = 0;
    size_t EcsVersion = 0;
};

struct TransformSystem : System
{
    using System::System;
    void Update(double dt) override;

  private:
    void Do2D(U::CheckedPtr<CMP::Transform2D> transform2D, U::CheckedPtr<CMP::Transform2D> parentTransform2D);
    void Do3D(U::CheckedPtr<CMP::Transform3D> transform3D, U::CheckedPtr<CMP::Transform3D> parentTransform3D);
};
} // namespace Ivy::C
