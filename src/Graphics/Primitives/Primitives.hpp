#pragma once
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Texture/Texture2D.hpp"

namespace Ivy::G::Primitives
{
Mesh CreateCube(const std::string& name);
Mesh CreateUVSphere(const std::string& name, float radius = 0.5, int sectors = 32, int stacks = 16);
Mesh CreateQuad(const std::string& name);
Mesh CreateLine(const std::string& name);
Mesh CreateCircle(const std::string& name, float radius = 0.5, int sectors = 32);
Mesh CreateArrow(const std::string& name);

Texture2D CreateWhiteTexture();
Texture2D CreateBlackTexture();
} // namespace Ivy::G::Primitives
