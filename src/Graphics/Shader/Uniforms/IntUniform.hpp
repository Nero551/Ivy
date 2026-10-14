#pragma once
#include "../../OpenGL.hpp"
#include "Uniform.hpp"

namespace Ivy::G
{
struct IntUniform : Uniform
{
    int Value;

    IntUniform(std::string_view name, const int value) : Uniform(name), Value(value) {}

    void Upload(const int location) override
    {
        glUniform1i(location, Value);
    }
};
} // namespace Ivy::G
