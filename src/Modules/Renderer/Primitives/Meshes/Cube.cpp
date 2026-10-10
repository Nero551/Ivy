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
C::Resource<G::Mesh>& Primitives::CreateCube(const std::string& name)
{
    if (C::Service::Get<C::ResourceManager>().Exists<G::Mesh>(name))
    {
        return C::Service::Get<C::ResourceManager>().Load<G::Mesh>(name);
    }

    std::vector<G::Vertex> vertices = {// Front (+Z)
        G::Vertex({-0.5f, -0.5f, 0.5f, 1}, {1, 0, 0, 1}, {0, 0}, {0, 0, 1}),
        G::Vertex({0.5f, -0.5f, 0.5f, 1}, {1, 0, 1, 1}, {1, 0}, {0, 0, 1}),
        G::Vertex({0.5f, 0.5f, 0.5f, 1}, {0, 1, 0, 1}, {1, 1}, {0, 0, 1}),
        G::Vertex({-0.5f, 0.5f, 0.5f, 1}, {1, 1, 1, 1}, {0, 1}, {0, 0, 1}),

        // Back (-Z)
        G::Vertex({0.5f, -0.5f, -0.5f, 1}, {1, 0, 0, 1}, {0, 0}, {0, 0, -1}),
        G::Vertex({-0.5f, -0.5f, -0.5f, 1}, {1, 0, 1, 1}, {1, 0}, {0, 0, -1}),
        G::Vertex({-0.5f, 0.5f, -0.5f, 1}, {0, 1, 0, 1}, {1, 1}, {0, 0, -1}),
        G::Vertex({0.5f, 0.5f, -0.5f, 1}, {1, 1, 1, 1}, {0, 1}, {0, 0, -1}),

        // Left (-X)
        G::Vertex({-0.5f, -0.5f, -0.5f, 1}, {1, 0, 0, 1}, {0, 0}, {-1, 0, 0}),
        G::Vertex({-0.5f, -0.5f, 0.5f, 1}, {1, 0, 1, 1}, {1, 0}, {-1, 0, 0}),
        G::Vertex({-0.5f, 0.5f, 0.5f, 1}, {0, 1, 0, 1}, {1, 1}, {-1, 0, 0}),
        G::Vertex({-0.5f, 0.5f, -0.5f, 1}, {1, 1, 1, 1}, {0, 1}, {-1, 0, 0}),

        // Right (+X)
        G::Vertex({0.5f, -0.5f, 0.5f, 1}, {1, 0, 0, 1}, {0, 0}, {1, 0, 0}),
        G::Vertex({0.5f, -0.5f, -0.5f, 1}, {1, 0, 1, 1}, {1, 0}, {1, 0, 0}),
        G::Vertex({0.5f, 0.5f, -0.5f, 1}, {0, 1, 0, 1}, {1, 1}, {1, 0, 0}),
        G::Vertex({0.5f, 0.5f, 0.5f, 1}, {1, 1, 1, 1}, {0, 1}, {1, 0, 0}),

        // Top (+Y)
        G::Vertex({-0.5f, 0.5f, 0.5f, 1}, {1, 0, 0, 1}, {0, 0}, {0, 1, 0}),
        G::Vertex({0.5f, 0.5f, 0.5f, 1}, {1, 0, 1, 1}, {1, 0}, {0, 1, 0}),
        G::Vertex({0.5f, 0.5f, -0.5f, 1}, {0, 1, 0, 1}, {1, 1}, {0, 1, 0}),
        G::Vertex({-0.5f, 0.5f, -0.5f, 1}, {1, 1, 1, 1}, {0, 1}, {0, 1, 0}),

        // Bottom (-Y)
        G::Vertex({-0.5f, -0.5f, -0.5f, 1}, {1, 0, 0, 1}, {0, 0}, {0, -1, 0}),
        G::Vertex({0.5f, -0.5f, -0.5f, 1}, {1, 0, 1, 1}, {1, 0}, {0, -1, 0}),
        G::Vertex({0.5f, -0.5f, 0.5f, 1}, {0, 1, 0, 1}, {1, 1}, {0, -1, 0}),
        G::Vertex({-0.5f, -0.5f, 0.5f, 1}, {1, 1, 1, 1}, {0, 1}, {0, -1, 0})};

    std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8, 12, 13, 14,
        14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};

    auto& mesh = C::Service::Get<C::ResourceManager>().Load<G::Mesh>(name);
    mesh().Vertices = std::move(vertices);
    mesh().Indices = std::move(indices);

    return mesh;
}
} // namespace Ivy::R