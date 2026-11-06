#include "Utilities/DataStructures/TypeTree.hpp"
#include "Utilities/DataStructures/TypedVector.hpp"
#include <gtest/gtest.h>

namespace Ivy::UnitTests
{
using namespace Ivy::U;

// ==================== TypedVector ====================
TEST(TypedVector, EmptyByDefault)
{
    TypedVector<int> v;
    EXPECT_EQ(v.Size(), 0u);
    EXPECT_TRUE(v.Empty());
}

TEST(TypedVector, PushDistinctTypes)
{
    TypedVector<int> v;
    v.Push<int>(10);
    v.Push<float>(20);
    v.Push<double>(30);
    EXPECT_EQ(v.Size(), 3u);
    EXPECT_EQ(v.Get<int>(), 10);
    EXPECT_EQ(v.Get<float>(), 20);
    EXPECT_EQ(v.Get<double>(), 30);
}

TEST(TypedVector, PushDuplicateIsNoOp)
{
    TypedVector<int> v;
    v.Push<int>(10);
    auto& r = v.Push<int>(99);
    EXPECT_EQ(r, 10); // existing preserved
    EXPECT_EQ(v.Size(), 1u);
}

TEST(TypedVector, ContainsDistinguishesCombinations)
{
    TypedVector<int> v;
    v.Push<int>(1);
    v.Push<int, float>(2);
    EXPECT_TRUE(v.Contains<int>());
    EXPECT_TRUE((v.Contains<int, float>()));
    EXPECT_FALSE(v.Contains<float>());
    EXPECT_FALSE((v.Contains<int, float, double>()));
}

TEST(TypedVector, ConstContains)
{
    TypedVector<int> v;
    v.Push<int>(1);
    const auto& cv = v;
    EXPECT_TRUE(cv.Contains<int>());
    EXPECT_FALSE(cv.Contains<float>());
}

TEST(TypedVector, AtMissingThrows)
{
    TypedVector<int> v;
    EXPECT_THROW(v.At<float>(), std::exception);
}

TEST(TypedVector, EmplaceWithArgs)
{
    TypedVector<std::string> v;
    auto it = v.Emplace<int>("hello");
    EXPECT_EQ(*it, "hello");
    EXPECT_EQ(v.Get<int>(), "hello");
}

TEST(TypedVector, FindHitAndMiss)
{
    TypedVector<int> v;
    v.Push<int>(7);
    auto it = v.Find<int>();
    EXPECT_EQ(*it, 7);
    EXPECT_EQ(v.Find<float>(), v.end());
}

TEST(TypedVector, Clear)
{
    TypedVector<int> v;
    v.Push<int>(1);
    v.Push<float>(2);
    v.Clear();
    EXPECT_TRUE(v.Empty());
    EXPECT_EQ(v.Size(), 0u);
}

// ==================== TypeTree ====================
TEST(TypeTree, LeafFlatten)
{
    using L = TypeTree::Leaf<int>;
    static_assert(std::is_same_v<L::Flatten, TypeTree::List<int>>);
    SUCCEED();
}

TEST(TypeTree, NodeFlatten)
{
    using N = TypeTree::Node<TypeTree::Leaf<int>, TypeTree::Leaf<float>>;
    static_assert(std::is_same_v<N::Flatten, TypeTree::List<int, float>>);
    SUCCEED();
}

TEST(TypeTree, NestedNodeFlatten)
{
    using Inner = TypeTree::Node<TypeTree::Leaf<int>, TypeTree::Leaf<float>>;
    using Outer = TypeTree::Node<Inner, TypeTree::Leaf<double>>;
    static_assert(Outer::Flatten::Size == 3);
    SUCCEED();
}

} // namespace Ivy::UnitTests
