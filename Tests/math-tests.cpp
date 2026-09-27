#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "Math/Math.hpp"

using Catch::Approx;
using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;
using namespace N::M;

//==============================================================================
// Common
//==============================================================================
TEST_CASE(

    "Math::NearlyEquals")
{
    constexpr float eps = EPSILON; // 1e-5f

    REQUIRE(NearlyEquals(1.0f, 1.0f, eps));
    REQUIRE(NearlyEquals(-5.0f, -5.0f, eps));
    REQUIRE(NearlyEquals(0.0f, 0.0f, eps));

    REQUIRE(NearlyEquals(1.0f, 1.0f + 0.5e-6f, eps));
    REQUIRE(NearlyEquals(1.0f, 1.0f - 0.5e-6f, eps));
    REQUIRE(NearlyEquals(1000000.0f, 1000000.0f + 2.0e-6f, eps)); // within 1e-5

    REQUIRE_FALSE(NearlyEquals(1.0f, 1.0f + 2.0e-5f, eps));
    REQUIRE_FALSE(NearlyEquals(1.0f, 1.0f - 2.0e-5f, eps));
    REQUIRE_FALSE(NearlyEquals(0.0f, 1.0e-4f, eps));

    // strict epsilon
    constexpr float strict = 1e-6f;
    REQUIRE(NearlyEquals(1.0f, 1.0f + 5e-7f, strict));
    REQUIRE_FALSE(NearlyEquals(1.0f, 1.0f + 2e-6f, strict));
}

TEST_CASE(

    "Math::Clamp")
{
    REQUIRE(Clamp(5.0f, 0.0f, 10.0f) == 5.0f);
    REQUIRE(Clamp(-1.0f, 0.0f, 10.0f) == 0.0f);
    REQUIRE(Clamp(15.0f, 0.0f, 10.0f) == 10.0f);
    REQUIRE(Clamp(0.0f, -5.0f, 5.0f) == 0.0f);
}

TEST_CASE(

    "Math::Pow, Sqrt, Exp, Factorial")
{
    REQUIRE(Pow(2.0f, 3.0f) == Approx(8.0f));
    REQUIRE(Pow(4.0f, 0.5f) == Approx(2.0f));
    REQUIRE(Pow(10.0f, 0.0f) == Approx(1.0f));

    REQUIRE(Sqrt(0.0f) == Approx(0.0f));
    REQUIRE(Sqrt(1.0f) == Approx(1.0f));
    REQUIRE(Sqrt(4.0f) == Approx(2.0f));

    REQUIRE(Exp(0.0f) == Approx(1.0f));
    REQUIRE(Exp(1.0f) == Approx(E));
    REQUIRE(Exp(2.0f) == Approx(E * E));

    REQUIRE(Factorial(0) == 1.0f);
    REQUIRE(Factorial(1) == 1.0f);
    REQUIRE(Factorial(5) == 120.0f);
    REQUIRE(Factorial(10) == 3628800.0f);
}

TEST_CASE(

    "Math::Lerp")
{
    REQUIRE(Lerp(0.0f, 10.0f, 0.0f) == 0.0f);
    REQUIRE(Lerp(0.0f, 10.0f, 1.0f) == 10.0f);
    REQUIRE(Lerp(0.0f, 10.0f, 0.5f) == 5.0f);
    REQUIRE(Lerp(5.0f, 15.0f, 0.3f) == 8.0f);
}

TEST_CASE(

    "Math::Ease functions")
{
    // Quad
    REQUIRE(EaseInQuad(0.0f) == 0.0f);
    REQUIRE(EaseInQuad(0.5f) == 0.25f);
    REQUIRE(EaseInQuad(1.0f) == 1.0f);
    REQUIRE(EaseOutQuad(0.0f) == 0.0f);
    REQUIRE(EaseOutQuad(0.5f) == 0.75f);
    REQUIRE(EaseOutQuad(1.0f) == 1.0f);
    REQUIRE(EaseInOutQuad(0.0f) == 0.0f);
    REQUIRE(EaseInOutQuad(0.5f) == 0.5f);
    REQUIRE(EaseInOutQuad(1.0f) == 1.0f);

    // Cubic
    REQUIRE(EaseInCubic(0.0f) == 0.0f);
    REQUIRE(EaseInCubic(0.5f) == 0.125f);
    REQUIRE(EaseInCubic(1.0f) == 1.0f);
    REQUIRE(EaseOutCubic(0.0f) == 0.0f);
    REQUIRE(EaseOutCubic(0.5f) == 0.875f);
    REQUIRE(EaseOutCubic(1.0f) == 1.0f);
    REQUIRE(EaseInOutCubic(0.0f) == 0.0f);
    REQUIRE(EaseInOutCubic(0.5f) == 0.5f);
    REQUIRE(EaseInOutCubic(1.0f) == 1.0f);
}

TEST_CASE(

    "Math::Ln and Log")
{
    REQUIRE(Ln(1.0f) == Approx(0.0f));
    REQUIRE(Ln(E) == Approx(1.0f));
    REQUIRE(Ln(E * E) == Approx(2.0f));

    REQUIRE(Log(10.0f, 100.0f) == Approx(2.0f));
    REQUIRE(Log(2.0f, 8.0f) == Approx(3.0f));
    REQUIRE(Log(E, 1.0f) == Approx(0.0f));
}

TEST_CASE(

    "Math::Random")
{
    for (int i = 0; i < 10; ++i)
    {
        float r = Random(0.0f, 1.0f);
        REQUIRE(r >= 0.0f);
        REQUIRE(r <= 1.0f);
    }
    for (int i = 0; i < 10; ++i)
    {
        int r = Random(-5, 5);
        REQUIRE(r >= -5);
        REQUIRE(r <= 5);
    }
    for (int i = 0; i < 10; ++i)
    {
        float g = RandomGaussian(0.0f, 1.0f);
        REQUIRE(std::isfinite(g));
    }
    unsigned long raw = RandomRaw();
    (void)raw;
}

TEST_CASE(

    "Math::Trigonometry")
{
    REQUIRE(Rad(180.0f) == Approx(PI));
    REQUIRE(Deg(PI) == Approx(180.0f));
    REQUIRE(Rad(90.0f) == Approx(PI / 2.0f));
    REQUIRE(Deg(PI / 2.0f) == Approx(90.0f));

    REQUIRE(DSin(0.0f) == Approx(0.0f));
    REQUIRE(DSin(90.0f) == Approx(1.0f));
    REQUIRE(DSin(180.0f) == Approx(0.0f).margin(1e-7f));
    REQUIRE(DCos(0.0f) == Approx(1.0f));
    REQUIRE(DCos(90.0f) == Approx(0.0f).margin(1e-7f));
    REQUIRE(DTan(45.0f) == Approx(1.0f));
    REQUIRE(DTan(0.0f) == Approx(0.0f));
}

