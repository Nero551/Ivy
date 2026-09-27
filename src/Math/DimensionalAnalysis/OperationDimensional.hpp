#pragma once
#include "FundamentalDimensionals.hpp"

namespace N::M
{

struct IOperationDimensional : IDimensional
{
};

template <std::size_t N> struct FixedString
{
    char Data[N];

    constexpr FixedString(const char (&string)[N])
    {
        std::copy_n(string, N, Data);
    }

    constexpr operator std::string_view() const
    {
        return {Data, N - 1};
    }
};

template <typename A, typename B, FixedString Name = "">
requires(std::derived_from<A, IDimensional> && std::derived_from<B, IDimensional>)
struct OperationDimensional;

template <typename T>
concept IsOperation = std::derived_from<T, IOperationDimensional>;
template <typename T>
concept IsTerm = !IsOperation<T>;

template <typename Left, typename Right> struct OperationNormalization
{
    using Type = OperationDimensional<Left, Right>;
};

template <IsTerm LeftTerm, IsTerm RightTerm> struct OperationNormalization<LeftTerm, RightTerm>
{
    static constexpr bool Equal = std::same_as<typename LeftTerm::template WithExponent<1>,
        typename RightTerm::template WithExponent<1>>;

    using Type = std::conditional_t<Equal,
        typename LeftTerm::template WithExponent<LeftTerm::Exponent + RightTerm::Exponent>,
        OperationDimensional<LeftTerm, RightTerm>>;
};

template <IsOperation LeftOp, IsTerm RightTerm> struct OperationNormalization<LeftOp, RightTerm>
{
    template <typename C, typename D>
    static constexpr bool Equal =
        std::same_as<typename C::template WithExponent<1>, typename D::template WithExponent<1>>;
    template <typename C, typename D> using AddTerms = C::template WithExponent<C::Exponent + D::Exponent>;

    using C1 = OperationDimensional<typename LeftOp::Left, AddTerms<typename LeftOp::Right, RightTerm>>;
    using C2 = OperationDimensional<AddTerms<typename LeftOp::Left, RightTerm>, typename LeftOp::Right>;
    using C3 = OperationDimensional<LeftOp, RightTerm>;

    using Type = std::conditional_t<Equal<typename LeftOp::Right, RightTerm>, C1,
        std::conditional_t<Equal<typename LeftOp::Left, RightTerm>, C2, C3>>;
};

template <IsTerm LeftTerm, IsOperation RightOp> struct OperationNormalization<LeftTerm, RightOp>
{
    template <typename C, typename D>
    static constexpr bool Equal =
        std::same_as<typename C::template WithExponent<1>, typename D::template WithExponent<1>>;
    template <typename C, typename D> using AddTerms = C::template WithExponent<C::Exponent + D::Exponent>;

    using C1 = OperationDimensional<typename RightOp::Left, AddTerms<typename RightOp::Right, LeftTerm>>;
    using C2 = OperationDimensional<AddTerms<LeftTerm, typename RightOp::Left>, typename RightOp::Right>;
    using C3 = OperationDimensional<LeftTerm, RightOp>;

    using Type = std::conditional_t<Equal<typename RightOp::Right, LeftTerm>, C1,
        std::conditional_t<Equal<typename RightOp::Left, LeftTerm>, C2, C3>>;
};

template <IsOperation LeftOp, IsOperation RightOp> struct OperationNormalization<LeftOp, RightOp>
{
    template <typename C, typename D>
    static constexpr bool Equal =
        std::same_as<typename C::template WithExponent<1>, typename D::template WithExponent<1>>;
    template <typename C, typename D> using AddTerms = C::template WithExponent<C::Exponent + D::Exponent>;

    static constexpr bool E1 = Equal<typename LeftOp::Left, typename RightOp::Left> &&
        Equal<typename LeftOp::Right, typename RightOp::Right> &&
        !Equal<typename LeftOp::Left, typename LeftOp::Right>;

    static constexpr bool E2 = Equal<typename LeftOp::Left, typename RightOp::Right> &&
        Equal<typename LeftOp::Right, typename RightOp::Left> &&
        !Equal<typename LeftOp::Left, typename LeftOp::Right>;

    static constexpr bool E3 = Equal<typename LeftOp::Left, typename RightOp::Left>;
    static constexpr bool E4 = Equal<typename LeftOp::Right, typename RightOp::Right>;
    static constexpr bool E5 = Equal<typename LeftOp::Left, typename RightOp::Right>;
    static constexpr bool E6 = Equal<typename LeftOp::Right, typename RightOp::Left>;

    using C1 = OperationDimensional<AddTerms<typename LeftOp::Left, typename RightOp::Left>,
        AddTerms<typename LeftOp::Right, typename RightOp::Right>>;

    using C2 = OperationDimensional<AddTerms<typename LeftOp::Left, typename RightOp::Right>,
        AddTerms<typename LeftOp::Right, typename RightOp::Left>>;

    using C3 = OperationDimensional<AddTerms<typename LeftOp::Left, typename RightOp::Left>,
        OperationDimensional<typename LeftOp::Right, typename RightOp::Right>>;

    using C4 = OperationDimensional<OperationDimensional<typename LeftOp::Left, typename RightOp::Left>,
        AddTerms<typename LeftOp::Right, typename RightOp::Right>>;

    using C5 = OperationDimensional<AddTerms<typename LeftOp::Left, typename RightOp::Right>,
        OperationDimensional<typename LeftOp::Right, typename RightOp::Left>>;

    using C6 = OperationDimensional<OperationDimensional<typename LeftOp::Left, typename RightOp::Right>,
        AddTerms<typename LeftOp::Right, typename RightOp::Left>>;

    using C7 = OperationDimensional<LeftOp, RightOp>;

    using Type = std::conditional_t<E1, C1,
        std::conditional_t<E2, C2,
            std::conditional_t<E3, C3,
                std::conditional_t<E4, C4, std::conditional_t<E5, C5, std::conditional_t<E6, C6, C7>>>>>>;
};

template <typename A, typename B, FixedString Name>
requires(std::derived_from<A, IDimensional> && std::derived_from<B, IDimensional>)
struct OperationDimensional : IOperationDimensional
{
    using Left = A::Normalized;
    using Right = B::Normalized;

