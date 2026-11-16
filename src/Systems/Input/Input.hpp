#pragma once

#include "Core/State.hpp"
#include "Core/System.hpp"
#include "Graphics/InputEnums/CursorMode.hpp"
#include "Graphics/InputEnums/Key.hpp"
#include "Graphics/InputEnums/MouseButton.hpp"
#include "Math/Vector/Vectors.hpp"
#include <array>

namespace Ivy::I
{
struct InputState : C::State
{
    G::CursorMode MouseMode = G::CursorMode::Normal;
    M::Vector<2> MousePosition{0};
    M::Vector<2> PreviousMousePosition{0};
    bool FirstMouse = true;

    M::Vector<2> ScrollOffset{0};

    std::array<bool, G::KeyCount> CurrentKeys{};
    std::array<bool, G::KeyCount> PreviousKeys{};

    std::array<bool, G::MouseButtonCount> CurrentMouseButtons{};
    std::array<bool, G::MouseButtonCount> PreviousMouseButtons{};
};

struct Input : C::System
{
    using C::System::System;
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
    constexpr unsigned int ToIndex(G::Key key);

    constexpr unsigned int ToIndex(G::MouseButton button);

  protected:
    void Start() override;

    void BeginFrame(double dt) override;

    void EndFrame(double dt) override;
};
} // namespace Ivy::I
