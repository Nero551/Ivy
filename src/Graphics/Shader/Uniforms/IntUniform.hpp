#pragma once
#include "OpenGL.hpp"
#include "Uniform.hpp"

namespace N::G
{
struct IntUniform : Uniform
{
    int Value;

    IntUniform(const std::string& name, const int value) : Uniform(name), Value(value) {}

    void Upload(const int location) override
    {
        glUniform1i(location, Value);
    }
};
} // namespace N::G
