#pragma once

#include "mth.h"

namespace pt
{
	class camera
	{
	private:
		/* Projection plane depth and minimal size */
		const float m_projDist = 1;
		/* Current projection plane size */
		double m_projectionW = 1, m_projectionH = 1;
		/* Camera position and orientation */
		vec
			m_location,
			m_direction,
			m_right,
			m_up,
			m_at;
		/* Screen size */
		unsigned int m_width{ 0 }, m_height{ 0 };
	public:
		camera( unsigned int W, unsigned int H );

		void response();
		void resize( int width, int height );

		inline unsigned int width() { return m_width; }
		inline unsigned int height() { return m_height; }

		void setView( const vec &loc, const vec &at, const vec &up );
		ray toRay( float Xs, float Ys );

		inline vec location() { return m_location; };
		inline vec direction() { return m_direction; };
		inline vec right() { return m_right; };
		inline vec up() { return m_up; };
		inline vec at() { return m_at; };
		inline double projectW() { return m_projectionW; }
		inline double projectH() { return m_projectionH; }
		inline double projectDistance() { return m_projDist; }
	};
}
