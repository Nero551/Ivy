#include "Utilities/StringUtils.hpp"
#include <gtest/gtest.h>

namespace Ivy::UnitTests
{
using namespace Ivy::U;

TEST(StringUtils, Superscript1IsEmpty)
{
    EXPECT_EQ(Superscript(1), "");
}

TEST(StringUtils, SuperscriptZero)
{
    EXPECT_EQ(Superscript(0), "⁰");
}

TEST(StringUtils, SuperscriptSingleDigits)
{
    EXPECT_EQ(Superscript(2), "²");
    EXPECT_EQ(Superscript(3), "³");
    EXPECT_EQ(Superscript(5), "⁵");
    EXPECT_EQ(Superscript(9), "⁹");
}

TEST(StringUtils, SuperscriptMultiDigit)
{
    EXPECT_EQ(Superscript(10), "¹⁰");
    EXPECT_EQ(Superscript(42), "⁴²");
    EXPECT_EQ(Superscript(123), "¹²³");
}

TEST(StringUtils, SuperscriptNegative)
{
    EXPECT_EQ(Superscript(-2), "⁻²");
    EXPECT_EQ(Superscript(-10), "⁻¹⁰");
}

} // namespace Ivy::UnitTests
