#include "Math/FiniteSet.hpp"
#include "Math/Ordered.hpp"
#include "Math/Set.hpp"
#include <gtest/gtest.h>
#include <tuple>

namespace Ivy::UnitTests
{
using namespace Ivy::M;

// Aliases so gtest macros don't choke on template commas.
using MT_fi = Ordered<float, int>;
using MT_ff = Ordered<float, float>;
using MT_fid = Ordered<float, int, double>;

// ==================== MathTuple ====================
TEST(MathTuple, ConstructionAndGet)
{
    MT_fi t(1.5f, 3);
    EXPECT_FLOAT_EQ(t.Get<0>(), 1.5f);
    EXPECT_EQ(t.Get<1>(), 3);

    EXPECT_EQ(MT_fi::Size, 2u);
}

TEST(MathTuple, IndexedGetModifies)
{
    MT_ff t(1.0f, 2.0f);
    t.Get<0>() = 5.0f;
    EXPECT_FLOAT_EQ(t.Get<0>(), 5.0f);
}

TEST(MathTuple, Apply)
{
    MT_ff t(3.0f, 4.0f);
    float sum = t.Apply([](float a, float b) { return a + b; });
    EXPECT_FLOAT_EQ(sum, 7.0f);

    MT_fid mixed(1.0f, 2, 3.0);
    double s = mixed.Apply([](float a, int b, double c) { return a + b + c; });
    EXPECT_DOUBLE_EQ(s, 6.0);
}

TEST(MathTuple, ElementWiseArithmetic)
{
    MT_ff a(1.0f, 2.0f);
    MT_ff b(3.0f, 4.0f);

    auto sum = a + b;
    EXPECT_FLOAT_EQ(sum.Get<0>(), 4.0f);
    EXPECT_FLOAT_EQ(sum.Get<1>(), 6.0f);

    auto diff = a - b;
    EXPECT_FLOAT_EQ(diff.Get<0>(), -2.0f);
    EXPECT_FLOAT_EQ(diff.Get<1>(), -2.0f);

    auto prod = a * b;
    EXPECT_FLOAT_EQ(prod.Get<0>(), 3.0f);
    EXPECT_FLOAT_EQ(prod.Get<1>(), 8.0f);

    auto quot = a / b;
    EXPECT_FLOAT_EQ(quot.Get<0>(), 1.0f / 3.0f);
    EXPECT_FLOAT_EQ(quot.Get<1>(), 0.5f);
}

TEST(MathTuple, ScalarBroadcast)
{
    MT_ff t(1.0f, 2.0f);

    auto plus = t + 10.0f;
    EXPECT_FLOAT_EQ(plus.Get<0>(), 11.0f);
    EXPECT_FLOAT_EQ(plus.Get<1>(), 12.0f);

    auto minus = t - 1.0f;
    EXPECT_FLOAT_EQ(minus.Get<0>(), 0.0f);
    EXPECT_FLOAT_EQ(minus.Get<1>(), 1.0f);

    auto times = t * 2.0f;
    EXPECT_FLOAT_EQ(times.Get<0>(), 2.0f);
    EXPECT_FLOAT_EQ(times.Get<1>(), 4.0f);

    auto div = t / 2.0f;
    EXPECT_FLOAT_EQ(div.Get<0>(), 0.5f);
    EXPECT_FLOAT_EQ(div.Get<1>(), 1.0f);
}

TEST(MathTuple, TupleSizeTrait)
{
    EXPECT_EQ(std::tuple_size_v<MT_fi>, 2u);
    EXPECT_EQ(std::tuple_size_v<MT_fid>, 3u);
}

TEST(MathTuple, ImplicitConversionToStdTuple)
{
    MT_fi t(1.5f, 3);
    std::tuple<float, int> std_t = t;
    EXPECT_FLOAT_EQ(std::get<0>(std_t), 1.5f);
    EXPECT_EQ(std::get<1>(std_t), 3);
}

// ==================== Set ====================
TEST(Set, MembershipAndPredicate)
{
    Set<int> even = [](int x) { return x % 2 == 0; };
    EXPECT_TRUE(even.Belongs(2));
    EXPECT_TRUE(even.Belongs(0));
    EXPECT_TRUE(even.Belongs(-4));
    EXPECT_FALSE(even.Belongs(3));
}

TEST(Set, UnionIntersectionDifference)
{
    Set<int> even = [](int x) { return x % 2 == 0; };
    Set<int> positive = [](int x) { return x > 0; };

    auto unionSet = even.Union(positive);
    EXPECT_TRUE(unionSet.Belongs(-2));
    EXPECT_TRUE(unionSet.Belongs(3));
    EXPECT_TRUE(unionSet.Belongs(2));
    EXPECT_FALSE(unionSet.Belongs(-3));

    auto inter = even.Intersection(positive);
    EXPECT_TRUE(inter.Belongs(2));
    EXPECT_FALSE(inter.Belongs(-2));
    EXPECT_FALSE(inter.Belongs(3));

    auto diff = even.Difference(positive);
    EXPECT_TRUE(diff.Belongs(-2));
    EXPECT_FALSE(diff.Belongs(2));
    EXPECT_FALSE(diff.Belongs(-3));
}

TEST(Set, SymmetricDifference)
{
    Set<int> even = [](int x) { return x % 2 == 0; };
    Set<int> positive = [](int x) { return x > 0; };

    auto sym = even.SymmetricDifference(positive);
    EXPECT_TRUE(sym.Belongs(-2));
    EXPECT_TRUE(sym.Belongs(3));
    EXPECT_FALSE(sym.Belongs(2));
    EXPECT_FALSE(sym.Belongs(-3));
}

TEST(Set, ComplementEmptyUniversal)
{
    Set<int> even = [](int x) { return x % 2 == 0; };
    EXPECT_FALSE(even.Complement().Belongs(2));
    EXPECT_TRUE(even.Complement().Belongs(3));

    EXPECT_FALSE(Set<int>::Empty().Belongs(0));
    EXPECT_FALSE(Set<int>::Empty().Belongs(100));

    EXPECT_TRUE(Set<int>::Universal().Belongs(0));
    EXPECT_TRUE(Set<int>::Universal().Belongs(-100));
}

// ==================== FiniteSet ====================
TEST(FiniteSet, AddBelongsRemove)
{
    FiniteSet<int> s;
    EXPECT_FALSE(s.Belongs(1));

    s.Add(1);
    s.Add(2);
    s.Add(1);
    EXPECT_TRUE(s.Belongs(1));
    EXPECT_TRUE(s.Belongs(2));
    EXPECT_FALSE(s.Belongs(3));

    s.Remove(1);
    EXPECT_FALSE(s.Belongs(1));
    EXPECT_TRUE(s.Belongs(2));
}

TEST(FiniteSet, UnionIntersectionDifference)
{
    FiniteSet<int> a;
    a.Add(1);
    a.Add(2);
    a.Add(3);
    FiniteSet<int> b;
    b.Add(3);
    b.Add(4);
    b.Add(5);

    auto u = a.Union(b);
    EXPECT_TRUE(u.Belongs(1));
    EXPECT_TRUE(u.Belongs(5));

    auto i = a.Intersection(b);
    EXPECT_TRUE(i.Belongs(3));
    EXPECT_FALSE(i.Belongs(1));
    EXPECT_FALSE(i.Belongs(5));

    auto d = a.Difference(b);
    EXPECT_TRUE(d.Belongs(1));
    EXPECT_TRUE(d.Belongs(2));
    EXPECT_FALSE(d.Belongs(3));
}

TEST(FiniteSet, SymmetricDifference)
{
    FiniteSet<int> a;
    a.Add(1);
    a.Add(2);
    a.Add(3);
    FiniteSet<int> b;
    b.Add(3);
    b.Add(4);
    b.Add(5);

    auto s = a.SymmetricDifference(b);
    EXPECT_TRUE(s.Belongs(1));
    EXPECT_TRUE(s.Belongs(5));
    EXPECT_FALSE(s.Belongs(3));
}

TEST(FiniteSet, SubsetSupersetEquality)
{
    FiniteSet<int> a;
    a.Add(1);
    a.Add(2);
    FiniteSet<int> b;
    b.Add(1);
    b.Add(2);
    b.Add(3);
    FiniteSet<int> c;
    c.Add(1);
    c.Add(2);

    EXPECT_TRUE(a.IsSubsetOf(b));
    EXPECT_FALSE(b.IsSubsetOf(a));
    EXPECT_TRUE(b.IsSupersetOf(a));
    EXPECT_TRUE(a.IsSubsetOf(c));
    EXPECT_TRUE(a == c);
    EXPECT_TRUE(a != b);
}

} // namespace Ivy::UnitTests
