#pragma once
#include "material.h"

namespace pt
{
#pragma pack(push, 4)
	struct vertex
	{
		vec position;
		vec normal;
		vec2 texCoord;

		vertex() = default;
		vertex( vec position, vec normal, vec2 texCoord )
			: position(position), normal(normal.normalize()), texCoord(texCoord) {}
		bool operator==( vertex v ) { return (position == v.position && normal == v.normal && texCoord == v.texCoord); }
	};
	struct triangle
	{
	private:
		vertex m_a0, m_a1, m_a2;
		vec m_normal;
		vec m_s1, m_s2;
		vec m_u1, m_v1;
		double m_u0, m_v0;
		//long long m_isLight;
		material m_mtl;

	public:
		triangle( material Mtl, vertex v0, vertex v1, vertex v2, bool IsLight = false, bool IsFaceU = false )
			: m_a0{ v0 }, m_a1{ v1 }, m_a2{ v2 }, m_mtl{ Mtl }//, m_isLight{ IsLight }
		{
			m_normal = (v2.position - v0.position) % (v1.position - v0.position);
			m_normal.normalize();
			if (IsFaceU)
			{
				m_normal = -m_normal;
				m_a0.normal = -m_a0.normal;
				m_a1.normal = -m_a1.normal;
				m_a2.normal = -m_a2.normal;
			}
			m_s1 = m_a1.position - m_a0.position;
			m_s2 = m_a2.position - m_a0.position;
			m_u1 = (m_s1 * (m_s2 & m_s2) - m_s2 * (m_s1 & m_s2)) / ((m_s1 & m_s1) * (m_s2 & m_s2) - (m_s1 & m_s2) * (m_s1 & m_s2));
			m_v1 = (m_s2 * (m_s1 & m_s1) - m_s1 * (m_s1 & m_s2)) / ((m_s1 & m_s1) * (m_s2 & m_s2) - (m_s1 & m_s2) * (m_s1 & m_s2));
			m_u0 = m_a0.position & m_u1;
			m_v0 = m_a0.position & m_v1;
		}

		void update()
		{
			m_s1 = m_a1.position - m_a0.position;
			m_s2 = m_a2.position - m_a0.position;
			m_u1 = (m_s1 * (m_s2 & m_s2) - m_s2 * (m_s1 & m_s2)) / ((m_s1 & m_s1) * (m_s2 & m_s2) - (m_s1 & m_s2) * (m_s1 & m_s2));
			m_v1 = (m_s2 * (m_s1 & m_s1) - m_s1 * (m_s1 & m_s2)) / ((m_s1 & m_s1) * (m_s2 & m_s2) - (m_s1 & m_s2) * (m_s1 & m_s2));
			m_u0 = m_a0.position & m_u1;
			m_v0 = m_a0.position & m_v1;
		}
		vertex & V0() { return m_a0; }
		vertex & V1() { return m_a1; }
		vertex & V2() { return m_a2; }

		vec normal() const { return m_normal; }
		//bool isLight() const { return m_isLight; }
	};
#pragma pack(pop)
}
