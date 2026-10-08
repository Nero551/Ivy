#pragma once

namespace Ivy::P
{

/** @brief Converts an integer exponent to its Unicode superscript representation. */
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

/** @brief Provides the common compile-time interface for a dimensional type. */
template <template <int> typename Derived, int Exp> struct Dimensional : IDimensional
{
    static constexpr int Exponent = Exp;

    template <int E> using WithExponent = Derived<E>;
    template <int E> requires(Exponent % E == 0)
    using WithRoot = Derived<Exponent / E>;

    static std::ostream& Print(std::ostream& os)
    {
        return Derived<Exp>::Print(os);
    }

    //these are here just so my program doesn't bomb at compile-time (see OperationDimensional.hpp for context)
    using Left = Dimensional;
    using Right = Dimensional;
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

} // namespace Ivy::P
