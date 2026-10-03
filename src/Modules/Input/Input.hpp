#pragma once

#include "Core/Module.hpp"
#include "Enums/Keys.hpp"
#include "Enums/MouseButton.hpp"
#include "Enums/MouseMode.hpp"
#include "Math/Vector/Vector2.hpp"

namespace N::I
{
struct Input : C::Module
{
    bool IsKeyHeld(Key key);

    bool IsKeyPressed(Key key);

    bool IsKeyReleased(Key key);

    bool IsMouseButtonHeld(MouseButton button);

    bool IsMouseButtonPressed(MouseButton button);

    bool IsMouseButtonReleased(MouseButton button);

    M::Vector<2> GetMousePosition() const;

    M::Vector<2> GetMouseDelta() const;

    M::Vector<2> GetScrollDelta() const;

    bool IsScrolling() const;

    MouseMode GetMouseMode() const;

    void SetMouseMode(MouseMode mode);

  private:
    MouseMode m_MouseMode = MouseMode::Normal;
    M::Vector<2> m_MousePosition = M::Vector<2>::Zero();
    M::Vector<2> m_PreviousMousePosition = M::Vector<2>::Zero();
    bool m_FirstMouse = true;

    M::Vector<2> m_ScrollOffset = M::Vector<2>::Zero();

    static constexpr unsigned int MouseButtonCount = GLFW_MOUSE_BUTTON_LAST + 1;
    static constexpr unsigned int KeyCount = GLFW_KEY_LAST + 1;

    constexpr unsigned int ToIndex(Key key);

    constexpr unsigned int ToIndex(MouseButton button);

    std::array<bool, KeyCount> m_CurrentKeys{};
    std::array<bool, KeyCount> m_PreviousKeys{};

    std::array<bool, MouseButtonCount> m_CurrentMouseButtons{};
    std::array<bool, MouseButtonCount> m_PreviousMouseButtons{};

  protected:
    void Start() override;

    void BeginFrame(double dt) override;

    void EndFrame(double dt) override;
};
} // namespace N::I
