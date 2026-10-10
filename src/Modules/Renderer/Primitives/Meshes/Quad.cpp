#include "../Primitives.hpp"
#include "Core/Service.hpp"
#include "Core/Services/ResourceManager/Resource.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Mesh/Vertex.hpp"
#include <string>
#include <utility>
#include <vector>

namespace Ivy::R
{
C::Resource<G::Mesh>& Primitives::CreateQuad(const std::string& name)
{
    if (C::Service::Get<C::ResourceManager>().Exists<G::Mesh>(name))
    {
        return C::Service::Get<C::ResourceManager>().Load<G::Mesh>(name);
    }

    std::vector<G::Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f, 1.0f}, {1, 1, 1, 1}, {0, 0}, {0, 0, 1}},
        {{0.5f, -0.5f, 0.0f, 1.0f},  {1, 1, 1, 1}, {1, 0}, {0, 0, 1}},
        {{0.5f, 0.5f, 0.0f, 1.0f},   {1, 1, 1, 1}, {1, 1}, {0, 0, 1}},
        {{-0.5f, 0.5f, 0.0f, 1.0f},  {1, 1, 1, 1}, {0, 1}, {0, 0, 1}}
    };

    std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};

    auto& mesh = C::Service::Get<C::ResourceManager>().Load<G::Mesh>(name);
    mesh().Vertices = std::move(vertices);
    mesh().Indices = std::move(indices);

    return mesh;
}
} // namespace Ivy::R