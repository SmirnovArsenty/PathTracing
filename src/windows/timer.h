#pragma once
#include <string>
#include <windows.h>

namespace pt
{
	class timer
	{
	private:
		unsigned long long
			m_startTime,
			m_oldTime,
			m_oldTimeFPS,
			m_pauseTime,
			m_timePerSec,
			m_frameCounter;
		double
			m_globalTime,
			m_globalDeltaTime,
			m_time,
			m_deltaTime,
			m_fps;
		bool m_isPause;
	public:
		timer();
		void response();
		double fps() { return m_fps; }
		double globalTime() { return m_globalTime; }
		double globalDeltaTime() { return m_globalDeltaTime; }
	};
}