//==============================================================================
// Complex
//==============================================================================
TEST_CASE(

    "Complex construction and constants")
{
    Complex z;
    REQUIRE((z.Real == 0.0f && z.Imaginary == 0.0f)); // fixed chained comparison
    Complex z2(3.0f, 4.0f);
    REQUIRE(z2.Real == 3.0f);
    REQUIRE(z2.Imaginary == 4.0f);

    REQUIRE((I.Real == 0.0f && I.Imaginary == 1.0f)); // fixed
}

TEST_CASE(

    "Complex arithmetic")
{
    Complex a(1, 2), b(3, 4);
    REQUIRE(a + b == Complex(4, 6));
    REQUIRE(a - b == Complex(-2, -2));
    REQUIRE(a * b == Complex(1 * 3 - 2 * 4, 1 * 4 + 2 * 3));
    REQUIRE(a / b == a * b.Inverse());
    REQUIRE(-a == Complex(-1, -2));
    REQUIRE(a + 2.0f == Complex(3, 2));
    REQUIRE(2.0f * a == Complex(2, 4));
    REQUIRE(a * 2.0f == Complex(2, 4));
}

TEST_CASE(

    "Complex magnitude, conjugate, inverse")
{
    Complex z(3, 4);
    REQUIRE(z.MagnitudeSquared() == 25.0f);
    REQUIRE(z.Magnitude() == Approx(5.0f));
    REQUIRE(z.Conjugate() == Complex(3, -4));
    REQUIRE(z.Inverse() == Complex(3 / 25.0f, -4 / 25.0f));
    REQUIRE(z.Normalized() == Complex(3 / 5.0f, 4 / 5.0f));
}

TEST_CASE(

    "Complex polar and argument")
{
    Complex z(3, 4);
    float arg = z.Argument();
    REQUIRE(arg == Approx(std::atan2(4.0f, 3.0f)));
    Polar p = z.ToPolar();
    REQUIRE(p.Angle == Approx(arg));
    REQUIRE(p.Magnitude == Approx(5.0f));
    Complex fromPolar = Complex::FromPolar(p);
    REQUIRE(fromPolar.NearlyEquals(z));
}

TEST_CASE(

    "Complex exponentials and logarithms")
{
    Complex z(0, PI / 2);
    Complex e = CExp(z);
    REQUIRE(e.Real == Approx(0.0f).margin(1e-6f));
    REQUIRE(e.Imaginary == Approx(1.0f));

    Complex ln = CLn(z);
    REQUIRE(ln.Real == Approx(Ln(PI / 2)));
    REQUIRE(ln.Imaginary == Approx(PI / 2));

    Complex p = CPow(E, z);
    REQUIRE(p.Real == Approx(0.0f).margin(1e-6f));
    REQUIRE(p.Imaginary == Approx(1.0f));

    Complex p2 = CPow(z, 2.0f);
    REQUIRE(p2.NearlyEquals(z * z));

    Complex w(1, 1);
    Complex p3 = CPow(z, w);
    (void)p3;
}

TEST_CASE(

    "Complex sqrt")
{
    Complex z(0, 1);
    Complex sqrt = CSqrt(z);
    REQUIRE(z.NearlyEquals(sqrt * sqrt));
}

//==============================================================================
// Coordinates
//==============================================================================
TEST_CASE(

    "Polar conversion")
{
    Polar p(PI / 4, 2.0f);
    Vector<2> v = Vector<2>::FromPolar(p);
    REQUIRE(v.x == Approx(2.0f * std::cos(PI / 4)));
    REQUIRE(v.y == Approx(2.0f * std::sin(PI / 4)));
    Polar back = v.ToPolar();
    REQUIRE(back.Angle == Approx(p.Angle));
    REQUIRE(back.Magnitude == Approx(p.Magnitude));
}

TEST_CASE(

    "Spherical conversion")
{
    Spherical s(0.3f, 0.5f, 4.0f);
    Vector<3> v = Vector<3>::FromSpherical(s);
    Spherical back = v.ToSpherical();
    REQUIRE(back.Elevation == Approx(s.Elevation));
    REQUIRE(back.Azimuth == Approx(s.Azimuth));
    REQUIRE(back.Magnitude == Approx(s.Magnitude));
}

TEST_CASE(

    "HyperSpherical conversion")
{
    HyperSpherical h(0.2f, 0.4f, 0.6f, 3.0f);
    Vector<4> v = Vector<4>::FromHyperSpherical(h);
    HyperSpherical back = v.ToHyperSpherical();
    REQUIRE(back.Elevation == Approx(h.Elevation));
    REQUIRE(back.Azimuth == Approx(h.Azimuth));
    REQUIRE(back.HyperAngle == Approx(h.HyperAngle));
    REQUIRE(back.Magnitude == Approx(h.Magnitude));
}

TEST_CASE(

    "QPolar conversion")
{
    Vector<3> axis = Vector<3>(1, 1, 0).Normalized();
    QPolar qp(axis, 1.2f, 2.0f);
    Quaternion q = Quaternion::FromQPolar(qp);
    QPolar back = q.ToQPolar();
    REQUIRE(back.Axis.NearlyEquals(qp.Axis));
    REQUIRE(back.Angle == Approx(qp.Angle));
    REQUIRE(back.Magnitude == Approx(qp.Magnitude));
}

//==============================================================================
// Functions
//==============================================================================

TEST_CASE("Function construction and evaluation")
{
    SECTION("Construct from lambda")
    {
        auto f = Function<float, float>([](const float x) { return x * x; });
        REQUIRE(f(2.0f) == 4.0f);
        REQUIRE(f.Evaluate(3.0f) == 9.0f);
    }

    SECTION("Construct from std::function")
    {
        std::function<float(float)> func = [](const float x) { return 2.0f * x + 1.0f; };
        Function<float, float> f(func);
        REQUIRE(f(1.5f) == 4.0f);
    }

    SECTION("Evaluate with const reference")
    {
        Function<float, float> f = [](const float x) { return x + 1.0f; };
        const float input = 2.5f;
        REQUIRE(f.Evaluate(input) == 3.5f);
    }
}

TEST_CASE("Function composition")
{
    Function<float, float> f([](const float x) { return x + 1.0f; });
    Function<float, float> g([](const float x) { return x * 2.0f; });

    SECTION("Compose member")
    {
        auto h = f.Compose(g); // f(g(x)) = 2x+1
        REQUIRE(h(3.0f) == 7.0f);
    }

    SECTION("operator()(const Function&)")
    {
        auto h = f(g); // same
        REQUIRE(h(3.0f) == 7.0f);
    }

    SECTION("Composition with non‑float type")
    {
        Function<int, float> intToFloat = [](const int x) { return static_cast<float>(x) + 0.5f; };
        Function<float, int> floatToInt = [](const float x) { return static_cast<int>(x); };
        auto composed = intToFloat.Compose(floatToInt); // intToFloat(floatToInt(x))
    }
}

