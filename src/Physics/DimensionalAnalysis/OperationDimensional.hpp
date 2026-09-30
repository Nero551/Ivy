#pragma once

#include "FundamentalDimensionals.hpp"

namespace N::P
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
constexpr bool SameTerm =
    std::same_as<typename A::template WithExponent<1>, typename B::template WithExponent<1>>;

template <typename A> constexpr bool ZeroExponent = A::Exponent == 0;
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
    using Type = std::conditional_t<SameTerm<Left, Right>, AddTerms<Left, Right>,
        std::conditional_t<ZeroExponent<Left>, Right,
            std::conditional_t<ZeroExponent<Right>, Left, OperationDimensional<Left, Right>>>>;
};

// Operation * Term
template <IsOperation Operation, IsTerm Term> struct OperationNormalization<Operation, Term>
{
  private:
    using Left = Operation::Left;
    using Right = Operation::Right;

    using MergeRight = OperationDimensional<Left, AddTerms<Right, Term>>;
    using MergeLeft = OperationDimensional<AddTerms<Left, Term>, Right>;
    using ZeroExpMerge = OperationDimensional<Left, Right>;
    using ZeroExpOpMerge = Term;

  public:
    using Type = std::conditional_t<ZeroExponent<Left> && ZeroExponent<Right>, ZeroExpOpMerge,
        std::conditional_t<SameTerm<Right, Term>,
            std::conditional_t<ZeroExponent<Term>, ZeroExpMerge, MergeRight>,
            std::conditional_t<SameTerm<Left, Term>,
                std::conditional_t<ZeroExponent<Term>, ZeroExpMerge, MergeLeft>,
                std::conditional_t<ZeroExponent<Term>, ZeroExpMerge,
                    OperationDimensional<Term, Operation>>>>>;
};

// Term * Operation
template <IsTerm Term, IsOperation Operation> struct OperationNormalization<Term, Operation>
{
  private:
    using Left = Operation::Left;
    using Right = Operation::Right;

    using MergeRight = OperationDimensional<Left, AddTerms<Right, Term>>;
    using MergeLeft = OperationDimensional<AddTerms<Term, Left>, Right>;
    using ZeroExpMerge = OperationDimensional<Left, Right>;
    using ZeroExpOpMerge = Term;

  public:
    using Type = std::conditional_t<ZeroExponent<Left> && ZeroExponent<Right>, ZeroExpOpMerge,
        std::conditional_t<SameTerm<Right, Term>,
            std::conditional_t<ZeroExponent<Term>, ZeroExpMerge, MergeRight>,
            std::conditional_t<SameTerm<Left, Term>,
                std::conditional_t<ZeroExponent<Term>, ZeroExpMerge, MergeLeft>,
                std::conditional_t<ZeroExponent<Term>, ZeroExpMerge,
                    OperationDimensional<Term, Operation>>>>>;
};

// Operation * Operation
template <IsOperation LeftOperation, IsOperation RightOperation>
struct OperationNormalization<LeftOperation, RightOperation>
{
  private:
    using R = RightOperation;
    using L = LeftOperation;
    using LL = L::Left;
    using LR = L::Right;
    using RL = R::Left;
    using RR = R::Right;

    static constexpr bool LLR = SameTerm<typename LL::Left, RL> && SameTerm<typename LL::Right, RR>;
    static constexpr bool LRR = SameTerm<typename LR::Left, RL> && SameTerm<typename LR::Right, RR>;
    static constexpr bool RLL = SameTerm<typename RL::Left, LL> && SameTerm<typename RL::Right, LR>;
    static constexpr bool RRL = SameTerm<typename RR::Left, LL> && SameTerm<typename RR::Right, LR>;

    static constexpr bool LL_RL = SameTerm<LL, RL>;
    static constexpr bool LR_RR = SameTerm<LR, RR>;
    static constexpr bool LL_RR = SameTerm<LL, RR>;
    static constexpr bool LR_RL = SameTerm<LR, RL>;
    static constexpr bool LL_RL_LR_RR = LL_RL && LR_RR && !SameTerm<LL, LR>;
    static constexpr bool LL_RR_LR_RL = LL_RR && LR_RL && !SameTerm<LL, LR>;

    using Add_LL_RL_LR_RR = OperationDimensional<AddTerms<LL, RL>, AddTerms<LR, RR>>;
    using Add_LL_RR_LR_RL = OperationDimensional<AddTerms<LL, RR>, AddTerms<LR, RL>>;
    using Add_LL_RL = OperationDimensional<AddTerms<LL, RL>, OperationDimensional<LR, RR>>;
    using Add_LR_RR = OperationDimensional<OperationDimensional<LL, RL>, AddTerms<LR, RR>>;
    using Add_LL_RR = OperationDimensional<AddTerms<LL, RR>, OperationDimensional<LR, RL>>;
    using Add_LR_RL = OperationDimensional<OperationDimensional<LL, RR>, AddTerms<LR, RL>>;

    using Add_LLR = OperationDimensional<OperationDimensional<AddTerms<typename LL::Left, typename R::Left>,
                                             AddTerms<typename LL::Right, typename R::Right>>,
        LR>;

    using Add_LRR = OperationDimensional<LL,
        OperationDimensional<AddTerms<typename LR::Left, typename R::Left>,
            AddTerms<typename LR::Right, typename R::Right>>>;

    using Add_RLL = OperationDimensional<OperationDimensional<AddTerms<typename L::Left, typename RL::Left>,
                                             AddTerms<typename L::Right, typename RL::Right>>,
        RR>;

    using Add_RRL = OperationDimensional<OperationDimensional<AddTerms<typename L::Left, typename RR::Left>,
                                             AddTerms<typename L::Right, typename RR::Right>>,
        RL>;

  public:
    using Type = std::conditional_t<ZeroExponent<RL> && ZeroExponent<RR>, L,
        std::conditional_t<ZeroExponent<LL> && ZeroExponent<LR>, R,
            std::conditional_t<RRL, Add_RRL,
                std::conditional_t<RLL, Add_RLL,
                    std::conditional_t<LLR, Add_LLR,
                        std::conditional_t<LRR, Add_LRR,
                            std::conditional_t<LL_RL_LR_RR, Add_LL_RL_LR_RR,
                                std::conditional_t<LL_RR_LR_RL, Add_LL_RR_LR_RL,
                                    std::conditional_t<LL_RL, Add_LL_RL,
                                        std::conditional_t<LR_RR, Add_LR_RR,
                                            std::conditional_t<LL_RR, Add_LR_RR,
                                                std::conditional_t<LR_RL, Add_LR_RL,
                                                    OperationDimensional<L, R>>>>>>>>>>>>>;
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

    static constexpr int Exponent = 1;
    template <int E>
    using WithExponent = OperationDimensional<typename Left::template WithExponent<Left::Exponent * E>,
        typename Right::template WithExponent<Right::Exponent * E>>;

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

} // namespace N::P