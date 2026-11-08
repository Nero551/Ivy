#pragma once

#include <concepts>
#include <type_traits>

namespace Ivy::U
{

template <typename T, template <template <auto...> typename, auto...> typename Template>
struct IsDerivedTvVSpecialization
{
  private:
    template <template <auto...> typename O, auto... Args>
    static std::true_type Check(const Template<O, Args...>*);

    static std::false_type Check(...);

  public:
    static constexpr bool value = decltype(Check(std::declval<T*>()))::value;
};

template <typename T, template <template <auto...> typename, auto...> typename Template>
concept IsDerivedTvVSpec = IsDerivedTvVSpecialization<std::remove_cvref_t<T>, Template>::value;

template <typename T, template <typename...> typename Template> struct IsTSpecialization : std::false_type
{
};

template <template <typename...> typename Template, typename... Args>
struct IsTSpecialization<Template<Args...>, Template> : std::true_type
{
};

/// @brief Checks whether a type is a specialization of a type-only template.
/// @details Removes const, volatile, and reference qualifiers before checking.
/// @tparam T The type to inspect.
/// @tparam Template The template to compare against.
template <typename T, template <typename...> typename Template>
concept IsTSpec = IsTSpecialization<std::remove_cvref_t<T>, Template>::value;

template <typename T, template <auto...> typename Template> struct IsVSpecialization : std::false_type
{
};

template <template <auto...> typename Template, auto... Args>
struct IsVSpecialization<Template<Args...>, Template> : std::true_type
{
};

/// @brief Checks whether a type is a specialization of a non-type-only template.
/// @details Removes const, volatile, and reference qualifiers before checking.
/// @tparam T The type to inspect.
/// @tparam Template The template to compare against.
template <typename T, template <auto...> typename Template>
concept IsVSpec = IsVSpecialization<std::remove_cvref_t<T>, Template>::value;

template <typename T, template <auto, typename...> typename Template>
struct IsVTSpecialization : std::false_type
{
};

template <template <auto, typename...> typename Template, auto A, typename... Args>
struct IsVTSpecialization<Template<A, Args...>, Template> : std::true_type
{
};

/// @brief Checks whether a type is a specialization of a non-type-then-type template.
/// @details Removes const, volatile, and reference qualifiers before checking.
/// @tparam T The type to inspect.
/// @tparam Template The template to compare against.
template <typename T, template <auto, typename...> typename Template>
concept IsVTSpec = IsVTSpecialization<std::remove_cvref_t<T>, Template>::value;

template <typename T, template <typename, auto...> typename Template>
struct IsTVSpecialization : std::false_type
{
};

template <template <typename, auto...> typename Template, typename A, auto... Args>
struct IsTVSpecialization<Template<A, Args...>, Template> : std::true_type
{
};

/// @brief Checks whether a type is a specialization of a type-then-non-type template.
/// @details Removes const, volatile, and reference qualifiers before checking.
/// @tparam T The type to inspect.
/// @tparam Template The template to compare against.
template <typename T, template <typename, auto...> typename Template>
concept IsTVSpec = IsTVSpecialization<std::remove_cvref_t<T>, Template>::value;

template <typename T, template <typename, auto, typename...> typename Template>
struct IsTVTSpecialization : std::false_type
{
};

template <template <typename, auto, typename...> typename Template, typename A, auto B, typename... Args>
struct IsTVTSpecialization<Template<A, B, Args...>, Template> : std::true_type
{
};

/// @brief Checks whether a type is a specialization of a type-then-non-type-then-type template.
/// @details Removes const, volatile, and reference qualifiers before checking.
/// @tparam T The type to inspect.
/// @tparam Template The template to compare against.
template <typename T, template <typename, auto, typename...> typename Template>
concept IsTVTSpec = IsTVTSpecialization<std::remove_cvref_t<T>, Template>::value;

template <typename T, template <typename, typename, auto...> typename Template>
struct IsTTVSpecialization : std::false_type
{
};

template <template <typename, typename, auto...> typename Template, typename A, typename B, auto... Args>
struct IsTTVSpecialization<Template<A, B, Args...>, Template> : std::true_type
{
};

/// @brief Checks whether a type is a specialization of a type-then-type-then-non-type template.
/// @details Removes const, volatile, and reference qualifiers before checking.
/// @tparam T The type to inspect.
/// @tparam Template The template to compare against.
template <typename T, template <typename, typename, auto...> typename Template>
concept IsTTVSpec = IsTTVSpecialization<std::remove_cvref_t<T>, Template>::value;

template <typename T, template <template <auto...> typename, auto...> typename Template>
struct IsTvVSpecialization : std::false_type
{
};

template <template <template <auto...> typename, auto...> typename Template, template <auto...> typename A,
    auto... Args>
struct IsTvVSpecialization<Template<A, Args...>, Template> : std::true_type
{
};

/// @brief Checks whether a type is a specialization of a template<non-type>-then-non-type template.
/// @details Removes const, volatile, and reference qualifiers before checking.
/// @tparam T The type to inspect.
/// @tparam Template The template to compare against.
template <typename T, template <template <auto...> typename, auto...> typename Template>
concept IsTvVSpec = IsTvVSpecialization<std::remove_cvref_t<T>, Template>::value;

template <typename T, typename Template>
concept IsSpec = std::derived_from<std::remove_cvref_t<T>, Template>;

template <typename T> struct IsTvType : std::false_type
{
};
template <template <auto...> typename Template, auto... Args>
struct IsTvType<Template<Args...>> : std::true_type
{
};

template <typename T>
concept IsTv = IsTvType<std::remove_cvref_t<T>>::value;

template <typename T, template <auto, auto, typename...> typename Template>
struct IsVVTSpecialization : std::false_type
{
};

template <template <auto, auto, typename...> typename Template, auto A, auto B, typename... Args>
struct IsVVTSpecialization<Template<A, B, Args...>, Template> : std::true_type
{
};

/// @brief Checks whether a type is a specialization of a non-type-then-non-type-then-type template.
/// @details Removes const, volatile, and reference qualifiers before checking.
/// @tparam T The type to inspect.
/// @tparam Template The template to compare against.
template <typename T, template <auto, auto, typename...> typename Template>
concept IsVVTSpec = IsVVTSpecialization<std::remove_cvref_t<T>, Template>::value;

} // namespace Ivy::U
