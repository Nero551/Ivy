#include "../Primitives.hpp"

#include "Graphics/Mesh/CullMode.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Mesh/Topology.hpp"
#include "Graphics/Mesh/Vertex.hpp"
#include "Math/Vector/Vector2.hpp"
#include "Math/Vector/Vector4.hpp"
#include <string>
#include <utility>
#include <vector>

namespace Ivy::G::Primitives
{
G::Mesh CreateLine(const std::string& name)
{
    std::vector<G::Vertex> vertices = {G::Vertex({-0.5f, 0, 0, 1}, {1, 1, 1, 1}, {0, 0}, {0, 0, 1}),
        G::Vertex({0.5f, 0, 0, 1}, {1, 1, 1, 1}, {0, 0}, {0, 0, 1})};

    std::vector<unsigned int> indices = {0, 1};

    G::Mesh mesh{name};
    mesh.Vertices = std::move(vertices);
    mesh.Indices = std::move(indices);
    mesh.Topology = Topology::Lines;
    mesh.CullMode = CullMode::None;

    return mesh;
}
} // namespace Ivy::G::Primitives
