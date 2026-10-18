#include "../Primitives.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Mesh/Vertex.hpp"
#include "Math/Common/Constants.hpp"
#include <cmath>

namespace Ivy::G::Primitives
{
Mesh Primitives::CreateCircle(const std::string& name, float radius, int sectors)
{
    Mesh mesh(name);

    if (sectors < 3 || radius <= 0.0)
    {
        return mesh;
    }

    mesh.Vertices.reserve(sectors + 1);
    mesh.Indices.reserve(sectors * 3);

    // Center vertex
    mesh.Vertices.emplace_back(Vertex{
        .Position = {0.0, 0.0, 0.0, 1},
        .Color = {1, 1, 1, 1},
        .UV = {0.5, 0.5},
        .Normal = {0.0, 0.0, 1.0},
    });

    // Perimeter vertices
    for (int i = 0; i < sectors; ++i)
    {
        const float angle = M::TAU * static_cast<float>(i) / static_cast<float>(sectors);

        const float x = radius * std::cos(angle);
        const float y = radius * std::sin(angle);

        mesh.Vertices.emplace_back(Vertex{
            .Position = {x, y, 0.0, 1},
            .Color = {1, 1, 1, 1},
            .UV = {0.5f + 0.5f * std::cos(angle), 0.5f + 0.5f * std::sin(angle)},
            .Normal = {0.0, 0.0, 1.0},
        });
    }

    // Triangle fan
    for (int i = 0; i < sectors; ++i)
    {
        mesh.Indices.push_back(0);
        mesh.Indices.push_back(static_cast<unsigned int>(i + 1));
        mesh.Indices.push_back(static_cast<unsigned int>((i + 1) % sectors + 1));
    }

    return mesh;
}
} // namespace Ivy::G::Primitives
