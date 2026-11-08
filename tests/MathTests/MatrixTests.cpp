#include "Math/Matrix/Matrices.hpp"
#include <cmath>
#include <gtest/gtest.h>

namespace Ivy::UnitTests
{
using namespace Ivy::M;

// Aliases so gtest macros don't choke on template commas.
using Mat22 = Matrix<2, 2>;
using Mat23 = Matrix<2, 3>;
using Mat33 = Matrix<3, 3>;
using Mat44 = Matrix<4, 4>;

// ==================== BasicMatrix (via Matrix<2,3>) ====================
TEST(BasicMatrix, ConstructionFromElements)
{
    Mat23 m(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
    // Row-major input: [1 2 3; 4 5 6]
    EXPECT_FLOAT_EQ(m(0, 0), 1.0f);
    EXPECT_FLOAT_EQ(m(0, 1), 2.0f);
    EXPECT_FLOAT_EQ(m(0, 2), 3.0f);
    EXPECT_FLOAT_EQ(m(1, 0), 4.0f);
    EXPECT_FLOAT_EQ(m(1, 1), 5.0f);
    EXPECT_FLOAT_EQ(m(1, 2), 6.0f);
}

TEST(BasicMatrix, FillConstruction)
{
    Mat22 m(2.0f);
    for (unsigned int i = 0; i < 2; ++i)
    {
        for (unsigned int j = 0; j < 2; ++j)
        {
            EXPECT_FLOAT_EQ(m(i, j), 2.0f);
        }
    }

    Mat22 zero;
    for (unsigned int i = 0; i < 2; ++i)
    {
        for (unsigned int j = 0; j < 2; ++j)
        {
            EXPECT_FLOAT_EQ(zero(i, j), 0.0f);
        }
    }
}

TEST(BasicMatrix, ZeroAndIdentity)
{
    EXPECT_TRUE(Mat33::Zero().NearlyEquals(Mat33(0.0f)));

    Mat33 id = Mat33::Identity();
    for (unsigned int i = 0; i < 3; ++i)
    {
        for (unsigned int j = 0; j < 3; ++j)
        {
            EXPECT_FLOAT_EQ(id(i, j), (i == j ? 1.0f : 0.0f));
        }
    }
}

TEST(BasicMatrix, AdditionSubtractionNegation)
{
    Mat22 a(1.0f, 2.0f, 3.0f, 4.0f);
    Mat22 b(5.0f, 6.0f, 7.0f, 8.0f);

    Mat22 sum = a + b;
    EXPECT_FLOAT_EQ(sum(0, 0), 6.0f);
    EXPECT_FLOAT_EQ(sum(1, 1), 12.0f);

    Mat22 diff = a - b;
    EXPECT_FLOAT_EQ(diff(0, 0), -4.0f);
    EXPECT_FLOAT_EQ(diff(1, 1), -4.0f);

    Mat22 neg = -a;
    EXPECT_FLOAT_EQ(neg(0, 0), -1.0f);
    EXPECT_FLOAT_EQ(neg(1, 1), -4.0f);
}

TEST(BasicMatrix, ScalarOps)
{
    Mat22 a(1.0f, 2.0f, 3.0f, 4.0f);
    Mat22 scaled = a * 2.0f;
    EXPECT_FLOAT_EQ(scaled(0, 0), 2.0f);
    EXPECT_FLOAT_EQ(scaled(1, 1), 8.0f);

    Mat22 scaled2 = 2.0f * a;
    EXPECT_TRUE(scaled2.NearlyEquals(scaled));

    Mat22 divided = a / 2.0f;
    EXPECT_FLOAT_EQ(divided(0, 0), 0.5f);
    EXPECT_FLOAT_EQ(divided(1, 1), 2.0f);
}

TEST(BasicMatrix, MatrixVectorMultiply)
{
    Mat23 m(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f); // 2x3
    Vector<3> v(1.0f, 1.0f, 1.0f);
    Vector<2> r = m * v;
    EXPECT_FLOAT_EQ(r(0), 6.0f);
    EXPECT_FLOAT_EQ(r(1), 15.0f);
}

TEST(BasicMatrix, RowEchelonAndRank)
{
    Mat33 m(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
    EXPECT_EQ(m.Rank(), 2u);
    EXPECT_FALSE(m.IsFullRank());

    EXPECT_EQ(Mat33::Identity().Rank(), 3u);
    EXPECT_TRUE(Mat33::Identity().IsFullRank());
}

TEST(BasicMatrix, MultiplyAddSwapRow)
{
    Mat22 m(1.0f, 2.0f, 3.0f, 4.0f);

    Mat22 r1 = m.MultiplyRow(0.0f, 2.0f);
    EXPECT_FLOAT_EQ(r1(0, 0), 2.0f);
    EXPECT_FLOAT_EQ(r1(0, 1), 4.0f);
    EXPECT_FLOAT_EQ(r1(1, 0), 3.0f);

    Mat22 r2 = m.AddRow(0, 1);
    EXPECT_FLOAT_EQ(r2(0, 0), 4.0f);
    EXPECT_FLOAT_EQ(r2(0, 1), 6.0f);

    Mat22 r3 = m.SwapRow(0, 1);
    EXPECT_FLOAT_EQ(r3(0, 0), 3.0f);
    EXPECT_FLOAT_EQ(r3(0, 1), 4.0f);
    EXPECT_FLOAT_EQ(r3(1, 0), 1.0f);
    EXPECT_FLOAT_EQ(r3(1, 1), 2.0f);
}

TEST(BasicMatrix, Equality)
{
    Mat22 a(1.0f, 2.0f, 3.0f, 4.0f);
    Mat22 b(1.0f, 2.0f, 3.0f, 4.0f);
    Mat22 c(1.0f, 2.0f, 3.0f, 5.0f);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
    EXPECT_TRUE(a != c);

    Mat22 d(1.0f + 1e-6f, 2.0f, 3.0f, 4.0f);
    EXPECT_TRUE(a.NearlyEquals(d));
    EXPECT_FALSE(a.NearlyEquals(d, 1e-7f));
}

// ==================== Matrix<2,2> ====================
TEST(Matrix2, Multiply)
{
    Mat22 a(1.0f, 2.0f, 3.0f, 4.0f);
    Mat22 b(5.0f, 6.0f, 7.0f, 8.0f);
    Mat22 c = a * b;
    EXPECT_FLOAT_EQ(c(0, 0), 19.0f);
    EXPECT_FLOAT_EQ(c(0, 1), 22.0f);
    EXPECT_FLOAT_EQ(c(1, 0), 43.0f);
    EXPECT_FLOAT_EQ(c(1, 1), 50.0f);

    EXPECT_TRUE((a * Mat22::Identity()).NearlyEquals(a));
}

TEST(Matrix2, Determinant)
{
    EXPECT_FLOAT_EQ(Mat22::Identity().Determinant(), 1.0f);
    EXPECT_FLOAT_EQ(Mat22(1.0f, 2.0f, 3.0f, 4.0f).Determinant(), -2.0f);
    EXPECT_FLOAT_EQ(Mat22(2.0f, 0.0f, 0.0f, 3.0f).Determinant(), 6.0f);
}

TEST(Matrix2, Inverse)
{
    Mat22 m(1.0f, 2.0f, 3.0f, 4.0f);
    Mat22 inv = m.Inverse();
    EXPECT_TRUE((m * inv).NearlyEquals(Mat22::Identity()));

    EXPECT_TRUE(Mat22::Identity().Inverse().NearlyEquals(Mat22::Identity()));
}

TEST(Matrix2, ScaleAndRotate)
{
    Mat22 m = Mat22::Identity();

    Mat22 scaled = m.Scale(Vector<2>(2.0f, 3.0f));
    EXPECT_FLOAT_EQ(scaled(0, 0), 2.0f);
    EXPECT_FLOAT_EQ(scaled(1, 1), 3.0f);

    Mat22 rot = m.Rotate(PI / 2.0f);
    Vector<2> rv = rot * Vector<2>(1.0f, 0.0f);
    EXPECT_NEAR(rv.x, 0.0f, 1e-6f);
    EXPECT_NEAR(rv.y, 1.0f, 1e-6f);

    EXPECT_TRUE(m.Rotate(TAU).NearlyEquals(Mat22::Identity()));
}

// ==================== Matrix<3,3> ====================
TEST(Matrix3, Determinant)
{
    EXPECT_FLOAT_EQ(Mat33::Identity().Determinant(), 1.0f);

    Mat33 m(1, 2, 3, 0, 1, 4, 5, 6, 0);
    EXPECT_FLOAT_EQ(m.Determinant(), 1.0f);

    Mat33 d(2, 0, 0, 0, 3, 0, 0, 0, 4);
    EXPECT_FLOAT_EQ(d.Determinant(), 24.0f);
}

TEST(Matrix3, Inverse)
{
    Mat33 m(1, 2, 3, 0, 1, 4, 5, 6, 0);
    Mat33 inv = m.Inverse();
    EXPECT_TRUE((m * inv).NearlyEquals(Mat33::Identity()));

    EXPECT_TRUE(Mat33::Identity().Inverse().NearlyEquals(Mat33::Identity()));
}

TEST(Matrix3, RotationsXYZ)
{
    Mat33 m = Mat33::Identity();

    Vector<3> y(0, 1, 0);
    Vector<3> ry = m.RotateX(PI / 2.0f) * y;
    EXPECT_NEAR(ry.x, 0.0f, 1e-6f);
    EXPECT_NEAR(ry.y, 0.0f, 1e-6f);
    EXPECT_NEAR(ry.z, 1.0f, 1e-6f);

    Vector<3> x(1, 0, 0);
    Vector<3> rx = m.RotateY(PI / 2.0f) * x;
    EXPECT_NEAR(rx.x, 0.0f, 1e-6f);
    EXPECT_NEAR(rx.y, 0.0f, 1e-6f);
    EXPECT_NEAR(rx.z, -1.0f, 1e-6f);

    Vector<3> rz = m.RotateZ(PI / 2.0f) * x;
    EXPECT_NEAR(rz.x, 0.0f, 1e-6f);
    EXPECT_NEAR(rz.y, 1.0f, 1e-6f);
    EXPECT_NEAR(rz.z, 0.0f, 1e-6f);
}

TEST(Matrix3, RotateAroundAxis)
{
    Mat33 m = Mat33::Identity();
    Vector<3> zAxis(0, 0, 1);
    Mat33 r = m.RotateAroundAxis(zAxis, PI / 2.0f);
    Vector<3> x(1, 0, 0);
    Vector<3> rx = r * x;
    EXPECT_NEAR(rx.x, 0.0f, 1e-6f);
    EXPECT_NEAR(rx.y, 1.0f, 1e-6f);
    EXPECT_NEAR(rx.z, 0.0f, 1e-6f);
}

TEST(Matrix3, Translate2D)
{
    Mat33 m = Mat33::Identity();
    Mat33 t = m.Translate(Vector<2>(5.0f, 7.0f));
    Vector<3> p(0.0f, 0.0f, 1.0f);
    Vector<3> tp = t * p;
    EXPECT_FLOAT_EQ(tp.x, 5.0f);
    EXPECT_FLOAT_EQ(tp.y, 7.0f);
    EXPECT_FLOAT_EQ(tp.z, 1.0f);
}

// ==================== Matrix<4,4> ====================
TEST(Matrix4, DeterminantAndInverse)
{
    EXPECT_FLOAT_EQ(Mat44::Identity().Determinant(), 1.0f);

    Mat44 m = Mat44::Identity();
    m(0, 0) = 2.0f;
    m(1, 1) = 3.0f;
    m(2, 2) = 4.0f;
    m(3, 3) = 5.0f;
    EXPECT_FLOAT_EQ(m.Determinant(), 120.0f);

    Mat44 inv = m.Inverse();
    EXPECT_TRUE((m * inv).NearlyEquals(Mat44::Identity()));
}

TEST(Matrix4, TranslateAndScale)
{
    Mat44 m = Mat44::Identity();
    Mat44 t = m.Translate(Vector<3>(1, 2, 3));

    Vector<4> p(0, 0, 0, 1);
    Vector<4> tp = t * p;
    EXPECT_FLOAT_EQ(tp.x, 1.0f);
    EXPECT_FLOAT_EQ(tp.y, 2.0f);
    EXPECT_FLOAT_EQ(tp.z, 3.0f);
    EXPECT_FLOAT_EQ(tp.w, 1.0f);

    Mat44 s = m.Scale(Vector<3>(2, 3, 4));
    Vector<4> v(1, 1, 1, 1);
    Vector<4> sv = s * v;
    EXPECT_FLOAT_EQ(sv.x, 2.0f);
    EXPECT_FLOAT_EQ(sv.y, 3.0f);
    EXPECT_FLOAT_EQ(sv.z, 4.0f);
    EXPECT_FLOAT_EQ(sv.w, 1.0f);
}

TEST(Matrix4, ToMatrix3)
{
    Mat44 m = Mat44::Identity();
    Mat33 m3 = m.ToMatrix3();
    EXPECT_TRUE(m3.NearlyEquals(Mat33::Identity()));
}

TEST(Matrix4, Orthographic)
{
    Mat44 ortho = Mat44::Orthographic(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    Vector<4> nearPt(-1.0f, -1.0f, -0.1f, 1.0f);
    Vector<4> r = ortho * nearPt;
    EXPECT_NEAR(r.x, -1.0f, 1e-5f);
    EXPECT_NEAR(r.y, -1.0f, 1e-5f);
    EXPECT_NEAR(r.z, -1.0f, 1e-5f);
}

TEST(Matrix4, Perspective)
{
    Mat44 proj = Mat44::Perspective(PI / 4.0f, 1.0f, 0.1f, 100.0f);

    Vector<4> pNear(0.0f, 0.0f, -0.1f, 1.0f);
    Vector<4> clipNear = proj * pNear;
    EXPECT_NEAR(clipNear.z / clipNear.w, -1.0f, 1e-5f);

    Vector<4> pFar(0.0f, 0.0f, -100.0f, 1.0f);
    Vector<4> clipFar = proj * pFar;
    EXPECT_NEAR(clipFar.z / clipFar.w, 1.0f, 1e-5f);
}

TEST(Matrix4, LookAt)
{
    Mat44 view = Mat44::LookAt(Vector<3>(0, 0, 5), Vector<3>(0, 0, 0), Vector<3>(0, 1, 0));
    Vector<4> origin(0, 0, 0, 1);
    Vector<4> vOrigin = view * origin;
    EXPECT_NEAR(vOrigin.x, 0.0f, 1e-5f);
    EXPECT_NEAR(vOrigin.y, 0.0f, 1e-5f);
    EXPECT_NEAR(vOrigin.z, -5.0f, 1e-5f);
    EXPECT_NEAR(vOrigin.w, 1.0f, 1e-5f);
}

TEST(Matrix4, RotationsApproxIdentityAtZero)
{
    Mat44 m = Mat44::Identity();
    EXPECT_TRUE(m.RotateX(0.0f).NearlyEquals(m));
    EXPECT_TRUE(m.RotateY(0.0f).NearlyEquals(m));
    EXPECT_TRUE(m.RotateZ(0.0f).NearlyEquals(m));
    EXPECT_TRUE(m.RotateAroundAxis(Vector<3>(0, 0, 1), 0.0f).NearlyEquals(m));
}

} // namespace Ivy::UnitTests
