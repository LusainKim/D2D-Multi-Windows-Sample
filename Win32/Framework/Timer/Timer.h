#pragma once



class CTimer
{
	CTimer();
	virtual ~CTimer();

	void Update();
	double GetFPS() const { return { m_dCumulativefps / static_cast<double>(m_nCumulativefpsCount) }; }
	double GetTimeElapsed() const { return m_TimeElapsed; }

protected:

	std::chrono::system_clock::time_point m_current_time;
	std::chrono::duration<float> m_timeElapsed; // 시간이 얼마나 지났나

	float		m_TimeElapsed = 0.f;

	float		m_dCumulativefps = 0.0;
	int			m_nCumulativefpsCount = 0;

	std::chrono::system_clock::time_point m_LastUpdate_time;
	std::chrono::duration<float> m_UpdateElapsed; // 시간이 얼마나 지났나

};