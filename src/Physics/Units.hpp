#pragma once
#include "Math/Common/Exponentials.hpp"

#include <glad/glad.h>

/**
 * @namespace N::P::Units
 * @brief Standard International System of Units (SI) constants used throughout Nova.
 *
 * Provides constexpr scale factors for common SI units.
 * Base units are represented by a value of 1.0f, while derived units are expressed
 * relative to their corresponding SI base unit.
 *
 * These constants provide a consistent unit convention across the engine and allow
 * values to be written explicitly in terms of physical units when useful.
 */
namespace N::P::Units
{
constexpr float Meter = 1.0f;
constexpr float Kilometer = 1e3f * Meter;
constexpr float Micrometer = 1e-6f * Meter;
constexpr float Millimeter = 1e-3f * Meter;
constexpr float Centimeter = 1e-2f * Meter;

constexpr float Kilogram = 1.0f;
constexpr float Gram = 1e-3f * Kilogram;
constexpr float Milligram = 1e-6f * Kilogram;

constexpr float Second = 1.0f;
constexpr float Microsecond = 1e-6f * Second;
constexpr float Millisecond = 1e-3f * Second;
constexpr float Minute = 60.0f * Second;
constexpr float Hour = 60.0f * Minute;
constexpr float Day = 24.0f * Hour;
// constexpr float Newton = Kilogram * (Meter / M::Pow(Second, 2.0f));
} // namespace N::P::Units
