#include "Physics/Include.hpp"
#include <gtest/gtest.h>
#include <type_traits>

namespace Ivy::UnitTests
{
using namespace Ivy::P;

// ==================== Base Dimensional Types ====================
TEST(DimensionalTypes, BaseExponentsAndOrders)
{
    static_assert(Length<1>::Exponent == 1);
    static_assert(Length<2>::Exponent == 2);
    static_assert(Length<-1>::Exponent == -1);
    static_assert(Time<1>::Exponent == 1);
    static_assert(Time<-3>::Exponent == -3);
    static_assert(Mass<1>::Exponent == 1);
    static_assert(Mass<5>::Exponent == 5);

    // Canonical sort order: Mass(1), Length(2), Time(3)
    static_assert(Mass<1>::Order == 1);
    static_assert(Length<1>::Order == 2);
    static_assert(Time<1>::Order == 3);

    SUCCEED();
}

TEST(DimensionalTypes, SelfNormalization)
{
    static_assert(std::is_same_v<Length<1>::Normalized, Length<1>>);
    static_assert(std::is_same_v<Length<2>::Normalized, Length<2>>);
    static_assert(std::is_same_v<Length<-3>::Normalized, Length<-3>>);
    static_assert(std::is_same_v<Time<-1>::Normalized, Time<-1>>);
    static_assert(std::is_same_v<Mass<4>::Normalized, Mass<4>>);
    static_assert(std::is_same_v<Dimensionless::Normalized, Dimensionless>);
    SUCCEED();
}

// ==================== WithExponent: REPLACE semantics ====================
TEST(DimensionalTypes, WithExponentReplaces)
{
    // WithExponent<E> on a Dimensional DISCARDS the current exponent
    // and replaces it with E.
    static_assert(std::is_same_v<Length<1>::WithExponent<2>, Length<2>>);
    static_assert(std::is_same_v<Length<2>::WithExponent<-1>, Length<-1>>); // NOT Length<-2>
    static_assert(std::is_same_v<Length<1>::WithExponent<0>, Length<0>>);
    static_assert(std::is_same_v<Time<-1>::WithExponent<3>, Time<3>>); // NOT Time<-3>
    static_assert(std::is_same_v<Mass<2>::WithExponent<-2>, Mass<-2>>);
    SUCCEED();
}

// ==================== MultiplyExponentBy: MULTIPLY semantics ====================
TEST(DimensionalTypes, MultiplyExponentByMultiplies)
{
    // MultiplyExponentBy<E> MULTIPLIES the current exponent by E.
    static_assert(std::is_same_v<Length<1>::MultiplyExponentBy<2>, Length<2>>);
    static_assert(std::is_same_v<Length<2>::MultiplyExponentBy<-1>, Length<-2>>);
    static_assert(std::is_same_v<Length<2>::MultiplyExponentBy<3>, Length<6>>);
    static_assert(std::is_same_v<Time<-1>::MultiplyExponentBy<3>, Time<-3>>);
    static_assert(std::is_same_v<Mass<2>::MultiplyExponentBy<-2>, Mass<-4>>);
    static_assert(std::is_same_v<Length<3>::MultiplyExponentBy<0>, Length<0>>);
    SUCCEED();
}

TEST(DimensionalTypes, WithExponentAndMultiplyExponentByDivergeWhenCurrentExponentIsNotOne)
{
    // When the current exponent is 1, the two aliases agree.
    static_assert(std::is_same_v<Length<1>::WithExponent<5>, Length<1>::MultiplyExponentBy<5>>);

    // When the current exponent is not 1, they diverge.
    static_assert(!std::is_same_v<Length<2>::WithExponent<3>,
        Length<2>::MultiplyExponentBy<3>>); // Length<3> vs Length<6>
    SUCCEED();
}

// ==================== WithRoot ====================
TEST(DimensionalTypes, WithRoot)
{
    static_assert(std::is_same_v<Length<4>::WithRoot<2>, Length<2>>);
    static_assert(std::is_same_v<Length<2>::WithRoot<2>, Length<1>>);
    static_assert(std::is_same_v<Length<-4>::WithRoot<2>, Length<-2>>);
    static_assert(std::is_same_v<Length<6>::WithRoot<3>, Length<2>>);
    static_assert(std::is_same_v<Length<9>::WithRoot<3>, Length<3>>);
    SUCCEED();
}

TEST(DimensionalTypes, SameTerm)
{
    static_assert(SameTerm<Length<1>, Length<2>>);
    static_assert(SameTerm<Length<1>, Length<-1>>);
    static_assert(SameTerm<Length<0>, Length<99>>);
    static_assert(SameTerm<Time<1>, Time<3>>);
    static_assert(SameTerm<Mass<1>, Mass<1>>);
    static_assert(!SameTerm<Length<1>, Time<1>>);
    static_assert(!SameTerm<Length<1>, Mass<1>>);
    static_assert(!SameTerm<Time<2>, Mass<2>>);
    SUCCEED();
}

TEST(DimensionalTypes, IsDimensionalConcept)
{
    static_assert(IsDimensional<Length<1>>);
    static_assert(IsDimensional<Length<-2>>);
    static_assert(IsDimensional<Time<3>>);
    static_assert(IsDimensional<Mass<0>>);
    static_assert(IsDimensional<Dimensionless>);
    static_assert(IsDimensional<Velocity>);
    static_assert(IsDimensional<Acceleration>);
    static_assert(IsDimensional<Force>);
    static_assert(IsDimensional<Joule>);
    SUCCEED();
}

// ==================== Composition & Canonical Normalization ====================
TEST(DimensionalTypes, VelocityNormalization)
{
    static_assert(std::is_same_v<Velocity::Normalized, OperationDimensional<Length<1>, Time<-1>>>);
    static_assert(std::is_same_v<Velocity::Normalized, Velocity>);
    SUCCEED();
}

TEST(DimensionalTypes, AccelerationNormalization)
{
    static_assert(std::is_same_v<Acceleration::Normalized, OperationDimensional<Length<1>, Time<-2>>>);
    SUCCEED();
}

TEST(DimensionalTypes, ForceNormalization)
{
    static_assert(std::is_same_v<Force::Normalized,
        OperationDimensional<Mass<1>, OperationDimensional<Length<1>, Time<-2>>>>);
    SUCCEED();
}

TEST(DimensionalTypes, JouleNormalization)
{
    // Joule = Force · Length. Duplicate Length<1> merges into Length<2>.
    static_assert(std::is_same_v<Joule::Normalized,
        OperationDimensional<Mass<1>, OperationDimensional<Length<2>, Time<-2>>>>);
    SUCCEED();
}

TEST(DimensionalTypes, AreaVolumeAliases)
{
    static_assert(std::is_same_v<Area, Length<2>>);
    static_assert(std::is_same_v<Volume, Length<3>>);
    SUCCEED();
}

TEST(DimensionalTypes, DensityNormalization)
{
    // Density = Mass / Volume = Mass · Length^-3
    static_assert(std::is_same_v<Density::Normalized, OperationDimensional<Mass<1>, Length<-3>>>);
    SUCCEED();
}

TEST(DimensionalTypes, SurfaceDensityNormalization)
{
    // SurfaceDensity = Mass / Area = Mass · Length^-2
    static_assert(std::is_same_v<SurfaceDensity::Normalized, OperationDimensional<Mass<1>, Length<-2>>>);
    SUCCEED();
}

// ==================== Invariants the names promise ====================
TEST(DimensionalTypes, DensityTimesVolumeIsMass)
{
    // The invariant that the pre-fix Density violated (was Mass·Length^-1).
    using Product = OperationDimensional<Density, Volume>;
    static_assert(std::is_same_v<Product::Normalized, Mass<1>>);

    using SProduct = OperationDimensional<SurfaceDensity, Area>;
    static_assert(std::is_same_v<SProduct::Normalized, Mass<1>>);
    SUCCEED();
}

// ==================== Equivalent compositions normalize identically ====================
TEST(DimensionalTypes, VelocityEqualsLengthOverTime)
{
    using LengthOverTime = OperationDimensional<Length<1>, Time<-1>>;
    static_assert(std::is_same_v<Velocity::Normalized, LengthOverTime::Normalized>);
    static_assert(SameNormalized<Velocity, LengthOverTime>);
    SUCCEED();
}

TEST(DimensionalTypes, ForceEqualsMassTimesAcceleration)
{
    using MassTimesAccel = OperationDimensional<Mass<1>, Acceleration>;
    static_assert(SameNormalized<Force, MassTimesAccel>);
    SUCCEED();
}

TEST(DimensionalTypes, JouleEqualsForceTimesLength)
{
    using ForceTimesLength = OperationDimensional<Force, Length<1>>;
    static_assert(SameNormalized<Joule, ForceTimesLength>);
    SUCCEED();
}

TEST(DimensionalTypes, LengthTimesInverseLengthIsDimensionless)
{
    using Cancellation = OperationDimensional<Length<1>, Length<-1>>;
    static_assert(std::is_same_v<Cancellation::Normalized, Dimensionless>);
    static_assert(SameNormalized<Cancellation, Dimensionless>);
    SUCCEED();
}

TEST(DimensionalTypes, ZeroExponentTermsAreDropped)
{
    using WithZero = OperationDimensional<Length<1>, Time<0>>;
    static_assert(std::is_same_v<WithZero::Normalized, Length<1>>);
    SUCCEED();
}

} // namespace Ivy::UnitTests
