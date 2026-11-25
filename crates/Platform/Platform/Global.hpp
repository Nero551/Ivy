#pragma once
#include "Diagnostics/Log.hpp"
#include "GLFW/glfw3.h"
#include "Input.hpp"

namespace Ivy::Platform
{

inline InputHandler Input{};

inline void PollEvents()
{
    glfwPollEvents();
}

inline double GetTime()
{
    return glfwGetTime();
}

inline void SwapInterval(int interval)
{
    glfwSwapInterval(interval);
}

inline void Initialize()
{
    glfwSetErrorCallback(
        [](const int error, const char* description) { D::Log::Error("[GLFW] ", error, ": ", description); });

    if (!glfwInit())
    {
        D::Log::Error("Failed to initialize GLFW");
    }
}

inline void Terminate()
{
    glfwTerminate();
}
} // namespace Ivy::Platform