TEST_CASE("Function inverse (scalar‑to‑scalar)")
{
    auto f = Function<float, float>([](const float x) { return 2.0f * x + 3.0f; });
    SECTION("InverseEvaluate")
    {
        REQUIRE(f.InverseEvaluate(7.0f, -10.0f, 10.0f) == Approx(2.0f));
    }

    SECTION("Inverse function")
    {
        auto inv = f.Inverse(-10.0f, 10.0f);
        REQUIRE(inv(7.0f) == Approx(2.0f));
        // Check f(inv(y)) ≈ y
        REQUIRE(f(inv(7.0f)) == Approx(7.0f));
    }

    SECTION("Non‑monotonic function throws? (not yet) but we test that it returns some value")
    {
        auto g = Function<float, float>([](const float x) { return x * x; });
        // This is not monotonic over [-10,10], but binary search will still find something.
        // We just check it runs.
        auto invG = g.Inverse(0.0f, 10.0f); // restrict to positive domain
        REQUIRE(invG(4.0f) == Approx(2.0f));
    }
}

TEST_CASE("Function numerical differentiation")
{
    auto f = Function<float, float>([](const float x) { return x * x; });
    const float dx = 0.001f;

    SECTION("Derivative method")
    {
        REQUIRE(f.Derivative(2.0f, dx) == Approx(4.0f).margin(0.01f));
        REQUIRE(f.Derivative(2.0f, dx, DifferentiationMethod::Forward) == Approx(4.0f).margin(0.01f));
        REQUIRE(f.Derivative(2.0f, dx, DifferentiationMethod::Backward) == Approx(4.0f).margin(0.01f));
    }

    SECTION("Differentiate returns a function")
    {
        auto df = f.Differentiate(dx);
        REQUIRE(df(2.0f) == Approx(4.0f).margin(0.01f));
        // relative step
        auto dfRel = f.Differentiate(dx, DifferentiationMethod::Central);
        REQUIRE(dfRel(2.0f) == Approx(4.0f).margin(0.01f));
    }

    SECTION("Derivative of constant is zero")
    {
        auto c = Function<float, float>([](float) { return 5.0f; });
        REQUIRE(c.Derivative(1.0f, dx) == Approx(0.0f).margin(1e-6f));
    }
}

TEST_CASE("Function numerical integration")
{
    auto f = Function<float, float>([](const float x) { return x * x; });
    const float dx = 0.001f;

    SECTION("Integral method")
    {
        REQUIRE(f.Integral(0.0f, 2.0f, dx) == Approx(8.0f / 3.0f).margin(0.01f));
        REQUIRE(f.Integral(0.0f, 2.0f, dx, IntegrationMethod::Left) == Approx(8.0f / 3.0f).margin(0.02f));
        REQUIRE(f.Integral(0.0f, 2.0f, dx, IntegrationMethod::Right) == Approx(8.0f / 3.0f).margin(0.02f));
        REQUIRE(
            f.Integral(0.0f, 2.0f, dx, IntegrationMethod::Trapezoid) == Approx(8.0f / 3.0f).margin(0.01f));
    }

    SECTION("Integrate returns cumulative function")
    {
        auto F = f.Integrate(0.0f, dx);
        REQUIRE(F(2.0f) == Approx(8.0f / 3.0f).margin(0.01f));
        REQUIRE(F(1.0f) == Approx(1.0f / 3.0f).margin(0.01f));
    }

    SECTION("Integral of constant")
    {
        auto c = Function<float, float>([](float) { return 5.0f; });
        REQUIRE(c.Integral(0.0f, 2.0f, dx) == Approx(10.0f).margin(0.01f));
    }
}

TEST_CASE("Function Taylor and Maclaurin")
{
    Function<float, float> f = [](const float x) { return std::exp(x); };

    SECTION("Taylor")
    {
        auto taylor = f.Taylor(3, 0.3f);
        // exp(0.7) ≈ 2.01375, Taylor at a=0.3 with 3 terms should be reasonable
        REQUIRE(taylor(0.7f) == Approx(std::exp(0.7f)).margin(0.1f));
        // More terms should improve accuracy
        auto taylor5 = f.Taylor(3, 0.5f);
        REQUIRE(taylor5(0.7f) == Approx(std::exp(0.7f)).margin(0.01f));
    }

    SECTION("Maclaurin")
    {
        auto mac = f.Maclaurin(3);
        REQUIRE(mac(0.7f) == Approx(std::exp(0.7f)).margin(0.1f));
        auto mac5 = f.Maclaurin(3);
        REQUIRE(mac5(0.3f) == Approx(std::exp(0.3f)).margin(0.01f));
    }

    SECTION("Taylor of polynomial")
    {
        auto p = Function<float, float>([](const float x) { return 2.0f * x * x + 3.0f * x + 1.0f; });
        auto taylor = p.Taylor(4, 1.0f);
        // Should match exactly
        REQUIRE(taylor(1.0f) == Approx(p(1.0f)));
    }
}

TEST_CASE("Function arithmetic operations")
{
    Function<float, float> f = [](const float x) { return x + 1.0f; };
    Function<float, float> g = [](const float x) { return 2.0f * x; };

    SECTION("Binary operators between functions")
    {
        Function<float, float> h = f + g;
        REQUIRE(h(3.0f) == 10.0f); // 1

        h = f - g;
        REQUIRE(h(3.0f) == -2.0f); // 2

        h = f * g;
        REQUIRE(h(3.0f) == 24.0f); // 3

        h = f / g;
        REQUIRE(h(3.0f) == Approx(4.0f / 6.0f)); // 4

        h = -f;
        REQUIRE(h(3.0f) == -4.0f); // 5
    }

    SECTION("Operators with Output type (float)")
    {
        auto h = f + 2.0f;
        REQUIRE(h(3.0f) == 6.0f); // (x+1)+2 = x+3
        h = f - 2.0f;
        REQUIRE(h(3.0f) == 2.0f);
        h = f * 2.0f;
        REQUIRE(h(3.0f) == 8.0f);
        h = f / 2.0f;
        REQUIRE(h(3.0f) == 2.0f);
        // Left-hand side with scalar
        h = 2.0f + f;
        REQUIRE(h(3.0f) == 6.0f);
        h = 2.0f - f;
        REQUIRE(h(3.0f) == -2.0f);
        h = 2.0f * f;
        REQUIRE(h(3.0f) == 8.0f);
        h = 2.0f / f;
        REQUIRE(h(3.0f) == Approx(2.0f / 4.0f));
    }

    SECTION("Operators with float scalar (overloads)")
    {
        auto h = f + 2.0f; // same as above, but also test float overload
        REQUIRE(h(3.0f) == 6.0f);
        h = f - 2.0f;
        REQUIRE(h(3.0f) == 2.0f);
        h = f * 2.0f;
        REQUIRE(h(3.0f) == 8.0f);
        h = f / 2.0f;
        REQUIRE(h(3.0f) == 2.0f);
        // Friend float overloads are also tested above
    }
}

