#include "Math/Color/Color.hpp"
#include "Math/Include.hpp"
#include <cmath>
#include <gtest/gtest.h>

namespace Ivy::UnitTests
{
using namespace Ivy::M;

// Alias so gtest macros don't choke on template commas.
using Mat44 = Matrix<4, 4>;

// ==================== Basis ====================
TEST(Basis, DefaultMatrix)
{
    Basis b;
    // Default basis: Right=(1,0,0), Up=(0,1,0), Forward=(0,0,-1)
    Mat44 m = b.GetMatrix();
    EXPECT_FLOAT_EQ(m(0, 0), 1.0f);
    EXPECT_FLOAT_EQ(m(1, 0), 0.0f);
    EXPECT_FLOAT_EQ(m(2, 0), 0.0f);
    EXPECT_FLOAT_EQ(m(0, 1), 0.0f);
    EXPECT_FLOAT_EQ(m(1, 1), 1.0f);
    EXPECT_FLOAT_EQ(m(0, 2), 0.0f);
    EXPECT_FLOAT_EQ(m(1, 2), 0.0f);
    EXPECT_FLOAT_EQ(m(2, 2), -1.0f);
}

TEST(Basis, InverseMatrixIsTranspose)
{
    Basis b;
    Mat44 m = b.GetMatrix();
    Mat44 inv = b.GetInverseMatrix();
    EXPECT_TRUE((m * inv).NearlyEquals(Mat44::Identity(), 1e-5f));
}

TEST(Basis, CustomBasis)
{
    Basis b;
    b.Right = Vector<3>(0, 1, 0);
    b.Up = Vector<3>(1, 0, 0);
    b.Forward = Vector<3>(0, 0, 1);

    Mat44 m = b.GetMatrix();
    EXPECT_FLOAT_EQ(m(0, 0), 0.0f);
    EXPECT_FLOAT_EQ(m(1, 0), 1.0f);
    EXPECT_FLOAT_EQ(m(0, 1), 1.0f);
    EXPECT_FLOAT_EQ(m(2, 2), 1.0f);
}

// ==================== Color ====================
TEST(Color, Constants)
{
    EXPECT_EQ(Color::Black, Vector<4>(0.0f, 0.0f, 0.0f, 1.0f));
    EXPECT_EQ(Color::White, Vector<4>(1.0f, 1.0f, 1.0f, 1.0f));
    EXPECT_EQ(Color::Red, Vector<4>(1.0f, 0.0f, 0.0f, 1.0f));
    EXPECT_EQ(Color::Green, Vector<4>(0.0f, 1.0f, 0.0f, 1.0f));
    EXPECT_EQ(Color::Blue, Vector<4>(0.0f, 0.0f, 1.0f, 1.0f));
    EXPECT_EQ(Color::Transparent, Vector<4>(0.0f, 0.0f, 0.0f, 0.0f));
}

TEST(Color, IsVectorFour)
{
    Color::Color c = Color::Red;
    Vector<4> v = c * 2.0f;
    EXPECT_FLOAT_EQ(v.x, 2.0f);
    EXPECT_FLOAT_EQ(v.w, 2.0f);

    Color::Color mixed = Color::Red + Color::Green;
    EXPECT_EQ(mixed, Vector<4>(1.0f, 1.0f, 0.0f, 2.0f));
}

TEST(Color, NearlyEqualsInherited)
{
    Vector<4> slightly(1.0f + 1e-6f, 0.0f, 0.0f, 1.0f);
    EXPECT_TRUE(Color::Red.NearlyEquals(slightly));
}

} // namespace Ivy::UnitTests
