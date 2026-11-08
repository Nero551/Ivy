#pragma once
#include "Utilities/Debug/Log.hpp"
#include <bitset>
namespace Ivy::ECS
{

struct Component
{
};

static constexpr int MaxComponentTypes = 64;

inline unsigned int NextComponentId()
{
    static unsigned int counter = 0;
    U::Log::Assert(counter < MaxComponentTypes, "Too many component types for Signature");
    return counter++;
}

template <typename T> inline unsigned int ComponentId()
{
    static const unsigned int id = NextComponentId();
    return id;
}

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
} // namespace Ivy::ECS
