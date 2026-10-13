#pragma once
#include "Math/Matrix/Matrix4.hpp"
#include "OpenGL.hpp"
#include "Uniform.hpp"

namespace Ivy::G
{
struct Matrix3Uniform : Uniform
{
    M::Matrix<3, 3> Value;

    Matrix3Uniform(const std::string& name, const M::Matrix<3, 3>& value) : Uniform(name), Value(value) {}

    void Upload(const int location) override
    {
        glUniformMatrix3fv(location, 1, GL_FALSE, &Value.Data()[0][0]);
    }
};
} // namespace Ivy::G
