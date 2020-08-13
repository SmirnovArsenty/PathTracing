#pragma once
#include "mth.h"
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
			: position(position), normal(normal.normalize()), texCoord(texCoord)
		{}
		bool operator==( vertex v )
		{ return (position == v.position && normal == v.normal && texCoord == v.texCoord); }
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
		triangle( material Mtl, vertex v0, vertex v1, vertex v2, bool IsLight = false, bool IsFaceU = false );
		void update();
		inline vertex & V0() { return m_a0; }
		inline vertex & V1() { return m_a1; }
		inline vertex & V2() { return m_a2; }
		inline vec normal() const { return m_normal; }
		//bool isLight() const { return m_isLight; }
	};
#pragma pack(pop)
}
