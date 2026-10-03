#pragma once
#include "Math/Matrix/Matrix4.hpp"
#include "OpenGL.hpp"
#include "Uniform.hpp"

namespace N::G
{
struct Matrix4Uniform : Uniform
{
    M::Matrix<4, 4> Value;

    Matrix4Uniform(const std::string& name, const M::Matrix<4, 4>& value) : Uniform(name), Value(value) {}

    void Upload(const int location) override
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, &Value.Data()[0][0]);
    }
};
} // namespace N::G
