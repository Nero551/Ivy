#pragma once
#include "Graphics/Buffers/BufferBit.hpp"
#include "Graphics/Capability.hpp"
#include "OpenGL.hpp"

/** @brief Handles initialization, modification and termination of the graphics state */
namespace Ivy::G::Context
{
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
} // namespace Ivy::G::Context
