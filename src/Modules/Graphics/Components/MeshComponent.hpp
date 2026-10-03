#pragma once
#include "Core/World/ECS/Component.hpp"
#include "Modules/Graphics/Resources/Mesh/Mesh.hpp"
#include "Utilities/CheckedPtr.hpp"

namespace N::G
{
struct MeshComponent : C::Component
{
    U::CheckedPtr<Mesh> Mesh{"Mesh Component Has No Mesh Assigned"};
};
} // namespace N::G
