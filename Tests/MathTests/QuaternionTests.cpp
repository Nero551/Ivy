#include "Math/Include.hpp"
#include <cmath>
#include <gtest/gtest.h>

namespace Ivy::UnitTests
{
using namespace Ivy::M;

TEST(Quaternion, DefaultConstruction)
{
    Quaternion<> q;
    EXPECT_FLOAT_EQ(q.w, 0.0f);
    EXPECT_FLOAT_EQ(q.x, 0.0f);
    EXPECT_FLOAT_EQ(q.y, 0.0f);
    EXPECT_FLOAT_EQ(q.z, 0.0f);
}

TEST(Quaternion, ValueConstruction)
{
    Quaternion<> q(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_FLOAT_EQ(q.w, 1.0f);
    EXPECT_FLOAT_EQ(q.x, 2.0f);
    EXPECT_FLOAT_EQ(q.y, 3.0f);
    EXPECT_FLOAT_EQ(q.z, 4.0f);
}

TEST(Quaternion, Identity)
{
    Quaternion<> id = Quaternion<>::Identity();
    EXPECT_FLOAT_EQ(id.w, 1.0f);
    EXPECT_FLOAT_EQ(id.x, 0.0f);
    EXPECT_FLOAT_EQ(id.y, 0.0f);
    EXPECT_FLOAT_EQ(id.z, 0.0f);
}

TEST(Quaternion, IndexAccess)
{
    Quaternion<> q(1.0f, 2.0f, 3.0f, 4.0f);
    // Index order is x, y, z, w
    EXPECT_FLOAT_EQ(q(0), 2.0f);
    EXPECT_FLOAT_EQ(q(1), 3.0f);
    EXPECT_FLOAT_EQ(q(2), 4.0f);
    EXPECT_FLOAT_EQ(q(3), 1.0f);
}

TEST(Quaternion, MagnitudeAndConjugate)
{
    Quaternion<> q(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_FLOAT_EQ(q.MagnitudeSquared(), 30.0f);
    EXPECT_FLOAT_EQ(q.Magnitude(), std::sqrt(30.0f));

    Quaternion<> c = q.Conjugate();
    EXPECT_FLOAT_EQ(c.w, 1.0f);
    EXPECT_FLOAT_EQ(c.x, -2.0f);
    EXPECT_FLOAT_EQ(c.y, -3.0f);
    EXPECT_FLOAT_EQ(c.z, -4.0f);
}

TEST(Quaternion, InverseAndNormalized)
{
    Quaternion<> q(1.0f, 2.0f, 3.0f, 4.0f);
    Quaternion<> inv = q.Inverse();
    Quaternion<> prod = q * inv;
    EXPECT_TRUE(prod.NearlyEquals(Quaternion<>::Identity()));

    Quaternion<> n = q.Normalized();
    EXPECT_NEAR(n.Magnitude(), 1.0f, 1e-6f);
}

TEST(Quaternion, MultiplicationNonCommutative)
{
    Quaternion<> a(1.0f, 2.0f, 3.0f, 4.0f);
    Quaternion<> b(5.0f, 6.0f, 7.0f, 8.0f);
    Quaternion<> ab = a * b;
    Quaternion<> ba = b * a;
    EXPECT_FALSE(ab.NearlyEquals(ba));

    // Verify one product explicitly: a*b for a=(1,2,3,4), b=(5,6,7,8)
    EXPECT_FLOAT_EQ(ab.w, -60.0f);
    EXPECT_FLOAT_EQ(ab.x, 12.0f);
    EXPECT_FLOAT_EQ(ab.y, 30.0f);
    EXPECT_FLOAT_EQ(ab.z, 24.0f);
}

TEST(Quaternion, IdentityMultiplication)
{
    Quaternion<> q(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_TRUE((q * Quaternion<>::Identity()).NearlyEquals(q));
    EXPECT_TRUE((Quaternion<>::Identity() * q).NearlyEquals(q));
}

TEST(Quaternion, DotProduct)
{
    Quaternion<> a(1.0f, 2.0f, 3.0f, 4.0f);
    Quaternion<> b(5.0f, 6.0f, 7.0f, 8.0f);
    EXPECT_FLOAT_EQ(a.Dot(b), 70.0f);
}

TEST(Quaternion, ScalarArithmetic)
{
    Quaternion<> q(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(q * 2.0f, Quaternion<>(2.0f, 4.0f, 6.0f, 8.0f));
    EXPECT_EQ(2.0f * q, Quaternion<>(2.0f, 4.0f, 6.0f, 8.0f));
    EXPECT_EQ(q / 2.0f, Quaternion<>(0.5f, 1.0f, 1.5f, 2.0f));

    // Adding scalar modifies real part only
    EXPECT_EQ(q + 1.0f, Quaternion<>(2.0f, 2.0f, 3.0f, 4.0f));
    EXPECT_EQ(q - 1.0f, Quaternion<>(0.0f, 2.0f, 3.0f, 4.0f));
    EXPECT_EQ(1.0f + q, Quaternion<>(2.0f, 2.0f, 3.0f, 4.0f));
    EXPECT_EQ(1.0f - q, Quaternion<>(0.0f, -2.0f, -3.0f, -4.0f));
}

TEST(Quaternion, VecAddSub)
{
    Quaternion<> a(1.0f, 2.0f, 3.0f, 4.0f);
    Quaternion<> b(5.0f, 6.0f, 7.0f, 8.0f);
    EXPECT_EQ(a + b, Quaternion<>(6.0f, 8.0f, 10.0f, 12.0f));
    EXPECT_EQ(a - b, Quaternion<>(-4.0f, -4.0f, -4.0f, -4.0f));
    EXPECT_EQ(-a, Quaternion<>(-1.0f, -2.0f, -3.0f, -4.0f));
}

TEST(Quaternion, AngleAndAxis)
{
    // Rotation by π/2 around Z (full-angle quaternion)
    QPolar<float> qp(Vector<3>(0, 0, 1), PI / 2.0f, 1.0f);
    Quaternion<> q = Quaternion<>::FromQPolar(qp);
    EXPECT_NEAR(q.Angle(), PI / 2.0f, 1e-6f);
    Vector<3> axis = q.Axis();
    EXPECT_NEAR(axis.x, 0.0f, 1e-6f);
    EXPECT_NEAR(axis.y, 0.0f, 1e-6f);
    EXPECT_NEAR(axis.z, 1.0f, 1e-6f);
}

TEST(Quaternion, IdentityAxisFallback)
{
    // Identity quaternion: Angle = 0, sin(0) = 0 → default axis (0,0,-1)
    Quaternion<> id = Quaternion<>::Identity();
    EXPECT_NEAR(id.Angle(), 0.0f, 1e-6f);
    Vector<3> axis = id.Axis();
    EXPECT_FLOAT_EQ(axis.x, 0.0f);
    EXPECT_FLOAT_EQ(axis.y, 0.0f);
    EXPECT_FLOAT_EQ(axis.z, -1.0f);
}

TEST(Quaternion, QPolarRoundTrip)
{
    Vector<3> axisInput(1, 1, 0);
    QPolar<float> qp(axisInput, PI / 4.0f, 2.0f);
    // QPolar ctor normalizes axis
    EXPECT_NEAR(qp.Axis.Length(), 1.0f, 1e-6f);

    Quaternion<> q = Quaternion<>::FromQPolar(qp);
    QPolar<float> back = q.ToQPolar();
    EXPECT_NEAR(back.Angle, PI / 4.0f, 1e-5f);
    EXPECT_NEAR(back.Magnitude, 2.0f, 1e-5f);
    EXPECT_TRUE(back.Axis.NearlyEquals(qp.Axis, 1e-5f));
}

TEST(Quaternion, TransformVector)
{
    // Identity → unchanged
    Vector<3> v(1.0f, 2.0f, 3.0f);
    EXPECT_TRUE(Quaternion<>::Identity().Transform(v).NearlyEquals(v));

    // π/2 rotation around Z sends (1,0,0) to (0,1,0)
    Quaternion<> q = Quaternion<>::FromQPolar(QPolar<float>(Vector<3>(0, 0, 1), PI / 2.0f));
    Vector<3> x(1.0f, 0.0f, 0.0f);
    Vector<3> rx = q.Transform(x);
    EXPECT_NEAR(rx.x, 0.0f, 1e-5f);
    EXPECT_NEAR(rx.y, 1.0f, 1e-5f);
    EXPECT_NEAR(rx.z, 0.0f, 1e-5f);
}

TEST(Quaternion, FromToSimple)
{
    // Same direction → identity
    EXPECT_TRUE(
        Quaternion<>::FromTo(Vector<3>(1, 0, 0), Vector<3>(1, 0, 0)).NearlyEquals(Quaternion<>::Identity()));

    // (1,0,0) → (0,1,0) is π/2 rotation around Z
    Quaternion<> q = Quaternion<>::FromTo(Vector<3>(1, 0, 0), Vector<3>(0, 1, 0));
    Vector<3> transformed = q.Transform(Vector<3>(1, 0, 0));
    EXPECT_NEAR(transformed.x, 0.0f, 1e-4f);
    EXPECT_NEAR(transformed.y, 1.0f, 1e-4f);
    EXPECT_NEAR(transformed.z, 0.0f, 1e-4f);
}

TEST(Quaternion, FromToAntiParallel)
{
    Quaternion<> q = Quaternion<>::FromTo(Vector<3>(1, 0, 0), Vector<3>(-1, 0, 0));
    Vector<3> t = q.Transform(Vector<3>(1, 0, 0));
    EXPECT_NEAR(t.x, -1.0f, 1e-3f);
    EXPECT_NEAR(t.y, 0.0f, 1e-3f);
    EXPECT_NEAR(t.z, 0.0f, 1e-3f);
}

TEST(Quaternion, EulerRoundTrip)
{
    Vector<3> euler(PI / 6.0f, PI / 4.0f, PI / 3.0f);
    Quaternion<> q = Quaternion<>::FromEulerXYZ(euler);
    Vector<3> back = q.ToEulerXYZ();
    EXPECT_NEAR(back.x, euler.x, 1e-3f);
    EXPECT_NEAR(back.y, euler.y, 1e-3f);
    EXPECT_NEAR(back.z, euler.z, 1e-3f);
}

TEST(Quaternion, Matrix3RoundTrip)
{
    Matrix<3, 3> rot = Matrix<3, 3>::Identity().RotateZ(PI / 2.0f);
    Quaternion<> q = Quaternion<>::FromMatrix3(rot);
    Matrix<3, 3> back = q.ToMatrix4().ToMatrix3();
    EXPECT_TRUE(back.NearlyEquals(rot, 1e-4f));
}

TEST(Quaternion, FromMatrix3Identity)
{
    Quaternion<> q = Quaternion<>::FromMatrix3(Matrix<3, 3>::Identity());
    EXPECT_TRUE(q.NearlyEquals(Quaternion<>::Identity(), 1e-5f));
}

TEST(Quaternion, ExpLnPureReal)
{
    // Exp of pure real quaternion
    Quaternion<> e1 = Exp(Quaternion<>(1.0f, 0.0f, 0.0f, 0.0f));
    EXPECT_NEAR(e1.w, E, 1e-5f);
    EXPECT_NEAR(e1.x, 0.0f, 1e-5f);

    // Exp of pure imaginary (πi) → -1
    Quaternion<> negOne = Exp(Quaternion<>(0.0f, PI, 0.0f, 0.0f));
    EXPECT_NEAR(negOne.w, -1.0f, 1e-5f);

    // Ln of e
    Quaternion<> lg = Ln(Quaternion<>(E, 0.0f, 0.0f, 0.0f));
    EXPECT_NEAR(lg.w, 1.0f, 1e-5f);
    EXPECT_NEAR(lg.x, 0.0f, 1e-5f);
}

TEST(Quaternion, ExpLnRoundTrip)
{
    Quaternion<> q(1.0f, 0.5f, 0.3f, 0.2f);
    Quaternion<> roundTrip = Exp(Ln(q));
    EXPECT_TRUE(q.NearlyEquals(roundTrip, 1e-4f));
}

TEST(Quaternion, PowOfSelfIsSquare)
{
    Quaternion<> q(1.0f, 2.0f, 3.0f, 4.0f);
    Quaternion<> squared = Pow(q, 2.0f);
    Quaternion<> mult = q * q;
    EXPECT_TRUE(squared.NearlyEquals(mult, 1e-4f));
}

TEST(Quaternion, SqrtSquaredIsInput)
{
    Quaternion<> q(2.0f, 1.0f, 1.0f, 1.0f);
    Quaternion<> s = Sqrt(q);
    Quaternion<> sq = s * s;
    EXPECT_TRUE(sq.NearlyEquals(q, 1e-4f));
}

TEST(Quaternion, PowRealToQuaternion)
{
    // e^0 = 1 (real→quaternion), with q zero quaternion → not tested (degenerate)
    // e^1 = e, with q = (1,0,0,0)
    Quaternion<> qOne(1.0f, 0.0f, 0.0f, 0.0f);
    Quaternion<> r = Pow(E, qOne);
    EXPECT_NEAR(r.w, E, 1e-4f);
}

TEST(Quaternion, EqualityAndNearlyEquals)
{
    Quaternion<> a(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(a, Quaternion<>(1.0f, 2.0f, 3.0f, 4.0f));
    EXPECT_NE(a, Quaternion<>(1.0f, 2.0f, 3.0f, 5.0f));

    Quaternion<> b(1.0f + 1e-6f, 2.0f, 3.0f, 4.0f);
    EXPECT_TRUE(a.NearlyEquals(b));
    EXPECT_FALSE(a.NearlyEquals(b, 1e-7f));
}

} // namespace Ivy::UnitTests
