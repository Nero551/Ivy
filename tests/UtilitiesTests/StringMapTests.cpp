#include "Utilities/DataStructures/StringMap.hpp"
#include <gtest/gtest.h>

namespace Ivy::UnitTests
{
using namespace Ivy::U;

TEST(StringMap, EmptyByDefault)
{
    StringMap<int> m;
    EXPECT_EQ(m.Size(), 0u);
    EXPECT_FALSE(m.Contains("x"));
}

TEST(StringMap, OperatorBracketInsertsDefault)
{
    StringMap<int> m;
    auto& v = m["a"];
    EXPECT_EQ(v, 0);
    EXPECT_TRUE(m.Contains("a"));
    EXPECT_EQ(m.Size(), 1u);
}

TEST(StringMap, OperatorBracketUpdates)
{
    StringMap<int> m;
    m["a"] = 42;
    EXPECT_EQ(m["a"], 42);
    m["a"] = 100;
    EXPECT_EQ(m["a"], 100);
    EXPECT_EQ(m.Size(), 1u);
}

TEST(StringMap, AtMissingThrows)
{
    StringMap<int> m;
    EXPECT_THROW(m.At("nope"), std::exception);
}

TEST(StringMap, AtHit)
{
    StringMap<int> m;
    m["x"] = 7;
    EXPECT_EQ(m.At("x"), 7);
}

TEST(StringMap, FindHitAndMiss)
{
    StringMap<int> m;
    m["x"] = 7;
    auto it = m.Find("x");
    EXPECT_EQ(*it, 7);
    EXPECT_EQ(m.Find("nope"), m.end());
}

TEST(StringMap, EmplaceInsert)
{
    StringMap<std::string> m;
    auto [it, inserted] = m.Emplace("greeting", "hello");
    EXPECT_TRUE(inserted);
    EXPECT_EQ(*it, "hello");
    EXPECT_EQ(m.Size(), 1u);
}

TEST(StringMap, EmplaceNoInsert)
{
    StringMap<std::string> m;
    m.Emplace("key", "first");
    auto [it, inserted] = m.Emplace("key", "second");
    EXPECT_FALSE(inserted);
    EXPECT_EQ(*it, "first");
    EXPECT_EQ(m.Size(), 1u);
}

TEST(StringMap, EraseMakesKeyDisappear)
{
    StringMap<int> m;
    m["a"] = 1;
    m["b"] = 2;
    m.Erase("a");
    EXPECT_FALSE(m.Contains("a"));
    EXPECT_TRUE(m.Contains("b"));
}

TEST(StringMap, ClearResets)
{
    StringMap<int> m;
    m["a"] = 1;
    m["b"] = 2;
    m.Clear();
    EXPECT_EQ(m.Size(), 0u);
    EXPECT_FALSE(m.Contains("a"));
}

TEST(StringMap, Iteration)
{
    StringMap<int> m;
    m["a"] = 10;
    m["b"] = 20;
    m["c"] = 30;
    int sum = 0;
    for (auto& v : m)
    {
        sum += v;
    }
    EXPECT_EQ(sum, 60);
}

TEST(StringMap, LookupWithStringView)
{
    // Should not allocate a std::string for lookup
    StringMap<int> m;
    m["alpha"] = 1;
    std::string_view sv = "alpha";
    EXPECT_TRUE(m.Contains(sv));
    EXPECT_EQ(m.At(sv), 1);
}
TEST(StringMap, EraseShrinksSizeAndPreservesOthers)
{
    StringMap<int> m;
    m["a"] = 1;
    m["b"] = 2;
    m["c"] = 3;

    m.Erase("a");

    EXPECT_EQ(m.Size(), 2u);
    EXPECT_FALSE(m.Contains("a"));
    EXPECT_TRUE(m.Contains("b"));
    EXPECT_TRUE(m.Contains("c"));

    // Values must still be reachable and correct despite swap-and-pop.
    EXPECT_EQ(m.At("b"), 2);
    EXPECT_EQ(m.At("c"), 3);
}

TEST(StringMap, EraseMissingIsNoOp)
{
    StringMap<int> m;
    m["a"] = 1;
    m.Erase("nope");
    EXPECT_EQ(m.Size(), 1u);
    EXPECT_EQ(m.At("a"), 1);
}

TEST(StringMap, EraseLastIndexDoesNotNeedRepoint)
{
    StringMap<int> m;
    m["a"] = 1;
    m["b"] = 2;
    m.Erase("b"); // lastIndex == index, no swap needed

    EXPECT_EQ(m.Size(), 1u);
    EXPECT_TRUE(m.Contains("a"));
    EXPECT_EQ(m.At("a"), 1);
}

TEST(StringMap, EraseThenReinsert)
{
    StringMap<int> m;
    m["a"] = 1;
    m["b"] = 2;
    m.Erase("a");
    m["a"] = 10; // fresh insert at end

    EXPECT_EQ(m.Size(), 2u);
    EXPECT_EQ(m.At("a"), 10);
    EXPECT_EQ(m.At("b"), 2);
}

TEST(StringMap, EraseAllEmptiesMap)
{
    StringMap<int> m;
    m["a"] = 1;
    m["b"] = 2;
    m["c"] = 3;
    m.Erase("a");
    m.Erase("b");
    m.Erase("c");
    EXPECT_EQ(m.Size(), 0u);
    EXPECT_FALSE(m.Contains("a"));
    EXPECT_FALSE(m.Contains("b"));
    EXPECT_FALSE(m.Contains("c"));
}

} // namespace Ivy::UnitTests
