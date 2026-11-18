#pragma once
#include <concepts>
namespace Ivy::C
{
struct IRes
{
};

template <typename T> struct Res : T, IRes
{
    using Base = T;
};

template <typename T>
concept IsRes = std::derived_from<T, C::IRes>;
} // namespace Ivy::C
