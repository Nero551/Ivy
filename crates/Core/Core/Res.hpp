#pragma once
#include "Utilities/IsSpecialization.hpp"
namespace Ivy::C
{
struct IRes
{
    virtual ~IRes() = default;
};

template <typename T> struct Res : T, IRes
{
    using Base = T;
};

template <typename T>
concept IsRes = U::IsTSpec<T, Res>;
} // namespace Ivy::C
