#pragma once
#include "Core/Engine.hpp"
#include "Uniform.hpp"

namespace N::G
{
struct FloatUniform : Uniform
{
    float Value;

    FloatUniform(const std::string& name, const float value) : Uniform(name), Value(value) {}

    void Upload(const int location) override
    {
        glUniform1f(location, Value);
    }
};
} // namespace N::G
