#include "Utilities/Debug/Inspector.hpp"
#include <gtest/gtest.h>

namespace Ivy::UnitTests
{
using namespace Ivy::U;

TEST(Inspector, DoesNotCrashOnPrintables)
{
    Inspector::Inspect(42);
    Inspector::Inspect(3.14);
    SUCCEED();
}

TEST(Inspector, DoesNotCrashOnNonPrintables)
{
    struct NotPrintable
    {
    };
    Inspector::Inspect(NotPrintable{});
    SUCCEED();
}

} // namespace Ivy::UnitTests
