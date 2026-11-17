#pragma once

#include <array>
#include <deque>
#include <format>

#include "Core/Engine.hpp"
#include "Core/State.hpp"

namespace Ivy
{
struct ProfilingState : C::State
{
    double FrameMs{0};
    int FrameCount{0};
    std::deque<double> FrameTimes;
    double Elapsed{0};
};

inline void Update_Profiling(C::Engine& eng, double dt)
{
    auto& state = eng.GetStates<ProfilingState>();

    state.Elapsed += dt;
    state.FrameMs = dt * 1000.0;
    ++state.FrameCount;

    if (state.FrameTimes.size() > 60)
    {
        state.FrameTimes.pop_front();
    }
    state.FrameTimes.push_back(state.FrameMs);

    if (state.Elapsed >= 1.0)
    {
        state.Elapsed = 0.0;

        double sum = 0.0;
        for (const double frame : state.FrameTimes)
        {
            sum += frame;
        }

        const double averageFrameMs = sum / state.FrameTimes.size();

        std::array<char, 128> buffer;
        const auto end = std::format_to(buffer.begin(), "FPS: {} | AVG: {:.2f} ms | {:.2f} ms",
            state.FrameCount, averageFrameMs, state.FrameMs);
        *end = '\0';

        eng.Window.SetTitle(buffer.data());
        state.FrameCount = 0;
    }
}
} // namespace Ivy
