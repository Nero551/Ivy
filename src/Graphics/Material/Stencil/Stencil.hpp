#pragma once
#include "StencilAction.hpp"
#include "StencilFunction.hpp"

namespace N::G
{
/**
 * @brief Controls stencil testing and stencil buffer operations.
 */
struct Stencil
{
    /** @brief Whether stencil testing is enabled. */
    bool Enabled;

    /** @brief Reference value used by the stencil comparison function. */
    int Ref = 1;

    /** @brief Bit mask applied to stencil values when comparing against Ref. */
    unsigned int FunctionMask = 0xFF;

    /** @brief Comparison function used by the stencil test. */
    StencilFunction Function = StencilFunction::Always;

    /** @brief Bit mask controlling which stencil bits can be written. */
    unsigned int WriteMask = 0xFF;

    /** @brief Action to take if the stencil test fails. */
    StencilAction SFail = StencilAction::Keep;

    /** @brief Action to take if the stencil test passes but the depth test fails. */
    StencilAction DFail = StencilAction::Keep;

    /** @brief Action to take if both stencil and depth tests pass. */
    StencilAction SDPass = StencilAction::Keep;

    Stencil(const bool enabled) : Enabled(enabled) {}

    /** @brief Applies the configured stencil state to the OpenGL context. */
    void Apply();
};
} // namespace N::G
