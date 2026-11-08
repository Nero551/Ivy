#include "Physics/Include.hpp"
#include <gtest/gtest.h>
#include <type_traits>

namespace Ivy::UnitTests
{
using namespace Ivy::P;

using Meter = Dimension<float, Length<1>>;
using Second = Dimension<float, Time<1>>;

// Compile-time helpers for "this does / doesn't compile"
template <typename A, typename B>
concept CanAdd = requires(A a, B b) { a + b; };

template <typename A, typename B>
concept CanSubtract = requires(A a, B b) { a - b; };

template <typename D>
concept CanSqrt = requires(D d) { Ivy::P::Sqrt(d); };

// ==================== Construction & Access ====================
TEST(DimensionValue, DefaultAndValueConstruction)
{
    Meter m0;
    EXPECT_FLOAT_EQ(m0(), 0.0f);

    Meter m(5.0f);
    EXPECT_FLOAT_EQ(m(), 5.0f);
    EXPECT_FLOAT_EQ(m.Value, 5.0f);
}

TEST(DimensionValue, OperatorValueIsMutableAndConstCorrect)
{
    Meter m(7.0f);
    m() = 9.0f;
    EXPECT_FLOAT_EQ(m.Value, 9.0f);

    const Meter cm(3.0f);
    EXPECT_FLOAT_EQ(cm(), 3.0f);
}

TEST(DimensionValue, CrossPrecisionConstruction)
{
    Dimension<double, Length<1>> d(3.14);
    Meter m(d);
    EXPECT_FLOAT_EQ(m(), 3.14f);
}

TEST(DimensionValue, CrossNameConstructionSameDimension)
{
    // Joule and Force*Length share ::Normalized, so conversion is allowed.
    Dimension<float, OperationDimensional<Force, Length<1>>> work(10.0f);
    Dimension<float, Joule> joule(work);
    EXPECT_FLOAT_EQ(joule(), 10.0f);

    // Force and Mass*Acceleration share ::Normalized, so conversion is allowed.
    Dimension<float, OperationDimensional<Mass<1>, Acceleration>> ma(6.0f);
    Dimension<float, Force> force(ma);
    EXPECT_FLOAT_EQ(force(), 6.0f);
}

// ==================== Same-Dimension Addition/Subtraction ====================
TEST(DimensionValue, AddAndSubtractSameDimension)
{
    Meter a(10.0f), b(3.0f);
    EXPECT_FLOAT_EQ((a + b)(), 13.0f);
    EXPECT_FLOAT_EQ((a - b)(), 7.0f);
    EXPECT_FLOAT_EQ((b - a)(), -7.0f);
}

TEST(DimensionValue, AddRejectsDifferentDimensions)
{
    static_assert(CanAdd<Meter, Meter>);
    static_assert(!CanAdd<Meter, Second>);
    static_assert(!CanAdd<Meter, Dimension<float, Length<2>>>);
    static_assert(!CanAdd<Dimension<float, Velocity>, Dimension<float, Acceleration>>);
    SUCCEED();
}

TEST(DimensionValue, EquivalentDimensionalTypesCanBeAdded)
{
    // Joule and Force*Length are same ::Normalized → addition is legal.
    Dimension<float, Joule> a(5.0f);
    Dimension<float, OperationDimensional<Force, Length<1>>> b(7.0f);
    auto c = a + b;
    EXPECT_FLOAT_EQ(c(), 12.0f);
}

// ==================== Multiplication / Division ====================
TEST(DimensionValue, MultiplySameDimensionYieldsLengthSquared)
{
    Meter a(3.0f), b(4.0f);
    auto c = a * b;
    EXPECT_FLOAT_EQ(c(), 12.0f);
    static_assert(std::is_same_v<decltype(c), Dimension<float, Length<2>>>);
}

TEST(DimensionValue, DivideSameDimensionYieldsDimensionless)
{
    Meter a(12.0f), b(4.0f);
    auto c = a / b;
    EXPECT_FLOAT_EQ(c(), 3.0f);
    static_assert(std::is_same_v<decltype(c)::Dimensional, Dimensionless>);
}

TEST(DimensionValue, DistanceEqualsVelocityTimesTime)
{
    Dimension<float, Velocity> v(3.0f);
    Second t(4.0f);
    auto d = v * t;
    EXPECT_FLOAT_EQ(d(), 12.0f);
    static_assert(std::is_same_v<decltype(d), Dimension<float, Length<1>>>);
}

TEST(DimensionValue, VelocityEqualsDistanceOverTime)
{
    Meter d(20.0f);
    Second t(4.0f);
    auto v = d / t;
    EXPECT_FLOAT_EQ(v(), 5.0f);
    static_assert(std::is_same_v<decltype(v)::Dimensional, Velocity::Normalized>);
}

TEST(DimensionValue, ForceEqualsMassTimesAcceleration)
{
    Dimension<float, Mass<1>> m(2.0f);
    Dimension<float, Acceleration> a(3.0f);
    auto f = m * a;
    EXPECT_FLOAT_EQ(f(), 6.0f);
    static_assert(std::is_same_v<decltype(f)::Dimensional, Force::Normalized>);
}

TEST(DimensionValue, JouleEqualsForceTimesLength)
{
    Dimension<float, Force> f(10.0f);
    Meter m(2.0f);
    auto e = f * m;
    EXPECT_FLOAT_EQ(e(), 20.0f);
    static_assert(std::is_same_v<decltype(e)::Dimensional, Joule::Normalized>);
}

// ==================== Scalar Operations ====================
TEST(DimensionValue, ScalarArithmeticPreservesDimension)
{
    Meter m(3.0f);

    auto a = m + 2.0f;
    EXPECT_FLOAT_EQ(a(), 5.0f);
    static_assert(std::is_same_v<decltype(a)::Dimensional, Length<1>>);

    auto b = 2.0f + m;
    EXPECT_FLOAT_EQ(b(), 5.0f);

    auto c = m - 2.0f;
    EXPECT_FLOAT_EQ(c(), 1.0f);

    auto d = 10.0f - m;
    EXPECT_FLOAT_EQ(d(), 7.0f);

    auto e = m * 4.0f;
    EXPECT_FLOAT_EQ(e(), 12.0f);

    auto f = 4.0f * m;
    EXPECT_FLOAT_EQ(f(), 12.0f);

    auto g = m / 2.0f;
    EXPECT_FLOAT_EQ(g(), 1.5f);
    static_assert(std::is_same_v<decltype(g)::Dimensional, Length<1>>);
}

TEST(DimensionValue, ScalarLeftDivisionInvertsDimension)
{
    Meter m(5.0f);
    auto inv = 100.0f / m;
    EXPECT_FLOAT_EQ(inv(), 20.0f);
    static_assert(std::is_same_v<decltype(inv)::Dimensional, Length<-1>>);
}

TEST(DimensionValue, ScalarLeftDivisionOnCompoundDimension)
{
    Dimension<float, Velocity> v(4.0f);
    auto inv = 1.0f / v;
    EXPECT_FLOAT_EQ(inv(), 0.25f);
    // 1 / (Length/Time) = Time / Length
    static_assert(std::is_same_v<decltype(inv)::Dimensional, OperationDimensional<Length<-1>, Time<1>>>);
}

TEST(DimensionValue, Negation)
{
    Meter m(5.0f);
    auto n = -m;
    EXPECT_FLOAT_EQ(n(), -5.0f);
    static_assert(std::is_same_v<decltype(n)::Dimensional, Length<1>>);
}

// ==================== Implicit Conversion ====================
TEST(DimensionValue, DimensionlessConvertsToScalar)
{
    Dimension<float, Dimensionless> d(3.14f);
    float f = d;
    EXPECT_FLOAT_EQ(f, 3.14f);

    static_assert(std::is_convertible_v<Dimension<float, Dimensionless>, float>);
    static_assert(!std::is_convertible_v<Meter, float>);
    static_assert(!std::is_convertible_v<Dimension<float, Velocity>, float>);
}

// ==================== Pow ====================
TEST(DimensionValue, PowSquare)
{
    Meter m(3.0f);
    auto sq = Pow<2>(m);
    EXPECT_FLOAT_EQ(sq(), 9.0f);
    static_assert(std::is_same_v<decltype(sq)::Dimensional, Length<2>>);
}

TEST(DimensionValue, PowCube)
{
    Meter m(2.0f);
    auto cube = Pow<3>(m);
    EXPECT_FLOAT_EQ(cube(), 8.0f);
    static_assert(std::is_same_v<decltype(cube)::Dimensional, Length<3>>);
}

TEST(DimensionValue, PowNegative)
{
    Meter m(4.0f);
    auto inv = Pow<-1>(m);
    EXPECT_NEAR(inv(), 0.25f, 1e-6f);
    static_assert(std::is_same_v<decltype(inv)::Dimensional, Length<-1>>);
}

TEST(DimensionValue, PowZeroGivesDimensionlessEquivalent)
{
    Meter m(5.0f);
    auto p = Pow<0>(m);
    EXPECT_FLOAT_EQ(p(), 1.0f);
    static_assert(std::is_same_v<decltype(p)::Dimensional, Length<0>>);
    // Length<0> has Exponent == 0, so it can convert to float
    static_assert(std::is_convertible_v<decltype(p), float>);
}

TEST(DimensionValue, PowOnCompoundDimension)
{
    // Pow<2> on Velocity scales every base exponent by 2.
    Dimension<float, Velocity> v(2.0f);
    auto vSq = Pow<2>(v);
    EXPECT_FLOAT_EQ(vSq(), 4.0f);
    static_assert(std::is_same_v<decltype(vSq)::Dimensional, OperationDimensional<Length<2>, Time<-2>>>);
}

// ==================== Sqrt ====================
TEST(DimensionValue, SqrtOnEvenExponents)
{
    Dimension<float, Length<4>> l4(16.0f);
    auto r = Sqrt(l4);
    EXPECT_FLOAT_EQ(r(), 4.0f);
    static_assert(std::is_same_v<decltype(r)::Dimensional, Length<2>>);

    Dimension<float, Length<2>> l2(25.0f);
    auto l1 = Sqrt(l2);
    EXPECT_FLOAT_EQ(l1(), 5.0f);
    static_assert(std::is_same_v<decltype(l1)::Dimensional, Length<1>>);
}

TEST(DimensionValue, SqrtRejectsOddExponents)
{
    static_assert(CanSqrt<Dimension<float, Length<2>>>);
    static_assert(CanSqrt<Dimension<float, Length<4>>>);
    static_assert(CanSqrt<Dimension<float, Length<6>>>);
    static_assert(!CanSqrt<Dimension<float, Length<1>>>);
    static_assert(!CanSqrt<Dimension<float, Length<3>>>);
    static_assert(!CanSqrt<Dimension<float, Length<5>>>);
    static_assert(!CanSqrt<Dimension<float, Force>>); // Mass exponent 1
    static_assert(!CanSqrt<Dimension<float, Joule>>); // Mass exponent 1
    SUCCEED();
}

// ==================== Physics Scenarios ====================
TEST(DimensionValue, PhysicsScenarioKineticEnergy)
{
    // KE = ½ m v²
    Dimension<float, Mass<1>> mass(2.0f);
    Dimension<float, Velocity> vel(3.0f);

    auto ke = 0.5f * mass * Pow<2>(vel);
    EXPECT_NEAR(ke(), 9.0f, 1e-5f);
    static_assert(std::is_same_v<decltype(ke)::Dimensional, Joule::Normalized>);
}

TEST(DimensionValue, PhysicsScenarioProjectileVerticalMotion)
{
    // v(t) = v0 - g·t
    // y(t) = v0·t - ½ g·t²
    Dimension<float, Velocity> v0(10.0f);
    Dimension<float, Acceleration> g(9.8f);
    Second t(1.0f);

    auto v = v0 - g * t;
    EXPECT_NEAR(v(), 10.0f - 9.8f, 1e-4f);
    static_assert(std::is_same_v<decltype(v)::Dimensional, Length<1>> == false); // it's Velocity
    static_assert(std::is_same_v<decltype(v)::Dimensional, Velocity::Normalized>);

    auto y = v0 * t - 0.5f * g * Pow<2>(t);
    EXPECT_NEAR(y(), 10.0f * 1.0f - 0.5f * 9.8f * 1.0f, 1e-4f);
    static_assert(std::is_same_v<decltype(y)::Dimensional, Length<1>>);
}

TEST(DimensionValue, PhysicsScenarioDensityTimesVolume)
{
    // m = ρ · V
    Dimension<float, Density> rho(1000.0f);
    Dimension<float, Volume> vol(0.5f);

    auto m = rho * vol;
    EXPECT_FLOAT_EQ(m(), 500.0f);
    static_assert(std::is_same_v<decltype(m)::Dimensional, Mass<1>::Normalized>);
}

TEST(DimensionValue, PhysicsScenarioWorkEqualsForceDotDistance)
{
    // W = F · d
    Dimension<float, Force> force(20.0f);
    Meter dist(5.0f);
    auto work = force * dist;
    EXPECT_FLOAT_EQ(work(), 100.0f);
    static_assert(std::is_same_v<decltype(work)::Dimensional, Joule::Normalized>);
}

} // namespace Ivy::UnitTests
