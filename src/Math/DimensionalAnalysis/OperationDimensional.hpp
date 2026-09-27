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
requires std::derived_from<A, IDimensional> && std::derived_from<B, IDimensional>
struct OperationDimensional;

template <typename T>
concept IsOperation = std::derived_from<T, IOperationDimensional>;

template <typename T>
concept IsTerm = !IsOperation<T>;

template <typename A, typename B>
inline constexpr bool SameTerm =
    std::same_as<typename A::template WithExponent<1>, typename B::template WithExponent<1>>;

template <typename A, typename B> using AddTerms = A::template WithExponent<A::Exponent + B::Exponent>;

// ============================================================================
// Normalization
// ============================================================================

template <typename Left, typename Right> struct OperationNormalization
{
    using Type = OperationDimensional<Left, Right>;
};

// Term * Term
template <IsTerm Left, IsTerm Right> struct OperationNormalization<Left, Right>
{
    using Type =
        std::conditional_t<SameTerm<Left, Right>, AddTerms<Left, Right>, OperationDimensional<Left, Right>>;
};

// Operation * Term
template <IsOperation Operation, IsTerm Term> struct OperationNormalization<Operation, Term>
{
  private:
    using Left = Operation::Left;
    using Right = Operation::Right;

    using MergeRight = OperationDimensional<Left, AddTerms<Right, Term>>;

    using MergeLeft = OperationDimensional<AddTerms<Left, Term>, Right>;

  public:
    using Type = std::conditional_t<SameTerm<Right, Term>, MergeRight,
        std::conditional_t<SameTerm<Left, Term>, MergeLeft, OperationDimensional<Operation, Term>>>;
};

// Term * Operation
template <IsTerm Term, IsOperation Operation> struct OperationNormalization<Term, Operation>
{
  private:
    using Left = Operation::Left;
    using Right = Operation::Right;

    using MergeRight = OperationDimensional<Left, AddTerms<Right, Term>>;

    using MergeLeft = OperationDimensional<AddTerms<Term, Left>, Right>;

  public:
    using Type = std::conditional_t<SameTerm<Right, Term>, MergeRight,
        std::conditional_t<SameTerm<Left, Term>, MergeLeft, OperationDimensional<Term, Operation>>>;
};

// Operation * Operation
template <IsOperation LeftOperation, IsOperation RightOperation>
struct OperationNormalization<LeftOperation, RightOperation>
{
  private:
    using LL = LeftOperation::Left;
    using LR = LeftOperation::Right;
    using RL = RightOperation::Left;
    using RR = RightOperation::Right;

    static constexpr bool SameLeft = SameTerm<LL, RL>;

    static constexpr bool SameRight = SameTerm<LR, RR>;

    static constexpr bool CrossLeft = SameTerm<LL, RR>;

    static constexpr bool CrossRight = SameTerm<LR, RL>;

    static constexpr bool SameStructure = SameLeft && SameRight && !SameTerm<LL, LR>;

    static constexpr bool CrossStructure = CrossLeft && CrossRight && !SameTerm<LL, LR>;

    using MergeSame = OperationDimensional<AddTerms<LL, RL>, AddTerms<LR, RR>>;

    using MergeCross = OperationDimensional<AddTerms<LL, RR>, AddTerms<LR, RL>>;

    using LeftMergedRight = OperationDimensional<AddTerms<LL, RL>, OperationDimensional<LR, RR>>;

    using RightMergedLeft = OperationDimensional<OperationDimensional<LL, RL>, AddTerms<LR, RR>>;

    using LeftCross = OperationDimensional<AddTerms<LL, RR>, OperationDimensional<LR, RL>>;

    using RightCross = OperationDimensional<OperationDimensional<LL, RR>, AddTerms<LR, RL>>;

  public:
    using Type = std::conditional_t<SameStructure, MergeSame,
        std::conditional_t<CrossStructure, MergeCross,
            std::conditional_t<SameLeft, LeftMergedRight,
                std::conditional_t<SameRight, RightMergedLeft,
                    std::conditional_t<CrossLeft, LeftCross,
                        std::conditional_t<CrossRight, RightCross,
                            OperationDimensional<LeftOperation, RightOperation>>>>>>>;
};

// ============================================================================
// Operation
// ============================================================================

/** @brief Represents a compound dimensional expression composed of two dimensional types. */
template <typename A, typename B, FixedString Name>
requires std::derived_from<A, IDimensional> && std::derived_from<B, IDimensional>
struct OperationDimensional : IOperationDimensional
{
    using Left = A::Normalized;
    using Right = B::Normalized;

    using Normalized = OperationNormalization<Left, Right>::Type;

    template <int> using WithExponent = OperationDimensional;

    static constexpr int Exponent = 1;

    static std::ostream& Print(std::ostream& os)
    {
        if constexpr (Name.Data[0] != '\0')
        {
            return os << std::string_view(Name);
        }
        else if constexpr (!IsOperation<Normalized>)
        {
            return Normalized::Print(os);
        }

        using L = Normalized::Left;
        using R = Normalized::Right;

        if constexpr (L::Exponent < 0 && R::Exponent < 0)
        {
            os << "1/(";

            L::template WithExponent<-L::Exponent>::Print(os);
            R::template WithExponent<-R::Exponent>::Print(os);

            return os << ")";
        }
        else if constexpr (L::Exponent < 0)
        {
            R::Print(os);
            os << "/";

            return L::template WithExponent<-L::Exponent>::Print(os);
        }
        else if constexpr (R::Exponent < 0)
        {
            L::Print(os);
            os << "/";

            return R::template WithExponent<-R::Exponent>::Print(os);
        }
        else
        {
            L::Print(os);
            return R::Print(os);
        }
    }
};

} // namespace N::M