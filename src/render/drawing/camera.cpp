#include "camera.h"
#include "anim.h"

using namespace pt;

camera::camera( unsigned int W, unsigned int H )
			: m_location(15, 15, 15)
			, m_up(0, 1, 0)
			, m_at(0, 0, 0)
			, m_width(W), m_height(H)
{
	m_direction = (m_at - m_location).normalize(),
	m_right = (m_direction % m_up).normalize(),
	m_up = (m_right % m_direction).normalize();

	if (W > H)
		m_projectionH = H * 1.0f / W;
	else
		m_projectionW = W * 1.0f / H;
}

void camera::response()
{
	if (GetAsyncKeyState('W'))
		m_location += m_direction * anim::instance()->globalDeltaTime() * 30;
	if (GetAsyncKeyState('S'))
		m_location -= m_direction * anim::instance()->globalDeltaTime() * 30;
	if (GetAsyncKeyState('A'))
		m_location -= m_right * anim::instance()->globalDeltaTime() * 30;
	if (GetAsyncKeyState('D'))
		m_location += m_right * anim::instance()->globalDeltaTime() * 30;
	if (GetAsyncKeyState(VK_LSHIFT))
		m_location += m_up * anim::instance()->globalDeltaTime() * 30;
	if (GetAsyncKeyState(VK_LCONTROL))
		m_location -= m_up * anim::instance()->globalDeltaTime() * 30;
	if (GetAsyncKeyState(VK_UP))
		m_direction += m_up * anim::instance()->globalDeltaTime();
	if (GetAsyncKeyState(VK_RIGHT))
		m_direction += m_right * anim::instance()->globalDeltaTime();
	if (GetAsyncKeyState(VK_DOWN))
		m_direction -= m_up * anim::instance()->globalDeltaTime();
	if (GetAsyncKeyState(VK_LEFT))
		m_direction -= m_right * anim::instance()->globalDeltaTime();
	m_direction.normalize();
	m_at = m_location + m_direction;
	if (GetAsyncKeyState(' '))
		m_at = vec(0.);
	m_up = vec(0, 1, 0);
	m_direction = (m_at - m_location).normalize();
	m_right = (m_direction % m_up).normalize();
	m_up = (m_right % m_direction).normalize();
}

void camera::resize(int width, int height)
{
	/* Refuse false resize */
	if (width == 0 || height == 0)
		return;
	/* Save screen resize */
	m_width = width;
	m_height = height;
	/* Calculate projection plane size */
	if (width > height)
		m_projectionW = 1.0f * width / height;
	else
		m_projectionH = 1.0f * height / width;
}

void camera::setView( const vec &loc, const vec &at, const vec &up )
{
	m_direction = (at - loc).normalize(),
	m_right = (m_direction % up).normalize(),
	m_up = (m_right % m_direction).normalize();
	m_location = loc;
	m_at = at;
}

ray camera::toRay( float Xs, float Ys )
{
	if (m_width == 0 || m_height == 0)
		return ray(m_location, m_direction);
	return ray(m_location, (m_direction * m_projDist + m_right * (Xs / m_width - 0.5f) * m_projectionW - m_up * (Ys / m_height - 0.5f) * m_projectionH));
}
