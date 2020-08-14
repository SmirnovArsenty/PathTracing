#pragma warning(disable:4267)
#include "timer.h"

using namespace pt;

timer::timer()
	: m_frameCounter{ 0 }, m_globalTime{ 0 }
	, m_globalDeltaTime{ 0 }, m_time{ 0 }, m_deltaTime{ 0 }
	, m_pauseTime{ 0 }, m_fps{ 59.30f }, m_isPause{ false }
{
	LARGE_INTEGER t;

	QueryPerformanceFrequency(&t);
	m_timePerSec = t.QuadPart;
	QueryPerformanceCounter(&t);
	m_startTime = m_oldTime = m_oldTimeFPS = t.QuadPart;
}

void timer::response()
{
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
	m_globalTime = (double)(t.QuadPart - m_startTime) / m_timePerSec;
	m_globalDeltaTime = (double)(t.QuadPart - m_oldTime) / m_timePerSec;
	if (m_isPause)
	{
		m_deltaTime = 0;
		m_pauseTime += t.QuadPart - m_oldTime;
	}
	else
	{
		m_deltaTime = m_globalDeltaTime;
		m_time = (double)(t.QuadPart - m_pauseTime - m_startTime) / m_timePerSec;
	}
	m_oldTime = t.QuadPart;
	if (t.QuadPart - m_oldTimeFPS > m_timePerSec)
	{
		m_fps = m_frameCounter * m_timePerSec / (double)(t.QuadPart - m_oldTimeFPS);
		OutputDebugString((std::to_string(m_fps) + "\n").c_str());
		m_oldTimeFPS = t.QuadPart;
		m_frameCounter = 0;
	}
	m_frameCounter++;
}
