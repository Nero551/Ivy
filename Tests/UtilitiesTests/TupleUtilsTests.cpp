#include "Utilities/TupleUtils.hpp"
#include <gtest/gtest.h>
#include <tuple>

namespace Ivy::UnitTests
{
using namespace Ivy::U;

// ==================== AreUnique ====================
TEST(AreUnique, TriviallyTrue)
{
    static_assert(AreUnique<int>);
    static_assert(AreUnique<int, float>);
    static_assert(AreUnique<int, float, double>);
    SUCCEED();
}

TEST(AreUnique, DuplicatesFalse)
{
    static_assert(!AreUnique<int, int>);
    static_assert(!AreUnique<int, float, int>);
    static_assert(!AreUnique<int, int, int>);
    SUCCEED();
}

// ==================== ConnectArgsIntoTuple ====================
TEST(ConnectArgsIntoTuple, SingleType)
{
    using T = ConnectArgsIntoTuple<int>;
    static_assert(std::is_same_v<T, std::tuple<int>>);
    SUCCEED();
}

TEST(ConnectArgsIntoTuple, MultipleTypes)
{
    using T = ConnectArgsIntoTuple<int, float>;
    static_assert(std::is_same_v<T, std::tuple<int, float>>);
    SUCCEED();
}

TEST(ConnectArgsIntoTuple, NestedTupleFlattening)
{
    using T = ConnectArgsIntoTuple<int, std::tuple<float, double>>;
    static_assert(std::is_same_v<T, std::tuple<int, float, double>>);
    SUCCEED();
}

// ==================== UnpackAndRun ====================
namespace
{
template <typename A, typename B> int SumPairs()
{
    return 0;
}

template <typename A> int SingleTypeSizeof()
{
    return sizeof(A);
}
} // namespace

TEST(UnpackAndRun, CallsLambda)
{
    using Tuple = std::tuple<int, float>;
    int captured = 0;
    UnpackAndRun<Tuple>(
        std::make_index_sequence<2>{}, [&]<typename A, typename B>() { captured = sizeof(A) + sizeof(B); });
    EXPECT_EQ(captured, sizeof(int) + sizeof(float));
}

} // namespace Ivy::UnitTests
