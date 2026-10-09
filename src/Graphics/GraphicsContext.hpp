#pragma once
#include "../Utilities/Debug/Log.hpp"
#include "OpenGL.hpp"
#include <glslang/Public/ShaderLang.h>

/** @brief Handles initialization and termination of graphics specific things
 * that need to run before window creation and everything else.
 */
namespace Ivy::G::GraphicsContext
{
inline void Initialize()
{
    glfwSetErrorCallback(
        [](const int error, const char* description) { U::Log::Error("[GLFW] ", error, ": ", description); });

    if (!glfwInit())
    {
        U::Log::Fatal("Failed to initialize GLFW");
    }

    if (!glslang::InitializeProcess())
    {
        U::Log::Fatal("Failed to initialize glslang");
    }
}

inline void Terminate()
{
    glslang::FinalizeProcess();
    glfwTerminate();
}
} // namespace Ivy::G::GraphicsContext
