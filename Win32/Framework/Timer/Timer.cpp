#include "stdafx.h"
#include "Timer.h"

CTimer::CTimer()
{
	// 타이머 초기화
	m_LastUpdate_time = chrono::system_clock::now();
	m_current_time = chrono::system_clock::now();

	m_dCumulativefps = 0.f;
	m_nCumulativefpsCount = 0;
}

CTimer::~CTimer()
{
}

void CTimer::Update()
{
	m_timeElapsed = chrono::system_clock::now() - m_current_time;
	float fps = 0.f;
	if (m_timeElapsed.count() > MAX_FPS)
	{
		m_current_time = chrono::system_clock::now();

		if (m_timeElapsed.count() > 0.0f)
		{
			m_TimeElapsed = m_timeElapsed.count();
			fps = 1.f / m_TimeElapsed;
		}
	}
	else return;



	m_dCumulativefps += fps;
	m_nCumulativefpsCount++;

	m_UpdateElapsed = chrono::system_clock::now() - m_LastUpdate_time;
	if (m_UpdateElapsed.count() > MAX_UPDATE_FPS)
		m_LastUpdate_time = chrono::system_clock::now();
	else return;

	m_dCumulativefps = 0.0;
	m_nCumulativefpsCount = 0;
}