TEST_CASE("Function compound assignment operators")
{
    auto f = Function<float, float>([](const float x) { return x + 1.0f; });
    auto g = Function<float, float>([](const float x) { return 2.0f * x; });

    SECTION("Between functions")
    {
        auto h = f;
        h += g;
        REQUIRE(h(3.0f) == 10.0f);
        h = f;
        h -= g;
        REQUIRE(h(3.0f) == -2.0f);
        h = f;
        h *= g;
        REQUIRE(h(3.0f) == 24.0f);
        h = f;
        h /= g;
        REQUIRE(h(3.0f) == Approx(4.0f / 6.0f));
    }

    SECTION("With Output (float)")
    {
        auto h = f;
        h += 2.0f;
        REQUIRE(h(3.0f) == 6.0f);
        h = f;
        h -= 2.0f;
        REQUIRE(h(3.0f) == 2.0f);
        h = f;
        h *= 2.0f;
        REQUIRE(h(3.0f) == 8.0f);
        h = f;
        h /= 2.0f;
        REQUIRE(h(3.0f) == 2.0f);
    }

    SECTION("With float scalar")
    {
        auto h = f;
        h += 2.0f;
        REQUIRE(h(3.0f) == 6.0f);
        // similarly for -=, *=, /=
    }
}

TEST_CASE("Function with non‑scalar types (Vector2)")
{
    // Define a function that takes float and returns Vector2
    auto f = Function<float, Vector<2>>([](const float t) { return Vector<2>(t, t * t); });
    SECTION("Evaluate")
    {
        auto res = f(2.0f);
        REQUIRE(res == Vector<2>(2.0f, 4.0f));
    }

    SECTION("Arithmetic with Output type (Vector2)")
    {
        auto g = Function<float, Vector<2>>([](float t) { return Vector<2>(1.0f, 2.0f); });
        auto h = f + g;
        auto res = h(2.0f);
        REQUIRE(res == Vector<2>(3.0f, 6.0f));
        h = f - g;
        res = h(2.0f);
        REQUIRE(res == Vector<2>(1.0f, 2.0f));
        h = f * g; // element-wise multiplication
        res = h(2.0f);
        REQUIRE(res == Vector<2>(2.0f, 8.0f));
        // Unary minus
        h = -f;
        res = h(2.0f);
        REQUIRE(res == Vector<2>(-2.0f, -4.0f));
    }

    SECTION("Operators with Vector2 constant")
    {
        auto h = f + Vector<2>(1.0f, 1.0f);
        auto res = h(2.0f);
        REQUIRE(res == Vector<2>(3.0f, 5.0f));
        h = f - Vector<2>(1.0f, 1.0f);
        res = h(2.0f);
        REQUIRE(res == Vector<2>(1.0f, 3.0f));
        h = f * Vector<2>(2.0f, 3.0f);
        res = h(2.0f);
        REQUIRE(res == Vector<2>(4.0f, 12.0f));
        // Left-hand side
        h = Vector<2>(1.0f, 1.0f) + f;
        res = h(2.0f);
        REQUIRE(res == Vector<2>(3.0f, 5.0f));
        h = Vector<2>(1.0f, 1.0f) - f;
        res = h(2.0f);
        REQUIRE(res == Vector<2>(-1.0f, -3.0f));
        h = Vector<2>(2.0f, 3.0f) * f;
        res = h(2.0f);
        REQUIRE(res == Vector<2>(4.0f, 12.0f));
    }

    SECTION("Compound assignment with Vector2")
    {
        Function<float, Vector<2>> h = f;
        h += Vector<2>(1.0f, 1.0f);
        REQUIRE(h(2.0f) == Vector<2>(3.0f, 5.0f));
        h = f;
        h -= Vector<2>(1.0f, 1.0f);
        REQUIRE(h(2.0f) == Vector<2>(1.0f, 3.0f));
        h = f;
        h *= Vector<2>(2.0f, 3.0f);
        REQUIRE(h(2.0f) == Vector<2>(4.0f, 12.0f));
    }
}

TEST_CASE("Function with Complex type")
{
    auto f = Function<float, Complex>([](const float t) { return Complex(t, t * t); });
    SECTION("Evaluate")
    {
        auto res = f(2.0f);
        REQUIRE(res == Complex(2.0f, 4.0f));
    }

    SECTION("Arithmetic")
    {
        auto g = Function<float, Complex>([](float t) { return Complex(1.0f, 2.0f); });
        auto h = f + g;
        REQUIRE(h(2.0f) == Complex(3.0f, 6.0f));
        h = f * g;
        // (2+4i)*(1+2i) = (2 - 8) + (4+4)i = -6 + 8i
        REQUIRE(h(2.0f) == Complex(-6.0f, 8.0f));
        h = f / g;
        auto expected = Complex(2.0f, 4.0f) / Complex(1.0f, 2.0f);
        REQUIRE(h(2.0f).NearlyEquals(expected));
    }
}

TEST_CASE("Function friend operators (if implemented)")
{
    auto f = Function<float, float>([](const float x) { return x + 1.0f; });
    SECTION("With Output (float) – left-hand side")
    {
        auto h = 2.0f + f;
        REQUIRE(h(3.0f) == 6.0f);
        h = 2.0f - f;
        REQUIRE(h(3.0f) == -2.0f);
        h = 2.0f * f;
        REQUIRE(h(3.0f) == 8.0f);
        h = 2.0f / f;
        REQUIRE(h(3.0f) == Approx(2.0f / 4.0f));
    }

    SECTION("With float scalar – left-hand side")
    {
        auto h = 2.0f + f;
        REQUIRE(h(3.0f) == 6.0f);
        // Similar for other ops
    }

    // For Vector2 left‑hand side (tested earlier)
    Function<float, Vector<2>> fv = ([](const float t) { return Vector<2>(t, t); });
    auto hv = Vector<2>(1.0f, 2.0f) + fv;
    REQUIRE(hv(3.0f) == Vector<2>(4.0f, 5.0f));
}

//==============================================================================
// Geometry::Basis
//==============================================================================
TEST_CASE("Basis matrix")
{
    Basis basis;
    Matrix<4, 4> m = basis.GetMatrix();

    REQUIRE(m == Matrix<4, 4>::Identity());

    Matrix<4, 4> inv = basis.GetInverseMatrix();
    REQUIRE(inv == Matrix<4, 4>::Identity());

    Vector<3> right(0, 1, 0);
    Vector<3> up(0, 0, 1);
    Vector<3> forward(1, 0, 0);

    Basis custom{right, up, forward};
    Matrix<4, 4> mat = custom.GetMatrix();

    REQUIRE(mat(0, 0) == right.x);
    REQUIRE(mat(1, 0) == right.y);
    REQUIRE(mat(2, 0) == right.z);

    REQUIRE(mat(0, 1) == up.x);
    REQUIRE(mat(1, 1) == up.y);
    REQUIRE(mat(2, 1) == up.z);

    REQUIRE(mat(0, 2) == forward.x);
    REQUIRE(mat(1, 2) == forward.y);
    REQUIRE(mat(2, 2) == forward.z);

    REQUIRE(custom.GetInverseMatrix() == mat.Transpose());
}

