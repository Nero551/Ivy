#pragma once

#include "../../Graphics/InputEnums/CursorMode.hpp"
#include "../../Graphics/InputEnums/Key.hpp"
#include "../../Graphics/InputEnums/MouseButton.hpp"
#include "Core/Module.hpp"
#include "Math/Vector/Vector2.hpp"
#include <array>

namespace Ivy::I
{
struct Input : C::Module
{
    using C::Module::Module;
    bool IsKeyHeld(G::Key key);

    bool IsKeyPressed(G::Key key);

    bool IsKeyReleased(G::Key key);

    bool IsMouseButtonHeld(G::MouseButton button);

    bool IsMouseButtonPressed(G::MouseButton button);

    bool IsMouseButtonReleased(G::MouseButton button);

    M::Vector<2> GetMousePosition() const;

    M::Vector<2> GetMouseDelta() const;

    M::Vector<2> GetScrollDelta() const;

    bool IsScrolling() const;

    G::CursorMode GetMouseMode() const;

    void SetMouseMode(G::CursorMode mode);

  private:
    G::CursorMode m_MouseMode = G::CursorMode::Normal;
    M::Vector<2> m_MousePosition{0};
    M::Vector<2> m_PreviousMousePosition{0};
    bool m_FirstMouse = true;

    M::Vector<2> m_ScrollOffset{0};

    constexpr unsigned int ToIndex(G::Key key);

    constexpr unsigned int ToIndex(G::MouseButton button);

    std::array<bool, G::KeyCount> m_CurrentKeys{};
    std::array<bool, G::KeyCount> m_PreviousKeys{};

    std::array<bool, G::MouseButtonCount> m_CurrentMouseButtons{};
    std::array<bool, G::MouseButtonCount> m_PreviousMouseButtons{};

  protected:
    void Start() override;

    void BeginFrame(double dt) override;

    void EndFrame(double dt) override;
};
} // namespace Ivy::I
