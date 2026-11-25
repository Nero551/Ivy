#include "Math/Common/Comparison.hpp"
#include "Math/Common/Constants.hpp"
#include "Math/Common/Constraints.hpp"
#include "Math/Common/Exponentials.hpp"
#include "Math/Common/Interpolation.hpp"
#include "Math/Common/Logarithms.hpp"
#include "Math/Common/Random.hpp"
#include "Math/Common/Trigonometry.hpp"
#include <cmath>
#include <gtest/gtest.h>

namespace Ivy::UnitTests
{
using namespace Ivy::M;

// ==================== Constants ====================
TEST(Constants, FundamentalValues)
{
    EXPECT_NEAR(PI, 3.14159265358979f, 1e-5f);
    EXPECT_NEAR(E, 2.71828182845905f, 1e-5f);
    EXPECT_NEAR(TAU, 2.0f * PI, 1e-6f);
    EXPECT_NEAR(PHI, 1.61803398874989f, 1e-5f);
    EXPECT_NEAR(EPSILON, 1e-5f, 1e-12f);
}

// ==================== NearlyEquals ====================
TEST(Comparison, NearlyEqualsBasic)
{
    EXPECT_TRUE(M::NearlyEquals(1.0f, 1.0f));
    EXPECT_TRUE(M::NearlyEquals(0.0f, 0.0f));
    EXPECT_TRUE(M::NearlyEquals(1.0f, 1.0f + 1e-6f));
    EXPECT_FALSE(M::NearlyEquals(1.0f, 1.0f + 1e-4f));
    EXPECT_FALSE(M::NearlyEquals(-1.0f, 1.0f));
}

TEST(Comparison, NearlyEqualsBoundary)
{
    // Uses <= so diff == epsilon still passes.
    // NOTE: `1.0f + EPSILON` rounds ABOVE EPSILON in float; anchor at 0.0f.
    EXPECT_TRUE(M::NearlyEquals(0.0f, EPSILON));
    EXPECT_TRUE(M::NearlyEquals(0.0f, -EPSILON));
    EXPECT_FALSE(M::NearlyEquals(0.0f, 2.0f * EPSILON));
}

TEST(Comparison, NearlyEqualsCustomEpsilon)
{
    EXPECT_TRUE(M::NearlyEquals(1.0f, 1.1f, 0.2f));
    EXPECT_FALSE(M::NearlyEquals(1.0f, 1.1f, 0.05f));
}

// ==================== Clamp ====================
TEST(Constraints, Clamp)
{
    EXPECT_FLOAT_EQ(Clamp(5.0f, 0.0f, 10.0f), 5.0f);
    EXPECT_FLOAT_EQ(Clamp(-1.0f, 0.0f, 10.0f), 0.0f);
    EXPECT_FLOAT_EQ(Clamp(11.0f, 0.0f, 10.0f), 10.0f);
    // Boundaries
    EXPECT_FLOAT_EQ(Clamp(0.0f, 0.0f, 10.0f), 0.0f);
    EXPECT_FLOAT_EQ(Clamp(10.0f, 0.0f, 10.0f), 10.0f);
}

// ==================== Exponentials ====================
TEST(Exponentials, Pow)
{
    EXPECT_FLOAT_EQ(Pow(2.0f, 3.0f), 8.0f);
    EXPECT_FLOAT_EQ(Pow(2.0f, 0.0f), 1.0f);
    EXPECT_FLOAT_EQ(Pow(0.0f, 0.0f), 1.0f);
    EXPECT_FLOAT_EQ(Pow(4.0f, 0.5f), 2.0f);
    EXPECT_FLOAT_EQ(Pow(9.0f, -0.5f), 1.0f / 3.0f);
}

TEST(Exponentials, Sqrt)
{
    EXPECT_FLOAT_EQ(Sqrt(16.0f), 4.0f);
    EXPECT_FLOAT_EQ(Sqrt(0.0f), 0.0f);
    // Negative → NaN
    EXPECT_TRUE(std::isnan(Sqrt(-1.0f)));
}

TEST(Exponentials, Exp)
{
    EXPECT_FLOAT_EQ(Exp(0.0f), 1.0f);
    EXPECT_NEAR(Exp(1.0f), E, 1e-6f);
    EXPECT_NEAR(Exp(-1.0f), 1.0f / E, 1e-6f);
}

TEST(Exponentials, Factorial)
{
    EXPECT_EQ(Factorial(0), 1u);
    EXPECT_EQ(Factorial(1), 1u);
    EXPECT_EQ(Factorial(5), 120u);
    EXPECT_EQ(Factorial(10), 3628800u);
}

// ==================== Logarithms ====================
TEST(Logarithms, Ln)
{
    EXPECT_NEAR(Ln(1.0f), 0.0f, 1e-6f);
    EXPECT_NEAR(Ln(E), 1.0f, 1e-6f);
    // Ln(0) = -inf
    EXPECT_TRUE(std::isinf(Ln(0.0f)));
    EXPECT_LT(Ln(0.0f), 0.0f);
}

TEST(Logarithms, Log)
{
    EXPECT_NEAR(Log(10.0f, 100.0f), 2.0f, 1e-5f);
    EXPECT_NEAR(Log(2.0f, 8.0f), 3.0f, 1e-5f);
    EXPECT_NEAR(Log(E, E * E), 2.0f, 1e-5f);
}

// ==================== Interpolation ====================
TEST(Interpolation, Lerp)
{
    EXPECT_FLOAT_EQ(Lerp(0.0f, 10.0f, 0.0f), 0.0f);
    EXPECT_FLOAT_EQ(Lerp(0.0f, 10.0f, 1.0f), 10.0f);
    EXPECT_FLOAT_EQ(Lerp(0.0f, 10.0f, 0.5f), 5.0f);
    // Extrapolation
    EXPECT_FLOAT_EQ(Lerp(0.0f, 10.0f, 2.0f), 20.0f);
}

TEST(Interpolation, EaseInOutBoundaries)
{
    // All easing curves must map 0 -> 0 and 1 -> 1
    using EasingFn = float (*)(float);
    constexpr EasingFn easings[] = {
        EaseInQuad,
        EaseOutQuad,
        EaseInOutQuad,
        EaseInCubic,
        EaseOutCubic,
        EaseInOutCubic,
    };
    for (EasingFn ease : easings)
    {
        EXPECT_FLOAT_EQ(ease(0.0f), 0.0f);
        EXPECT_FLOAT_EQ(ease(1.0f), 1.0f);
    }
}
TEST(Interpolation, EaseMidpoints)
{
    EXPECT_FLOAT_EQ(EaseInQuad(0.5f), 0.25f);
    EXPECT_FLOAT_EQ(EaseOutQuad(0.5f), 0.75f);
    EXPECT_FLOAT_EQ(EaseInOutQuad(0.5f), 0.5f);

    EXPECT_FLOAT_EQ(EaseInCubic(0.5f), 0.125f);
    EXPECT_FLOAT_EQ(EaseOutCubic(0.5f), 0.875f);
    EXPECT_FLOAT_EQ(EaseInOutCubic(0.5f), 0.5f);
}

// ==================== Trigonometry ====================
TEST(Trigonometry, RadDeg)
{
    EXPECT_NEAR(Rad(180.0f), PI, 1e-5f);
    EXPECT_NEAR(Rad(90.0f), PI / 2.0f, 1e-5f);
    EXPECT_NEAR(Deg(PI), 180.0f, 1e-5f);
    EXPECT_NEAR(Deg(PI / 2.0f), 90.0f, 1e-5f);
}

TEST(Trigonometry, DegreeVersions)
{
    EXPECT_NEAR(DSin(30.0f), 0.5f, 1e-5f);
    EXPECT_NEAR(DCos(60.0f), 0.5f, 1e-5f);
    EXPECT_NEAR(DTan(45.0f), 1.0f, 1e-5f);
    EXPECT_NEAR(DSec(60.0f), 2.0f, 1e-5f);
    EXPECT_NEAR(DCsc(30.0f), 2.0f, 1e-5f);
    EXPECT_NEAR(DCot(45.0f), 1.0f, 1e-5f);
}

TEST(Trigonometry, InverseDegreeVersions)
{
    EXPECT_NEAR(AsinD(0.5f), 30.0f, 1e-5f);
    EXPECT_NEAR(AcosD(0.5f), 60.0f, 1e-5f);
    EXPECT_NEAR(AtanD(1.0f), 45.0f, 1e-5f);
    EXPECT_NEAR(Atan2D(1.0f, 1.0f), 45.0f, 1e-5f);
    EXPECT_NEAR(Atan2D(1.0f, -1.0f), 135.0f, 1e-5f);
}

// ==================== Random ====================
TEST(Random, UniformRange)
{
    for (int i = 0; i < 50; ++i)
    {
        float r = Random();
        EXPECT_GE(r, 0.0f);
        EXPECT_LE(r, 1.0f);
    }
}

TEST(Random, BoundedFloat)
{
    for (int i = 0; i < 50; ++i)
    {
        float r = Random(5.0f, 10.0f);
        EXPECT_GE(r, 5.0f);
        EXPECT_LE(r, 10.0f);
    }
}

TEST(Random, BoundedInt)
{
    for (int i = 0; i < 50; ++i)
    {
        int r = Random(1, 6);
        EXPECT_GE(r, 1);
        EXPECT_LE(r, 6);
    }
}

TEST(Random, GaussianBoundsReasonable)
{
    // Gaussian with mean 0, stddev 1 — all samples should be within [-10, 10]
    for (int i = 0; i < 50; ++i)
    {
        float r = RandomGaussian(0.0f, 1.0f);
        EXPECT_GE(r, -10.0f);
        EXPECT_LE(r, 10.0f);
    }
}

} // namespace Ivy::UnitTests
