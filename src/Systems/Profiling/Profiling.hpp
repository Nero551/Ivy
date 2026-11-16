#pragma once

#include <deque>

#include "Core/State.hpp"
#include "Core/System.hpp"

namespace Ivy
{
struct ProfilingState : C::State
{
    double FrameMs{0};
    int FrameCount{0};
    std::deque<double> FrameTimes;
    double Elapsed{0};
};

struct Profiling : C::System
{
    using C::System::System;

  protected:
    void Update(double dt) override;
};
} // namespace Ivy
