#include "Input.hpp"
#include "Core/Engine.hpp"
#include "Graphics/InputEnums/Action.hpp"
#include "Graphics/InputEnums/CursorMode.hpp"
#include "Graphics/Window.hpp"
#include "Math/Vector/Vectors.hpp"

namespace Ivy::I
{
bool Input::IsKeyHeld(const G::Key key)
{
    return m_Engine.GetStates<InputState>().CurrentKeys[ToIndex(key)];
}

bool Input::IsKeyPressed(const G::Key key)
{
    const auto& s = m_Engine.GetStates<InputState>();
    return s.CurrentKeys[ToIndex(key)] && !s.PreviousKeys[ToIndex(key)];
}

bool Input::IsKeyReleased(const G::Key key)
{
    const auto& s = m_Engine.GetStates<InputState>();
    return !s.CurrentKeys[ToIndex(key)] && s.PreviousKeys[ToIndex(key)];
}

bool Input::IsMouseButtonHeld(const G::MouseButton button)
{
    return m_Engine.GetStates<InputState>().CurrentMouseButtons[ToIndex(button)];
}

bool Input::IsMouseButtonPressed(const G::MouseButton button)
{
    const auto& s = m_Engine.GetStates<InputState>();
    return s.CurrentMouseButtons[ToIndex(button)] && !s.PreviousMouseButtons[ToIndex(button)];
}

bool Input::IsMouseButtonReleased(const G::MouseButton button)
{
    const auto& s = m_Engine.GetStates<InputState>();
    return !s.CurrentMouseButtons[ToIndex(button)] && s.PreviousMouseButtons[ToIndex(button)];
}

M::Vector<2> Input::GetMousePosition() const
{
    return m_Engine.GetStates<InputState>().MousePosition;
}

M::Vector<2> Input::GetMouseDelta() const
{
    const auto& s = m_Engine.GetStates<InputState>();
    return s.MousePosition - s.PreviousMousePosition;
}

M::Vector<2> Input::GetScrollDelta() const
{
    return m_Engine.GetStates<InputState>().ScrollOffset;
}

bool Input::IsScrolling() const
{
    return m_Engine.GetStates<InputState>().ScrollOffset != M::Vector<2>::Zero();
}

G::CursorMode Input::GetMouseMode() const
{
    return m_Engine.GetStates<InputState>().MouseMode;
}

void Input::SetMouseMode(G::CursorMode mode)
{
    auto& s = m_Engine.GetStates<InputState>();
    s.MouseMode = mode;
    m_Engine.Window.SetCursorMode(mode);
    s.PreviousMousePosition = s.MousePosition;
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

    window.SetCursorPosCallback(
        [&](G::Window&, const double xPos, const double yPos)
        {
            auto& s = m_Engine.GetStates<InputState>();
            s.MousePosition = {static_cast<float>(xPos), static_cast<float>(yPos)};
        });

    window.SetScrollCallback(
        [&](G::Window&, const double xOffset, const double yOffset)
        {
            m_Engine.GetStates<InputState>().ScrollOffset = {
                static_cast<float>(xOffset), static_cast<float>(yOffset)};
        });

    window.SetKeyCallback(
        [&](G::Window&, G::Key key, int scancode, G::Action action, int mod)
        {
            auto& s = m_Engine.GetStates<InputState>();
            if (action == G::Action::Press || action == G::Action::Repeat)
            {
                s.CurrentKeys[ToIndex(key)] = true;
            }
            else if (action == G::Action::Release)
            {
                s.CurrentKeys[ToIndex(key)] = false;
            }
        });

    window.SetMouseButtonCallback(
        [&](G::Window&, G::MouseButton button, G::Action action, int mods)
        {
            auto& s = m_Engine.GetStates<InputState>();
            if (action == G::Action::Press)
            {
                s.CurrentMouseButtons[ToIndex(button)] = true;
            }
            else if (action == G::Action::Release)
            {
                s.CurrentMouseButtons[ToIndex(button)] = false;
            }
        });
}

void Input::BeginFrame(double dt)
{
    auto& s = m_Engine.GetStates<InputState>();
    if (s.FirstMouse)
    {
        s.PreviousMousePosition = s.MousePosition;
        s.FirstMouse = false;
    }
}

void Input::EndFrame(double dt)
{
    auto& s = m_Engine.GetStates<InputState>();
    s.ScrollOffset = {0, 0};
    s.PreviousMousePosition = s.MousePosition;
    s.PreviousKeys = s.CurrentKeys;
    s.PreviousMouseButtons = s.CurrentMouseButtons;
}
} // namespace Ivy::I
