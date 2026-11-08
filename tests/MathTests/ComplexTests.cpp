#include "Math/Include.hpp"
#include <cmath>
#include <gtest/gtest.h>

namespace Ivy::UnitTests
{
using namespace Ivy::M;

TEST(Complex, DefaultAndValue)
{
    Complex<> c0;
    EXPECT_FLOAT_EQ(c0.Real, 0.0f);
    EXPECT_FLOAT_EQ(c0.Imaginary, 0.0f);

    Complex<> c(3.0f, -2.0f);
    EXPECT_FLOAT_EQ(c.Real, 3.0f);
    EXPECT_FLOAT_EQ(c.Imaginary, -2.0f);
}

TEST(Complex, IndexAccess)
{
    Complex<> c(3.0f, 4.0f);
    EXPECT_FLOAT_EQ(c(0), 3.0f);
    EXPECT_FLOAT_EQ(c(1), 4.0f);

    c(0) = 5.0f;
    EXPECT_FLOAT_EQ(c.Real, 5.0f);
}

TEST(Complex, MagnitudeAndArgument)
{
    Complex<> c(3.0f, 4.0f);
    EXPECT_FLOAT_EQ(c.MagnitudeSquared(), 25.0f);
    EXPECT_FLOAT_EQ(c.Magnitude(), 5.0f);
    EXPECT_NEAR(c.Argument(), std::atan2(4.0f, 3.0f), 1e-6f);

    // Pure imaginary
    Complex<> i(0.0f, 1.0f);
    EXPECT_NEAR(i.Argument(), PI / 2.0f, 1e-6f);
}

TEST(Complex, ConjugateAndInverse)
{
    Complex<> c(3.0f, 4.0f);
    Complex<> conj = c.Conjugate();
    EXPECT_EQ(conj, Complex<>(3.0f, -4.0f));

    Complex<> inv = c.Inverse();
    Complex<> p = c * inv;
    EXPECT_NEAR(p.Real, 1.0f, 1e-6f);
    EXPECT_NEAR(p.Imaginary, 0.0f, 1e-6f);
}

TEST(Complex, Normalized)
{
    Complex<> c(3.0f, 4.0f);
    Complex<> n = c.Normalized();
    EXPECT_NEAR(n.Magnitude(), 1.0f, 1e-6f);
    EXPECT_NEAR(n.Real, 0.6f, 1e-6f);
    EXPECT_NEAR(n.Imaginary, 0.8f, 1e-6f);
}

TEST(Complex, Arithmetic)
{
    Complex<> a(1.0f, 2.0f);
    Complex<> b(3.0f, 4.0f);

    EXPECT_EQ(a + b, Complex<>(4.0f, 6.0f));
    EXPECT_EQ(a - b, Complex<>(-2.0f, -2.0f));

    Complex<> p = a * b;
    EXPECT_FLOAT_EQ(p.Real, -5.0f);      // 1*3 - 2*4
    EXPECT_FLOAT_EQ(p.Imaginary, 10.0f); // 1*4 + 2*3

    Complex<> q = a / b;
    // (a / b) * b == a == (1, 2)  — Imaginary is 2, not 0.
    EXPECT_NEAR((q * b).Real, 1.0f, 1e-6f);
    EXPECT_NEAR((q * b).Imaginary, 2.0f, 1e-6f);
}

TEST(Complex, ScalarArithmetic)
{
    Complex<> a(3.0f, 4.0f);

    EXPECT_EQ(a + 2.0f, Complex<>(5.0f, 4.0f));
    EXPECT_EQ(2.0f + a, Complex<>(5.0f, 4.0f));
    EXPECT_EQ(a - 2.0f, Complex<>(1.0f, 4.0f));
    EXPECT_EQ(2.0f - a, Complex<>(-1.0f, -4.0f));
    EXPECT_EQ(a * 2.0f, Complex<>(6.0f, 8.0f));
    EXPECT_EQ(2.0f * a, Complex<>(6.0f, 8.0f));
    EXPECT_EQ(a / 2.0f, Complex<>(1.5f, 2.0f));

    // 5 / (3+4i) = 5*(3-4i)/25 = (15-20i)/25 = (0.6, -0.8)
    Complex<> r = 5.0f / a;
    EXPECT_NEAR(r.Real, 0.6f, 1e-6f);
    EXPECT_NEAR(r.Imaginary, -0.8f, 1e-6f);
}

TEST(Complex, CompoundAssignment)
{
    Complex<> c(1.0f, 2.0f);
    c += Complex<>(1.0f, 1.0f);
    EXPECT_EQ(c, Complex<>(2.0f, 3.0f));
    c -= Complex<>(1.0f, 1.0f);
    EXPECT_EQ(c, Complex<>(1.0f, 2.0f));
    c *= Complex<>(1.0f, 0.0f);
    EXPECT_EQ(c, Complex<>(1.0f, 2.0f));
    c /= Complex<>(1.0f, 0.0f);
    EXPECT_EQ(c, Complex<>(1.0f, 2.0f));

    c = Complex<>(1.0f, 2.0f);
    c += 5.0f;
    EXPECT_EQ(c, Complex<>(6.0f, 2.0f));
    c -= 5.0f;
    EXPECT_EQ(c, Complex<>(1.0f, 2.0f));
    c *= 2.0f;
    EXPECT_EQ(c, Complex<>(2.0f, 4.0f));
    c /= 2.0f;
    EXPECT_EQ(c, Complex<>(1.0f, 2.0f));
}

TEST(Complex, Negation)
{
    Complex<> c(1.0f, -2.0f);
    EXPECT_EQ(-c, Complex<>(-1.0f, 2.0f));
}

TEST(Complex, EqualityAndNearlyEquals)
{
    Complex<> a(1.0f, 2.0f);
    EXPECT_EQ(a, Complex<>(1.0f, 2.0f));
    EXPECT_NE(a, Complex<>(1.0f, 2.1f));

    Complex<> b(1.0f + 1e-6f, 2.0f);
    EXPECT_TRUE(a.NearlyEquals(b));
    EXPECT_FALSE(a.NearlyEquals(b, 1e-7f));
}

TEST(Complex, PolarConversions)
{
    Polar<float> p(PI / 4.0f, std::sqrt(2.0f));
    Complex<> c = Complex<>::FromPolar(p);
    EXPECT_NEAR(c.Real, 1.0f, 1e-6f);
    EXPECT_NEAR(c.Imaginary, 1.0f, 1e-6f);

    Polar<float> p2 = c.ToPolar();
    EXPECT_NEAR(p2.Angle, PI / 4.0f, 1e-6f);
    EXPECT_NEAR(p2.Magnitude, std::sqrt(2.0f), 1e-6f);
}

TEST(Complex, ExpLn)
{
    // e^0 = 1
    Complex<> expZero = Exp(Complex<>(0.0f, 0.0f));
    EXPECT_NEAR(expZero.Real, 1.0f, 1e-6f);
    EXPECT_NEAR(expZero.Imaginary, 0.0f, 1e-6f);

    // e^(iπ) = -1
    Complex<> expIpi = Exp(Complex<>(0.0f, PI));
    EXPECT_NEAR(expIpi.Real, -1.0f, 1e-6f);
    EXPECT_NEAR(expIpi.Imaginary, 0.0f, 1e-6f);

    // Ln(1) = 0
    Complex<> lnOne = Ln(Complex<>(1.0f, 0.0f));
    EXPECT_NEAR(lnOne.Real, 0.0f, 1e-6f);
    EXPECT_NEAR(lnOne.Imaginary, 0.0f, 1e-6f);

    // Ln(-1) = πi
    Complex<> lnNegOne = Ln(Complex<>(-1.0f, 0.0f));
    EXPECT_NEAR(lnNegOne.Real, 0.0f, 1e-6f);
    EXPECT_NEAR(lnNegOne.Imaginary, PI, 1e-6f);
}

TEST(Complex, PowWithRealExponent)
{
    // i² = -1
    Complex<> i(0.0f, 1.0f);
    Complex<> i2 = Pow(i, 2.0f);
    EXPECT_NEAR(i2.Real, -1.0f, 1e-6f);
    EXPECT_NEAR(i2.Imaginary, 0.0f, 1e-6f);

    // (1+i)² = 2i
    Complex<> onePlusI(1.0f, 1.0f);
    Complex<> sq = Pow(onePlusI, 2.0f);
    EXPECT_NEAR(sq.Real, 0.0f, 1e-6f);
    EXPECT_NEAR(sq.Imaginary, 2.0f, 1e-6f);
}

TEST(Complex, Sqrt)
{
    // Sqrt(-1) = i
    Complex<> i = Sqrt(Complex<>(-1.0f, 0.0f));
    EXPECT_NEAR(i.Real, 0.0f, 1e-6f);
    EXPECT_NEAR(i.Imaginary, 1.0f, 1e-6f);

    // Sqrt(4) = 2
    Complex<> two = Sqrt(Complex<>(4.0f, 0.0f));
    EXPECT_NEAR(two.Real, 2.0f, 1e-6f);
    EXPECT_NEAR(two.Imaginary, 0.0f, 1e-6f);
}

TEST(Complex, PowComplexBase)
{
    // 2^(3+0i) = 8
    Complex<> r = Pow(2.0f, Complex<>(3.0f, 0.0f));
    EXPECT_NEAR(r.Real, 8.0f, 1e-5f);
    EXPECT_NEAR(r.Imaginary, 0.0f, 1e-5f);

    // Log base e of e is 1
    Complex<> lg = Log(Complex<>(E, 0.0f), Complex<>(E, 0.0f));
    EXPECT_NEAR(lg.Real, 1.0f, 1e-5f);
    EXPECT_NEAR(lg.Imaginary, 0.0f, 1e-5f);
}

TEST(Complex, IdentityConstantI)
{
    EXPECT_EQ(I.Real, 0.0f);
    EXPECT_EQ(I.Imaginary, 1.0f);
    Complex<> sq = I * I;
    EXPECT_NEAR(sq.Real, -1.0f, 1e-6f);
    EXPECT_NEAR(sq.Imaginary, 0.0f, 1e-6f);
}

} // namespace Ivy::UnitTests
