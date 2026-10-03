#include "Input.hpp"

#include <OpenGL.hpp>

#include "Core/Engine.hpp"

namespace Ivy::I
{
bool Input::IsKeyHeld(const Key key)
{
    return m_CurrentKeys[ToIndex(key)];
}

bool Input::IsKeyPressed(const Key key)
{
    return m_CurrentKeys[ToIndex(key)] && !m_PreviousKeys[ToIndex(key)];
}

bool Input::IsKeyReleased(const Key key)
{
    return !m_CurrentKeys[ToIndex(key)] && m_PreviousKeys[ToIndex(key)];
}

bool Input::IsMouseButtonHeld(const MouseButton button)
{
    return m_CurrentMouseButtons[ToIndex(button)];
}

bool Input::IsMouseButtonPressed(const MouseButton button)
{
    return m_CurrentMouseButtons[ToIndex(button)] && !m_PreviousMouseButtons[ToIndex(button)];
}

bool Input::IsMouseButtonReleased(const MouseButton button)
{
    return !m_CurrentMouseButtons[ToIndex(button)] && m_PreviousMouseButtons[ToIndex(button)];
}

M::Vector<2> Input::GetMousePosition() const
{
    return m_MousePosition;
}

M::Vector<2> Input::GetMouseDelta() const
{
    return m_MousePosition - m_PreviousMousePosition;
}

M::Vector<2> Input::GetScrollDelta() const
{
    return m_ScrollOffset;
}

bool Input::IsScrolling() const
{
    return m_ScrollOffset != M::Vector<2>::Zero();
}

MouseMode Input::GetMouseMode() const
{
    return m_MouseMode;
}

void Input::SetMouseMode(MouseMode mode)
{
    m_MouseMode = mode;
    glfwSetInputMode(C::Engine::Get().Window.GetGlfwWindow(), GLFW_CURSOR, static_cast<int>(mode));
    m_PreviousMousePosition = m_MousePosition;
}

constexpr unsigned int Input::ToIndex(Key key)
{
    return static_cast<unsigned int>(key);
}

constexpr unsigned int Input::ToIndex(MouseButton button)
{
    return static_cast<unsigned int>(button);
}

void Input::Start()
{
    G::Window& window = C::Engine::Get().Window;
    glfwSetCursorPosCallback(window.GetGlfwWindow(),
        [](GLFWwindow*, const double xPos, const double yPos)
        {
            C::Engine::Get().GetModule<Input>().m_MousePosition = {
                static_cast<float>(xPos), static_cast<float>(yPos)};
        });

    glfwSetScrollCallback(window.GetGlfwWindow(),
        [](GLFWwindow*, const double xOffset, const double yOffset)
        {
            C::Engine::Get().GetModule<Input>().m_ScrollOffset = {
                static_cast<float>(xOffset), static_cast<float>(yOffset)};
        });
}

void Input::BeginFrame(double dt)
{
    if (m_FirstMouse)
    {
        m_PreviousMousePosition = GetMousePosition();
        m_FirstMouse = false;
    }

    GLFWwindow* window = C::Engine::Get().Window.GetGlfwWindow();
    for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key)
    {
        m_CurrentKeys[key] = glfwGetKey(window, key) == GLFW_PRESS;
    }

    for (int button = GLFW_MOUSE_BUTTON_1; button <= GLFW_MOUSE_BUTTON_LAST; ++button)
    {
        m_CurrentMouseButtons[button] = glfwGetMouseButton(window, button);
    }
}

void Input::EndFrame(double dt)
{
    m_ScrollOffset = {0, 0};
    m_PreviousMousePosition = m_MousePosition;
    m_PreviousKeys = m_CurrentKeys;
    m_PreviousMouseButtons = m_CurrentMouseButtons;
}
} // namespace Ivy::I
