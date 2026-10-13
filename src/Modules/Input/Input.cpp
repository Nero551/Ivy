#include "Input.hpp"
#include "../../Graphics/InputEnums/Action.hpp"
#include "../../Graphics/InputEnums/CursorMode.hpp"
#include "Core/Engine.hpp"
#include "Graphics/Window.hpp"
#include "Math/Vector/Vector2.hpp"

namespace Ivy::I
{
bool Input::IsKeyHeld(const G::Key key)
{
    return m_CurrentKeys[ToIndex(key)];
}

bool Input::IsKeyPressed(const G::Key key)
{
    return m_CurrentKeys[ToIndex(key)] && !m_PreviousKeys[ToIndex(key)];
}

bool Input::IsKeyReleased(const G::Key key)
{
    return !m_CurrentKeys[ToIndex(key)] && m_PreviousKeys[ToIndex(key)];
}

bool Input::IsMouseButtonHeld(const G::MouseButton button)
{
    return m_CurrentMouseButtons[ToIndex(button)];
}

bool Input::IsMouseButtonPressed(const G::MouseButton button)
{
    return m_CurrentMouseButtons[ToIndex(button)] && !m_PreviousMouseButtons[ToIndex(button)];
}

bool Input::IsMouseButtonReleased(const G::MouseButton button)
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

G::CursorMode Input::GetMouseMode() const
{
    return m_MouseMode;
}

void Input::SetMouseMode(G::CursorMode mode)
{
    m_MouseMode = mode;
    m_Engine.Window.SetCursorMode(mode);
    m_PreviousMousePosition = m_MousePosition;
}

constexpr unsigned int Input::ToIndex(G::Key key)
{
    return static_cast<unsigned int>(key);
}

constexpr unsigned int Input::ToIndex(G::MouseButton button)
{
    return static_cast<unsigned int>(button);
}

void Input::Start()
{
    G::Window& window = m_Engine.Window;
    window.SetCursorPosCallback([&](G::Window& win, const double xPos, const double yPos)
        { m_MousePosition = {static_cast<float>(xPos), static_cast<float>(yPos)}; });
    ;

    window.SetScrollCallback([&](G::Window& win, const double xOffset, const double yOffset)
        { m_ScrollOffset = {static_cast<float>(xOffset), static_cast<float>(yOffset)}; });

    window.SetKeyCallback(
        [&](G::Window& win, G::Key key, int scancode, G::Action action, int mod)
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
        [&](G::Window& win, G::MouseButton button, G::Action action, int mods)
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

void Input::BeginFrame(double dt)
{
    if (m_FirstMouse)
    {
        m_PreviousMousePosition = GetMousePosition();
        m_FirstMouse = false;
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
