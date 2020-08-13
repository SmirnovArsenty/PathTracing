#pragma once

#include "triangle.h"

namespace pt
{
	class obj
	{
	public:
		struct mtl : public resource
		{
			double
				d,
				Ka[3],
				Kd[3],
				Ks[3];
			mtl() = delete;
			mtl(std::string name) : resource{ name }
			{
				if (name.compare("default") == 0)
				{
					d = 1;
					Ka[0] = 0;
					Ka[1] = 0;
					Ka[2] = 0;
					Kd[0] = 0;
					Kd[1] = 0;
					Kd[2] = 0;
					Ks[0] = 0;
					Ks[1] = 0;
					Ks[2] = 0;
				}
			}
		};
		class Geometry : public resource
		{
		public:
			std::vector<triangle> triangles;
			mtl *material{ nullptr };
			Geometry() = delete;
			Geometry(std::string name) : resource{ name } {}
		};
		class mtllib
		{
		private:
			std::vector<mtl> m_materials;
		public:
			mtllib() = delete;
			mtllib(const mtllib&) = delete;
			mtllib(const std::string& mtlFileName);

			void getMaterial(const std::string& name, Geometry& out);
			void getMaterial(int index, mtl& out);
			mtl getDefaultMaterial();
			int count();
		};
	private:
		struct FaceElement { int vertexId{ 0 }, texCoordId{ 0 }, normalId{ 0 }; };
		struct Face { std::vector<vertex> elements; };

		std::vector<vec> m_vertices;
		std::vector<vec2> m_textureCoord;
		std::vector<vec> m_normals;

		std::vector<Geometry> m_geometries;
		mtllib *m_mtllib;
	public:
		obj() = delete;
		obj(const obj&) = delete;
		obj(const std::string& objFileName);
		~obj();

		std::vector<Geometry> geometries();
	};

	__forceinline material toMaterial(obj::mtl& objMtl) { return material{objMtl.name(), .5, .5, objMtl.Ka}; }
}
