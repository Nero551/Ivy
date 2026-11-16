#pragma once

#include "Graphics/InputEnums/CursorMode.hpp"
#include "Graphics/InputEnums/Key.hpp"
#include "Graphics/InputEnums/MouseButton.hpp"
#include "Graphics/Window.hpp"
#include "Math/Vector/Vectors.hpp"
#include <array>

namespace Ivy::G
{
struct InputHandler
{
    void Initialize(G::Window& window);
    void FirstMouseCheck();
    void Refresh();

    bool IsKeyHeld(G::Key key) const;

    bool IsKeyPressed(G::Key key) const;

    bool IsKeyReleased(G::Key key) const;

    bool IsMouseButtonHeld(G::MouseButton button) const;

    bool IsMouseButtonPressed(G::MouseButton button) const;

    bool IsMouseButtonReleased(G::MouseButton button) const;

    M::Vector<2> GetMousePosition() const;

    M::Vector<2> GetMouseDelta() const;
    M::Vector<2> GetScrollDelta() const;

    bool IsScrolling() const;

    G::CursorMode GetMouseMode() const;

    void SetMouseMode(G::CursorMode mode);

    constexpr unsigned int ToIndex(G::Key key) const;

    constexpr unsigned int ToIndex(G::MouseButton button) const;

  private:
    G::Window* m_Window;
    G::CursorMode m_MouseMode = G::CursorMode::Normal;
    M::Vector<2> m_MousePosition{0};
    M::Vector<2> m_PreviousMousePosition{0};
    bool m_FirstMouse = true;

    M::Vector<2> m_ScrollOffset{0};

    std::array<bool, G::KeyCount> m_CurrentKeys{};
    std::array<bool, G::KeyCount> m_PreviousKeys{};

    std::array<bool, G::MouseButtonCount> m_CurrentMouseButtons{};
    std::array<bool, G::MouseButtonCount> m_PreviousMouseButtons{};
};

} // namespace Ivy::G
