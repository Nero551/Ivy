#pragma once
#include "../OpenGL.hpp"

namespace Ivy::G
{

static constexpr unsigned int MouseButtonCount = GLFW_MOUSE_BUTTON_LAST + 1;

/** @brief Mouse buttons for input */
enum class MouseButton
{
    Left = GLFW_MOUSE_BUTTON_LEFT,
    Right = GLFW_MOUSE_BUTTON_RIGHT,
    Middle = GLFW_MOUSE_BUTTON_MIDDLE,
    Button4 = GLFW_MOUSE_BUTTON_4,
    Button5 = GLFW_MOUSE_BUTTON_5,
    Button6 = GLFW_MOUSE_BUTTON_6,
    Button7 = GLFW_MOUSE_BUTTON_7,
    Button8 = GLFW_MOUSE_BUTTON_8,
};
} // namespace Ivy::G
