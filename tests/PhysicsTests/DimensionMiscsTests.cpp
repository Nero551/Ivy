#include "Physics/DimensionalAnalysis/Dimensionals.hpp"
#include "Physics/DimensionalAnalysis/Dimension.hpp"
#include "Physics/Units.hpp"
#include <gtest/gtest.h>
#include <sstream>
#include <string>

namespace Ivy::UnitTests
{
using namespace Ivy::P;

// ==================== Units ====================
TEST(Units, LengthScaleFactors)
{
    EXPECT_FLOAT_EQ(Units::Meter, 1.0f);
    EXPECT_FLOAT_EQ(Units::Kilometer, 1e3f);
    EXPECT_FLOAT_EQ(Units::Millimeter, 1e-3f);
    EXPECT_FLOAT_EQ(Units::Centimeter, 1e-2f);
    EXPECT_FLOAT_EQ(Units::Micrometer, 1e-6f);

    // Internal consistency
    EXPECT_FLOAT_EQ(Units::Kilometer, 1000.0f * Units::Meter);
    EXPECT_NEAR(Units::Centimeter, 0.01f * Units::Meter, 1e-9f);
}

TEST(Units, MassScaleFactors)
{
    EXPECT_FLOAT_EQ(Units::Kilogram, 1.0f);
    EXPECT_FLOAT_EQ(Units::Gram, 1e-3f);
    EXPECT_FLOAT_EQ(Units::Milligram, 1e-6f);

    EXPECT_NEAR(Units::Gram, 1e-3f * Units::Kilogram, 1e-9f);
    EXPECT_NEAR(Units::Milligram, 1e-3f * Units::Gram, 1e-12f);
}

TEST(Units, TimeScaleFactors)
{
    EXPECT_FLOAT_EQ(Units::Second, 1.0f);
    EXPECT_FLOAT_EQ(Units::Millisecond, 1e-3f);
    EXPECT_FLOAT_EQ(Units::Microsecond, 1e-6f);
    EXPECT_FLOAT_EQ(Units::Minute, 60.0f);
    EXPECT_FLOAT_EQ(Units::Hour, 3600.0f);
    EXPECT_FLOAT_EQ(Units::Day, 86400.0f);

    // Composition
    EXPECT_FLOAT_EQ(Units::Minute, 60.0f * Units::Second);
    EXPECT_FLOAT_EQ(Units::Hour, 60.0f * Units::Minute);
    EXPECT_FLOAT_EQ(Units::Day, 24.0f * Units::Hour);
}

TEST(Units, UnitsAreConstexpr)
{
    constexpr float km = Units::Kilometer;
    constexpr float hour = Units::Hour;
    static_assert(km == 1000.0f);
    static_assert(hour == 3600.0f);
    SUCCEED();
}

// ==================== Print ====================
TEST(DimensionPrint, BaseUnitsPrintContainTheirSymbol)
{
    std::ostringstream lengthOss;
    Length<1>::Print(lengthOss);
    EXPECT_NE(lengthOss.str().find('m'), std::string::npos);

    std::ostringstream timeOss;
    Time<1>::Print(timeOss);
    EXPECT_NE(timeOss.str().find('s'), std::string::npos);

    std::ostringstream massOss;
    Mass<1>::Print(massOss);
    EXPECT_NE(massOss.str().find("kg"), std::string::npos);
}

TEST(DimensionPrint, NamedDimensionsPrintAsName)
{
    std::ostringstream forceOss;
    Force::Print(forceOss);
    EXPECT_EQ(forceOss.str(), "N");

    std::ostringstream jouleOss;
    Joule::Print(jouleOss);
    EXPECT_EQ(jouleOss.str(), "J");
}

TEST(DimensionPrint, DimensionlessPrintsEmpty)
{
    std::ostringstream oss;
    Dimensionless::Print(oss);
    EXPECT_EQ(oss.str(), "");
}

TEST(DimensionPrint, NamedDimensionalValuesFormatValueAndUnit)
{
    Dimension<float, Force> f(5.0f);
    std::ostringstream oss;
    oss << f;
    EXPECT_EQ(oss.str(), "5 N");

    Dimension<float, Joule> e(20.0f);
    std::ostringstream oss2;
    oss2 << e;
    EXPECT_EQ(oss2.str(), "20 J");
}

TEST(DimensionPrint, DimensionlessValueHasNoTrailingSpace)
{
    Dimension<float, Dimensionless> d(3.14f);
    std::ostringstream oss;
    oss << d;
    // Should NOT contain a trailing space (Exponent == 0 short-circuits)
    EXPECT_NE(oss.str().find("3.14"), std::string::npos);
    EXPECT_EQ(oss.str().find(' '), std::string::npos);
}

TEST(DimensionPrint, BaseDimensionValueHasSpaceThenUnit)
{
    Dimension<float, Length<1>> m(5.0f);
    std::ostringstream oss;
    oss << m;
    std::string s = oss.str();
    // Should be "<number> <unit>"
    EXPECT_NE(s.find('5'), std::string::npos);
    EXPECT_NE(s.find(' '), std::string::npos);
    EXPECT_NE(s.find('m'), std::string::npos);
}

} // namespace Ivy::UnitTests
