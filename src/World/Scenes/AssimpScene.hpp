#pragma once

#include "Core/World/Scene.hpp"

namespace N
{
struct AssimpScene : C::Scene
{
    AssimpScene(const std::string& filepath);
};
} // namespace N
