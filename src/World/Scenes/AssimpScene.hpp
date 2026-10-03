#pragma once

#include "Core/World/Scene.hpp"

namespace Ivy
{
struct AssimpScene : C::Scene
{
    AssimpScene(const std::string& filepath);
};
} // namespace Ivy
