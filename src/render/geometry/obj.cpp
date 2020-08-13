#include "mth.h"
#include "obj.h"

using namespace pt;

obj::mtllib::mtllib(const std::string &filename)
{
	std::string fileString;
	{
		FILE *pMtlFile = fopen(filename.c_str(), "r");
		if (pMtlFile == nullptr)
			throw std::exception("couldn't find *.mtl file");
		unsigned int len;
		fseek(pMtlFile, 0, SEEK_END);
		len = ftell(pMtlFile);
		fseek(pMtlFile, 0, SEEK_SET);
		fileString.reserve(len);
		char ch{0};
		for (unsigned int i = 0; i < len; i++)
		{
			ch = fgetc(pMtlFile);
			if (ch != EOF)
				fileString += ch;
		}
		fclose(pMtlFile);
	}

	obj::mtl currentMaterial{ "default_name" };
	std::string currentMtlName{};
	for (int i = 0; i < fileString.size(); i++)
	{
		if (fileString[i] == '#') /* comment */
			while (fileString[i++] != '\n'); /* just skip it */
		else if (fileString[i] == 'n') /* newmtl */
		{
			if (currentMtlName.size() != 0)
			{
				currentMaterial.setName(currentMtlName);
				m_materials.push_back(currentMaterial);
			}
			i += 7;
			std::string mtlName{};
			while (fileString[i] != '\n' && fileString[i] != '\r')
				mtlName += fileString[i++];
			currentMtlName = mtlName;
		}
		else if (fileString[i] == 'd') /* d */
		{
			i += 2;
			std::string d;
			while (fileString[i] != '\n' && fileString[i] != '\r')
				d += fileString[i++];
			currentMaterial.d = 1 - std::stod(d);
		}
		else if (fileString[i] == 'K')
		{
			std::string r, g, b;
			char nextLetter = fileString[i + 1];
			i += 3;
			while (fileString[i] != ' ')
				r += fileString[i++];
			i++;
			while (fileString[i] != ' ')
				g += fileString[i++];
			i++;
			while (fileString[i] != '\n' && fileString[i] != '\r')
				b += fileString[i++];
			if (nextLetter == 'a') /* Ka */
			{
				currentMaterial.Ka[0] = std::stod(r);
				currentMaterial.Ka[1] = std::stod(g);
				currentMaterial.Ka[2] = std::stod(b);
			}
			else if (nextLetter == 'd') /* Kd */
			{
				currentMaterial.Kd[0] = std::stod(r);
				currentMaterial.Kd[1] = std::stod(g);
				currentMaterial.Kd[2] = std::stod(b);
			}
			else if (nextLetter == 's') /* Ks */
			{
				currentMaterial.Ks[0] = std::stod(r);
				currentMaterial.Ks[1] = std::stod(g);
				currentMaterial.Ks[2] = std::stod(b);
			}
		}
	}
	if (currentMtlName.size() != 0)
	{
		currentMaterial.setName(currentMtlName);
		m_materials.push_back(currentMaterial);
	}
}

void obj::mtllib::getMaterial(const std::string& name, obj::Geometry& out)
{
	if (name.size() == 0 || this == nullptr)
	{
		if (!out.material)
			out.material = new mtl{ "default" };
		return;
	}
	for (auto material : m_materials)
		if (material.name() == name)
		{
			out.material = new mtl{ material };
			return;
		}
}

void obj::mtllib::getMaterial(int index, mtl& out)
{
	if (index < 0 || index >= m_materials.size())
		return;
	out = m_materials[index];
}

obj::mtl obj::mtllib::getDefaultMaterial()
{
	if (m_materials.size() > 0)
		return m_materials[0];
	mtl def{ "default" };
	def.d = 1;
	def.Ka[0] = 0;
	def.Ka[1] = 0;
	def.Ka[2] = 0;
	def.Kd[0] = 0;
	def.Kd[1] = 0;
	def.Kd[2] = 0;
	def.Ks[0] = 0;
	def.Ks[1] = 0;
	def.Ks[2] = 0;
	return def;
}
int obj::mtllib::count() { return m_materials.size(); }

