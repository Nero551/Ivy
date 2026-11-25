#include "Utilities/FixedString.hpp"
#include <gtest/gtest.h>
#include <string_view>

namespace Ivy::UnitTests
{
using namespace Ivy::U;

TEST(FixedString, ConstructionFromLiteral)
{
    constexpr FixedString<6> s("hello");
    static_assert(s.Data[0] == 'h');
    static_assert(s.Data[4] == 'o');
    static_assert(s.Data[5] == '\0');
}

TEST(FixedString, EmptyCheck)
{
    constexpr FixedString<1> empty("");
    EXPECT_TRUE(empty.Empty());

    constexpr FixedString<2> oneChar("a");
    EXPECT_FALSE(oneChar.Empty());
}

TEST(FixedString, StringViewConversion)
{
    constexpr FixedString<6> s("hello");
    std::string_view sv = s;
    EXPECT_EQ(sv, "hello");
    EXPECT_EQ(sv.size(), 5u);
}

TEST(FixedString, OperatorPlus)
{
    constexpr FixedString<3> a("ab");
    constexpr FixedString<4> b("cde");
    constexpr auto combined = a + b;
    static_assert(std::is_same_v<std::remove_cvref_t<decltype(combined)>, FixedString<6>>);
    std::string_view sv = combined;
    EXPECT_EQ(sv, "abcde");
}

TEST(FixedString, OperatorPlusWithEmpty)
{
    constexpr FixedString<1> empty("");
    constexpr FixedString<3> word("hi");
    constexpr auto a = empty + word;
    constexpr auto b = word + empty;
    EXPECT_EQ(std::string_view(a), "hi");
    EXPECT_EQ(std::string_view(b), "hi");
}

TEST(FixedString, HashStringDeterministic)
{
    FixedString<6> s("hello");
    auto h1 = s.HashString("hello");
    auto h2 = s.HashString("hello");
    EXPECT_EQ(h1, h2);

    auto h3 = s.HashString("world");
    EXPECT_NE(h1, h3);
}

TEST(FixedString, UnicodeLiteral)
{
    constexpr FixedString<3> greek("α"); // 2-byte UTF-8
    std::string_view sv = greek;
    EXPECT_EQ(sv, "α");
}

} // namespace Ivy::UnitTests
