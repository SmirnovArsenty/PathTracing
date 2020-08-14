#include "anim.h"

pt::anim* pt::anim::g_instance{ nullptr };

using namespace pt;

anim::anim()
	: win{}, input{ win::m_hWnd, win::m_mouseWheel }
	, renderer{ win::m_hWnd, win::m_width, win::m_height }
	, timer{}, m_shadersReloadTime{ globalTime() }
{
	g_instance = this;
}

void anim::render()
{
	if (m_isActive)
	{
		/* Update timer data */
		timer::response();

		/* Camera response */
		m_camera.response();

		input::response();
		if (globalTime() - m_shadersReloadTime > 3)
		{
			//CHAR Buf[100];
			//sprintf(Buf, "%.5f", fps());
			//for (auto s : Shaders.Stock)
				//s.second->Update();
			m_shadersReloadTime = (float)globalTime();
		}
		makeFrame();
	}
}

anim* anim::instance()
{
	if (g_instance == nullptr)
		g_instance = new anim();
	return g_instance;
}
