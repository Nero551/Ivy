#include "../Primitives.hpp"
#include "Core/Engine.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Math/Color/Color.hpp"

namespace N::R
{
C::Resource<G::Mesh>& Primitives::CreateLine(const std::string& name)
{
    if (C::Service::Get<C::ResourceManager>().Exists<G::Mesh>(name))
    {
        return C::Service::Get<C::ResourceManager>().Acquire<G::Mesh>(name);
    }

    std::vector<G::Vertex> vertices = {
        G::Vertex({0, 0, -0.5f, 1}, M::Vector<4>{1}, M::Vector<2>{0}, {0, 0, 1}),
        G::Vertex({0, 0, 0.5f, 1}, M::Vector<4>{1}, M::Vector<2>{0}, {0, 0, 1})};

    std::vector<unsigned int> indices = {0, 1};

    C::Resource<G::Mesh>& mesh = C::Service::Get<C::ResourceManager>().Load<G::Mesh>(name);
    mesh().Vertices = std::move(vertices);
    mesh().Indices = std::move(indices);
    mesh().Topology = G::Topology::Lines;

    return mesh;
}
} // namespace N::R