#include "Profiling.hpp"

#include "Core/Engine.hpp"
#include <string>

namespace Ivy
{
void Profiling::Start() {}

static double elapsed = 0;

void Profiling::Update(const double dt)
{
    elapsed += dt;

    FrameMs = dt * 1000.0;
    ++FrameCount;

    if (FrameTimes.size() > 60)
    {
        FrameTimes.pop_front();
    }
    FrameTimes.push_back(dt * 1000.0);

    if (elapsed >= 1.0)
    {
        elapsed = 0.0;

        double sum = 0.0;

        for (double frame : FrameTimes)
        {
            sum += frame;
        }

        double averageFrameMs = sum / FrameTimes.size();

        C::Engine::Get().Window.SetTitle("FPS: " + std::to_string(FrameCount) + " | " +
            "AVG: " + std::to_string(averageFrameMs) + " ms" + " | " + std::to_string(FrameMs) + " ms");

        // Ivy::U::Logger::Info(
        //     "FPS: " + std::to_string(FrameCount) + " | " + "AVG: " +
        //     std::to_string(averageFrameMs) + " ms" + " | " +
        //     std::to_string(FrameMs) + " ms");
        FrameCount = 0;
    }
}
} // namespace Ivy
