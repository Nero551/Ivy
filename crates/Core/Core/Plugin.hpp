#pragma once

#include <concepts>
namespace Ivy::C
{
struct Engine;
struct Plugin
{
    Plugin() = delete;
    static void Setup(Engine& engine) {}
};

template <typename T>
concept IsPlugin = std::derived_from<T, Plugin>;
} // namespace Ivy::C
