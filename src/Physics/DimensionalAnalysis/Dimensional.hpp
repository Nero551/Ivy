#pragma once

#include "Utilities/FixedString.hpp"
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
template <typename... Ts> struct List;
template <typename A, typename B, U::FixedString Name = ""> struct OperationDimensional;
template <typename List> struct RebuildType;

template <typename T> struct RebuildType<List<T>>
{
    using Type = T;
};

template <typename First, typename Second, typename... Rest> struct RebuildType<List<First, Second, Rest...>>
{
  private:
    using Tail = typename RebuildType<List<Second, Rest...>>::Type;

  public:
    using Type = OperationDimensional<First, Tail>;
};

template <typename... Ts> struct List
{
    static constexpr std::size_t Size = sizeof...(Ts);
    using Rebuild = RebuildType<List<Ts...>>::Type;
};

template <typename Left, typename Right> struct Concat;
template <typename... Ls, typename... Rs> struct Concat<List<Ls...>, List<Rs...>>
{

    using Type = List<Ls..., Rs...>;
};

/** @brief Provides the common compile-time interface for a dimensional type. */
template <template <int> typename Derived, int Exp> struct Dimensional
{
    static constexpr int Exponent = Exp;

    template <int E> using WithExponent = Derived<E>;
    template <int E> requires(Exponent % E == 0)
    using WithRoot = Derived<Exponent / E>;

    // static std::ostream& Print(std::ostream& os)
    // {
    // return Derived<Exp>::Print(os);
    // }

    //these are here just so my program doesn't bomb at compile-time (see OperationDimensional.hpp for context)
    using Left = Derived<Exp>;
    using Right = Derived<Exp>;
    using Normalized = Derived<Exp>;
    using Flatten = List<Derived<Exp>>;
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

template <typename T> struct IsDimensionalType : std::false_type
{
};
template <template <int> typename Derived, int Exp> struct IsDimensionalType<Derived<Exp>> : std::true_type
{
};

template <template <int> typename Derived, int Exp>
struct IsDimensionalType<Dimensional<Derived, Exp>> : std::true_type
{
};

template <typename A, typename B, U::FixedString N>
struct IsDimensionalType<OperationDimensional<A, B, N>> : std::true_type
{
};

template <typename T>
concept IsDimensional = IsDimensionalType<T>::value;

template <typename A, typename B>
concept DimensionalPair = IsDimensional<A> && IsDimensional<B>;

template <typename T> struct IsOperationType : std::false_type
{
};

template <typename A, typename B, U::FixedString Name>
struct IsOperationType<OperationDimensional<A, B, Name>> : std::true_type
{
};

template <typename T>
concept IsOperation = IsOperationType<T>::value;

template <typename T>
concept IsTerm = !IsOperation<T>;

template <typename A, int E> using Exponentiate = typename A::template WithExponent<E>;

template <typename A, typename B>
constexpr bool SameTerm = std::same_as<Exponentiate<A, 1>, Exponentiate<B, 1>>;

template <typename A> constexpr bool ZeroExponent = A::Exponent == 0;

template <typename A, typename B> using AddTerms = Exponentiate<A, A::Exponent + B::Exponent>;

// ============================================================================
// Normalization
// ============================================================================

template <typename Left, typename Right> struct OperationNormalization
{
    using Operation = OperationDimensional<Left, Right>;

    using Type = std::conditional_t<ZeroExponent<Left> && ZeroExponent<Right>, Exponentiate<Operation, 0>,
        Exponentiate<Operation, 1>>;
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

template <int Remaining, typename List> struct FindAndMerge;

template <typename Head, typename... Ts> struct FindAndMerge<0, List<Head, Ts...>>
{
    using Type = List<Ts..., Head>;
};

template <int Remaining, typename Head> requires(Remaining > 0)
struct FindAndMerge<Remaining, List<Head>>
{
    using Type = List<Head>;
};

template <int Remaining, typename Head, typename Target, typename... Tail> requires(Remaining > 0)
struct FindAndMerge<Remaining, List<Head, Target, Tail...>>
{
    using Type = std::conditional_t<SameTerm<Head, Target>,
        FindAndMerge<Remaining - 1, List<AddTerms<Head, Target>, Tail...>>,
        FindAndMerge<Remaining - 1, List<Head, Tail..., Target>>>::Type;
};

template <int Remaining, typename List> struct FindAndMergeAll;
template <typename... Ts> struct FindAndMergeAll<0, List<Ts...>>
{
    using Type = List<Ts...>;
};

template <int Remaining, typename Head> requires(Remaining > 0)
struct FindAndMergeAll<Remaining, List<Head>>
{
    using Type = List<Head>;
};

template <int Remaining, typename Head, typename... Tail> requires(Remaining > 0)
struct FindAndMergeAll<Remaining, List<Head, Tail...>>
{

    using Type = FindAndMergeAll<Remaining - 1,
        typename FindAndMerge<List<Head, Tail...>::Size, List<Head, Tail...>>::Type>::Type;
};

template <typename T> struct Merge
{
};

template <typename... Ts> struct Merge<List<Ts...>>
{
    using Type = FindAndMergeAll<List<Ts...>::Size, List<Ts...>>::Type;
};

// ============================================================================
// Operation
// ============================================================================

/** @brief Represents a compound dimensional expression composed of two dimensional types. */
template <typename A, typename B, U::FixedString Name> requires DimensionalPair<A, B>
struct OperationDimensional<A, B, Name>
{
    using Left = A::Normalized;
    using Right = B::Normalized;

    //TODO: it appears i can't use TypeTree for this. so just define a Flatten and Rebuild specifically for this.

    using Flatten = Concat<typename Left::Flatten, typename Right::Flatten>::Type;
    using Normalized = Merge<Flatten>::Type::Rebuild;
    // using Normalized = OperationNormalization<Left, Right>::Type;
    static constexpr int Exponent = 1;

    template <int E>
    using WithExponent = OperationDimensional<Exponentiate<Left, Left::Exponent * E>,
        Exponentiate<Right, Right::Exponent * E>>;

    template <int E>
    using WithRoot =
        OperationDimensional<typename Left::template WithRoot<E>, typename Right::template WithRoot<E>>;

    static std::ostream& Print(std::ostream& os)
    {
        if constexpr (!Name.Empty())
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

            Exponentiate<L, -L::Exponent>::Print(os);
            Exponentiate<R, -R::Exponent>::Print(os);

            return os << ")";
        }
        else if constexpr (L::Exponent < 0)
        {
            R::Print(os);
            os << "/";

            return Exponentiate<L, -L::Exponent>::Print(os);
        }
        else if constexpr (R::Exponent < 0)
        {
            L::Print(os);
            os << "/";

            return Exponentiate<R, -R::Exponent>::Print(os);
        }
        else
        {
            L::Print(os);
            return R::Print(os);
        }
    }
};
} // namespace Ivy::P
