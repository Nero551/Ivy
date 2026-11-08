#include "Math/Coordinates/HyperSpherical.hpp"
#include "Math/Coordinates/Polar.hpp"
#include "Math/Coordinates/QPolar.hpp"
#include "Math/Coordinates/Spherical.hpp"
#include <gtest/gtest.h>

namespace Ivy::UnitTests
{
using namespace Ivy::M;

TEST(Polar, ConstructionAndDefaultMagnitude)
{
    Polar<float> p(PI / 2.0f);
    EXPECT_FLOAT_EQ(p.Angle, PI / 2.0f);
    EXPECT_FLOAT_EQ(p.Magnitude, 1.0f);

    Polar<float> p2(PI / 4.0f, 2.5f);
    EXPECT_FLOAT_EQ(p2.Angle, PI / 4.0f);
    EXPECT_FLOAT_EQ(p2.Magnitude, 2.5f);
}

TEST(Polar, VectorConversions)
{
    // (0, 5) → angle π/2, magnitude 5
    Vector<2> v(0.0f, 5.0f);
    Polar<float> p = v.ToPolar();
    EXPECT_NEAR(p.Angle, PI / 2.0f, 1e-6f);
    EXPECT_NEAR(p.Magnitude, 5.0f, 1e-6f);

    Vector<2> back = Vector<2>::FromPolar(p);
    EXPECT_TRUE(back.NearlyEquals(v));
}

TEST(Spherical, Construction)
{
    Spherical<float> s(PI / 6.0f, PI / 3.0f);
    EXPECT_FLOAT_EQ(s.Elevation, PI / 6.0f);
    EXPECT_FLOAT_EQ(s.Azimuth, PI / 3.0f);
    EXPECT_FLOAT_EQ(s.Magnitude, 1.0f);
}

TEST(Spherical, KnownRoundTrip)
{
    Spherical<float> s(PI / 6.0f, PI / 3.0f, 2.0f);
    Vector<3> v = Vector<3>::FromSpherical(s);
    Spherical<float> s2 = v.ToSpherical();
    EXPECT_NEAR(s.Elevation, s2.Elevation, 1e-5f);
    EXPECT_NEAR(s.Azimuth, s2.Azimuth, 1e-5f);
    EXPECT_NEAR(s.Magnitude, s2.Magnitude, 1e-5f);
}

TEST(Spherical, PoleAndEquator)
{
    // North pole: elevation = π/2
    Spherical<float> north(PI / 2.0f, 0.0f, 1.0f);
    Vector<3> v = Vector<3>::FromSpherical(north);
    EXPECT_NEAR(v.x, 0.0f, 1e-6f);
    EXPECT_NEAR(v.y, 1.0f, 1e-6f);
    EXPECT_NEAR(v.z, 0.0f, 1e-6f);
}

TEST(HyperSpherical, Construction)
{
    HyperSpherical<float> hs(PI / 4.0f, PI / 3.0f, PI / 6.0f, 2.0f);
    EXPECT_FLOAT_EQ(hs.Elevation, PI / 4.0f);
    EXPECT_FLOAT_EQ(hs.Azimuth, PI / 3.0f);
    EXPECT_FLOAT_EQ(hs.HyperAngle, PI / 6.0f);
    EXPECT_FLOAT_EQ(hs.Magnitude, 2.0f);
}

TEST(HyperSpherical, RoundTripModerateAngles)
{
    HyperSpherical<float> hs(PI / 4.0f, PI / 3.0f, PI / 6.0f, 2.0f);
    Vector<4> v = Vector<4>::FromHyperSpherical(hs);
    HyperSpherical<float> back = v.ToHyperSpherical();
    EXPECT_NEAR(hs.Elevation, back.Elevation, 1e-5f);
    EXPECT_NEAR(hs.Azimuth, back.Azimuth, 1e-5f);
    EXPECT_NEAR(hs.HyperAngle, back.HyperAngle, 1e-5f);
    EXPECT_NEAR(hs.Magnitude, back.Magnitude, 1e-5f);
}

TEST(QPolar, ConstructionNormalizesAxis)
{
    Vector<3> unnormalized(3.0f, 4.0f, 0.0f);
    QPolar<float> qp(unnormalized, PI / 2.0f, 1.0f);
    EXPECT_NEAR(qp.Axis.Length(), 1.0f, 1e-6f);
    EXPECT_NEAR(qp.Axis.x, 0.6f, 1e-6f);
    EXPECT_NEAR(qp.Axis.y, 0.8f, 1e-6f);
}

TEST(QPolar, DefaultMagnitude)
{
    QPolar<float> qp(Vector<3>(1, 0, 0), PI / 4.0f);
    EXPECT_FLOAT_EQ(qp.Magnitude, 1.0f);
}

} // namespace Ivy::UnitTests
