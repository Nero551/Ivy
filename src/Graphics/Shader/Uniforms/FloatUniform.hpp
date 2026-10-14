#pragma once
#include "Core/Engine.hpp"
#include "Uniform.hpp"

namespace Ivy::G
{
struct FloatUniform : Uniform
{
    float Value;

    FloatUniform(std::string_view name, const float value) : Uniform(name), Value(value) {}

    void Upload(const int location) override
    {
        glUniform1f(location, Value);
    }
};
} // namespace Ivy::G
