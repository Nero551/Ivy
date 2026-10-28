#pragma once
#include "../Utilities/Debug/Log.hpp"
#include "GLFW/glfw3.h"
#include "Graphics/Buffers/BufferBit.hpp"
#include "Graphics/Capability.hpp"
#include "OpenGL.hpp"
// #include <glslang/Public/ShaderLang.h>

/** @brief Handles initialization, modification and termination of the graphics state */
namespace Ivy::G::Context
{

enum class GPUBackend
{
    OpenGL,
    Vulkan
};

inline double GetTime()
{
    return glfwGetTime();
}

inline void SwapInterval(int interval)
{
    glfwSwapInterval(interval);
}

inline void ClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

inline void Clear(BufferBit b)
{
    glClear(static_cast<GLbitfield>(b));
}
inline void Enable(Capability capability)
{
    glEnable(static_cast<GLenum>(capability));
}

inline void Disable(Capability capability)
{
    glDisable(static_cast<GLenum>(capability));
}

inline void SetViewport(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
    glViewport(x1, y1, x2, y2);
}

inline bool IsEnabled(Capability capability)
{
    return glIsEnabled(static_cast<GLenum>(capability));
}

inline void Initialize()
{
    glfwSetErrorCallback(
        [](const int error, const char* description) { U::Log::Error("[GLFW] ", error, ": ", description); });

    if (!glfwInit())
    {
        U::Log::Fatal("Failed to initialize GLFW");
    }
}

inline void Terminate()
{
    glfwTerminate();
}
} // namespace Ivy::G::Context
