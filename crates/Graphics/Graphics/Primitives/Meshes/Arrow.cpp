#include "../Primitives.hpp"
#include "Graphics/Mesh/Mesh.hpp"

namespace Ivy::G
{
Mesh Primitives::CreateArrow2D(const std::string& name)
{
    Mesh mesh(name);

    mesh.Vertices = {
        // Shaft and head outline
        {.Position = {0.0, -0.15, 0.0, 1.0}, .UV = {0.0, 0.0}, .Normal = {0.0, 0.0, 1.0}},
        {.Position = {0.6, -0.15, 0.0, 1.0}, .UV = {0.6, 0.0}, .Normal = {0.0, 0.0, 1.0}},
        {.Position = {0.6, -0.35, 0.0, 1.0}, .UV = {0.6, 0.0}, .Normal = {0.0, 0.0, 1.0}},
        {.Position = {1.0, 0.0, 0.0, 1.0},   .UV = {1.0, 0.5}, .Normal = {0.0, 0.0, 1.0}},
        {.Position = {0.6, 0.35, 0.0, 1.0},  .UV = {0.6, 1.0}, .Normal = {0.0, 0.0, 1.0}},
        {.Position = {0.6, 0.15, 0.0, 1.0},  .UV = {0.6, 1.0}, .Normal = {0.0, 0.0, 1.0}},
        {.Position = {0.0, 0.15, 0.0, 1.0},  .UV = {0.0, 1.0}, .Normal = {0.0, 0.0, 1.0}}
    };

    mesh.Indices = {0, 1, 6, 1, 5, 6, 1, 2, 5, 2, 4, 5, 2, 3, 4};

    return mesh;
}

Mesh Primitives::CreateArrow3D(const std::string& name)
{
    Mesh mesh(name);

    constexpr unsigned Segments = 8;

    constexpr float Length = 1.0f;
    constexpr float ShaftRadius = 0.04f;
    constexpr float HeadRadius = 0.12f;
    constexpr float HeadLength = 0.25f;

    constexpr float ShaftLength = Length - HeadLength;

    // Shaft
    for (unsigned i = 0; i < Segments; ++i)
    {
        float Angle = 2.0f * std::numbers::pi_v<float> * i / Segments;

        float Y = std::cos(Angle);
        float Z = std::sin(Angle);

        mesh.Vertices.push_back({
            .Position = {0.0f, Y * ShaftRadius, Z * ShaftRadius, 1.0f},
            .UV = {0.0f, static_cast<float>(i) / Segments},
            .Normal = {0.0f, Y, Z}
        });

        mesh.Vertices.push_back({
            .Position = {ShaftLength, Y * ShaftRadius, Z * ShaftRadius, 1.0f},
            .UV = {ShaftLength, static_cast<float>(i) / Segments},
            .Normal = {0.0f, Y, Z}
        });
    }

    // Shaft indices
    for (unsigned i = 0; i < Segments; ++i)
    {
        unsigned Next = (i + 1) % Segments;

        unsigned A = i * 2;
        unsigned B = Next * 2;
        unsigned C = A + 1;
        unsigned D = B + 1;

        mesh.Indices.insert(mesh.Indices.end(), {A, B, C, C, B, D});
    }

    // Arrowhead base
    unsigned HeadBase = static_cast<unsigned>(mesh.Vertices.size());

    for (unsigned i = 0; i < Segments; ++i)
    {
        float Angle = 2.0f * std::numbers::pi_v<float> * i / Segments;

        float Y = std::cos(Angle);
        float Z = std::sin(Angle);

        mesh.Vertices.push_back({
            .Position = {ShaftLength, Y * HeadRadius, Z * HeadRadius, 1.0f},
            .UV = {0.0f, static_cast<float>(i) / Segments},
            .Normal = {Y, Z, 0.0f}
        });
    }

    // Arrowhead tip
    unsigned Tip = static_cast<unsigned>(mesh.Vertices.size());

    mesh.Vertices.push_back({
        .Position = {Length, 0.0f, 0.0f, 1.0f},
          .UV = {1.0f, 0.5f},
          .Normal = {1.0f, 0.0f, 0.0f}
    });

    // Arrowhead triangles
    for (unsigned i = 0; i < Segments; ++i)
    {
        unsigned Next = (i + 1) % Segments;

        mesh.Indices.insert(mesh.Indices.end(), {HeadBase + i, HeadBase + Next, Tip});
    }

    return mesh;
}
} // namespace Ivy::G
