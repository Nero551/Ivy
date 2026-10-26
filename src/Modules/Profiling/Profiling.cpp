#include "Profiling.hpp"

#include "Core/Engine.hpp"

#include <array>
#include <format>

namespace Ivy
{
void Profiling::Start() {}

void Profiling::FixedUpdate(const double dt)
{
    m_Elapsed += dt;

    FrameMs = dt * 1000.0;
    ++FrameCount;

    if (FrameTimes.size() > 60)
    {
        FrameTimes.pop_front();
    }

    FrameTimes.push_back(FrameMs);

    if (m_Elapsed >= 1.0)
    {
        m_Elapsed = 0.0;

        double sum = 0.0;

        for (const double frame : FrameTimes)
        {
            sum += frame;
        }

        const double averageFrameMs = sum / FrameTimes.size();

        std::array<char, 128> buffer;

        const auto end = std::format_to(
            buffer.begin(), "FPS: {} | AVG: {:.2f} ms | {:.2f} ms", FrameCount, averageFrameMs, FrameMs);

        *end = '\0';

        m_Engine.Window.SetTitle(buffer.data());

        FrameCount = 0;
    }
}
} // namespace Ivy
