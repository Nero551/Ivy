#pragma once

namespace Ivy::G
{
struct Uniform
{
    std::string Name;

    Uniform(const std::string& name) : Name(name) {};
    virtual ~Uniform() {}

    virtual void Upload(int location) {};
};
} // namespace Ivy::G
