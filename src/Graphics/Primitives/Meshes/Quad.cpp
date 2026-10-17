#include "../Primitives.hpp"

#include "Graphics/Mesh/CullMode.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Mesh/Vertex.hpp"
#include <string>
#include <utility>
#include <vector>

namespace Ivy::G::Primitives
{
G::Mesh CreateQuad(const std::string& name)
{
    std::vector<G::Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f, 1.0f}, {1, 1, 1, 1}, {0, 0}, {0, 0, 1}},
        {{0.5f, -0.5f, 0.0f, 1.0f},  {1, 1, 1, 1}, {1, 0}, {0, 0, 1}},
        {{0.5f, 0.5f, 0.0f, 1.0f},   {1, 1, 1, 1}, {1, 1}, {0, 0, 1}},
        {{-0.5f, 0.5f, 0.0f, 1.0f},  {1, 1, 1, 1}, {0, 1}, {0, 0, 1}}
    };

    std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};

    G::Mesh mesh{name};
    mesh.Vertices = std::move(vertices);
    mesh.Indices = std::move(indices);
    mesh.CullMode = CullMode::None;

    return mesh;
}
} // namespace Ivy::G::Primitives
