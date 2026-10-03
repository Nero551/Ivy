#pragma once
namespace Ivy::M
{
inline static std::mt19937 rng(std::random_device{}());

inline float Random()
{
    std::uniform_real_distribution distribution(0.0f, 1.0f);
    return distribution(rng);
}

inline float Random(const float min, const float max)
{
    std::uniform_real_distribution distribution(min, max);
    return distribution(rng);
}

inline int Random(const int min, const int max)
{
    std::uniform_int_distribution distribution(min, max);
    return distribution(rng);
}

inline float RandomGaussian(const float mean, const float standardDeviation)
{
    std::normal_distribution distribution(mean, standardDeviation);
    return distribution(rng);
}

inline unsigned long RandomRaw()
{
    return rng();
}
} // namespace Ivy::M
