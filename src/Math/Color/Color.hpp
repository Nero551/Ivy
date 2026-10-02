#pragma once
#include "../Vector/Vector4.hpp"

namespace N::M::Color
{
using Color = Vector<4>;

inline static constexpr Color Black = {0.0f, 0.0f, 0.0f, 1.0f};
inline static constexpr Color Red = {1.0f, 0.0f, 0.0f, 1.0f};
inline static constexpr Color White = {1.0f, 1.0f, 1.0f, 1.0f};
inline static constexpr Color Green = {0.0f, 1.0f, 0.0f, 1.0f};
inline static constexpr Color Blue = {0.0f, 0.0f, 1.0f, 1.0f};
inline static constexpr Color Yellow = {1.0f, 1.0f, 0.0f, 1.0f};
inline static constexpr Color Cyan = {0.0f, 1.0f, 1.0f, 1.0f};
inline static constexpr Color Magenta = {1.0f, 0.0f, 1.0f, 1.0f};
inline static constexpr Color Gray = {0.5f, 0.5f, 0.5f, 1.0f};
inline static constexpr Color LightGray = {0.75f, 0.75f, 0.75f, 1.0f};
inline static constexpr Color DarkGray = {0.25f, 0.25f, 0.25f, 1.0f};
inline static constexpr Color Orange = {1.0f, 0.5f, 0.0f, 1.0f};
inline static constexpr Color Purple = {0.5f, 0.0f, 1.0f, 1.0f};
inline static constexpr Color Brown = {0.6f, 0.3f, 0.1f, 1.0f};
inline static constexpr Color Pink = {1.0f, 0.4f, 0.7f, 1.0f};
inline static constexpr Color Transparent = {0.0f, 0.0f, 0.0f, 0.0f};

} // namespace N::M::Color
