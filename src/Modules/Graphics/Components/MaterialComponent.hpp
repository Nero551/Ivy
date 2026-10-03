#pragma once
#include "Core/World/ECS/Component.hpp"
#include "Modules/Graphics/Resources/Material/Material.hpp"
#include "Utilities/CheckedPtr.hpp"

namespace N::G
{
struct MaterialComponent : C::Component
{
    U::CheckedPtr<Material> Material{"Material Component Has No Material Assigned"};
};
} // namespace N::G
