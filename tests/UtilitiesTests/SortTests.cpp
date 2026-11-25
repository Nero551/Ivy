#include "Utilities/Sort.hpp"
#include <array>
#include <gtest/gtest.h>

namespace Ivy::UnitTests
{
using namespace Ivy::U;

static constexpr std::array<int, 5> Unsorted = {5, 3, 1, 4, 2};
static constexpr std::array<int, 5> Sorted = {1, 2, 3, 4, 5};
static constexpr std::array<int, 5> Reversed = {5, 4, 3, 2, 1};

// ==================== Swap ====================
TEST(SortSwap, SwapsElements)
{
    std::array<int, 5> arr = {1, 2, 3, 4, 5};
    U::Swap(arr, 0, 4);
    EXPECT_EQ(arr[0], 5);
    EXPECT_EQ(arr[4], 1);
    EXPECT_EQ(arr[2], 3);
}

// ==================== BubbleSort ====================
TEST(SortBubble, SortsRandom)
{
    auto r = U::BubbleSort(Unsorted);
    EXPECT_EQ(r, Sorted);
}

TEST(SortBubble, SortedInputStable)
{
    auto r = U::BubbleSort(Sorted);
    EXPECT_EQ(r, Sorted);
}

TEST(SortBubble, ReverseInput)
{
    auto r = U::BubbleSort(Reversed);
    EXPECT_EQ(r, Sorted);
}

TEST(SortBubble, EmptyAndSingle)
{
    constexpr std::array<int, 0> e = {};
    constexpr std::array<int, 1> s = {42};
    EXPECT_EQ(U::BubbleSort(e), e);
    EXPECT_EQ(U::BubbleSort(s), s);
}

// ==================== InsertionSort ====================
TEST(SortInsertion, SortsRandom)
{
    EXPECT_EQ(U::InsertionSort(Unsorted), Sorted);
    EXPECT_EQ(U::InsertionSort(Reversed), Sorted);
}

TEST(SortInsertion, SortedInput)
{
    EXPECT_EQ(U::InsertionSort(Sorted), Sorted);
}

// ==================== SelectionSort ====================
TEST(SortSelection, SortsRandom)
{
    EXPECT_EQ(U::SelectionSort(Unsorted), Sorted);
    EXPECT_EQ(U::SelectionSort(Reversed), Sorted);
}

// ==================== MergeSort ====================
TEST(SortMerge, MergesTwoSorted)
{
    constexpr std::array<int, 3> a = {1, 3, 5};
    constexpr std::array<int, 3> b = {2, 4, 6};
    auto m = U::MergeSort(a, b);
    constexpr std::array<int, 6> expected = {1, 2, 3, 4, 5, 6};
    EXPECT_EQ(m, expected);
}

TEST(SortMerge, UnevenSizes)
{
    constexpr std::array<int, 2> a = {1, 5};
    constexpr std::array<int, 4> b = {2, 3, 4, 6};
    auto m = U::MergeSort(a, b);
    constexpr std::array<int, 6> expected = {1, 2, 3, 4, 5, 6};
    EXPECT_EQ(m, expected);
}

TEST(SortMerge, EmptySides)
{
    constexpr std::array<int, 0> e = {};
    constexpr std::array<int, 3> s = {1, 2, 3};
    EXPECT_EQ(U::MergeSort(e, s), s);
    EXPECT_EQ(U::MergeSort(s, e), s);
}

TEST(SortMerge, AllLeftFirst)
{
    constexpr std::array<int, 3> a = {1, 2, 3};
    constexpr std::array<int, 3> b = {4, 5, 6};
    auto m = U::MergeSort(a, b);
    constexpr std::array<int, 6> expected = {1, 2, 3, 4, 5, 6};
    EXPECT_EQ(m, expected);
}

} // namespace Ivy::UnitTests
