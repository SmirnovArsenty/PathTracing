#pragma once
#include "win.h"
#include "timer.h"
#include "input.h"
#include "render.h"

namespace pt
{
	class anim;
	class unit
	{
	public:
		virtual void response(anim*) = 0;
		virtual void render(anim*) = 0;
	};

	class anim : public win, public timer, public input, public renderer
	{
	private:
		static anim* g_instance;
		double m_shadersReloadTime;

		anim();
		~anim() = default;
		void render();

		/***
		 * Virtual functions for window customization
		 ***/

		void init() override {}
		void close() override {}
		void erase( HDC hDC ) override {}
		void paint( HDC hDC ) override {}
		void clock() override { render(); }
		void activate() override {}
		void idle() override { /*render();*/ }
	public:
		static anim * instance();
	};
}
