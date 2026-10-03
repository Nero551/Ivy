#include "../Primitives.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Math/Color/Color.hpp"

namespace N::R
{
C::Resource<G::Mesh>& Primitives::CreateUVSphere(
    const std::string& name, const float radius, const int sectors, const int stacks)
{

    if (C::Service::Get<C::ResourceManager>().Exists<G::Mesh>(name))
    {
        return C::Service::Get<C::ResourceManager>().Acquire<G::Mesh>(name);
    }

    std::vector<G::Vertex> vertices;
    std::vector<unsigned int> indices;

    vertices.reserve((stacks + 1) * (sectors + 1));
    indices.reserve(stacks * sectors * 6);

    for (int stack = 0; stack <= stacks; ++stack)
    {
        const float phi = std::numbers::pi * stack / stacks;

        const float y = std::cos(phi);
        const float r = std::sin(phi);

        for (int sector = 0; sector <= sectors; ++sector)
        {
            const float theta = 2.0f * std::numbers::pi * sector / sectors;

            const float x = r * std::cos(theta);
            const float z = r * std::sin(theta);

            vertices.emplace_back(G::Vertex{
                {radius * x, radius * y, radius * z, 1.0f},
                M::Color::White,
                {sector / static_cast<float>(sectors), stack / static_cast<float>(stacks)},
                {x, y, z}
            });
        }
    }

    for (int stack = 0; stack < stacks; ++stack)
    {
        int k1 = stack * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int sector = 0; sector < sectors; ++sector)
        {
            if (stack != 0)
            {
                indices.insert(indices.end(),
                    {static_cast<unsigned>(k1), static_cast<unsigned>(k2), static_cast<unsigned>(k1 + 1)});
            }

            if (stack != stacks - 1)
            {
                indices.insert(indices.end(),
                    {static_cast<unsigned>(k1 + 1), static_cast<unsigned>(k2),
                        static_cast<unsigned>(k2 + 1)});
            }

            ++k1;
            ++k2;
        }
    }

    auto& mesh = C::Service::Get<C::ResourceManager>().Load<G::Mesh>(name);

    mesh().Vertices = std::move(vertices);
    mesh().Indices = std::move(indices);

    return mesh;
}
} // namespace N::R