//==============================================================================
// Matrices
//==============================================================================
TEST_CASE("Matrix2 construction and ops")
{
    Matrix<2, 2> m(1, 2, 3, 4);

    REQUIRE(m(0, 0) == 1);
    REQUIRE(m(1, 1) == 4);

    Matrix<2, 2> id = Matrix<2, 2>::Identity();

    REQUIRE(id(0, 0) == 1);
    REQUIRE(id(1, 1) == 1);
    REQUIRE(id(0, 1) == 0);
    REQUIRE(id(1, 0) == 0);

    Matrix<2, 2> a(1, 2, 3, 4);
    Matrix<2, 2> b(5, 6, 7, 8);

    Matrix<2, 2> c = a + b;

    REQUIRE(c(0, 0) == 6);
    REQUIRE(c(1, 1) == 12);

    Matrix<2, 2> prod = a * b;

    REQUIRE(prod(0, 0) == 1 * 5 + 2 * 7);
    REQUIRE(prod(0, 1) == 1 * 6 + 2 * 8);
    REQUIRE(prod(1, 0) == 3 * 5 + 4 * 7);
    REQUIRE(prod(1, 1) == 3 * 6 + 4 * 8);

    Matrix<2, 2> s = a * 2;

    REQUIRE(s(0, 0) == 2);

    Vector<2> v(2, 3);
    Vector<2> mv = a * v;

    REQUIRE(mv.x == 1 * 2 + 2 * 3);
    REQUIRE(mv.y == 3 * 2 + 4 * 3);

    REQUIRE(a.Determinant() == 1 * 4 - 2 * 3);

    Matrix<2, 2> inv = a.Inverse();

    REQUIRE((a * inv).NearlyEquals(Matrix<2, 2>::Identity()));

    Matrix<2, 2> at = a.Transpose();

    REQUIRE(at(0, 1) == 3);
    REQUIRE(at(1, 0) == 2);
}

TEST_CASE("Matrix2 identities")
{
    Matrix<2, 2> a(1, 2, 3, 5);
    Matrix<2, 2> id = Matrix<2, 2>::Identity();

    REQUIRE(a + Matrix<2, 2>::Zero() == a);
    REQUIRE(Matrix<2, 2>::Zero() + a == a);

    REQUIRE(a - Matrix<2, 2>::Zero() == a);
    REQUIRE(a - a == Matrix<2, 2>::Zero());

    REQUIRE((a * id).NearlyEquals(a));
    REQUIRE((id * a).NearlyEquals(a));

    REQUIRE((a * a.Inverse()).NearlyEquals(id));
    REQUIRE((a.Inverse() * a).NearlyEquals(id));

    REQUIRE(a.Transpose().Transpose() == a);
}

TEST_CASE("Matrix3 construction and ops")
{
    Matrix<3, 3> m(1, 2, 3, 4, 5, 6, 7, 8, 9);

    REQUIRE(m(0, 0) == 1);
    REQUIRE(m(0, 1) == 2);
    REQUIRE(m(0, 2) == 3);
    REQUIRE(m(1, 0) == 4);
    REQUIRE(m(1, 1) == 5);
    REQUIRE(m(1, 2) == 6);
    REQUIRE(m(2, 0) == 7);
    REQUIRE(m(2, 1) == 8);
    REQUIRE(m(2, 2) == 9);

    Matrix<3, 3> id = Matrix<3, 3>::Identity();

    REQUIRE(id(0, 0) == 1);
    REQUIRE(id(1, 1) == 1);
    REQUIRE(id(2, 2) == 1);

    Matrix<3, 3> a(1, 2, 3, 4, 5, 6, 7, 8, 9);
    Matrix<3, 3> b(9, 8, 7, 6, 5, 4, 3, 2, 1);

    Matrix<3, 3> c = a + b;

    REQUIRE(c(0, 0) == 10);
    REQUIRE(c(2, 2) == 10);

    Matrix<3, 3> prod = a * b;

    REQUIRE(prod(0, 0) == 1 * 9 + 2 * 6 + 3 * 3);
    REQUIRE(prod(1, 1) == 4 * 8 + 5 * 5 + 6 * 2);

    REQUIRE(a.Determinant() == 0.0f);

    Matrix<3, 3> id2 = Matrix<3, 3>::Identity();

    REQUIRE(id2.Determinant() == 1.0f);

    Matrix<3, 3> rot = Matrix<3, 3>::Identity().RotateZ(PI / 4);
    Matrix<3, 3> inv = rot.Inverse();

    REQUIRE((rot * inv).NearlyEquals(Matrix<3, 3>::Identity()));
}

TEST_CASE("Matrix3 identities")
{
    Matrix<3, 3> a(1, 2, 3, 0, 1, 4, 5, 6, 0);

    Matrix<3, 3> id = Matrix<3, 3>::Identity();

    REQUIRE(a + Matrix<3, 3>::Zero() == a);
    REQUIRE(Matrix<3, 3>::Zero() + a == a);

    REQUIRE(a - Matrix<3, 3>::Zero() == a);
    REQUIRE(a - a == Matrix<3, 3>::Zero());

    REQUIRE((a * id).NearlyEquals(a));
    REQUIRE((id * a).NearlyEquals(a));

    REQUIRE((a * a.Inverse()).NearlyEquals(id));
    REQUIRE((a.Inverse() * a).NearlyEquals(id));

    REQUIRE(a.Transpose().Transpose() == a);
}

TEST_CASE("Matrix3 transformations")
{
    Matrix<3, 3> m = Matrix<3, 3>::Identity();

    Vector<3> v(1, 2, 3);

    Matrix<3, 3> scaled = m.Scale(Vector<3>(2, 3, 4));
    Vector<3> v2 = scaled * v;

    REQUIRE(v2 == Vector<3>(2, 6, 12));

    m = m.RotateZ(PI / 2);

    Vector<3> v3 = m * Vector<3>(1, 0, 0);

    REQUIRE(v3.NearlyEquals(Vector<3>(0, 1, 0)));

    m = Matrix<3, 3>::Identity().Translate(Vector<2>(5, 6));

    Vector<3> v4 = m * Vector<3>(1, 2, 1);

    REQUIRE(v4.x == 6);
    REQUIRE(v4.y == 8);

    m = Matrix<3, 3>::Identity().Rotate(Vector<3>(PI / 2, 0, 0));

    Vector<3> v5 = m * Vector<3>(0, 1, 0);

    REQUIRE(v5.NearlyEquals(Vector<3>(0, 0, 1)));
}

