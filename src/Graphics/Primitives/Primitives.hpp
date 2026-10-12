#pragma once
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Texture/Texture.hpp"

namespace Ivy::G
{
struct Texture2D;
}
namespace Ivy::G::Primitives
{
G::Mesh CreateCube(const std::string& name);

G::Mesh CreateUVSphere(const std::string& name, float radius = 0.5, int sectors = 32, int stacks = 16);

G::Mesh CreateQuad(const std::string& name);

G::Mesh CreateLine(const std::string& name);

G::Texture2D CreateWhiteTexture();

G::Texture2D CreateBlackTexture();
} // namespace Ivy::G::Primitives
