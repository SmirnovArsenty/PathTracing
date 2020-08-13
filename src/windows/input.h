#pragma once

#include <windows.h>
#include <cstring>
#include <mmsystem.h>
#include <joystickapi.h>

#pragma comment(lib, "winmm")

#define INPUT_GET_JOYSTICK_AXIS(A) \
	(2.0 * (ji.dw ## A ## pos - jc.w ## A ## min) / (jc.w ## A ## max - jc.w ## A ## min - 1) - 1)

namespace pt
{
	class input
	{
		class keyboard
		{
		private:
			unsigned char
				m_keys[256],		/* Current key states */
				m_oldKeys[256],		/* Previous frame key states */
				m_keysClick[256];	/* Click key flags */
		public:
			keyboard();
			void response();
		} m_keyboard;

		class mouse
		{
		private:
			HWND m_hWnd;
			/* Mouse wheel delta value reference */
			int &m_mouseWheel;
			int
				m_mx, m_my,				/* Mouse cursor position */
				m_mz,					/* Mouse wheel absolute value */
				m_mdx, m_mdy, m_mdz;	/* Delta values of mouse axes */

			unsigned char
				m_mouseButtons[3],		/* Current mouse button states */
				m_oldMouseButtons[3],	/* Previous frame mouse button states */
				m_mouseButtonsClick[3];	/* Click mouse button flags */

		public:
			mouse( HWND hWnd, int &MouseWheelStoreVariable );
			void response();

			inline int x() { return m_mx; }
			inline int y() { return m_my; }
			inline int z() { return m_mz; }

			inline int deltaX() { return m_mdx; }
			inline int deltaY() { return m_mdy; }
			inline int deltaZ() { return m_mdz; }
		} m_mouse;

		class joystick
		{
		private:
			unsigned char
				JBut[32],		/* Joystick button state */
				OldJBut[32],	/* Previous frame joystick button states */
				JButClick[32];	/* Click joystick button flags */
			int JPov;			/* Joystick point-of-view control [0,1..8] */
			double
				JX, JY, JZ, JR;	/* Joystick axes */

		public:
			joystick();
			void response();
		} m_joystick;
	public:
		input( HWND hWnd, int &MouseWheelStoreVariable );
		void response();
	};
}
