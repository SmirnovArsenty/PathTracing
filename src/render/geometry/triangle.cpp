#include "triangle.h"

using namespace pt;

triangle::triangle( material Mtl, vertex v0, vertex v1, vertex v2, bool IsLight, bool IsFaceU )
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

void triangle::update()
{
	m_s1 = m_a1.position - m_a0.position;
	m_s2 = m_a2.position - m_a0.position;
	m_u1 = (m_s1 * (m_s2 & m_s2) - m_s2 * (m_s1 & m_s2)) / ((m_s1 & m_s1) * (m_s2 & m_s2) - (m_s1 & m_s2) * (m_s1 & m_s2));
	m_v1 = (m_s2 * (m_s1 & m_s1) - m_s1 * (m_s1 & m_s2)) / ((m_s1 & m_s1) * (m_s2 & m_s2) - (m_s1 & m_s2) * (m_s1 & m_s2));
	m_u0 = m_a0.position & m_u1;
	m_v0 = m_a0.position & m_v1;
}
