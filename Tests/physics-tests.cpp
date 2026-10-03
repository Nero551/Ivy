#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "Math/Math.hpp"
#include "Physics/Physics.hpp"

using Catch::Approx;
using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;
using namespace Ivy::M;
using namespace Ivy::P;

//==============================================================================
// Dimensional Analysis
//==============================================================================

TEST_CASE("Dimensional: term exponents and printing")
{
    Dimension<float, Time<1>> t1{2.0f};
    Dimension<float, Time<2>> t2{3.0f};
    Dimension<float, Time<-1>> tInv{4.0f};
    Dimension<float, Length<1>> l1{5.0f};
    Dimension<float, Mass<1>> m1{6.0f};

    REQUIRE(t1.Value == 2.0f);
    REQUIRE(t2.Value == 3.0f);
    REQUIRE(tInv.Value == 4.0f);
    REQUIRE(l1.Value == 5.0f);
    REQUIRE(m1.Value == 6.0f);

    std::ostringstream oss;
    oss << t1;
    REQUIRE(oss.str() == "2 s");
    oss.str("");
    oss << t2;
    REQUIRE(oss.str() == "3 s²");
    oss.str("");
    oss << tInv;
    REQUIRE(oss.str() == "4 s⁻¹");
    oss.str("");
    oss << l1;
    REQUIRE(oss.str() == "5 m");
    oss.str("");
    oss << m1;
    REQUIRE(oss.str() == "6 kg");
}

TEST_CASE("Dimensional: term-term operations normalize exponents")
{
    Dimension<float, Length<1>> l1{10.0f};
    Dimension<float, Length<2>> l2{3.0f};
    Dimension<float, Time<1>> t{2.0f};

    // Length<1> * Length<2> -> Length<3>
    auto l3 = l1 * l2;
    REQUIRE(l3.Value == 30.0f);
    REQUIRE((std::is_same_v<decltype(l3), Dimension<float, Length<3>>>));
    std::ostringstream oss;
    oss << l3;
    REQUIRE(oss.str() == "30 m³");

    // Length<2> / Length<1> -> Length<1>
    auto lDiv = l2 / l1;
    REQUIRE(lDiv.Value == 0.3f);
    REQUIRE((std::is_same_v<decltype(lDiv), Dimension<float, Length<1>>>));
    oss.str("");
    oss << lDiv;
    REQUIRE(oss.str() == "0.3 m");

    // Length / Time -> Op<Length<1>, Time<-1>>
    auto speed = l1 / t;
    REQUIRE(speed.Value == 5.0f);
    REQUIRE((std::is_same_v<decltype(speed), Dimension<float, OperationDimensional<Length<1>, Time<-1>>>>));
    oss.str("");
    oss << speed;
    REQUIRE(oss.str() == "5 m/s");

    // Op / Time -> Op<Length<1>, Time<-2>>
    auto accel = speed / t;
    REQUIRE(accel.Value == 2.5f);
    REQUIRE((std::is_same_v<decltype(accel), Dimension<float, OperationDimensional<Length<1>, Time<-2>>>>));
    oss.str("");
    oss << accel;
    REQUIRE(oss.str() == "2.5 m/s²");
}

TEST_CASE("Dimensional: operation with term combinations")
{
    Dimension<float, Length<1>> l{10.0f};
    Dimension<float, Time<1>> t{2.0f};
    auto speed = l / t; // Op<Length<1>, Time<-1>>

    // Term * Op
    Dimension<float, Mass<1>> m{5.0f};
    auto momentum = m * speed;
    REQUIRE(momentum.Value == 25.0f);
    REQUIRE((std::is_same_v<decltype(momentum),
        Dimension<float, OperationDimensional<Mass<1>, OperationDimensional<Length<1>, Time<-1>>>>>));
    std::ostringstream oss;
    oss << momentum;
    REQUIRE(oss.str() == "25 kgm/s");

    // Op * Term (Term cancels part of Op)
    Dimension<float, Time<2>> t2{3.0f};
    auto l_t = speed * t2; // Op<Length<1>, Time<-1>> * Time<2> -> Op<Length<1>, Time<1>>
    REQUIRE(l_t.Value == 15.0f);
    REQUIRE((std::is_same_v<decltype(l_t), Dimension<float, OperationDimensional<Length<1>, Time<1>>>>));
    oss.str("");
    oss << l_t;
    REQUIRE(oss.str() == "15 ms");

    // Term * Op (Term matches left of Op)
    auto l_speed = l * speed; // Length<1> * Op<Length<1>, Time<-1>> -> Op<Length<2>, Time<-1>>
    REQUIRE(l_speed.Value == 50.0f);
    REQUIRE((std::is_same_v<decltype(l_speed), Dimension<float, OperationDimensional<Length<2>, Time<-1>>>>));
    oss.str("");
    oss << l_speed;
    REQUIRE(oss.str() == "50 m²/s");

    // Op / Term (Term matches right of Op)
    auto accel = speed / t; // Op<Length<1>, Time<-1>> / Time<1> -> Op<Length<1>, Time<-2>>
    REQUIRE(accel.Value == 2.5f);
    REQUIRE((std::is_same_v<decltype(accel), Dimension<float, OperationDimensional<Length<1>, Time<-2>>>>));
    oss.str("");
    oss << accel;
    REQUIRE(oss.str() == "2.5 m/s²");
}

TEST_CASE("Dimensional: operation-operation combinations")
{
    Dimension<float, Length<1>> l{10.0f};
    Dimension<float, Time<1>> t{2.0f};
    auto speed = l / t;     // Op<Length<1>, Time<-1>>
    auto accel = speed / t; // Op<Length<1>, Time<-2>>

    // Op * Op
    Dimension<float, Mass<1>> m{5.0f};
    auto timeMass = t * m; // Op<Time<1>, Mass<1>>
    auto speed_timeMass = speed * timeMass;
    REQUIRE(speed_timeMass.Value == 50.0f); // 5 * (2*5) = 50

    // Op / Op
    auto ratio = accel / speed;
    REQUIRE(ratio.Value == 0.5f); // 2.5 / 5 = 0.5
}

TEST_CASE("Dimensional: addition and subtraction")
{
    Dimension<float, Length<1>> a{3.0f};
    Dimension<float, Length<1>> b{4.0f};
    auto sum = a + b;
    REQUIRE(sum.Value == 7.0f);
    REQUIRE((std::is_same_v<decltype(sum), Dimension<float, Length<1>>>));

    auto diff = b - a;
    REQUIRE(diff.Value == 1.0f);

    Dimension<float, Time<2>> t1{2.0f};
    Dimension<float, Time<2>> t2{5.0f};
    auto tSum = t1 + t2;
    REQUIRE(tSum.Value == 7.0f);
    std::ostringstream oss;
    oss << tSum;
    REQUIRE(oss.str() == "7 s²");
}

TEST_CASE("Dimensional: value access")
{
    Dimension<float, Length<1>> l{3.14f};
    REQUIRE(l() == 3.14f);

    l() = 2.71f;
    REQUIRE(l.Value == 2.71f);

    const auto& cl = l;
    REQUIRE(cl() == 2.71f);
}