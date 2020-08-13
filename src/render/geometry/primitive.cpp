#include "mth.h"
#include "primitive.h"
#include "obj.h"

using namespace pt;

primitive::primitive(
	std::string fileName,
	std::vector<triangle> *triangles,
	vec *Min, vec *Max, bool IsLight,
	matr matrix, bool IsFaceU, bool IsFirst)
{
	auto extIter = fileName.end();
	do
	{
		if (--extIter == fileName.begin())
			throw "file without extension";
	} while (extIter[0] != '.');
	std::string ext(extIter + 1, fileName.end());
	if (ext == "obj")
	{
		obj ObjLoader(fileName);
		for (int i = 0; i < ObjLoader.geometries().size(); ++i)
		{
			obj::Geometry geom = ObjLoader.geometries()[i];
			for (auto tr : geom.triangles)
			{
				tr.V0().position = matrix * tr.V0().position;
				tr.V0().normal = matrix.transformVector(tr.V0().normal).normal();
				tr.V1().position = matrix * tr.V1().position;
				tr.V1().normal = matrix.transformVector(tr.V1().normal).normal();
				tr.V2().position = matrix * tr.V2().position;
				tr.V2().normal = matrix.transformVector(tr.V2().normal).normal();

				tr.update();

				if (IsFirst)
				{
					*Min = tr.V0().position;
					*Max = tr.V0().position;
					IsFirst = false;
				}

#define checkMinMax(v) \
	if (v.position.c_x() > Max->c_x())	Max->x() = v.position.c_x(); \
	if (v.position.c_x() < Min->c_x())	Min->x() = v.position.c_x(); \
	if (v.position.c_y() > Max->c_y())	Max->y() = v.position.c_y(); \
	if (v.position.c_y() < Min->c_y())	Min->y() = v.position.c_y(); \
	if (v.position.c_z() > Max->c_z())	Max->z() = v.position.c_z(); \
	if (v.position.c_z() < Min->c_z())	Min->z() = v.position.c_z()
				checkMinMax(tr.V0());
				checkMinMax(tr.V1());
				checkMinMax(tr.V2());
#undef checkMinMax

				triangles->push_back(tr);
			}
		}
	}
}
