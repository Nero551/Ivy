#pragma once
#include "OpenGL.hpp"

namespace N::G
{
/**
 * @brief Defines which faces of a mesh are culled during rendering.
 * Face culling prevents selected faces from being rasterized based on their
 * orientation relative to the camera.
 */
enum class CullMode
{
    /** @brief Disables face culling. */
    None,

    /** @brief Culls front-facing faces. */
    Front = GL_FRONT,

    /** @brief Culls back-facing faces. */
    Back = GL_BACK,

    /** @brief Culls both front-facing and back-facing faces. */
    FrontAndBack = GL_FRONT_AND_BACK
};
} // namespace N::G
