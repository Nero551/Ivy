#pragma once
#include "Utilities/Debug/Log.hpp"
#include <bitset>
namespace Ivy::C
{

inline unsigned int NextComponentId()
{
    static unsigned int counter = 0;
    U::Log::Assert(counter < 64, "Too many component types for Signature");
    return ++counter;
}

template <typename T> inline unsigned int ComponentId()
{
    static const unsigned int id = NextComponentId();
    return id;
}

using Signature = std::bitset<64>;

template <typename... Args> Signature MakeSignature()
{
    Signature s;
    (s.set(ComponentId<Args>()), ...);
    return s;
}

struct Component
{
};
} // namespace Ivy::C
