#include "Utilities/IsSpecialization.hpp"
#include <array>
#include <gtest/gtest.h>
#include <tuple>
#include <vector>

namespace Ivy::UnitTests
{
using namespace Ivy::U;

// ==================== IsTSpec (type-only template specialization) ====================
TEST(IsTSpec, Tuple)
{
    static_assert(IsTSpec<std::tuple<int>, std::tuple>);
    static_assert(IsTSpec<std::tuple<int, float>, std::tuple>);
    static_assert(!IsTSpec<int, std::tuple>);
    SUCCEED();
}

TEST(IsTSpec, Vector)
{
    static_assert(IsTSpec<std::vector<int>, std::vector>);
    static_assert(!IsTSpec<std::tuple<int>, std::vector>);
    SUCCEED();
}

TEST(IsTSpec, ConstAndRef)
{
    static_assert(IsTSpec<const std::tuple<int>, std::tuple>);
    static_assert(IsTSpec<const std::tuple<int>&, std::tuple>);
    static_assert(IsTSpec<std::tuple<int>&&, std::tuple>);
    SUCCEED();
}

// ==================== IsSpec (derived from) ====================
namespace
{
struct Base
{
};
struct Derived : Base
{
};
struct Unrelated
{
};
} // namespace

TEST(IsSpec, DerivedFromBase)
{
    static_assert(IsSpec<Derived, Base>);
    static_assert(IsSpec<Derived&, Base>);
    static_assert(IsSpec<const Derived, Base>);
    static_assert(!IsSpec<Base, Derived>);
    static_assert(!IsSpec<Unrelated, Base>);
    SUCCEED();
}

TEST(IsSpec, Self)
{
    static_assert(IsSpec<Base, Base>);
    SUCCEED();
}

// ==================== IsVTSpec (auto-then-type template) ====================
namespace
{
template <auto N, typename T> struct AutoTypeTemplate
{
};
} // namespace

TEST(IsVTSpec, AutoTypeTemplate)
{
    static_assert(IsVTSpec<AutoTypeTemplate<5, int>, AutoTypeTemplate>);
    static_assert(!IsVTSpec<int, AutoTypeTemplate>);
    SUCCEED();
}

// ==================== IsTVSpec (type-then-auto template) ====================
namespace
{
template <typename T, auto N> struct TypeAutoTemplate
{
};
} // namespace

TEST(IsTVSpec, TypeAutoTemplate)
{
    static_assert(IsTVSpec<TypeAutoTemplate<int, 5>, TypeAutoTemplate>);
    static_assert(!IsTVSpec<int, TypeAutoTemplate>);
    SUCCEED();
}

// ==================== IsTTVSpec (type-type-auto template) ====================
namespace
{
template <typename A, typename B, auto N> struct TypeTypeAutoTemplate
{
};
} // namespace

TEST(IsTTVSpec, TypeTypeAutoTemplate)
{
    static_assert(IsTTVSpec<TypeTypeAutoTemplate<int, float, 5>, TypeTypeAutoTemplate>);
    static_assert(!IsTTVSpec<int, TypeTypeAutoTemplate>);
    SUCCEED();
}

// ==================== IsVVTSpec (auto-auto-type template) ====================
namespace
{
template <auto A, auto B, typename T> struct AutoAutoTypeTemplate
{
};
} // namespace

TEST(IsVVTSpec, AutoAutoTypeTemplate)
{
    static_assert(IsVVTSpec<AutoAutoTypeTemplate<1, 2, int>, AutoAutoTypeTemplate>);
    static_assert(!IsVVTSpec<int, AutoAutoTypeTemplate>);
    SUCCEED();
}

} // namespace Ivy::UnitTests