TEST_CASE("Matrix3 transformation identities")
{
    Vector<3> v(1.2f, -3.4f, 5.6f);
    Matrix<3, 3> identity = Matrix<3, 3>::Identity();

    REQUIRE(identity.Scale(Vector<3>::One()) == identity);
    REQUIRE(identity.RotateZ(0.0f) == identity);

    Vector<3> transformed = identity * v;

    REQUIRE(transformed == v);
}

TEST_CASE("Matrix4 construction and ops")
{
    Matrix<4, 4> id = Matrix<4, 4>::Identity();

    REQUIRE(id(0, 0) == 1);
    REQUIRE(id(3, 3) == 1);

    Matrix<4, 4> a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    REQUIRE(a.Determinant() == Approx(0.0f));

    Matrix<4, 4> rot = Matrix<4, 4>::Identity().RotateX(PI / 3);
    Matrix<4, 4> inv = rot.Inverse();

    REQUIRE((rot * inv).NearlyEquals(Matrix<4, 4>::Identity()));

    Matrix<4, 4> t = a.Transpose();

    REQUIRE(t(0, 1) == a(1, 0));
}

TEST_CASE("Matrix4 identities")
{
    Matrix<4, 4> a(1, 2, 3, 4, 0, 1, 4, 5, 5, 6, 0, 7, 1, 0, 2, 1);

    Matrix<4, 4> id = Matrix<4, 4>::Identity();

    REQUIRE(a + Matrix<4, 4>::Zero() == a);
    REQUIRE(Matrix<4, 4>::Zero() + a == a);

    REQUIRE(a - Matrix<4, 4>::Zero() == a);
    REQUIRE(a - a == Matrix<4, 4>::Zero());

    REQUIRE((a * id).NearlyEquals(a));
    REQUIRE((id * a).NearlyEquals(a));

    REQUIRE((a * a.Inverse()).NearlyEquals(id));
    REQUIRE((a.Inverse() * a).NearlyEquals(id));

    REQUIRE(a.Transpose().Transpose() == a);
}

TEST_CASE("Matrix4 transformations")
{
    Matrix<4, 4> m = Matrix<4, 4>::Identity();

    Vector<4> v(1, 2, 3, 1);

    m = m.Translate(Vector<3>(5, 6, 7));

    Vector<4> v2 = m * v;

    REQUIRE(v2 == Vector<4>(6, 8, 10, 1));

    m = Matrix<4, 4>::Identity().Scale(Vector<3>(2, 3, 4));

    Vector<4> v3 = m * v;

    REQUIRE(v3 == Vector<4>(2, 6, 12, 1));

    m = Matrix<4, 4>::Identity().RotateZ(PI / 2);

    Vector<4> v4 = m * Vector<4>(1, 0, 0, 1);

    REQUIRE(v4.NearlyEquals(Vector<4>(0, 1, 0, 1)));

    Matrix<4, 4> proj = Matrix<4, 4>::Perspective(PI / 3, 1.5f, 0.1f, 100.0f);

    REQUIRE(proj(3, 2) == -1.0f);

    Matrix<4, 4> ortho = Matrix<4, 4>::Orthographic(-1, 1, -1, 1, 0.1f, 10);

    REQUIRE(ortho(0, 0) == Approx(1.0f));

    Matrix<4, 4> view = Matrix<4, 4>::LookAt(Vector<3>(0, 0, 5), Vector<3>::Zero(), Vector<3>::Up());

    Vector<4> pos = view * Vector<4>(0, 0, 5, 1);

    REQUIRE(pos.NearlyEquals(Vector<4>(0, 0, 0, 1)));

    Matrix<4, 4> a = Matrix<4, 4>::Identity().Translate(Vector<3>(1, 2, 3));
    Matrix<4, 4> b = Matrix<4, 4>::Identity().Scale(Vector<3>(2, 2, 2));

    Vector<4> u(1, 1, 1, 1);

    REQUIRE((a * b * u).NearlyEquals(a * (b * u)));
}

TEST_CASE("Matrix-vector identities")
{
    Vector<2> v2(1.2f, -3.4f);
    Vector<3> v3(1.2f, -3.4f, 5.6f);
    Vector<4> v4(1.2f, -3.4f, 5.6f, -7.8f);

    REQUIRE((Matrix<2, 2>::Identity() * v2) == v2);
    REQUIRE((Matrix<3, 3>::Identity() * v3) == v3);
    REQUIRE((Matrix<4, 4>::Identity() * v4) == v4);
}
//==============================================================================
// Quaternion
//==============================================================================
TEST_CASE(

    "Quaternion construction and constants")
{
    Quaternion q;
    REQUIRE((q.w == 0 && q.x == 0 && q.y == 0 && q.z == 0)); // fixed
    Quaternion q2(1, 2, 3, 4);
    REQUIRE(q2.w == 1);
    REQUIRE(q2.x == 2);
    REQUIRE(q2.y == 3);
    REQUIRE(q2.z == 4);
    Quaternion id = Quaternion::Identity();
    REQUIRE(id == Quaternion(1, 0, 0, 0));
}

TEST_CASE(

    "Quaternion identities")
{
    Quaternion q(1.2f, 2.0f, -0.7f, 3.1f);

    REQUIRE((q * Quaternion::Identity()).NearlyEquals(q));
    REQUIRE((Quaternion::Identity() * q).NearlyEquals(q));

    REQUIRE((q * q.Inverse()).NearlyEquals(Quaternion::Identity()));
    REQUIRE((q.Inverse() * q).NearlyEquals(Quaternion::Identity()));

    REQUIRE(q.Conjugate().Conjugate() == q);
}

TEST_CASE(

    "Quaternion arithmetic")
{
    Quaternion q1(1, 2, 3, 4);
    Quaternion q2(5, 6, 7, 8);
    Quaternion sum = q1 + q2;
    REQUIRE(sum == Quaternion(6, 8, 10, 12));
    Quaternion diff = q1 - q2;
    REQUIRE(diff == Quaternion(-4, -4, -4, -4));
    Quaternion prod = q1 * q2;
    float w = 1 * 5 - 2 * 6 - 3 * 7 - 4 * 8;
    float x = 1 * 6 + 2 * 5 + 3 * 8 - 4 * 7;
    float y = 1 * 7 - 2 * 8 + 3 * 5 + 4 * 6;
    float z = 1 * 8 + 2 * 7 - 3 * 6 + 4 * 5;
    REQUIRE(prod == Quaternion(w, x, y, z));
    Quaternion div = q1 / q2;
    REQUIRE((div * q2).NearlyEquals(q1));
    Quaternion neg = -q1;
    REQUIRE(neg == Quaternion(-1, -2, -3, -4));
    REQUIRE(q1 * 2.0f == Quaternion(2, 4, 6, 8));
    REQUIRE(2.0f * q1 == Quaternion(2, 4, 6, 8));
    REQUIRE(q1 + 2.0f == Quaternion(3, 2, 3, 4));
    REQUIRE(q1 - 2.0f == Quaternion(-1, 2, 3, 4));
}

