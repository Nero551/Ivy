#pragma once
#include "Core/World/ECS/Component.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Utilities/CheckedPtr.hpp"

namespace N::R
{
struct MeshComponent : C::Component
{
    U::CheckedPtr<C::Resource<G::Mesh>> Mesh{"Mesh Component Has No Mesh Assigned"};
};
} // namespace N::R
