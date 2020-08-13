#pragma once
#include "resources.h"

namespace pt
{
	#pragma pack(push, 1)
	class material
	{
	private:
		double m_metalness;
		double m_roughness;
		vec m_color;
	
	public:
		material( std::string name, double metalness, double roughness, vec color )
			: m_metalness{ metalness }
			, m_roughness{ roughness }
			, m_color{ color }
		{}

		static material default_material();
	};
	#pragma pack(pop)
}
