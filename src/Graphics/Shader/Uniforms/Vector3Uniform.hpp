#pragma once
#include "Core/Engine.hpp"
#include "Math/Vector/Vector3.hpp"
#include "Uniform.hpp"

namespace Ivy::G
{
struct Vector3Uniform : Uniform
{
    M::Vector<3> Value;

    Vector3Uniform(const std::string& name, const M::Vector<3>& value) : Uniform(name), Value(value) {}

    void Upload(const int location) override
    {
        glUniform3fv(location, 1, &Value.x);
    }
};
} // namespace Ivy::G
