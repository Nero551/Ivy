#pragma once

#include "Utilities/FixedString.hpp"
namespace Ivy::P
{

template <typename... Ts> struct List;

struct Dimensionless
{
    static constexpr int Exponent = 0;
    static constexpr int Order = 0;

    template <int E> using WithExponent = Dimensionless;

    template <int E> using WithRoot = Dimensionless;

    using Left = Dimensionless;
    using Right = Dimensionless;
    using Normalized = Dimensionless;
    using Flattenend = List<>;

    static std::ostream& Print(std::ostream& os)
    {
        return os << "1";
    }
};

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

template <> struct RebuildType<List<>>
{
    using Type = Dimensionless;
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
template <template <int> typename Derived, int Exp, int Ord> struct Dimensional
{
    static constexpr int Exponent = Exp;
    static constexpr int Order = Ord;

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
    using Flattenend = List<Derived<Exp>>;
};

template <typename T> struct IsDimensionalType : std::false_type
{
};
template <template <int> typename Derived, int Exp> struct IsDimensionalType<Derived<Exp>> : std::true_type
{
};

template <template <int> typename Derived, int Exp, int Ord>
struct IsDimensionalType<Dimensional<Derived, Exp, Ord>> : std::true_type
{
};

template <> struct IsDimensionalType<Dimensionless> : std::true_type
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

template <typename A, int E> using Exponentiate = typename A::template WithExponent<E>;

template <typename A, typename B>
constexpr bool SameTerm = std::same_as<Exponentiate<A, 1>, Exponentiate<B, 1>>;

template <typename A> constexpr bool ZeroExponent = A::Exponent == 0;

template <typename A, typename B> using AddTerms = Exponentiate<A, A::Exponent + B::Exponent>;

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

template <typename T, typename O> static constexpr bool LowerThan = T::Order < O::Order;

template <typename T> struct FindSmallest;

template <typename Head> struct FindSmallest<List<Head>>
{
    using Type = Head;
};

template <typename Head, typename Next, typename... Tail> struct FindSmallest<List<Head, Next, Tail...>>
{
  private:
    using Candidate = typename FindSmallest<List<Next, Tail...>>::Type;

  public:
    // Lower Order comes first: Mass (1), Length (2), Time (3).
    using Type = std::conditional_t<LowerThan<Candidate, Head>, Candidate, Head>;
};
template <typename List, typename Target> struct RemoveFirst;

template <typename Target, typename... Tail> struct RemoveFirst<List<Target, Tail...>, Target>
{
    using Type = List<Tail...>;
};

template <typename Head, typename... Tail, typename Target> requires(!std::same_as<Head, Target>)
struct RemoveFirst<List<Head, Tail...>, Target>
{
  private:
    using Remaining = typename RemoveFirst<List<Tail...>, Target>::Type;

  public:
    using Type = typename Concat<List<Head>, Remaining>::Type;
};
template <typename T> struct Sort;

template <> struct Sort<List<>>
{
    using Type = List<>;
};

template <typename Head, typename... Tail> struct Sort<List<Head, Tail...>>
{
  private:
    using Input = List<Head, Tail...>;
    using First = typename FindSmallest<Input>::Type;
    using Remaining = typename RemoveFirst<Input, First>::Type;
    using SortedTail = typename Sort<Remaining>::Type;

  public:
    using Type = typename Concat<List<First>, SortedTail>::Type;
};

template <int Remaining, typename T> struct RemoveZeroExponents;

template <typename... Ts> struct RemoveZeroExponents<0, List<Ts...>>
{
    using Type = List<Ts...>;
};

template <int Remaining, typename Head, typename... Ts> requires(Remaining > 0)
struct RemoveZeroExponents<Remaining, List<Head, Ts...>>
{
    using Type = std::conditional_t<ZeroExponent<Head>, RemoveZeroExponents<Remaining - 1, List<Ts...>>,
        RemoveZeroExponents<Remaining - 1, List<Ts..., Head>>>::Type;
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

    using Flattenend = Concat<typename Left::Flattenend, typename Right::Flattenend>::Type;
    using Merged = Merge<Flattenend>::Type;
    using ZeroExpRemoved = RemoveZeroExponents<Merged::Size, Merged>::Type;
    using Sorted = Sort<ZeroExpRemoved>::Type;
    using Normalized = Sorted::Rebuild;

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
