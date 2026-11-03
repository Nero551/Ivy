#pragma once

#include <functional>
namespace Ivy::stl
{
template <typename Signature> using function = std::copyable_function<Signature>;
} // namespace Ivy::stl
