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
    void FirstMouseCheck();
    void Refresh();

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
    }

    bool IsKeyHeld(Key key) const;

    bool IsKeyPressed(Key key) const;

    bool IsKeyReleased(Key key) const;

    bool IsMouseButtonHeld(MouseButton button) const;

    bool IsMouseButtonPressed(MouseButton button) const;

    bool IsMouseButtonReleased(MouseButton button) const;

    M::Vector<2> GetMousePosition() const;

    M::Vector<2> GetMouseDelta() const;
    M::Vector<2> GetScrollDelta() const;

    bool IsScrolling() const;

    CursorMode GetMouseMode() const;

    void SetMouseMode(CursorMode mode);

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
