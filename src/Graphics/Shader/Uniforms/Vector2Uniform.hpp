#pragma once
#include "Math/Vector/Vector2.hpp"
#include "Uniform.hpp"

namespace Ivy::G
{
struct Vector2Uniform : Uniform
{
    M::Vector<2> Value;

    Vector2Uniform(const std::string& name, const M::Vector<2>& value) : Uniform(name), Value(value) {}

    void Upload(int location) override
    {
        glUniform2fv(location, 1, &Value.x);
    }
};
} // namespace Ivy::G
