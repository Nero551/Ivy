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
        {.Position = {-0.5f, -0.5f, 0.0f, 1.0f}, .Color = {1, 1, 1, 1}, .UV = {0, 0}, .Normal = {0, 0, 1}},
        {.Position = {0.5f, -0.5f, 0.0f, 1.0f},  .Color = {1, 1, 1, 1}, .UV = {1, 0}, .Normal = {0, 0, 1}},
        {.Position = {0.5f, 0.5f, 0.0f, 1.0f},   .Color = {1, 1, 1, 1}, .UV = {1, 1}, .Normal = {0, 0, 1}},
        {.Position = {-0.5f, 0.5f, 0.0f, 1.0f},  .Color = {1, 1, 1, 1}, .UV = {0, 1}, .Normal = {0, 0, 1}}
    };

    std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};

    G::Mesh mesh{name};
    mesh.Vertices = std::move(vertices);
    mesh.Indices = std::move(indices);
    mesh.CullMode = CullMode::None;

    return mesh;
}
} // namespace Ivy::G::Primitives
