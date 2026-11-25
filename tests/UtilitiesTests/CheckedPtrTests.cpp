#include "Utilities/CheckedPtr.hpp"
#include <gtest/gtest.h>

namespace Ivy::UnitTests
{
using namespace Ivy::U;

TEST(CheckedPtr, DefaultIsNull)
{
    CheckedPtr<int> p;
    EXPECT_TRUE(p.IsNull());
    EXPECT_EQ(p.Get(), nullptr);
    EXPECT_FALSE(static_cast<bool>(p));
}

TEST(CheckedPtr, ConstructFromRawPointer)
{
    int x = 42;
    CheckedPtr<int> p(&x);
    EXPECT_FALSE(p.IsNull());
    EXPECT_EQ(p.Get(), &x);
    EXPECT_TRUE(static_cast<bool>(p));
    EXPECT_EQ(*p, 42);
}

TEST(CheckedPtr, ArrowAndDeref)
{
    struct S
    {
        int a = 7;
    };
    S s;
    CheckedPtr<S> p(&s);
    EXPECT_EQ(p->a, 7);
    EXPECT_EQ((*p).a, 7);
}

TEST(CheckedPtr, Assignment)
{
    int a = 1, b = 2;
    CheckedPtr<int> p(&a);
    p = &b;
    EXPECT_EQ(p.Get(), &b);
}

TEST(CheckedPtr, Reset)
{
    int x = 42;
    CheckedPtr<int> p(&x);
    p.Reset();
    EXPECT_TRUE(p.IsNull());
    EXPECT_EQ(p.Get(), nullptr);
}

TEST(CheckedPtr, Comparisons)
{
    int a = 1, b = 2;
    CheckedPtr<int> pa(&a), pb(&b), pa2(&a);
    EXPECT_TRUE(pa == &a);
    EXPECT_FALSE(pa == &b);
    EXPECT_TRUE(pa == pa2);
    EXPECT_FALSE(pa == pb);
}

TEST(CheckedPtr, NullMessageConstructors)
{
    CheckedPtr<int> p1("custom message");
    EXPECT_TRUE(p1.IsNull());

    int x = 5;
    CheckedPtr<int> p2(&x, "another message");
    EXPECT_FALSE(p2.IsNull());
    EXPECT_EQ(*p2, 5);
}

// DOCUMENTATION: despite the name, deref of a null CheckedPtr does NOT check.
TEST(CheckedPtr, DoesNotActuallyCheckNullOnDeref)
{
    CheckedPtr<int> p;
    EXPECT_TRUE(p.IsNull());
    // Calling *p or p-> would be UB. Skip.
    SUCCEED();
}

} // namespace Ivy::UnitTests
