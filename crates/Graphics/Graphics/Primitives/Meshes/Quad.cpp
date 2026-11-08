#include "../Primitives.hpp"
#include "Graphics/Mesh/CullMode.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Mesh/Vertex.hpp"
#include "Utilities/Debug/Log.hpp"

#include <string>
#include <utility>
#include <vector>

namespace Ivy::G::Primitives
{
G::Mesh CreateQuad(const std::string& name, unsigned int widthSegments, unsigned int heightSegments)
{
    U::Log::Assert(widthSegments > 0, "Quad width segments must be greater than zero");
    U::Log::Assert(heightSegments > 0, "Quad height segments must be greater than zero");

    std::vector<G::Vertex> vertices;
    std::vector<unsigned int> indices;

    const unsigned int vertexCount = (widthSegments + 1) * (heightSegments + 1);
    const unsigned int indexCount = widthSegments * heightSegments * 6;

    vertices.reserve(vertexCount);
    indices.reserve(indexCount);

    for (unsigned int y = 0; y <= heightSegments; ++y)
    {
        for (unsigned int x = 0; x <= widthSegments; ++x)
        {
            const float u = static_cast<float>(x) / static_cast<float>(widthSegments);
            const float v = static_cast<float>(y) / static_cast<float>(heightSegments);

            vertices.push_back({
                .Position = {u - 0.5f, v - 0.5f, 0.0f, 1.0f},
                .Color = {1, 1, 1, 1},
                .UV = {u, v},
                .Normal = {0, 0, 1}
            });
        }
    }

    const unsigned int rowSize = widthSegments + 1;

    for (unsigned int y = 0; y < heightSegments; ++y)
    {
        for (unsigned int x = 0; x < widthSegments; ++x)
        {
            const unsigned int bottomLeft = y * rowSize + x;
            const unsigned int bottomRight = bottomLeft + 1;
            const unsigned int topLeft = bottomLeft + rowSize;
            const unsigned int topRight = topLeft + 1;

            indices.insert(indices.end(), {bottomLeft, bottomRight, topRight, topRight, topLeft, bottomLeft});
        }
    }

    G::Mesh mesh{name};
    mesh.Vertices = std::move(vertices);
    mesh.Indices = std::move(indices);
    mesh.CullMode = CullMode::None;

    return mesh;
}
} // namespace Ivy::G::Primitives
