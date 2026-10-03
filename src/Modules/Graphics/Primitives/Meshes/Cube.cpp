#include "../Primitives.hpp"
#include "Core/Engine.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"

namespace N::G
{
Mesh& Primitives::CreateCube(const std::string& name)
{
    if (C::Service::Get<C::ResourceManager>().Exists<Mesh>(name))
    {
        return C::Service::Get<C::ResourceManager>().Load<Mesh>(name);
    }

    std::vector vertices = {// Front (+Z)
        Vertex({-0.5f, -0.5f, 0.5f, 1}, {1, 0, 0, 1}, {0, 0}, {0, 0, 1}),
        Vertex({0.5f, -0.5f, 0.5f, 1}, {1, 0, 1, 1}, {1, 0}, {0, 0, 1}),
        Vertex({0.5f, 0.5f, 0.5f, 1}, {0, 1, 0, 1}, {1, 1}, {0, 0, 1}),
        Vertex({-0.5f, 0.5f, 0.5f, 1}, {1, 1, 1, 1}, {0, 1}, {0, 0, 1}),

        // Back (-Z)
        Vertex({0.5f, -0.5f, -0.5f, 1}, {1, 0, 0, 1}, {0, 0}, {0, 0, -1}),
        Vertex({-0.5f, -0.5f, -0.5f, 1}, {1, 0, 1, 1}, {1, 0}, {0, 0, -1}),
        Vertex({-0.5f, 0.5f, -0.5f, 1}, {0, 1, 0, 1}, {1, 1}, {0, 0, -1}),
        Vertex({0.5f, 0.5f, -0.5f, 1}, {1, 1, 1, 1}, {0, 1}, {0, 0, -1}),

        // Left (-X)
        Vertex({-0.5f, -0.5f, -0.5f, 1}, {1, 0, 0, 1}, {0, 0}, {-1, 0, 0}),
        Vertex({-0.5f, -0.5f, 0.5f, 1}, {1, 0, 1, 1}, {1, 0}, {-1, 0, 0}),
        Vertex({-0.5f, 0.5f, 0.5f, 1}, {0, 1, 0, 1}, {1, 1}, {-1, 0, 0}),
        Vertex({-0.5f, 0.5f, -0.5f, 1}, {1, 1, 1, 1}, {0, 1}, {-1, 0, 0}),

        // Right (+X)
        Vertex({0.5f, -0.5f, 0.5f, 1}, {1, 0, 0, 1}, {0, 0}, {1, 0, 0}),
        Vertex({0.5f, -0.5f, -0.5f, 1}, {1, 0, 1, 1}, {1, 0}, {1, 0, 0}),
        Vertex({0.5f, 0.5f, -0.5f, 1}, {0, 1, 0, 1}, {1, 1}, {1, 0, 0}),
        Vertex({0.5f, 0.5f, 0.5f, 1}, {1, 1, 1, 1}, {0, 1}, {1, 0, 0}),

        // Top (+Y)
        Vertex({-0.5f, 0.5f, 0.5f, 1}, {1, 0, 0, 1}, {0, 0}, {0, 1, 0}),
        Vertex({0.5f, 0.5f, 0.5f, 1}, {1, 0, 1, 1}, {1, 0}, {0, 1, 0}),
        Vertex({0.5f, 0.5f, -0.5f, 1}, {0, 1, 0, 1}, {1, 1}, {0, 1, 0}),
        Vertex({-0.5f, 0.5f, -0.5f, 1}, {1, 1, 1, 1}, {0, 1}, {0, 1, 0}),

        // Bottom (-Y)
        Vertex({-0.5f, -0.5f, -0.5f, 1}, {1, 0, 0, 1}, {0, 0}, {0, -1, 0}),
        Vertex({0.5f, -0.5f, -0.5f, 1}, {1, 0, 1, 1}, {1, 0}, {0, -1, 0}),
        Vertex({0.5f, -0.5f, 0.5f, 1}, {0, 1, 0, 1}, {1, 1}, {0, -1, 0}),
        Vertex({-0.5f, -0.5f, 0.5f, 1}, {1, 1, 1, 1}, {0, 1}, {0, -1, 0})};

    std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8, 12, 13, 14,
        14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};

    auto& mesh = C::Service::Get<C::ResourceManager>().Load<Mesh>(name);
    mesh.Vertices = std::move(vertices);
    mesh.Indices = std::move(indices);

    return mesh;
}
} // namespace N::G