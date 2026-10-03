#pragma once
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Texture/Texture.hpp"

namespace Ivy::G
{
struct Texture2D;
}
namespace Ivy::R::Primitives
{
C::Resource<G::Mesh>& CreateCube(const std::string& name);

C::Resource<G::Mesh>& CreateUVSphere(
    const std::string& name, float radius = 0.5, int sectors = 32, int stacks = 16);

C::Resource<G::Mesh>& CreateQuad(const std::string& name);

C::Resource<G::Mesh>& CreateLine(const std::string& name);

C::Resource<G::Texture2D>& CreateWhiteTexture();

C::Resource<G::Texture2D>& CreateBlackTexture();
} // namespace Ivy::R::Primitives
