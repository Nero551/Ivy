#pragma once

namespace Ivy::G
{
struct Uniform
{
    std::string_view Name;

    Uniform(std::string_view name) : Name(name) {};
    virtual ~Uniform() {}

    virtual void Upload(int location) {};
};
} // namespace Ivy::G
