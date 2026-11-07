#pragma once
#include "Utilities/Debug/Log.hpp"
#include <bitset>
namespace Ivy::ECS
{

inline unsigned int NextComponentId()
{
    static unsigned int counter = 0;
    U::Log::Assert(counter < 64, "Too many component types for Signature");
    return counter++;
}

template <typename T> inline unsigned int ComponentId()
{
    static const unsigned int id = NextComponentId();
    return id;
}

static constexpr int MaxComponentTypes = 64;
using Signature = std::bitset<MaxComponentTypes>;

template <typename... Args> Signature MakeSignature()
{
    static const Signature signature = []
    {
        Signature result;
        (result.set(ComponentId<Args>()), ...);
        return result;
    }();

    return signature;
}

struct Component
{
};
} // namespace Ivy::ECS
