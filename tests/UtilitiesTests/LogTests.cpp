#include "Diagnostics/Log.hpp"
#include <gtest/gtest.h>

namespace Ivy::UnitTests
{
using namespace Ivy::D;

TEST(Log, PrintDoesNotCrash)
{
    Log::Print("hello");
    Log::Print("value = ", 42, " and ", 3.14);
    SUCCEED();
}

TEST(Log, FatalThrows)
{
    EXPECT_THROW(Log::Error("boom"), std::exception);
}

TEST(Log, AssertPassesThrough)
{
    EXPECT_NO_THROW(Log::Assert(true, "should not throw"));
}

TEST(Log, AssertThrows)
{
    EXPECT_THROW(Log::Assert(false, "assertion failed"), std::exception);
}

TEST(Log, RequireReturnsValue)
{
    int x = 42;
    EXPECT_EQ(Log::Require(&x), 42);
}

TEST(Log, RequireThrowsOnNull)
{
    int* p = nullptr;
    EXPECT_THROW(Log::Require(p, "was null"), std::exception);
}

} // namespace Ivy::UnitTests
