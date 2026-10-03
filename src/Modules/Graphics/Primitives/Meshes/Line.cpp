#include "../Primitives.hpp"
#include "Core/Engine.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Math/Color/Color.hpp"

namespace N::G
{
Mesh& Primitives::CreateLine(const std::string& name)
{
    if (C::Service::Get<C::ResourceManager>().Exists<Mesh>(name))
    {
        return C::Service::Get<C::ResourceManager>().Load<Mesh>(name);
    }

    std::vector vertices = {Vertex({0, 0, -0.5f, 1}, M::Vector<4>{1}, M::Vector<2>{0}, {0, 0, 1}),
        Vertex({0, 0, 0.5f, 1}, M::Vector<4>{1}, M::Vector<2>{0}, {0, 0, 1})};

    std::vector<unsigned int> indices = {0, 1};

    Mesh& mesh = C::Service::Get<C::ResourceManager>().Load<Mesh>(name);
    mesh.Vertices = std::move(vertices);
    mesh.Indices = std::move(indices);
    mesh.Topology = Topology::Lines;

    return mesh;
}
} // namespace N::G