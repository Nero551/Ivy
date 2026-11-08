#pragma once
#include "Math/Scalar.hpp"

#include <random>
namespace Ivy::M
{
inline std::mt19937 rng(std::random_device{}());

inline float Random()
{
    std::uniform_real_distribution distribution(0.0f, 1.0f);
    return distribution(rng);
}

template <Scalar T> inline T Random(const T min, const T max)
{
    std::uniform_real_distribution distribution(min, max);
    return distribution(rng);
}

inline int Random(const int min, const int max)
{
    std::uniform_int_distribution distribution(min, max);
    return distribution(rng);
}

template <Scalar T> inline T RandomGaussian(const T mean, const T standardDeviation)
{
    std::normal_distribution distribution(mean, standardDeviation);
    return distribution(rng);
}

inline unsigned long RandomRaw()
{
    return rng();
}
} // namespace Ivy::M
