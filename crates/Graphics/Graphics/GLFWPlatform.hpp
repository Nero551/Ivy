#pragma once
#include "OpenGL.hpp"
namespace Ivy::G
{

enum class GLFWPlatform
{
    Wayland = GLFW_PLATFORM_WAYLAND,
    X11 = GLFW_PLATFORM_X11,
    Win32 = GLFW_PLATFORM_WIN32,
    Cocoa = GLFW_PLATFORM_COCOA,
    Null = GLFW_PLATFORM_NULL
};

} // namespace Ivy::G