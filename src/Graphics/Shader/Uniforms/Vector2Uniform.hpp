#pragma once
#include "Math/Vector/Vector2.hpp"
#include "Uniform.hpp"

namespace N::G
{
struct Vector2Uniform : Uniform
{
    N::M::Vector<2> Value;

    Vector2Uniform(const std::string& name, const N::M::Vector<2>& value) : Uniform(name), Value(value) {}

    void Upload(int location) override
    {
        glUniform2fv(location, 1, &Value.x);
    }
};
} // namespace N::G
