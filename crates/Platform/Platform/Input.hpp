#pragma once

#include "Action.hpp"
#include "CursorMode.hpp"
#include "Key.hpp"
#include "Math/Vector/Vectors.hpp"
#include "MouseButton.hpp"
#include "Window.hpp"
#include <array>

namespace Ivy::Platform
{
struct InputHandler
{
    void FirstMouseCheck()
    {
        if (m_FirstMouse)
        {
            m_PreviousMousePosition = m_MousePosition;
            m_FirstMouse = false;
        }
    }

    void Refresh()
    {
        m_ScrollOffset = {0, 0};
        m_PreviousMousePosition = m_MousePosition;
        m_PreviousKeys = m_CurrentKeys;
        m_PreviousMouseButtons = m_CurrentMouseButtons;
    }

    void SetActiveWindow(Window& window)
    {
        m_Window = &window;

        window.SetCursorPosCallback([&](Window&, const double xPos, const double yPos)
            { m_MousePosition = {static_cast<float>(xPos), static_cast<float>(yPos)}; });

        window.SetScrollCallback([&](Window&, const double xOffset, const double yOffset)
            { m_ScrollOffset = {static_cast<float>(xOffset), static_cast<float>(yOffset)}; });

        window.SetKeyCallback(
            [&](Window&, Key key, int scancode, Action action, int mod)
            {
                if (action == Action::Press || action == Action::Repeat)
                {
                    m_CurrentKeys[ToIndex(key)] = true;
                }
                else if (action == Action::Release)
                {
                    m_CurrentKeys[ToIndex(key)] = false;
                }
            });

        window.SetMouseButtonCallback(
            [&](Window&, MouseButton button, Action action, int mods)
            {
                if (action == Action::Press)
                {
                    m_CurrentMouseButtons[ToIndex(button)] = true;
                }
                else if (action == Action::Release)
                {
                    m_CurrentMouseButtons[ToIndex(button)] = false;
                }
            });

        m_Window->SetCursorMode(m_MouseMode);
    }

    bool HasActiveWindow() const
    {
        return m_Window != nullptr;
    }

    bool IsKeyHeld(Key key) const
    {
        return m_CurrentKeys[ToIndex(key)];
    }

    bool IsKeyPressed(Key key) const
    {
        return m_CurrentKeys[ToIndex(key)] && !m_PreviousKeys[ToIndex(key)];
    }

    bool IsKeyReleased(Key key) const
    {
        return !m_CurrentKeys[ToIndex(key)] && m_PreviousKeys[ToIndex(key)];
    }

    bool IsMouseButtonHeld(MouseButton button) const
    {
        return m_CurrentMouseButtons[ToIndex(button)];
    }

    bool IsMouseButtonPressed(MouseButton button) const
    {
        return m_CurrentMouseButtons[ToIndex(button)] && !m_PreviousMouseButtons[ToIndex(button)];
    }

    bool IsMouseButtonReleased(MouseButton button) const
    {
        return !m_CurrentMouseButtons[ToIndex(button)] && m_PreviousMouseButtons[ToIndex(button)];
    }

    M::Vector<2> GetMousePosition() const
    {
        return m_MousePosition;
    }

    M::Vector<2> GetMouseDelta() const
    {
        return m_MousePosition - m_PreviousMousePosition;
    }

    M::Vector<2> GetScrollDelta() const
    {
        return m_ScrollOffset;
    }

    bool IsScrolling() const
    {
        return m_ScrollOffset != M::Vector<2>::Zero();
    }

    CursorMode GetMouseMode() const
    {
        return m_MouseMode;
    }

    void SetMouseMode(CursorMode mode)
    {
        m_MouseMode = mode;
        if (HasActiveWindow())
        {
            m_Window->SetCursorMode(mode);
        }
        m_PreviousMousePosition = m_MousePosition;
    }

    constexpr unsigned int ToIndex(Key key) const
    {
        return static_cast<unsigned int>(key);
    }

    constexpr unsigned int ToIndex(MouseButton button) const
    {
        return static_cast<unsigned int>(button);
    }

  private:
    Window* m_Window;
    CursorMode m_MouseMode = CursorMode::Normal;
    M::Vector<2> m_MousePosition{0};
    M::Vector<2> m_PreviousMousePosition{0};
    bool m_FirstMouse = true;

    M::Vector<2> m_ScrollOffset{0};

    std::array<bool, KeyCount> m_CurrentKeys{};
    std::array<bool, KeyCount> m_PreviousKeys{};

    std::array<bool, MouseButtonCount> m_CurrentMouseButtons{};
    std::array<bool, MouseButtonCount> m_PreviousMouseButtons{};
};

} // namespace Ivy::Platform
