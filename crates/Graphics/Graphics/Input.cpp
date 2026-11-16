#include "Input.hpp"
#include "Graphics/InputEnums/Action.hpp"
#include "Graphics/InputEnums/CursorMode.hpp"
#include "Graphics/InputEnums/Key.hpp"
#include "Graphics/InputEnums/MouseButton.hpp"
#include "Graphics/Window.hpp"
#include "Math/Vector/Vectors.hpp"

namespace Ivy::G
{
bool InputHandler::IsKeyHeld(const G::Key key) const
{
    return m_CurrentKeys[ToIndex(key)];
}

bool InputHandler::IsKeyPressed(const G::Key key) const
{
    return m_CurrentKeys[ToIndex(key)] && !m_PreviousKeys[ToIndex(key)];
}

bool InputHandler::IsKeyReleased(const G::Key key) const
{
    return !m_CurrentKeys[ToIndex(key)] && m_PreviousKeys[ToIndex(key)];
}

bool InputHandler::IsMouseButtonHeld(const G::MouseButton button) const
{
    return m_CurrentMouseButtons[ToIndex(button)];
}

bool InputHandler::IsMouseButtonPressed(const G::MouseButton button) const
{
    return m_CurrentMouseButtons[ToIndex(button)] && !m_PreviousMouseButtons[ToIndex(button)];
}

bool InputHandler::IsMouseButtonReleased(const G::MouseButton button) const
{
    return !m_CurrentMouseButtons[ToIndex(button)] && m_PreviousMouseButtons[ToIndex(button)];
}

M::Vector<2> InputHandler::GetMousePosition() const
{
    return m_MousePosition;
}

M::Vector<2> InputHandler::GetMouseDelta() const
{
    return m_MousePosition - m_PreviousMousePosition;
}

M::Vector<2> InputHandler::GetScrollDelta() const
{
    return m_ScrollOffset;
}

bool InputHandler::IsScrolling() const
{
    return m_ScrollOffset != M::Vector<2>::Zero();
}

G::CursorMode InputHandler::GetMouseMode() const
{
    return m_MouseMode;
}

void InputHandler::SetMouseMode(G::CursorMode mode)
{
    m_MouseMode = mode;
    m_Window->SetCursorMode(mode);
    m_PreviousMousePosition = m_MousePosition;
}

constexpr unsigned int InputHandler::ToIndex(G::Key key) const
{
    return static_cast<unsigned int>(key);
}

constexpr unsigned int InputHandler::ToIndex(G::MouseButton button) const
{
    return static_cast<unsigned int>(button);
}

void InputHandler::Initialize(G::Window& window)
{
    m_Window = &window;

    window.SetCursorPosCallback([&](G::Window&, const double xPos, const double yPos)
        { m_MousePosition = {static_cast<float>(xPos), static_cast<float>(yPos)}; });

    window.SetScrollCallback([&](G::Window&, const double xOffset, const double yOffset)
        { m_ScrollOffset = {static_cast<float>(xOffset), static_cast<float>(yOffset)}; });

    window.SetKeyCallback(
        [&](G::Window&, G::Key key, int scancode, G::Action action, int mod)
        {
            if (action == G::Action::Press || action == G::Action::Repeat)
            {
                m_CurrentKeys[ToIndex(key)] = true;
            }
            else if (action == G::Action::Release)
            {
                m_CurrentKeys[ToIndex(key)] = false;
            }
        });

    window.SetMouseButtonCallback(
        [&](G::Window&, G::MouseButton button, G::Action action, int mods)
        {
            if (action == G::Action::Press)
            {
                m_CurrentMouseButtons[ToIndex(button)] = true;
            }
            else if (action == G::Action::Release)
            {
                m_CurrentMouseButtons[ToIndex(button)] = false;
            }
        });
}

void InputHandler::FirstMouseCheck()
{
    if (m_FirstMouse)
    {
        m_PreviousMousePosition = m_MousePosition;
        m_FirstMouse = false;
    }
}

void InputHandler::Refresh()
{
    m_ScrollOffset = {0, 0};
    m_PreviousMousePosition = m_MousePosition;
    m_PreviousKeys = m_CurrentKeys;
    m_PreviousMouseButtons = m_CurrentMouseButtons;
}
} // namespace Ivy::G
