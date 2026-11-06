#include "Utilities/DataStructures/Indirect2DVector.hpp"
#include <gtest/gtest.h>

namespace Ivy::UnitTests
{
using namespace Ivy::U;
// ==================== Indirect2DVector ====================
TEST(Indirect2DVector, EmptyByDefault)
{
    Indirect2DVector<int> v;
    EXPECT_EQ(v.Size(), 0u);
    EXPECT_FALSE(v.Contains(0, 0));
}

TEST(Indirect2DVector, PushAndAt)
{
    Indirect2DVector<int> v;
    v.Push(1, 2, 100);
    EXPECT_TRUE(v.Contains(1, 2));
    EXPECT_EQ(v.At(1, 2), 100);
    EXPECT_EQ(v.Size(), 1u);
}

TEST(Indirect2DVector, PushSameKeyReturnsExisting)
{
    Indirect2DVector<int> v;
    v.Push(0, 0, 10);
    auto& r = v.Push(0, 0, 99);
    EXPECT_EQ(r, 10);
    EXPECT_EQ(v.Size(), 1u);
}

TEST(Indirect2DVector, AtMissingThrows)
{
    Indirect2DVector<int> v;
    EXPECT_THROW(v.At(5, 5), std::exception);
}

TEST(Indirect2DVector, Erase)
{
    Indirect2DVector<int> v;
    v.Push(0, 0, 10);
    v.Push(1, 1, 20);
    v.Push(2, 2, 30);
    v.Erase(1, 1);
    EXPECT_FALSE(v.Contains(1, 1));
    EXPECT_EQ(v.Size(), 2u);
    EXPECT_EQ(v.At(0, 0), 10);
    EXPECT_EQ(v.At(2, 2), 30);
}

TEST(Indirect2DVector, SwapAndPopEraseKeepsOthersReachable)
{
    Indirect2DVector<int> v;
    v.Push(0, 0, 10);
    v.Push(1, 1, 20);
    v.Push(2, 2, 30);
    v.Erase(0, 0); // (2,2) moved into slot 0
    EXPECT_EQ(v.At(2, 2), 30);
    EXPECT_EQ(v.At(1, 1), 20);
    EXPECT_EQ(v.Size(), 2u);
}

TEST(Indirect2DVector, Iteration)
{
    Indirect2DVector<int> v;
    v.Push(0, 0, 1);
    v.Push(0, 1, 2);
    v.Push(1, 0, 3);

    int sum = 0;
    for (auto x : v)
    {
        sum += x;
    }
    EXPECT_EQ(sum, 6);
}

TEST(Indirect2DVector, FindHitAndMiss)
{
    Indirect2DVector<int> v;
    v.Push(0, 0, 42);
    auto it = v.Find(0, 0);
    EXPECT_EQ(*it, 42);
    EXPECT_EQ(v.Find(0, 1), v.end());
}

TEST(Indirect2DVector, GetOrEmplace)
{
    Indirect2DVector<std::string> v;
    v.GetOrEmplace(0, 0, "hello");
    v.GetOrEmplace(0, 0, "world"); // ignored
    EXPECT_EQ(v.At(0, 0), "hello");
}
} // namespace Ivy::UnitTests
