#pragma once

#include <deque>

#include "Core/Module.hpp"

namespace Ivy
{
struct Profiling : C::Module
{
    using C::Module::Module;
    double FrameMs{0};
    int FrameCount{0};

  protected:
    void Start() override;

    void Update(double dt) override;

  private:
    std::deque<double> FrameTimes;
    double m_Elapsed{0};
};
} // namespace Ivy
