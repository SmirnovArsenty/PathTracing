#include "win.h"

using namespace pt;
win* win::g_win{ nullptr };

BOOL win::OnCreate( CREATESTRUCT *CS )
{
	SetTimer(m_hWnd, m_initTimerId, 20, nullptr);
	return TRUE;
}

void win::OnDestroy()
{
	if (m_isInit)
	{
		m_isInit = FALSE;
		close();
		KillTimer(m_hWnd, m_refreshTimerId);
	}
	else
	{
		KillTimer(m_hWnd, m_initTimerId);
		PostQuitMessage(30);
		exit(0);
	}
	m_hWnd = nullptr;
}

void win::OnSize( unsigned int State, int W, int H )
{
	m_width = W;
	m_height = H;
	if (m_isInit)
		resize();
}

BOOL win::OnEraseBkgnd( HDC hDC )
{
	if (m_isInit)
		erase(hDC);
	return TRUE;
}

void win::OnPaint()
{
	HDC hDC;
	PAINTSTRUCT ps;

	hDC = BeginPaint(m_hWnd, &ps);
	if (m_isInit)
		paint(hDC);
	EndPaint(m_hWnd, &ps);
}

void win::OnActivate( unsigned int action, HWND hWndActDeact, bool IsMinimized )
{
	m_isActive = action == WA_CLICKACTIVE || action == WA_ACTIVE;
	if (m_isInit)
		activate();
}

void win::OnTimer( int Id )
{
	if (Id == m_initTimerId && !m_isInit)
	{
		KillTimer(m_hWnd, m_initTimerId);
		SetTimer(m_hWnd, m_refreshTimerId, 3, nullptr);
		m_isInit = TRUE;

		init();
		resize();
		activate();
		clock();
		idle();
	}
	else
		clock();
}

void win::FlipFullScreen()
{
	RECT rc;
	static RECT SaveRC;

	if (!m_isFullScreen)
	{
		HMONITOR hmon;
		MONITORINFOEX moninfo;

		GetWindowRect(m_hWnd, &SaveRC);

		hmon = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);

		moninfo.cbSize = sizeof(moninfo);
		GetMonitorInfo(hmon, (MONITORINFO *)&moninfo);

		rc = moninfo.rcMonitor;

		AdjustWindowRect(&rc, GetWindowLong(m_hWnd, GWL_STYLE), FALSE);

		SetWindowPos(m_hWnd, HWND_NOTOPMOST,
			rc.left, rc.top,
			rc.right - rc.left, rc.bottom - rc.top + 201,
			SWP_NOOWNERZORDER);
		m_isFullScreen = TRUE;
	}
	else
	{
		SetWindowPos(m_hWnd, HWND_NOTOPMOST,
			SaveRC.left, SaveRC.top,
			SaveRC.right - SaveRC.left, SaveRC.bottom - SaveRC.top,
			SWP_NOOWNERZORDER);
		m_isFullScreen = FALSE;
	}
}

void win::OnButtonDown( bool IsDoubleClick, int x, int y, unsigned int keys )
{
	SetCapture(m_hWnd);
	if (IsDoubleClick)
		FlipFullScreen();
}

void win::OnButtonUp( int m_x, int m_y, unsigned int m_keys ) { ReleaseCapture(); }
void win::OnMouseWheel( int m_x, int m_y, int m_z, unsigned int m_keys ) { m_mouseWheel += m_z; }
LRESULT CALLBACK win::WindowFunc( HWND hWnd, unsigned int Msg, WPARAM wParam, LPARAM lParam )
{
	win *Win;
	switch (Msg)
	{
	case WM_GETMINMAXINFO:
		((MINMAXINFO *)lParam)->ptMaxTrackSize.y =
			GetSystemMetrics(SM_CYMAXTRACK) +
			GetSystemMetrics(SM_CYCAPTION) +
			GetSystemMetrics(SM_CYMENU) +
			GetSystemMetrics(SM_CYBORDER) * 2;
		return 0;
	case WM_CREATE:
		g_win = (win*)((CREATESTRUCT *)lParam)->lpCreateParams;
	default:
		Win = g_win;
		if (Win != nullptr)
		switch (Msg)
		{
		case WM_CREATE:
			Win->m_hWnd = hWnd;
			return Win->OnCreate((CREATESTRUCT *)lParam) ? 0 : -1;
		case WM_CLOSE:
		case WM_DESTROY:
		case WM_LBUTTONDBLCLK:
			Win->OnDestroy();
			return 0;
		case WM_ACTIVATE:
			Win->OnActivate((unsigned int)LOWORD(wParam), (HWND)(lParam), (bool)HIWORD(wParam));
			return 0;
		case WM_ERASEBKGND:
			return (LRESULT)Win->OnEraseBkgnd((HDC)wParam);
		case WM_PAINT:
			Win->OnPaint();
			return 0;
		case WM_TIMER:
			Win->OnTimer((unsigned int)wParam);
			return 0;
		case WM_MOUSEWHEEL:
			Win->OnMouseWheel((int)(SHORT)LOWORD(lParam), (int)(SHORT)HIWORD(lParam),
				(int)(SHORT)HIWORD(wParam), (unsigned int)(SHORT)LOWORD(wParam));
			return 0;
		case WM_LBUTTONDOWN:
			Win->OnButtonDown(FALSE, (int)(SHORT)LOWORD(lParam), (int)(SHORT)HIWORD(lParam), (unsigned int)(SHORT)LOWORD(wParam));
			return 0;
		case WM_LBUTTONUP:
			Win->OnButtonUp((int)(SHORT)LOWORD(lParam), (int)(SHORT)HIWORD(lParam), (unsigned int)(SHORT)LOWORD(wParam));
			return 0;
		}
	}
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

win::win( HINSTANCE hInstance )
{
	WNDCLASS wc;

	wc.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(void *);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(hInstance, (CHAR *)IDC_ARROW);
	wc.hIcon = LoadIcon(hInstance, (CHAR *)IDI_APPLICATION);
	wc.lpszMenuName = NULL;
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WindowFunc;
	wc.lpszClassName = WindowClassName.c_str();

	if (!RegisterClass(&wc))
	{
		MessageBox(nullptr, "Error register window class", "ERROR", MB_OK);
		return;
	}
	m_hWnd =
		CreateWindow(WindowClassName.c_str(),
		"PathTracing",
		WS_OVERLAPPED,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr, nullptr, hInstance, (void *)this);

	RECT rc;
	GetWindowRect(m_hWnd, &rc);
	m_width = rc.right - rc.left;
	m_height = rc.bottom - rc.top;

	/* Show window */
	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(m_hWnd);
}

win::~win()
{
	if (m_isInit)
		close(), m_isInit = FALSE;
	if (m_hWnd != nullptr)
		DestroyWindow(m_hWnd);
}

int win::exec()
{
	try
	{
		MSG msg;
		while (TRUE)
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
				if (msg.message == WM_QUIT)
				{
					doExit();
					return 0;
				}
				else
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			else
				if (m_isInit)
					idle();
	}
	catch (...)
	{
		return -1;
	}
}

void win::doExit() { PostQuitMessage(30); }
