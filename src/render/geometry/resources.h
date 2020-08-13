#pragma once

#include <ostream>
#include <istream>
#include <fstream>
#include <map>
#include <string>
#include <cassert>
#include <iostream>
#include <cstdio>
#include <vector>
#include <streambuf>
#include <exception>

namespace pt
{
	class resource
	{
	protected:
		std::string m_name;
	public:
		resource() = delete;
		resource(const std::string &name);
		__forceinline std::string name() const { return m_name; }
		__forceinline void setName(const std::string& name ) { m_name = name; }
	};

	class resource_manager
	{
	private:
		std::map<std::string, resource*> m_stock;
	public:
		resource_manager() = default;
		~resource_manager();
		resource_manager& add( resource *Res );
		resource* find( const std::string &name );
	};

	class shader : public resource
	{
	private:
		ID3D11VertexShader *VS = nullptr;
		ID3D11PixelShader *PS = nullptr;

		static void log(const std::string &Stage, const std::string &Text);
		static std::string loadTextFile( const std::string &FileName );

		void load();
		void unload();
		
		class renderer *m_renderer;
	public:
		shader(const shader&) = delete;
		shader(class renderer* rnd, const std::string &FileNamePrefix);
		~shader();

		void apply();
		void update();
	};

	class image : public resource
	{
	private:
		int m_width = 0, m_height = 0;
		int m_bytesPerPixel;
		mutable std::vector<BYTE> m_pixels;
		std::vector<DWORD *> m_rowsDword;
		std::vector<BYTE (*)[4]> m_rowsByte;
	public:
		image( const std::string &fileName );
		int width() const { return m_width; }
		int height() const { return m_height; }
		BYTE* pixelData() const;
	};

	class texture : public resource
	{
		int m_width, m_height;

		ID3D11Texture2D *m_tex;
	public:
		texture( const std::string &fileName );
		texture( const std::string &name, const int &width, const int &height, const int &bytesPerPixel, BYTE *m_pixels );
		texture( const image &Img );

		~texture();
		void apply();
	};
}
