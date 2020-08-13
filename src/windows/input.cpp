#include "input.h"

using namespace pt;

input::keyboard::keyboard()
{
	memset(m_keys, 0, 256);
	memset(m_oldKeys, 0, 256);
	memset(m_keysClick, 0, 256);
}

void input::keyboard::response()
{
	GetKeyboardState(m_keys);
	for (auto &key : m_keys)
		key >>= 7;
	for (int i = 0; i < 256; i++)
		m_keysClick[i] = !m_oldKeys[i] && m_keys[i];
	memcpy(m_oldKeys, m_keys, 256);
}

input::mouse::mouse(HWND hWnd, int &MouseWheelStoreVariable)
	: m_hWnd{ hWnd }, m_mouseWheel(MouseWheelStoreVariable)
	, m_mx{ 0 }, m_my{ 0 }, m_mz{ 0 }
	, m_mdx{ 0 }, m_mdy{ 0 }, m_mdz{ 0 }
{
	memset(m_mouseButtons, 0, 3);
	memset(m_oldMouseButtons, 0, 3);
	memset(m_mouseButtonsClick, 0, 3);
}

void input::mouse::response()
{
	/* Mouse cursor position */
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd, &pt);
	m_mdx = pt.x - m_mx;
	m_mdy = pt.y - m_my;
	m_mx = pt.x;
	m_my = pt.y;

	/* Mouse wheel value */
	m_mz += m_mouseWheel;
	m_mdz = m_mouseWheel;
	m_mouseWheel = 0;

	/* Mouse buttons */
	unsigned char keys[256];
	GetKeyboardState(keys);
	m_mouseButtons[0] = keys[VK_LBUTTON] >> 7; // & 0x80
	m_mouseButtons[1] = keys[VK_RBUTTON] >> 7;
	m_mouseButtons[2] = keys[VK_MBUTTON] >> 7;

	for (int i = 0; i < 3; i++)
		m_mouseButtonsClick[i] = !m_oldMouseButtons[i] && m_mouseButtons[i];
	memcpy(m_oldMouseButtons, m_mouseButtons, 3);
}

input::joystick::joystick()
	: JPov(0)
	, JX(0), JY(0)
	, JZ(0), JR(0)
{
	memset(JBut, 0, 32);
	memset(OldJBut, 0, 32);
	memset(JButClick, 0, 32);
}

void input::joystick::response()
{
	if (joyGetNumDevs() > 0)
	{
		JOYCAPS jc;

		if (joyGetDevCaps(JOYSTICKID1, &jc, sizeof(jc)) == JOYERR_NOERROR)
		{
			JOYINFOEX ji = {sizeof(JOYINFOEX), JOY_RETURNALL};

			if (joyGetPosEx(JOYSTICKID1, &ji) == JOYERR_NOERROR)
			{
				/* Buttons */
				for (int i = 0; i < 32; i++)
					JBut[i] = (ji.dwButtons >> i) & 1;
				for (int i = 0; i < 32; i++)
					JButClick[i] = !OldJBut[i] && JBut[i];
				memcpy(OldJBut, JBut, 32);

				/* Axis */
				JX = INPUT_GET_JOYSTICK_AXIS(X);
				JY = INPUT_GET_JOYSTICK_AXIS(Y);
				JZ = INPUT_GET_JOYSTICK_AXIS(Z);
				JR = INPUT_GET_JOYSTICK_AXIS(R);

				/* Point-of-view */
				JPov = ji.dwPOV == 0xFFFF ? 0 : ji.dwPOV / 4500 + 1;
			}
		}
	}
}

input::input( HWND hWnd, int &MouseWheelStoreVariable )
	: m_keyboard{}
	, m_mouse(hWnd, MouseWheelStoreVariable)
	, m_joystick{}
{}

void input::response()
{
	m_keyboard.response();
	m_mouse.response();
	m_joystick.response();
}
