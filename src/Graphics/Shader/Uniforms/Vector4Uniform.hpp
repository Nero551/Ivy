#pragma once
#include "Math/Vector/Vector4.hpp"
#include "Uniform.hpp"

namespace N::G
{
struct Vector4Uniform : Uniform
{
    M::Vector<4> Value;

    Vector4Uniform(const std::string& name, const M::Vector<4>& value) : Uniform(name), Value(value) {}

    void Upload(const int location) override
    {
        glUniform4fv(location, 1, &Value.x);
    }
};
} // namespace N::G
