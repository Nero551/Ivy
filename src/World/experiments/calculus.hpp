#pragma once

#include "Core/World/ECS/System.hpp"

namespace Ivy
{
struct calculus : C::System
{
    void Start() override;

    void Update(double dt) override;

  private:
    void TwoDimensionalProjection(int increase);
    void ThreeDimensionalProjection(int increase);
    void FourDimensionalProjection(int increase);
};
} // namespace Ivy
