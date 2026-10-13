#pragma once
#include "../OpenGL.hpp"

namespace Ivy::G
{
/** @brief Specifies how the mouse cursor is handled by the window. */
enum class CursorMode
{
    /** @brief The cursor is visible and unrestricted. */
    Normal = GLFW_CURSOR_NORMAL,

    /** @brief The cursor is hidden but remains unrestricted. */
    Hidden = GLFW_CURSOR_HIDDEN,

    /** @brief The cursor is hidden and locked to the window. */
    Disabled = GLFW_CURSOR_DISABLED,

    /** @brief The cursor is visible and confined to the window. */
    Captured = GLFW_CURSOR_CAPTURED
};
} // namespace Ivy::G