obj::obj(const std::string& filename)
	: m_geometries{}
	, m_mtllib{ nullptr }
{
	std::string fileString;
	{
		FILE *pObjFile = fopen(filename.c_str(), "r");
		if (pObjFile == nullptr)
			throw std::exception("couldn't find *.mtl file");
		unsigned int len;
		fseek(pObjFile, 0, SEEK_END);
		len = ftell(pObjFile);
		fseek(pObjFile, 0, SEEK_SET);
		fileString.reserve(len);
		char ch{0};
		for (unsigned int i = 0; i < len; i++)
		{
			ch = fgetc(pObjFile);
			if (ch != EOF)
				fileString += ch;
		}
		fclose(pObjFile);
	}

	std::string currentMtl{};
	for (int i = 0; i < fileString.size(); i++)
	{

		if (fileString[i] == '#') /* comment */
			while (fileString[i++] != '\n' && i < fileString.size()); /* just skip it */
		else if (fileString[i] == 'm') /* mtllib */
		{
			i += 7;
			std::string materialFilename;
			while (fileString[i] != '\r' && fileString[i] != '\n')
				materialFilename += fileString[i++];
			std::string filePath;
			for (int j = filename.size() - 1; j >= 0; j--)
				if (filename[j] == '/' || filename[j] == '\\')
				{
					int k = 0;
					for (; k <= j; k++)
						filePath += filename[k];
				}
			materialFilename = filePath + materialFilename;
			m_mtllib = new mtllib{ materialFilename };
		}
		else if (fileString[i] == 'o') /* object */
		{
			i += 2;
			if (m_geometries.size() != 0)
			{
				if (!m_geometries[m_geometries.size() - 1].material)
					m_mtllib->getMaterial(currentMtl, m_geometries[m_geometries.size() - 1]);
			}
			std::string geomName;
			while (fileString[i] != '\r' && fileString[i] != '\n')
				geomName += fileString[i++];
			m_geometries.push_back(Geometry{ geomName }); // clear geometry
		}
		else if (fileString[i] == 'v')
		{
			if (fileString[i + 1] == ' ') /* vertex */
			{
				std::string x, y, z;
				i += 2;
				while (fileString[i] != ' ')
					x += fileString[i++];
				i++;
				while (fileString[i] != ' ')
					y += fileString[i++];
				i++;
				while (fileString[i] != '\r' && fileString[i] != '\n')
					z += fileString[i++];

				vec v;
				v.x() = std::stod(x);
				v.y() = std::stod(y);
				v.z() = std::stod(z);
				m_vertices.push_back(v);
			}
			else if (fileString[i + 1] == 't') /* texture coordinates */
			{
				std::string x, y;
				i += 3;
				while (fileString[i] != ' ')
					x += fileString[i++];
				i++;
				while (fileString[i] != '\r' && fileString[i] != '\n')
					y += fileString[i++];

				vec2 v;
				v.x() = std::stod(x);
				v.y() = std::stod(y);
				m_textureCoord.push_back(v);
			}
			else if (fileString[i + 1] == 'n') /* normals */
			{
				std::string x, y, z;
				i += 3;
				while (fileString[i] != ' ')
					x += fileString[i++];
				i++;
				while (fileString[i] != ' ')
					y += fileString[i++];
				i++;
				while (fileString[i] != '\r' && fileString[i] != '\n')
					z += fileString[i++];

				vec v;
				v.x() = std::stod(x);
				v.y() = std::stod(y);
				v.z() = std::stod(z);
				m_normals.push_back(v);
			}
		}
		else if (fileString[i] == 'u') /* usemtl */
		{
			i += 7;
			currentMtl.clear();
			while (fileString[i] != '\r' && fileString[i] != '\n')
				currentMtl += fileString[i++];
			m_mtllib->getMaterial(currentMtl, m_geometries[m_geometries.size() - 1]);
		}
		else if (fileString[i] == 'f') /* face */
		{
			i += 2;
			Face face{};
			while (fileString[i] != '\n' && fileString[i] != '\r')
			{
				std::string vertexStr;
				while (fileString[i] == ' ')
					i++;
				if (fileString[i] == '\n' || fileString[i] == '\r')
					break;
				while (fileString[i] != ' ' && fileString[i] != '\n' && fileString[i] != '\r' || i >= fileString.size())
					vertexStr += fileString[i++];
				int j{ 0 };
				std::string v, t, n;
				while (vertexStr[j] != '/' && vertexStr[j] != ' ' && vertexStr[j] != '\n' && j < vertexStr.size())
					v += vertexStr[j++];
				if (vertexStr[j] != ' ' || vertexStr[j] != '\n' || j >= vertexStr.size())
				{
					j++;
					while (vertexStr.size() > j&& vertexStr[j] != '/')
						t += vertexStr[j++];
					j++;
					while (vertexStr.size() > j)
						n += vertexStr[j++];
				}
				FaceElement elem;
				if (v.size() != 0)
					elem.vertexId = std::stoi(v);
				if (t.size() != 0)
					elem.texCoordId = std::stoi(t);
				if (n.size() != 0)
					elem.normalId = std::stoi(n);
				vertex triangleVertex;
				if (elem.vertexId > 0)
					triangleVertex.position = m_vertices[elem.vertexId - 1];
				if (elem.normalId > 0)
					triangleVertex.normal = m_normals[elem.normalId - 1];
				if (elem.texCoordId > 0)
					triangleVertex.texCoord = m_textureCoord[elem.texCoordId - 1];
				face.elements.push_back(triangleVertex);
			}

			if (face.elements.size() >= 3)
			{
				vertex v0 = face.elements[0];
				for (int index = 1; index < face.elements.size() - 1; ++index)
				{
					if (m_geometries[m_geometries.size() - 1].material)
						m_geometries[m_geometries.size() - 1].triangles.push_back(
							{ toMaterial(*m_geometries[m_geometries.size() - 1].material), v0, face.elements[index], face.elements[index + 1] });
					else
						m_geometries[m_geometries.size() - 1].triangles.push_back(
							{ material::default_material(), v0, face.elements[index], face.elements[index + 1] });
				}
			}
		}
		//else if (fileString[i] == 'l') /* line */
		//{
		//	i += 2;
		//	currentGeometry.isLine = true;
		//	Line line{};
		//	QString id1;
		//	QString id2;
		//	while (fileString[i] != ' ')
		//		id1 += fileString[i++];
		//	while (fileString[i] != '\n' && fileString[i] != '\r')
		//		id2 += fileString[i++];
		//	line.points[0] = id1.toInt();
		//	line.points[1] = id2.toInt();
		//	
		//	currentGeometry.lines.push_back(line);
		//}
	}
	if (!m_geometries[m_geometries.size() - 1].material)
		m_mtllib->getMaterial(currentMtl, m_geometries[m_geometries.size() - 1]);
}

obj::~obj() { delete m_mtllib; }

std::vector<obj::Geometry> obj::geometries()
{
	return m_geometries;
}
