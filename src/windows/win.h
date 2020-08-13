#pragma once

#include <windows.h>
#include <string>

namespace pt
{
	const std::string WindowClassName = "PathTracing WindowClass";
	class win
	{
	private:
		static win* g_win;
		const unsigned int m_initTimerId = 23, m_refreshTimerId = 53;
	protected:
		unsigned int m_width, m_height;
		HWND m_hWnd;
		BOOL m_isFullScreen;
		int m_mouseWheel{ 0 };
		BOOL m_isActive;
		BOOL m_isInit{ false };

		virtual void init() {}
		virtual void close() {}
		virtual void resize() {}
		virtual void erase( HDC hDC ) {}
		virtual void paint( HDC hDC ) {}
		virtual void clock() {}
		virtual void activate() {}
		virtual void idle() {}

		// Window messages
		BOOL OnCreate( CREATESTRUCT *CS );
		void OnDestroy();
		void OnSize( unsigned int State, int W, int H );
		BOOL OnEraseBkgnd( HDC hDC );
		void OnPaint();
		void OnActivate( unsigned int Reason, HWND hWndActDeact, bool IsMinimized );
		void OnTimer( int id );
		void FlipFullScreen();
		void OnButtonDown( bool IsDoubleClick, int m_x, int m_y, unsigned int m_keys );
		void OnButtonUp( int m_x, int m_y, unsigned int m_keys );
		void OnMouseWheel( int m_x, int m_y, int m_z, unsigned int m_keys );

		static LRESULT CALLBACK WindowFunc( HWND hWnd, unsigned int Msg, WPARAM wParam, LPARAM lParam );
	public:
		win( HINSTANCE hInstance = GetModuleHandle(NULL) );
		~win();
		int exec();
		void doExit();
	};
}
