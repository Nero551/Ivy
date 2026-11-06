#include "Math/Include.hpp"
#include <cmath>
#include <gtest/gtest.h>

namespace Ivy::UnitTests
{
using namespace Ivy::M;

// ==================== Vector<2> ====================
TEST(Vector2, DefaultAndValueConstruction)
{
    Vector<2> zero;
    EXPECT_FLOAT_EQ(zero.x, 0.0f);
    EXPECT_FLOAT_EQ(zero.y, 0.0f);

    Vector<2> filled(3.0f);
    EXPECT_FLOAT_EQ(filled.x, 3.0f);
    EXPECT_FLOAT_EQ(filled.y, 3.0f);

    Vector<2> v(1.5f, -2.5f);
    EXPECT_FLOAT_EQ(v.x, 1.5f);
    EXPECT_FLOAT_EQ(v.y, -2.5f);
}

TEST(Vector2, IndexAccess)
{
    Vector<2> v(7.0f, 9.0f);
    EXPECT_FLOAT_EQ(v(0), 7.0f);
    EXPECT_FLOAT_EQ(v(1), 9.0f);

    const Vector<2> cv(1.0f, 2.0f);
    EXPECT_FLOAT_EQ(cv(0), 1.0f);
    EXPECT_FLOAT_EQ(cv(1), 2.0f);
}

TEST(Vector2, ZeroAndOne)
{
    EXPECT_EQ(Vector<2>::Zero(), Vector<2>(0.0f, 0.0f));
    EXPECT_EQ(Vector<2>::One(), Vector<2>(1.0f, 1.0f));
}

TEST(Vector2, LengthAndNormalized)
{
    Vector<2> v(3.0f, 4.0f);
    EXPECT_FLOAT_EQ(v.LengthSquared(), 25.0f);
    EXPECT_FLOAT_EQ(v.Length(), 5.0f);

    Vector<2> n = v.Normalized();
    EXPECT_NEAR(n.Length(), 1.0f, 1e-6f);
    EXPECT_NEAR(n.x, 0.6f, 1e-6f);
    EXPECT_NEAR(n.y, 0.8f, 1e-6f);

    // Zero vector guards
    Vector<2> zero;
    EXPECT_EQ(zero.Normalized(), Vector<2>::Zero());
}

TEST(Vector2, AngleAndPolarRoundTrip)
{
    Polar<float> p(PI / 3.0f, 1.5f);
    Vector<2> v = Vector<2>::FromPolar(p);
    EXPECT_NEAR(v.x, 1.5f * 0.5f, 1e-6f);
    EXPECT_NEAR(v.y, 1.5f * std::sqrt(3.0f) / 2.0f, 1e-6f);

    Polar<float> p2 = v.ToPolar();
    EXPECT_NEAR(p2.Angle, PI / 3.0f, 1e-6f);
    EXPECT_NEAR(p2.Magnitude, 1.5f, 1e-6f);

    // Angle of (1,1) is π/4
    EXPECT_NEAR(Vector<2>(1.0f, 1.0f).Angle(), PI / 4.0f, 1e-6f);
}

TEST(Vector2, DotAndDistance)
{
    Vector<2> a(1.0f, 2.0f);
    Vector<2> b(3.0f, 4.0f);
    EXPECT_FLOAT_EQ(a.Dot(b), 11.0f);
    EXPECT_FLOAT_EQ(a.Distance(b), std::sqrt(8.0f));

    // Orthogonal
    EXPECT_FLOAT_EQ(Vector<2>(1.0f, 0.0f).Dot(Vector<2>(0.0f, 1.0f)), 0.0f);
}

TEST(Vector2, LerpAndBoundary)
{
    Vector<2> a(0.0f, 0.0f);
    Vector<2> b(10.0f, 20.0f);
    EXPECT_EQ(a.Lerp(b, 0.0f), a);
    EXPECT_EQ(a.Lerp(b, 1.0f), b);
    EXPECT_EQ(a.Lerp(b, 0.5f), Vector<2>(5.0f, 10.0f));
    // Extrapolation
    EXPECT_EQ(a.Lerp(b, 2.0f), Vector<2>(20.0f, 40.0f));
}

TEST(Vector2, ParallelAndPerpendicular)
{
    Vector<2> a(1.0f, 0.0f);
    EXPECT_TRUE(a.IsParallelTo(Vector<2>(2.0f, 0.0f)));
    EXPECT_TRUE(a.IsPerpendicularTo(Vector<2>(0.0f, 5.0f)));
    EXPECT_FALSE(a.IsParallelTo(Vector<2>(0.0f, 1.0f)));
    EXPECT_FALSE(a.IsPerpendicularTo(Vector<2>(1.0f, 0.0f)));

    // Zero vectors are never parallel/perpendicular
    Vector<2> zero;
    EXPECT_FALSE(zero.IsParallelTo(a));
    EXPECT_FALSE(zero.IsPerpendicularTo(a));
}

TEST(Vector2, StereoProject)
{
    // For v=(0,0), r=0, result = 0*0/(0-0) — NaN, skip.
    // For v=(1,0), r=1, result = 1*1/(1-0) = 1
    EXPECT_FLOAT_EQ(Vector<2>(1.0f, 0.0f).StereoProject(), 1.0f);
    // For v=(0,1), r=1, result = 1*0/(1-1) — div by zero; skip.
    // Use v=(3,4), r=5, result = 5*3/(5-4) = 15
    EXPECT_FLOAT_EQ(Vector<2>(3.0f, 4.0f).StereoProject(), 15.0f);
}

TEST(Vector2, ArithmeticVectorToVector)
{
    Vector<2> a(1.0f, 2.0f);
    Vector<2> b(3.0f, 4.0f);
    EXPECT_EQ(a + b, Vector<2>(4.0f, 6.0f));
    EXPECT_EQ(a - b, Vector<2>(-2.0f, -2.0f));
    EXPECT_EQ(a * b, Vector<2>(3.0f, 8.0f));
    // NOTE: Vector<2> has no vector-vector division operator.
}

TEST(Vector2, ArithmeticScalar)
{
    Vector<2> a(1.0f, 2.0f);
    EXPECT_EQ(a + 1.0f, Vector<2>(2.0f, 3.0f));
    EXPECT_EQ(a - 1.0f, Vector<2>(0.0f, 1.0f));
    EXPECT_EQ(a * 2.0f, Vector<2>(2.0f, 4.0f));
    EXPECT_EQ(a / 2.0f, Vector<2>(0.5f, 1.0f));
    EXPECT_EQ(1.0f + a, Vector<2>(2.0f, 3.0f));
    EXPECT_EQ(1.0f - a, Vector<2>(0.0f, -1.0f));
    EXPECT_EQ(2.0f * a, Vector<2>(2.0f, 4.0f));
    EXPECT_EQ(2.0f / a, Vector<2>(2.0f, 1.0f));
}

TEST(Vector2, CompoundAssignment)
{
    Vector<2> a(1.0f, 2.0f);
    Vector<2> b(3.0f, 4.0f);

    Vector<2> t = a;
    t += b;
    EXPECT_EQ(t, Vector<2>(4.0f, 6.0f));
    t = a;
    t -= b;
    EXPECT_EQ(t, Vector<2>(-2.0f, -2.0f));
    t = a;
    t *= b;
    EXPECT_EQ(t, Vector<2>(3.0f, 8.0f));

    t = a;
    t += 1.0f;
    EXPECT_EQ(t, Vector<2>(2.0f, 3.0f));
    t = a;
    t -= 1.0f;
    EXPECT_EQ(t, Vector<2>(0.0f, 1.0f));
    t = a;
    t *= 2.0f;
    EXPECT_EQ(t, Vector<2>(2.0f, 4.0f));
    t = a;
    t /= 2.0f;
    EXPECT_EQ(t, Vector<2>(0.5f, 1.0f));
}

TEST(Vector2, EqualityAndNearlyEquals)
{
    Vector<2> a(1.0f, 2.0f);
    Vector<2> b(1.0f, 2.0f);
    Vector<2> c(1.0f, 2.1f);

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
    EXPECT_TRUE(a != c);

    Vector<2> d(1.0f + 1e-6f, 2.0f);
    EXPECT_TRUE(a.NearlyEquals(d));
    EXPECT_FALSE(a.NearlyEquals(d, 1e-7f));
    // Boundary: `NearlyEquals` uses `<=` so diff == epsilon still passes.
    // NOTE: `1.0f + EPSILON` rounds ABOVE EPSILON in float (~1.001e-5),
    //       so we must anchor at 0.0f where EPSILON is exactly representable.
    Vector<2> zero2(0.0f, 0.0f);
    Vector<2> atEps(EPSILON, 0.0f);
    EXPECT_TRUE(zero2.NearlyEquals(atEps));
}

TEST(Vector2, Negation)
{
    Vector<2> a(1.0f, -2.0f);
    EXPECT_EQ(-a, Vector<2>(-1.0f, 2.0f));
}

// ==================== Vector<3> ====================
TEST(Vector3, ConstructAndStatic)
{
    Vector<3> zero;
    EXPECT_EQ(zero, Vector<3>(0.0f));
    EXPECT_EQ(Vector<3>::Up(), Vector<3>(0.0f, 1.0f, 0.0f));
    EXPECT_EQ(Vector<3>::Right(), Vector<3>(1.0f, 0.0f, 0.0f));
    EXPECT_EQ(Vector<3>::Forward(), Vector<3>(0.0f, 0.0f, -1.0f));
}

TEST(Vector3, CrossProductIdentities)
{
    // Right × Up = Forward (with -Z convention, (1,0,0)×(0,1,0) = (0,0,1))
    Vector<3> c = Vector<3>::Right().Cross(Vector<3>::Up());
    EXPECT_EQ(c, Vector<3>(0.0f, 0.0f, 1.0f));

    // Anti-commutative
    Vector<3> a(1.0f, 2.0f, 3.0f);
    Vector<3> b(4.0f, 5.0f, 6.0f);
    Vector<3> ab = a.Cross(b);
    Vector<3> ba = b.Cross(a);
    EXPECT_TRUE(ab.NearlyEquals(-ba));

    // Parallel → zero
    EXPECT_TRUE(a.Cross(a * 3.0f).NearlyEquals(Vector<3>(0.0f)));
}

TEST(Vector3, SphericalRoundTrip)
{
    Spherical<float> s(PI / 6.0f, PI / 3.0f, 2.0f);
    Vector<3> v = Vector<3>::FromSpherical(s);
    Spherical<float> s2 = v.ToSpherical();
    EXPECT_NEAR(s.Elevation, s2.Elevation, 1e-5f);
    EXPECT_NEAR(s.Azimuth, s2.Azimuth, 1e-5f);
    EXPECT_NEAR(s.Magnitude, s2.Magnitude, 1e-5f);
}

TEST(Vector3, ElevationAzimuthForKnownVectors)
{
    // (0, 1, 0): elevation = π/2, azimuth = 0
    Vector<3> up(0.0f, 1.0f, 0.0f);
    EXPECT_NEAR(up.Elevation(), PI / 2.0f, 1e-6f);
    EXPECT_NEAR(up.Azimuth(), 0.0f, 1e-6f);

    // (1, 0, 0): elevation = 0, azimuth = 0
    Vector<3> right(1.0f, 0.0f, 0.0f);
    EXPECT_NEAR(right.Elevation(), 0.0f, 1e-6f);
    EXPECT_NEAR(right.Azimuth(), 0.0f, 1e-6f);

    // (0, 0, 1): elevation = 0, azimuth = π/2
    Vector<3> z(0.0f, 0.0f, 1.0f);
    EXPECT_NEAR(z.Elevation(), 0.0f, 1e-6f);
    EXPECT_NEAR(z.Azimuth(), PI / 2.0f, 1e-6f);
}

TEST(Vector3, StereoProject2D)
{
    EXPECT_EQ(Vector<3>(1.0f, 0.0f, 0.0f).StereoProject(), Vector<2>(1.0f, 0.0f));
    Vector<2> p = Vector<3>(0.0f, 0.0f, -1.0f).StereoProject();
    EXPECT_NEAR(p.x, 0.0f, 1e-6f);
    EXPECT_NEAR(p.y, 0.0f, 1e-6f);
}

TEST(Vector3, VectorDivisionAvailable)
{
    // Vector<3> is the only specialization with vector-vector division.
    Vector<3> a(4.0f, 9.0f, 16.0f);
    Vector<3> b(2.0f, 3.0f, 4.0f);
    EXPECT_EQ(a / b, Vector<3>(2.0f, 3.0f, 4.0f));

    Vector<3> t = a;
    t /= b;
    EXPECT_EQ(t, Vector<3>(2.0f, 3.0f, 4.0f));
}

TEST(Vector3, AllOperators)
{
    Vector<3> a(1.0f, 2.0f, 3.0f);
    Vector<3> b(4.0f, 5.0f, 6.0f);
    EXPECT_EQ(a + b, Vector<3>(5.0f, 7.0f, 9.0f));
    EXPECT_EQ(a - b, Vector<3>(-3.0f, -3.0f, -3.0f));
    EXPECT_EQ(a * b, Vector<3>(4.0f, 10.0f, 18.0f));
    EXPECT_EQ(a / b, Vector<3>(0.25f, 0.4f, 0.5f));

    EXPECT_EQ(a + 1.0f, Vector<3>(2.0f, 3.0f, 4.0f));
    EXPECT_EQ(2.0f * a, Vector<3>(2.0f, 4.0f, 6.0f));
}

// ==================== Vector<4> ====================
TEST(Vector4, ConstructAndStatic)
{
    Vector<4> v(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_FLOAT_EQ(v.x, 1.0f);
    EXPECT_FLOAT_EQ(v.y, 2.0f);
    EXPECT_FLOAT_EQ(v.z, 3.0f);
    EXPECT_FLOAT_EQ(v.w, 4.0f);
    EXPECT_EQ(Vector<4>::Zero(), Vector<4>(0.0f));
    EXPECT_EQ(Vector<4>::One(), Vector<4>(1.0f));
}

TEST(Vector4, IndexAccess)
{
    Vector<4> v(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_FLOAT_EQ(v(0), 1.0f);
    EXPECT_FLOAT_EQ(v(1), 2.0f);
    EXPECT_FLOAT_EQ(v(2), 3.0f);
    EXPECT_FLOAT_EQ(v(3), 4.0f);
}

TEST(Vector4, LengthAndNormalized)
{
    Vector<4> v(1.0f, 2.0f, 2.0f, 4.0f);
    EXPECT_FLOAT_EQ(v.LengthSquared(), 25.0f);
    EXPECT_FLOAT_EQ(v.Length(), 5.0f);

    Vector<4> n = v.Normalized();
    EXPECT_NEAR(n.Length(), 1.0f, 1e-6f);

    Vector<4> zero;
    EXPECT_EQ(zero.Normalized(), Vector<4>::Zero());
}

TEST(Vector4, DotAndLerp)
{
    Vector<4> a(1.0f, 2.0f, 3.0f, 4.0f);
    Vector<4> b(5.0f, 6.0f, 7.0f, 8.0f);
    EXPECT_FLOAT_EQ(a.Dot(b), 70.0f);
    EXPECT_EQ(a.Lerp(b, 0.5f), Vector<4>(3.0f, 4.0f, 5.0f, 6.0f));
    EXPECT_EQ(a.Lerp(b, 0.0f), a);
    EXPECT_EQ(a.Lerp(b, 1.0f), b);
}

TEST(Vector4, HyperSphericalRoundTrip)
{
    HyperSpherical<float> hs(PI / 4.0f, PI / 3.0f, PI / 6.0f, 2.0f);
    Vector<4> v = Vector<4>::FromHyperSpherical(hs);
    HyperSpherical<float> hs2 = v.ToHyperSpherical();
    EXPECT_NEAR(hs.Elevation, hs2.Elevation, 1e-5f);
    EXPECT_NEAR(hs.Azimuth, hs2.Azimuth, 1e-5f);
    EXPECT_NEAR(hs.HyperAngle, hs2.HyperAngle, 1e-5f);
    EXPECT_NEAR(hs.Magnitude, hs2.Magnitude, 1e-5f);
}

TEST(Vector4, StereoProject3D)
{
    Vector<3> p = Vector<4>(1.0f, 0.0f, 0.0f, 0.0f).StereoProject();
    EXPECT_NEAR(p.x, 1.0f, 1e-6f);
    EXPECT_NEAR(p.y, 0.0f, 1e-6f);
    EXPECT_NEAR(p.z, 0.0f, 1e-6f);
}

TEST(Vector4, AllOperators)
{
    Vector<4> a(1.0f, 2.0f, 3.0f, 4.0f);
    Vector<4> b(5.0f, 6.0f, 7.0f, 8.0f);
    EXPECT_EQ(a + b, Vector<4>(6.0f, 8.0f, 10.0f, 12.0f));
    EXPECT_EQ(a - b, Vector<4>(-4.0f, -4.0f, -4.0f, -4.0f));
    EXPECT_EQ(a * b, Vector<4>(5.0f, 12.0f, 21.0f, 32.0f));
    // NOTE: Vector<4> has no vector-vector division.

    EXPECT_EQ(a * 2.0f, Vector<4>(2.0f, 4.0f, 6.0f, 8.0f));
    EXPECT_EQ(2.0f * a, Vector<4>(2.0f, 4.0f, 6.0f, 8.0f));
}

// ==================== Generic Vector<N> ====================
TEST(VectorGeneric, ConstructionAndData)
{
    Vector<5> v(1.0f, 2.0f, 3.0f, 4.0f, 5.0f);
    EXPECT_FLOAT_EQ(v(0), 1.0f);
    EXPECT_FLOAT_EQ(v(4), 5.0f);
    EXPECT_EQ(v.Data().size(), 5u);
    EXPECT_FLOAT_EQ(v.Data()[2], 3.0f);
}

TEST(VectorGeneric, LengthAndNormalization)
{
    Vector<5> v(1.0f, 2.0f, 3.0f, 4.0f, 5.0f);
    EXPECT_FLOAT_EQ(v.LengthSquared(), 55.0f);
    EXPECT_NEAR(v.Length(), std::sqrt(55.0f), 1e-6f);

    Vector<5> n = v.Normalized();
    EXPECT_NEAR(n.Length(), 1.0f, 1e-6f);

    Vector<5> zero;
    EXPECT_EQ(zero.Normalized(), Vector<5>::Zero());
}

TEST(VectorGeneric, DotAndDistance)
{
    Vector<5> a(1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    Vector<5> b(2.0f, 2.0f, 2.0f, 2.0f, 2.0f);
    EXPECT_FLOAT_EQ(a.Dot(b), 10.0f);
    EXPECT_FLOAT_EQ(a.Distance(b), std::sqrt(5.0f));
}

TEST(VectorGeneric, StereoProject)
{
    Vector<5> v(1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    Vector<4> p = v.StereoProject();
    EXPECT_NEAR(p.x, 1.0f, 1e-6f);
    EXPECT_NEAR(p.y, 0.0f, 1e-6f);
    EXPECT_NEAR(p.z, 0.0f, 1e-6f);
    EXPECT_NEAR(p.w, 0.0f, 1e-6f);
}

TEST(VectorGeneric, AllOperators)
{
    Vector<5> a(1.0f, 2.0f, 3.0f, 4.0f, 5.0f);
    Vector<5> b(2.0f, 4.0f, 6.0f, 8.0f, 10.0f);

    Vector<5> sum = a + b;
    EXPECT_FLOAT_EQ(sum(0), 3.0f);
    EXPECT_FLOAT_EQ(sum(4), 15.0f);

    Vector<5> diff = a - b;
    EXPECT_FLOAT_EQ(diff(4), -5.0f);

    Vector<5> prod = a * b;
    EXPECT_FLOAT_EQ(prod(0), 2.0f);
    EXPECT_FLOAT_EQ(prod(4), 50.0f);

    // Vector<5> DOES have vector-vector division
    Vector<5> quot = a / b;
    EXPECT_FLOAT_EQ(quot(0), 0.5f);
    EXPECT_FLOAT_EQ(quot(4), 0.5f);

    Vector<5> scaled = a * 2.0f;
    EXPECT_FLOAT_EQ(scaled(4), 10.0f);
}

} // namespace Ivy::UnitTests
