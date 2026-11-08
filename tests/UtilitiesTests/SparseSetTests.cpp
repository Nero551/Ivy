#include "Utilities/DataStructures/SparseSetAoS.hpp"
#include "Utilities/DataStructures/SparseSetSoA.hpp"
#include <gtest/gtest.h>

namespace Ivy::UnitTests
{
using namespace Ivy::U;

// ==================== SparseSetAoS ====================
TEST(SparseSetAoS, EmptyByDefault)
{
    SparseSetAoS<int> s;
    EXPECT_TRUE(s.Empty());
    EXPECT_EQ(s.Size(), 0u);
    EXPECT_FALSE(s.Contains(0));
}

TEST(SparseSetAoS, PushAndContains)
{
    SparseSetAoS<int> s;
    s.Push(5, 42);
    EXPECT_TRUE(s.Contains(5));
    EXPECT_FALSE(s.Contains(4));
    EXPECT_EQ(s.At(5), 42);
    EXPECT_EQ(s.Size(), 1u);
}

TEST(SparseSetAoS, PushSameIndexReturnsExisting)
{
    SparseSetAoS<int> s;
    s.Push(5, 42);
    auto& v = s.Push(5, 99);
    EXPECT_EQ(v, 42); // existing preserved
    EXPECT_EQ(s.Size(), 1u);
}

TEST(SparseSetAoS, EmplaceAndAccess)
{
    SparseSetAoS<int> s;
    auto it = s.Emplace(3, 7);

    // AoS iterator dereferences to Entry, not to T.
    EXPECT_EQ(it->SparseIndex, 3u);
    EXPECT_EQ(it->Value, 7);
    EXPECT_EQ((*it).Value, 7);
    EXPECT_EQ((*it).SparseIndex, 3u);
}

TEST(SparseSetIterators, AoS_DereferencesToEntry)
{
    SparseSetAoS<int> s;
    s.Push(10, 100);
    auto& e = *s.begin();
    static_assert(std::is_same_v<std::remove_reference_t<decltype(e)>, SparseSetAoS<int>::Entry>);
    EXPECT_EQ(e.SparseIndex, 10u);
    EXPECT_EQ(e.Value, 100);
}

TEST(SparseSetAoS, AtMissingThrows)
{
    SparseSetAoS<int> s;
    EXPECT_THROW(s.At(0), std::exception);
}

TEST(SparseSetAoS, EraseAndSizeUpdate)
{
    SparseSetAoS<int> s;
    s.Push(0, 10);
    s.Push(1, 20);
    s.Push(2, 30);
    EXPECT_TRUE(s.Erase(1));
    EXPECT_FALSE(s.Contains(1));
    EXPECT_EQ(s.Size(), 2u);
    EXPECT_TRUE(s.Contains(0));
    EXPECT_TRUE(s.Contains(2));
}

TEST(SparseSetAoS, EraseMissingReturnsFalse)
{
    SparseSetAoS<int> s;
    EXPECT_FALSE(s.Erase(0));
}

TEST(SparseSetAoS, SwapAndPopRepacksSparseMapping)
{
    SparseSetAoS<int> s;
    s.Push(0, 10);
    s.Push(1, 20);
    s.Push(2, 30);
    s.Erase(0); // swaps 30 into slot 0, clears 0
    EXPECT_TRUE(s.Contains(2));
    EXPECT_EQ(s.At(2), 30); // still reachable through sparse
    EXPECT_EQ(s.Size(), 2u);
}

TEST(SparseSetAoS, Iteration)
{
    SparseSetAoS<int> s;
    s.Push(0, 10);
    s.Push(1, 20);
    s.Push(2, 30);

    int sum = 0;
    for (auto& e : s)
    {
        sum += e.Value;
    }
    EXPECT_EQ(sum, 60);
}

TEST(SparseSetAoS, DenseIndexOfAndSparseIndexOf)
{
    SparseSetAoS<int> s;
    s.Push(10, 100);
    s.Push(20, 200);
    auto di = s.DenseIndexOf(20);
    EXPECT_EQ(s.SparseIndexOf(di), 20u);
}

TEST(SparseSetAoS, Clear)
{
    SparseSetAoS<int> s;
    s.Push(0, 1);
    s.Push(1, 2);
    s.Clear();
    EXPECT_TRUE(s.Empty());
    EXPECT_FALSE(s.Contains(0));
    EXPECT_FALSE(s.Contains(1));
}

// ==================== SparseSetSoA ====================
TEST(SparseSetSoA, PushContainsAt)
{
    SparseSetSoA<int> s;
    s.Push(3, 33);
    EXPECT_TRUE(s.Contains(3));
    EXPECT_EQ(s.At(3), 33);
    EXPECT_EQ(s.Size(), 1u);
}

TEST(SparseSetSoA, ErasePreservesOtherEntries)
{
    SparseSetSoA<int> s;
    s.Push(1, 10);
    s.Push(2, 20);
    s.Push(3, 30);
    s.Erase(2);
    EXPECT_FALSE(s.Contains(2));
    EXPECT_EQ(s.At(1), 10);
    EXPECT_EQ(s.At(3), 30);
    EXPECT_EQ(s.Size(), 2u);
}

TEST(SparseSetSoA, EraseByDense)
{
    SparseSetSoA<int> s;
    s.Push(1, 10);
    s.Push(2, 20);
    EXPECT_TRUE(s.EraseByDense(0));
    EXPECT_FALSE(s.Contains(1));
    EXPECT_TRUE(s.Contains(2));
}

TEST(SparseSetSoA, AtMissingThrows)
{
    SparseSetSoA<int> s;
    EXPECT_THROW(s.At(42), std::exception);
}

TEST(SparseSetSoA, DataAccess)
{
    SparseSetSoA<int> s;
    s.Push(1, 10);
    s.Push(2, 20);
    auto& data = s.Data();
    EXPECT_EQ(data.size(), 2u);
    EXPECT_TRUE(std::find(data.begin(), data.end(), 10) != data.end());
}

TEST(SparseSetIterators, SoA_DereferencesToValue)
{
    SparseSetSoA<int> s;
    s.Push(10, 100);
    auto& v = *s.begin();
    static_assert(std::is_same_v<std::remove_reference_t<decltype(v)>, int>);
    EXPECT_EQ(v, 100);
}
} // namespace Ivy::UnitTests