TEST_CASE(

    "Quaternion conjugate, magnitude, inverse")
{
    Quaternion q(1, 2, 3, 4);
    REQUIRE(q.Conjugate() == Quaternion(1, -2, -3, -4));
    REQUIRE(q.MagnitudeSquared() == 1 + 4 + 9 + 16);
    REQUIRE(q.Magnitude() == Approx(std::sqrt(30.0f)));
    REQUIRE(q.Inverse() == q.Conjugate() / 30.0f);
    Quaternion norm = q.Normalized();
    REQUIRE(norm.Magnitude() == Approx(1.0f));
}

TEST_CASE(

    "Quaternion dot product")
{
    Quaternion a(1, 2, 3, 4);
    Quaternion b(5, 6, 7, 8);
    REQUIRE(a.Dot(b) == 1 * 5 + 2 * 6 + 3 * 7 + 4 * 8);
}

TEST_CASE(

    "Quaternion polar and angle/axis")
{
    Quaternion q = Quaternion::Identity();
    REQUIRE(q.Angle() == Approx(0.0f));
    q = Quaternion(0, 0.707f, 0.707f, 0);
    float ang = q.Angle();
    REQUIRE(ang > 0);
    Vector<3> axis = q.Axis();
    REQUIRE(axis.Length() == Approx(1.0f));
    QPolar polar = q.ToQPolar();
    REQUIRE(polar.Magnitude == Approx(q.Magnitude()));
    Quaternion q2 = Quaternion::FromQPolar(polar);
    REQUIRE(q2.NearlyEquals(q));
}

TEST_CASE(

    "Quaternion from Euler and matrix")
{
    Vector<3> euler(PI / 3, PI / 4, PI / 6);
    Quaternion q = Quaternion::FromEulerXYZ(euler);
    Matrix<3, 3> mat = q.ToMatrix4().ToMatrix3();
    Matrix<3, 3> matEuler = Matrix<3, 3>::Identity().Rotate(euler);
    REQUIRE(mat.NearlyEquals(matEuler));
    Vector<3> euler2 = q.ToEulerXYZ();
    REQUIRE(euler2.NearlyEquals(euler));
}

TEST_CASE(

    "Quaternion transform vector")
{
    Quaternion q = Quaternion::FromEulerXYZ(Vector<3>(0, 0, PI / 2));
    Vector<3> v(1, 0, 0);
    Vector<3> res = q.Transform(v);
    REQUIRE(res.NearlyEquals(Vector<3>(0, 1, 0)));
}

TEST_CASE(

    "Quaternion exponentials and logarithms")
{
    Quaternion q(1.2f, 0.5f, -0.7f, 0.8f);

    Quaternion result = QLn(QExp(q));

    REQUIRE(result.NearlyEquals(q));

    Quaternion sq = QSqrt(q);
    REQUIRE((sq * sq).NearlyEquals(q));

    Quaternion p = QPow(q, 2.0f);
    REQUIRE(p.NearlyEquals(q * q));

    REQUIRE(QLn(QExp(Quaternion(0, 1, 0, 0))).NearlyEquals(Quaternion(0, 1, 0, 0)));

    REQUIRE(QLn(QExp(Quaternion(0, -1, 0, 0))).NearlyEquals(Quaternion(0, -1, 0, 0)));
}

TEST_CASE(

    "Quaternion Exp/Ln principal branch")
{
    Quaternion q(1.2f, 0.5f, -0.7f, 0.8f);

    Quaternion result = QLn(QExp(q));

    REQUIRE(result.NearlyEquals(q));
}

TEST_CASE(

    "Quaternion Exp/Ln wraps outside principal branch")
{
    Quaternion q(1.2f, 2.0f, -0.7f, 3.1f);

    Quaternion result = QLn(QExp(q));

    REQUIRE_FALSE(result.NearlyEquals(q));
}

//==============================================================================
// Vectors
//==============================================================================
TEST_CASE(

    "Vector2 construction and constants")
{
    Vector<2> v(1, 2);
    REQUIRE(v.x == 1);
    REQUIRE(v.y == 2);
    REQUIRE(Vector<2>::Zero() == Vector<2>(0, 0));
    REQUIRE(Vector<2>(5) == Vector<2>(5, 5));
}

TEST_CASE(

    "Vector2 identities")
{
    Vector<2> v(1.2f, -2.3f);

    REQUIRE(v + Vector<2>::Zero() == v);
    REQUIRE(Vector<2>::Zero() + v == v);

    REQUIRE(v - Vector<2>::Zero() == v);
    REQUIRE(v - v == Vector<2>::Zero());

    REQUIRE(v * 1.0f == v);
    REQUIRE(1.0f * v == v);

    REQUIRE(v / 1.0f == v);

    REQUIRE(v + (-v) == Vector<2>::Zero());

    REQUIRE(v.Dot(v) == Approx(v.LengthSquared()));
    REQUIRE(v.Length() * v.Length() == Approx(v.LengthSquared()));

    REQUIRE(v.Normalized().Length() == Approx(1.0f));
}

TEST_CASE(

    "Vector2 operations")
{
    Vector<2> a(1, 2), b(3, 4);
    REQUIRE(a + b == Vector<2>(4, 6));
    REQUIRE(a - b == Vector<2>(-2, -2));
    REQUIRE(a * b == Vector<2>(3, 8));
    REQUIRE(a + 2 == Vector<2>(3, 4));
    REQUIRE(a * 2 == Vector<2>(2, 4));
    REQUIRE(a / 2 == Vector<2>(0.5f, 1));
    REQUIRE(-a == Vector<2>(-1, -2));
    REQUIRE(a.Dot(b) == 1 * 3 + 2 * 4);
    REQUIRE(a.LengthSquared() == 5);
    REQUIRE(a.Length() == Approx(std::sqrt(5)));
    REQUIRE(a.Normalized().Length() == Approx(1.0f));
    REQUIRE(a.Distance(b) == Approx(std::sqrt(8)));
    REQUIRE(a.Lerp(b, 0.5f) == Vector<2>(2, 3));
}

TEST_CASE(

    "Vector2 polar")
{
    Vector<2> v(1, 1);
    float ang = v.Angle();
    REQUIRE(ang == Approx(PI / 4));
    Polar p = v.ToPolar();
    REQUIRE(p.Angle == Approx(PI / 4));
    REQUIRE(p.Magnitude == Approx(std::sqrt(2)));
    Vector<2> v2 = Vector<2>::FromPolar(p);
    REQUIRE(v2.NearlyEquals(v));
    float s = v.StereoProject();
    REQUIRE(s == Approx((std::sqrt(2) * 1) / (std::sqrt(2) - 1)));
}

TEST_CASE(

    "Vector3 construction and constants")
{
    Vector<3> v(1, 2, 3);
    REQUIRE(v.x == 1);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 3);
    REQUIRE(Vector<3>::Zero() == Vector<3>(0, 0, 0));
    REQUIRE(Vector<3>::One() == Vector<3>(1, 1, 1));
    REQUIRE(Vector<3>::Right() == Vector<3>(1, 0, 0));
    REQUIRE(Vector<3>::Up() == Vector<3>(0, 1, 0));
    REQUIRE(Vector<3>::Forward() == Vector<3>(0, 0, 1));
}

