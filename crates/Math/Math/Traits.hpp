#pragma once
#include "Math/OperationTraits.hpp"
#include "Math/Scalar.hpp"
#include <tuple>
#include <type_traits>
namespace Ivy::M
{

template <typename T> struct VariableCountTrait;
template <M::Scalar T> struct VariableCountTrait<T> : std::integral_constant<unsigned int, 1>
{
};

template <typename... Ts>
static constexpr unsigned int VariableCount = ((VariableCountTrait<Ts>::value) + ...);

template <typename T> struct ScalarTypeTrait;
template <Scalar T> struct ScalarTypeTrait<T> : std::type_identity<T>
{
};

template <typename T> using ScalarType = ScalarTypeTrait<T>::type;

template <unsigned int Index, typename T, typename... Ts>
decltype(auto) GrabScalar(const T& value, const Ts&... values)
    requires((M::Indexable<Ts> && ...) && (!M::Scalar<Ts> && ...))
{
    if constexpr (Index < M::VariableCount<T>)
    {
        return value(Index);
    }
    else
    {
        return AcquireScalar<Index - M::VariableCount<T>>(values...);
    }
}

template <unsigned int Index, M::Scalar... Ts> decltype(auto) GrabScalar(const Ts&... scalars)
{
    return std::get<Index>(std::tuple<Ts...>{scalars...});
}

} // namespace Ivy::M
