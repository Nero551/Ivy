#pragma once

#include <GLFW/glfw3.h>
namespace Ivy::Platform
{
enum class OpenGLProfile
{
    Core = GLFW_OPENGL_CORE_PROFILE,
    Compatibility = GLFW_OPENGL_COMPAT_PROFILE
};
} // namespace Ivy::Platform
