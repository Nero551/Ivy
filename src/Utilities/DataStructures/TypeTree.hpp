#pragma once
namespace Ivy::U
{
struct TypeTree
{
    template <typename... T> struct List;
    template <typename Left, typename Right> struct Node;

  private:
    template <typename List> struct Rebuild;

    template <typename T> struct Rebuild<List<T>>
    {
        using Type = T;
    };

    template <typename First, typename Second, typename... Rest> struct Rebuild<List<First, Second, Rest...>>
    {
      private:
        using Tail = typename Rebuild<List<Second, Rest...>>::Type;

      public:
        using Type = Node<First, Tail>;
    };

    template <typename List, typename T> struct Append;

    template <typename... Ts, typename T> struct Append<List<Ts...>, T>
    {
        using Type = List<Ts..., T>;
    };

  public:
    template <typename Left, typename Right> struct Concat;
    template <typename... Ls, typename... Rs> struct Concat<List<Ls...>, List<Rs...>>
    {

        using Type = List<Ls..., Rs...>;
    };

    template <typename... Ts> struct List
    {
        static constexpr std::size_t Size = sizeof...(Ts);
        using Rebuild = Rebuild<List<Ts...>>::Type;
        template <typename T> using Append = Append<List<Ts...>, T>::Type;
    };

    template <typename T> struct Leaf
    {
        using Type = T;
        using Flatten = List<T>;
        using Left = Leaf;
        using Right = Leaf;
    };

    template <typename L, typename R> struct Node
    {
        using Left = L;
        using Right = R;

        using Flatten = typename Concat<typename Left::Flatten, typename Right::Flatten>::Type;
    };

    TypeTree() = delete;
    ~TypeTree() = delete;
    TypeTree& operator=(TypeTree&) = delete;
};

} // namespace Ivy::U
