#pragma once

#include <deque>

#include "Core/Module.hpp"

namespace Ivy
{
struct Profiling : C::Module
{
    double FrameMs;
    int FrameCount;

  protected:
    void Start() override;

    void Update(double dt) override;

  private:
    std::deque<double> FrameTimes;
};
} // namespace Ivy
