#pragma once
#include "Core/ECS/Component.hpp"
#include "Graphics/Material/Material.hpp"
#include "Utilities/CheckedPtr.hpp"

namespace Ivy::R
{
struct MaterialComponent : C::Component
{
    U::CheckedPtr<C::Resource<G::Material>> Material{"Material Component Has No Material Assigned"};
};
} // namespace Ivy::R
