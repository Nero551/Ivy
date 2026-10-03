#pragma once

#include "BlendEquation.hpp"
#include "BlendFactor.hpp"
#include "Math/Vector/Vector4.hpp"

namespace N::G
{
/**
 * @brief Controls color blending for rendered fragments.
 *
 * Blending combines the fragment's output color with the color already
 * stored in the color buffer using configurable source and destination
 * factors and blend equations.
 */
struct Blend
{
    /** @brief Whether blending is enabled. */
    bool Enabled;

    /** @brief Factor applied to the source RGB color. */
    BlendFactor SourceRGB = BlendFactor::SourceAlpha;

    /** @brief Factor applied to the destination RGB color. */
    BlendFactor DestinationRGB = BlendFactor::OneMinusSourceAlpha;

    /** @brief Factor applied to the source alpha value. */
    BlendFactor SourceAlpha = BlendFactor::One;

    /** @brief Factor applied to the destination alpha value. */
    BlendFactor DestinationAlpha = BlendFactor::OneMinusSourceAlpha;

    /** @brief Equation used to combine the source and destination RGB colors. */
    BlendEquation EquationRGB = BlendEquation::Add;

    /** @brief Equation used to combine the source and destination alpha values. */
    BlendEquation EquationAlpha = BlendEquation::Add;

    /** @brief Constant color used by blend factors that reference a constant color. */
    M::Vector<4> ConstantColor = {0, 0, 0, 0};

    Blend(const bool enabled) : Enabled(enabled) {}

    /** @brief Applies the configured blending state to the OpenGL context. */
    void Apply();
};
} // namespace N::G
