#pragma once
namespace N::M
{

inline std::string Superscript(int exponent)
{
    static constexpr std::string_view Digits[] = {"⁰", "¹", "²", "³", "⁴", "⁵", "⁶", "⁷", "⁸", "⁹"};
    if (exponent == 1)
    {
        return "";
    }

    std::string result;

    if (exponent < 0)
    {
        result += "⁻";
        exponent = -exponent;
    }

    std::string digits = std::to_string(exponent);

    for (char digit : digits)
    {
        result += Digits[digit - '0'];
    }

    return result;
};

struct IDimensional
{
};

template <template <int> typename Derived, int Exp> struct Dimensional : IDimensional
{
    static constexpr int Exponent = Exp;
    template <int E> using WithExponent = Derived<E>;
    using Normalized = Derived<Exp>;
};

template <int Exp> struct Time : Dimensional<Time, Exp>
{
    static std::ostream& Print(std::ostream& os)
    {
        return os << "s" << Superscript(Exp);
    }
};
template <int Exp> struct Length : Dimensional<Length, Exp>
{
    static std::ostream& Print(std::ostream& os)
    {
        return os << "m" << Superscript(Exp);
    }
};
template <int Exp> struct Mass : Dimensional<Mass, Exp>
{
    static std::ostream& Print(std::ostream& os)
    {
        return os << "kg" << Superscript(Exp);
    }
};
} // namespace N::M