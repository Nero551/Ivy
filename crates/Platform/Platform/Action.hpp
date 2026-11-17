#pragma once
#include "GLFW/glfw3.h"

namespace Ivy::Platform
{
enum class Action
{
    Press = GLFW_PRESS,
    Repeat = GLFW_REPEAT,
    Release = GLFW_RELEASE,
};

} // namespace Ivy::Platform
