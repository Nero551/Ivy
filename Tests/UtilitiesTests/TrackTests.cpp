#include "Utilities/Track.hpp"
#include <gtest/gtest.h>

namespace Ivy::UnitTests
{
using namespace Ivy::U;

TEST(Track, DefaultIsZeroAndUnchanged)
{
    Track<int> t;
    EXPECT_EQ(t.Get(), 0);
    EXPECT_FALSE(t.IsChanged());
}

TEST(Track, ConstructionFromValue)
{
    Track<int> t(42);
    EXPECT_EQ(t.Get(), 42);
    EXPECT_FALSE(t.IsChanged());
}

TEST(Track, SetMarksChanged)
{
    Track<int> t;
    t.Set(5);
    EXPECT_EQ(t.Get(), 5);
    EXPECT_TRUE(t.IsChanged());
}

TEST(Track, SetSameValueDoesNotMarkChanged)
{
    Track<int> t(5);
    t.Set(5);
    EXPECT_FALSE(t.IsChanged());
}

TEST(Track, OperatorParensMarksChanged)
{
    Track<int> t;
    t() = 7;
    EXPECT_EQ(t.Get(), 7);
    EXPECT_TRUE(t.IsChanged());
}

TEST(Track, AssignmentOperator)
{
    Track<int> t;
    t = 99;
    EXPECT_EQ(t.Get(), 99);
    EXPECT_TRUE(t.IsChanged());
}

TEST(Track, ClearChanged)
{
    Track<int> t;
    t.Set(5);
    EXPECT_TRUE(t.IsChanged());
    t.ClearChanged();
    EXPECT_FALSE(t.IsChanged());
}

TEST(Track, MarkChangedExplicit)
{
    Track<int> t;
    t.MarkChanged();
    EXPECT_TRUE(t.IsChanged());
    t.ClearChanged();
    t.MarkChanged();
    EXPECT_TRUE(t.IsChanged());
}

TEST(Track, ArithmeticPlus)
{
    Track<int> t(10);
    EXPECT_EQ(t + 5, 15);
    EXPECT_EQ(5 + t, 15);
    EXPECT_EQ(t - 3, 7);
    EXPECT_EQ(20 - t, 10);
    EXPECT_EQ(t * 2, 20);
    EXPECT_EQ(2 * t, 20);
}

TEST(Track, CompoundAssignmentMarksChanged)
{
    Track<int> t(10);
    t += 5;
    EXPECT_EQ(t.Get(), 15);
    EXPECT_TRUE(t.IsChanged());

    t.ClearChanged();
    t -= 3;
    EXPECT_EQ(t.Get(), 12);
    EXPECT_TRUE(t.IsChanged());

    t.ClearChanged();
    t *= 2;
    EXPECT_EQ(t.Get(), 24);
    EXPECT_TRUE(t.IsChanged());

    t.ClearChanged();
    t /= 4;
    EXPECT_EQ(t.Get(), 6);
    EXPECT_TRUE(t.IsChanged());
}

TEST(Track, ImplicitConversionToT)
{
    Track<int> t(7);
    int x = t;
    EXPECT_EQ(x, 7);
}

} // namespace Ivy::UnitTests
