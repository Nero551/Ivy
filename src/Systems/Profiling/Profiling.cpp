#include "Profiling.hpp"

#include "Core/Engine.hpp"

#include <array>
#include <format>

namespace Ivy
{

void Profiling::Update(const double dt)
{
    auto& state = m_Engine.GetStates<ProfilingState>();

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

        m_Engine.Window.SetTitle(buffer.data());

        state.FrameCount = 0;
    }
}
} // namespace Ivy