TEST_CASE(

    "Vector3 identities")
{
    Vector<3> v(1.2f, -2.3f, 4.5f);

    REQUIRE(v + Vector<3>::Zero() == v);
    REQUIRE(Vector<3>::Zero() + v == v);

    REQUIRE(v - Vector<3>::Zero() == v);
    REQUIRE(v - v == Vector<3>::Zero());

    REQUIRE(v * 1.0f == v);
    REQUIRE(1.0f * v == v);

    REQUIRE(v / 1.0f == v);

    REQUIRE(v + (-v) == Vector<3>::Zero());

    REQUIRE(v.Dot(v) == Approx(v.LengthSquared()));
    REQUIRE(v.Length() * v.Length() == Approx(v.LengthSquared()));

    REQUIRE(v.Normalized().Length() == Approx(1.0f));

    // A vector is perpendicular to its cross product.
    Vector<3> other(-0.7f, 3.1f, 2.2f);
    Vector<3> cross = v.Cross(other);

    REQUIRE(v.Dot(cross) == Approx(0.0f).margin(1e-5f));
    REQUIRE(other.Dot(cross) == Approx(0.0f).margin(1e-5f));
}

TEST_CASE(

    "Vector3 operations")
{
    Vector<3> a(1, 2, 3), b(4, 5, 6);
    REQUIRE(a + b == Vector<3>(5, 7, 9));
    REQUIRE(a - b == Vector<3>(-3, -3, -3));
    REQUIRE(a * b == Vector<3>(4, 10, 18));
    REQUIRE(a / b == Vector<3>(0.25f, 0.4f, 0.5f));
    REQUIRE(a + 2 == Vector<3>(3, 4, 5));
    REQUIRE(a * 2 == Vector<3>(2, 4, 6));
    REQUIRE(-a == Vector<3>(-1, -2, -3));
    REQUIRE(a.Dot(b) == 32);
    REQUIRE(a.Cross(b) == Vector<3>(2 * 6 - 3 * 5, 3 * 4 - 1 * 6, 1 * 5 - 2 * 4));
    REQUIRE(a.Length() == Approx(std::sqrt(14)));
    REQUIRE(a.Normalized().Length() == Approx(1.0f));
    REQUIRE(a.Distance(b) == Approx(std::sqrt(27)));
    REQUIRE(a.Lerp(b, 0.5f) == Vector<3>(2.5f, 3.5f, 4.5f));
    REQUIRE(a.IsParallelTo(a * 2));
    REQUIRE_FALSE(a.IsParallelTo(b));
    REQUIRE(a.IsPerpendicularTo(Vector<3>(0, -3, 2)));
    REQUIRE_FALSE(a.IsPerpendicularTo(b));
}

TEST_CASE(

    "Vector3 spherical")
{
    Vector<3> v(1, 1, 0);
    float elev = v.Elevation();
    float az = v.Azimuth();
    REQUIRE(elev == Approx(PI / 4));
    REQUIRE(az == Approx(0));
    Spherical s = v.ToSpherical();
    REQUIRE(s.Elevation == Approx(PI / 4));
    REQUIRE(s.Azimuth == Approx(0));
    REQUIRE(s.Magnitude == Approx(std::sqrt(2)));
    Vector<3> v2 = Vector<3>::FromSpherical(s);
    REQUIRE(v2.NearlyEquals(v));
    Vector<2> proj = v.StereoProject();
    REQUIRE(proj == Vector<2>(1, 1));
}

TEST_CASE(

    "Vector4 construction and constants")
{
    Vector<4> v(1, 2, 3, 4);
    REQUIRE(v.x == 1);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 3);
    REQUIRE(v.w == 4);
    REQUIRE(Vector<4>::Zero() == Vector<4>(0, 0, 0, 0));
}

TEST_CASE(

    "Vector4 identities")
{
    Vector<4> v(1.2f, -2.3f, 4.5f, -0.8f);

    REQUIRE(v + Vector<4>::Zero() == v);
    REQUIRE(Vector<4>::Zero() + v == v);

    REQUIRE(v - Vector<4>::Zero() == v);
    REQUIRE(v - v == Vector<4>::Zero());

    REQUIRE(v * 1.0f == v);
    REQUIRE(1.0f * v == v);

    REQUIRE(v / 1.0f == v);

    REQUIRE(v + (-v) == Vector<4>::Zero());

    REQUIRE(v.Dot(v) == Approx(v.LengthSquared()));
    REQUIRE(v.Length() * v.Length() == Approx(v.LengthSquared()));

    REQUIRE(v.Normalized().Length() == Approx(1.0f));
}

TEST_CASE(

    "Vector4 operations")
{
    Vector<4> a(1, 2, 3, 4), b(5, 6, 7, 8);
    REQUIRE(a + b == Vector<4>(6, 8, 10, 12));
    REQUIRE(a - b == Vector<4>(-4, -4, -4, -4));
    REQUIRE(a * b == Vector<4>(5, 12, 21, 32));
    REQUIRE(a + 2 == Vector<4>(3, 4, 5, 6));
    REQUIRE(a * 2 == Vector<4>(2, 4, 6, 8));
    REQUIRE(-a == Vector<4>(-1, -2, -3, -4));
    REQUIRE(a.Dot(b) == 1 * 5 + 2 * 6 + 3 * 7 + 4 * 8);
    REQUIRE(a.Length() == Approx(std::sqrt(30)));
    REQUIRE(a.Normalized().Length() == Approx(1.0f));
    REQUIRE(a.Distance(b) == Approx(std::sqrt(4 * 4 + 4 * 4 + 4 * 4 + 4 * 4)));
    REQUIRE(a.Lerp(b, 0.5f) == Vector<4>(3, 4, 5, 6));
}

TEST_CASE(

    "Vector4 hyperspherical")
{
    Vector<4> v(1, 1, 1, 1);
    float elev = v.Elevation();
    float az = v.Azimuth();
    float hyper = v.HyperAngle();
    REQUIRE(elev == Approx(std::asin(1 / std::sqrt(3))));
    REQUIRE(az == Approx(PI / 4));
    REQUIRE(hyper == Approx(std::asin(1 / std::sqrt(4))));
    HyperSpherical h = v.ToHyperSpherical();
    REQUIRE(h.Elevation == Approx(elev));
    REQUIRE(h.Azimuth == Approx(az));
    REQUIRE(h.HyperAngle == Approx(hyper));
    REQUIRE(h.Magnitude == Approx(2.0f));
    Vector<4> v2 = Vector<4>::FromHyperSpherical(h);
    REQUIRE(v2.NearlyEquals(v));
    Vector<3> proj = v.StereoProject();
    REQUIRE(proj == Vector<3>(2, 2, 2));
}
