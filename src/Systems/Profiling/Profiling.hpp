#pragma once

#include <deque>

#include "Core/System.hpp"

namespace Ivy
{
struct Profiling : C::System
{
    using C::System::System;
    double FrameMs{0};
    int FrameCount{0};

  protected:
    void Update(double dt) override;

  private:
    std::deque<double> FrameTimes;
    double m_Elapsed{0};
};
} // namespace Ivy