    using Normalized = OperationNormalization<Left, Right>::Type;

    static std::ostream& Print(std::ostream& os)
    {
        if constexpr (Name.Data[0] != '\0')
        {
            os << std::string_view(Name);
            return os;
        }
        else if constexpr (!std::derived_from<Normalized, IOperationDimensional>)
        {
            return Normalized::Print(os);
        }
        else
        {
            if (Normalized::Left::Exponent < 0 && Normalized::Right::Exponent < 0)
            {
                os << "1/(";
                Normalized::Left::template WithExponent<-Normalized::Left::Exponent>::Print(os);
                Normalized::Right::template WithExponent<-Normalized::Right::Exponent>::Print(os);
                os << ")";
            }

            if (Normalized::Left::Exponent < 0 && Normalized::Right::Exponent > 0)
            {
                Normalized::Right::Print(os);
                os << "/";
                Normalized::Left::template WithExponent<-Normalized::Left::Exponent>::Print(os);
            }

            if (Normalized::Left::Exponent > 0 && Normalized::Right::Exponent < 0)
            {
                Normalized::Left::Print(os);
                os << "/";
                Normalized::Right::template WithExponent<-Normalized::Right::Exponent>::Print(os);
            }

            if (Normalized::Left::Exponent > 0 && Normalized::Right::Exponent > 0)
            {
                Normalized::Left::Print(os);
                Normalized::Right::Print(os);
            }

            return os;
        }
    }
    template <int E> using WithExponent = OperationDimensional;
    static constexpr int Exponent = 1;
};
} // namespace N::M