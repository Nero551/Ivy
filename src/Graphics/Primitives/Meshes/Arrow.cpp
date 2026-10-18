#include "../Primitives.hpp"
#include "Graphics/Mesh/Mesh.hpp"

namespace Ivy::G
{
Mesh Primitives::CreateArrow(const std::string& name)
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
} // namespace Ivy::G
