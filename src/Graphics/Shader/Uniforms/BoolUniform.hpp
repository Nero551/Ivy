#pragma once
#include "OpenGL.hpp"
#include "Uniform.hpp"

namespace Ivy::G
{
struct BoolUniform : Uniform
{
    bool Value;

    BoolUniform(const std::string& name, bool value) : Uniform(name), Value(value) {}

    void Upload(int location) override
    {
        glUniform1i(location, Value);
    }
};
} // namespace Ivy::G
