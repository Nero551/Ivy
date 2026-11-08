#include "Utilities/DataStructures/GIndexPool.hpp"
#include "Utilities/DataStructures/IndexPool.hpp"
#include <gtest/gtest.h>

namespace Ivy::UnitTests
{
using namespace Ivy::U;

// ==================== IndexPool ====================
TEST(IndexPool, FreshPoolIsEmpty)
{
    IndexPool<unsigned int> pool;
    EXPECT_TRUE(pool.Empty());
    EXPECT_EQ(pool.AcquiredSize(), 0u);
    EXPECT_EQ(pool.FullSize(), 0u);
}

TEST(IndexPool, SequentialAcquire)
{
    IndexPool<unsigned int> pool;
    EXPECT_EQ(pool.Acquire(), 0u);
    EXPECT_EQ(pool.Acquire(), 1u);
    EXPECT_EQ(pool.Acquire(), 2u);
    EXPECT_EQ(pool.FullSize(), 3u);
    EXPECT_EQ(pool.AcquiredSize(), 3u);
    EXPECT_FALSE(pool.Empty());
}

TEST(IndexPool, ReleaseThenReuse)
{
    IndexPool<unsigned int> pool;
    auto a = pool.Acquire(); // 0
    auto b = pool.Acquire(); // 1
    auto c = pool.Acquire(); // 2

    pool.Release(b);
    EXPECT_EQ(pool.AcquiredSize(), 2u);
    EXPECT_FALSE(pool.IsAcquired(b));

    auto d = pool.Acquire(); // reuses b=1
    EXPECT_EQ(d, b);
    EXPECT_EQ(pool.AcquiredSize(), 3u);
    EXPECT_TRUE(pool.IsAcquired(d));
}

TEST(IndexPool, ReleaseUnacquiredIsNoOp)
{
    IndexPool<unsigned int> pool;
    pool.Release(5);
    EXPECT_EQ(pool.AcquiredSize(), 0u);
    EXPECT_EQ(pool.FullSize(), 0u);
}

TEST(IndexPool, DoubleReleaseIsNoOp)
{
    IndexPool<unsigned int> pool;
    auto i = pool.Acquire();
    pool.Release(i);
    pool.Release(i); // ignored
    EXPECT_EQ(pool.AcquiredSize(), 0u);
    EXPECT_EQ(pool.FullSize(), 1u);
    // Only one free slot
    auto j = pool.Acquire();
    EXPECT_EQ(j, i);
    EXPECT_EQ(pool.AcquiredSize(), 1u);
}

TEST(IndexPool, ClearResetsEverything)
{
    IndexPool<unsigned int> pool;
    pool.Acquire();
    pool.Acquire();
    pool.Clear();
    EXPECT_TRUE(pool.Empty());
    EXPECT_EQ(pool.FullSize(), 0u);
    EXPECT_EQ(pool.Acquire(), 0u);
}

// ==================== GIndexPool ====================
TEST(GIndexPool, FreshPoolIsEmpty)
{
    GIndexPool<> pool;
    EXPECT_TRUE(pool.Empty());
}

TEST(GIndexPool, AcquireReturnsHandleWithGeneration1)
{
    GIndexPool<> pool;
    auto h = pool.Acquire();
    EXPECT_EQ(h.Index, 0u);
    EXPECT_EQ(h.Generation, 1u);
    EXPECT_TRUE(pool.IsAcquired(h));
}

TEST(GIndexPool, ReleaseInvalidatesHandle)
{
    GIndexPool<> pool;
    auto h = pool.Acquire();
    pool.Release(h);
    EXPECT_FALSE(pool.IsAcquired(h));
    EXPECT_EQ(pool.AcquiredSize(), 0u);
}

TEST(GIndexPool, ReleaseThenAcquireBumpsGeneration)
{
    GIndexPool<> pool;
    auto h1 = pool.Acquire(); // {0, 1}
    pool.Release(h1);
    auto h2 = pool.Acquire(); // reuses idx 0, generation 2
    EXPECT_EQ(h2.Index, 0u);
    EXPECT_EQ(h2.Generation, 2u);
    EXPECT_FALSE(pool.IsAcquired(h1)); // stale
    EXPECT_TRUE(pool.IsAcquired(h2));
}

TEST(GIndexPool, MultipleCyclesIncreaseGeneration)
{
    GIndexPool<> pool;
    auto h = pool.Acquire();
    for (int i = 0; i < 5; ++i)
    {
        pool.Release(h);
        h = pool.Acquire();
    }
    EXPECT_EQ(h.Generation, 6u); // 1 + 5 releases
}

TEST(GIndexPool, HandleEqualityIncludesGeneration)
{
    GIndexPool<>::Handle h1{0, 1};
    GIndexPool<>::Handle h2{0, 2};
    EXPECT_FALSE(h1 == h2);
    EXPECT_TRUE((h1 == GIndexPool<>::Handle{0, 1}));
}

TEST(GIndexPool, GetGenerationOutOfBoundsThrows)
{
    GIndexPool<> pool;
    EXPECT_THROW(pool.GetGeneration(100), std::exception);
}

TEST(GIndexPool, ReleaseByIndex)
{
    GIndexPool<> pool;
    auto h = pool.Acquire();
    pool.Release(h.Index); // by index
    EXPECT_FALSE(pool.IsAcquired(h));
}

} // namespace Ivy::UnitTests
