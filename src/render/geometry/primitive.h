#pragma once

#include <vector>
#include <string>
#include <Windows.h>
#include "triangle.h"

#pragma warning(disable : 4244)

namespace pt
{
	class primitive
	{
	public:
		primitive(
			std::string fileName,
			std::vector<triangle> *m_triangles,
			vec *Min, vec *Max, bool IsLight = false,
			matr Matrix = matr::identity(),
			bool IsFaceU = false, bool IsFirst = false );
		~primitive() = default;
	};
}
