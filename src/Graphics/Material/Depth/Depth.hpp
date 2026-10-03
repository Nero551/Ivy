#pragma once
#include "DepthFunction.hpp"

namespace N::G
{
/**
 * @brief Controls depth testing and depth buffer writes.
 */
struct Depth
{
    /** @brief Whether depth testing is enabled. */
    bool Enabled;

    /** @brief Whether successful depth-tested fragments write to the depth buffer. */
    bool Write = true;

    /** @brief Comparison function used by the depth test. */
    DepthFunction Function = DepthFunction::Less;

    Depth(const bool enabled) : Enabled(enabled) {}

    /** @brief Applies the configured depth state to the OpenGL context. */
    void Apply();
};
} // namespace N::G
