#pragma once

namespace Ivy::C
{
struct TimeHandler
{
  private:
    double m_Time = 0.0;
    double m_DeltaTime = 0.0;
    double m_FixedDeltaTime = 1.0 / 60.0;

    double m_LastFrame = 0.0;
    double m_Accumulator = 0.0;

  public:
    double GetTime() const
    {
        return m_Time;
    }

    double GetDelta() const
    {
        return m_DeltaTime;
    }
    double GetFixedDelta() const
    {
        return m_FixedDeltaTime;
    }

    void Initialize(double currentTime)
    {
        m_LastFrame = currentTime;
        m_Time = currentTime;
        m_DeltaTime = 0.0;
        m_Accumulator = 0.0;
    }

    void CalculateTime(double currentTime)
    {
        m_DeltaTime = currentTime - m_LastFrame;
        m_LastFrame = currentTime;
        m_Time = currentTime;

        m_Accumulator += m_DeltaTime;
        m_Accumulator = std::min(m_Accumulator, 1.0);
    }

    bool ShouldFixedUpdate() const
    {
        return m_Accumulator >= m_FixedDeltaTime;
    }

    void ConsumeFixedUpdate()
    {
        m_Accumulator -= m_FixedDeltaTime;
    }
};
} // namespace Ivy::C
