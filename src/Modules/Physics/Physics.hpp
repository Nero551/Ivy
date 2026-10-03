#pragma once

#include "Core/Module.hpp"

namespace N
{
struct Physics : C::Module
{
  protected:
    void Start() override;
    void FixedUpdate(double fdt) override;
};
} // namespace N
