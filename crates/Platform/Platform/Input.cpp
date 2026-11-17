#include "Input.hpp"
#include "Action.hpp"
#include "CursorMode.hpp"
#include "Key.hpp"
#include "Math/Vector/Vectors.hpp"
#include "MouseButton.hpp"
#include "Window.hpp"

namespace Ivy::Platform
{
bool InputHandler::IsKeyHeld(const Key key) const
{
    return m_CurrentKeys[ToIndex(key)];
}

bool InputHandler::IsKeyPressed(const Key key) const
{
    return m_CurrentKeys[ToIndex(key)] && !m_PreviousKeys[ToIndex(key)];
}

bool InputHandler::IsKeyReleased(const Key key) const
{
    return !m_CurrentKeys[ToIndex(key)] && m_PreviousKeys[ToIndex(key)];
}

bool InputHandler::IsMouseButtonHeld(const MouseButton button) const
{
    return m_CurrentMouseButtons[ToIndex(button)];
}

bool InputHandler::IsMouseButtonPressed(const MouseButton button) const
{
    return m_CurrentMouseButtons[ToIndex(button)] && !m_PreviousMouseButtons[ToIndex(button)];
}

bool InputHandler::IsMouseButtonReleased(const MouseButton button) const
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

CursorMode InputHandler::GetMouseMode() const
{
    return m_MouseMode;
}

void InputHandler::SetMouseMode(CursorMode mode)
{
    m_MouseMode = mode;
    m_Window->SetCursorMode(mode);
    m_PreviousMousePosition = m_MousePosition;
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
} // namespace Ivy::Platform
