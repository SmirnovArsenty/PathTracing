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

#include <d3d11_3.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <d3dcompiler.h>

#include "render.h"
#include "resources.h"
#include "anim.h"

using namespace pt;

std::string g_shadersPath{ "resources\\shaders\\" };

resource::resource(const std::string& name) : m_name(name)
{
	/// TODO: don't create new instance with same name, just return stocked from resource manager
}

resource_manager::~resource_manager()
{
	for (auto r : m_stock)
		delete r.second;
}

resource_manager& resource_manager::add(resource *res)
{
	m_stock[res->name()] = res;
	return *this;
}

resource* resource_manager::find( const std::string &name )
{
	if (m_stock.find(name) == m_stock.end())
		return nullptr;
	return m_stock[name];
}

shader::shader(renderer* rnd, const std::string& shaderName) : resource(shaderName), m_renderer(rnd)
{ load(); }

shader::~shader()
{ unload(); }

void shader::update()
{
	unload();
	load();
}

void shader::log(const std::string &Stage, const std::string &Text) { std::ofstream("SV{SHAD}.LOG", std::ios_base::app) << Stage << ":" << Text << "\n"; }

std::string shader::loadTextFile( const std::string &FileName )
{
	FILE *F;
	F = fopen(FileName.c_str(), "r");
	unsigned int len;
	fseek(F, 0, SEEK_END);
	len = ftell(F);
	fseek(F, 0, SEEK_SET);
	std::string Buf;
	char Str[2] = {0};
	for (unsigned int i = 0; i < len; i++)
	{
		Str[0] = fgetc(F);
		if (Str[0] != -1)
			Buf += Str;
	}
	fclose(F);
	return Buf;
}

void shader::load()
{
	/* Set debug flag */
	unsigned int Flags = 0;
#ifndef NDEBUG
	Flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	std::string Buf;
	ID3DBlob *VsCode, *PsCode, *Error;

	// Load and compile vertex shader
	OutputDebugString((g_shadersPath + m_name + ".vertex.hlsl shader compiling\n").c_str());
	Buf = loadTextFile(g_shadersPath + m_name + ".vertex.hlsl");
	if (D3DCompile((void *)Buf.c_str(), Buf.size(), (g_shadersPath + m_name + ".vertex").c_str(), nullptr, nullptr,
			"main", "vs_5_0", Flags, 0, &VsCode, &Error) != S_OK)
	{
		OutputDebugString((CHAR *)Error->GetBufferPointer());
		throw std::exception((CHAR *)Error->GetBufferPointer());
	}
	else
		OutputDebugString((g_shadersPath + m_name + ".vertex.hlsl shader compiled successfuly\n").c_str());

	// Load and compile pixel shader
	OutputDebugString((g_shadersPath + m_name + ".pixel.hlsl shader compiling\n").c_str());
	Buf = loadTextFile(g_shadersPath + m_name + ".pixel.hlsl");
	if (D3DCompile((void *)Buf.c_str(), Buf.size(), (g_shadersPath + m_name + ".pixel").c_str(), nullptr, nullptr,
			"main", "ps_5_0", Flags, 0, &PsCode, &Error) != S_OK)
	{
		OutputDebugString((CHAR *)Error->GetBufferPointer());
		throw std::exception((CHAR *)Error->GetBufferPointer());
	}
	else
		OutputDebugString((g_shadersPath + m_name + ".pixel.hlsl shader compiled successfuly\n").c_str());

	// Create shaders
	if (m_renderer->m_device->CreateVertexShader(VsCode->GetBufferPointer(), VsCode->GetBufferSize(),
			nullptr, &VS) != S_OK)
		throw std::exception("Error creation vertex shader");
	m_renderer->m_device->CreatePixelShader(PsCode->GetBufferPointer(), PsCode->GetBufferSize(),
		nullptr, &PS);

	VsCode->Release();
	PsCode->Release();
}

void shader::unload()
{
	VS->Release();
	PS->Release();
	VS = nullptr;
	PS = nullptr;
}

void shader::apply()
{
	m_renderer->m_context->VSSetShader(VS, nullptr, 0);
	m_renderer->m_context->PSSetShader(PS, nullptr, 0);
}

image::image( const std::string &fileName ) : resource{ fileName }
{
	HBITMAP hBm;
	if ((hBm = (HBITMAP)LoadImage(nullptr, fileName.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE)) != nullptr)
	{
		HDC hDC = GetDC(nullptr);
		HDC hMemDC = CreateCompatibleDC(hDC);
		HDC hMemDC1 = CreateCompatibleDC(hDC);
		SelectObject(hMemDC, hBm);
		ReleaseDC(nullptr, hDC);

		BITMAP bm;
		GetObject(hBm, sizeof(bm), &bm);

		BITMAPINFOHEADER bmih;

		m_width = bm.bmWidth;
		m_height = bm.bmHeight;

		/* Build DIB section */
		memset(&bmih, 0, sizeof(bmih));
		bmih.biSize = sizeof(BITMAPINFOHEADER);
		bmih.biBitCount = 32;                   /* BGRx - DWORD */
		bmih.biPlanes = 1;                      /* always */
		bmih.biCompression = BI_RGB;            /* 0 - no compression */
		bmih.biWidth = bm.bmWidth;              /* width */
		bmih.biHeight = -bm.bmHeight;           /* height + first coded row is 0 */
		bmih.biSizeImage = bm.bmWidth * bm.bmHeight * 4;  /* image size in bytes */

		void *Bits;
		HBITMAP hImage = CreateDIBSection(nullptr, (BITMAPINFO *)&bmih, DIB_RGB_COLORS,
							(void **)&Bits, nullptr, 0);
		SelectObject(hMemDC1, hImage);

		BitBlt(hMemDC1, 0, 0, m_width, m_height, hMemDC, 0, 0, SRCCOPY);
		DeleteDC(hMemDC);
		DeleteDC(hMemDC1);

		m_pixels.resize(bmih.biSizeImage);
		CopyMemory(&m_pixels[0], Bits, bmih.biSizeImage);
		DeleteObject(hImage);

		// Make alpha channel
		for (int i = 3; i < m_width * m_height * 4; i += 4)
			m_pixels[i] = 255;
		}
	// Setup row pointers
	int i;
	m_rowsDword.resize(m_height);
	i = 0;
	for (auto &r : m_rowsDword)
		r = (DWORD *)&m_pixels[i++ * m_width * 4];
	m_rowsByte.resize(m_height);
	i = 0;
	for (auto &r : m_rowsByte)
		r = (BYTE (*)[4])&m_pixels[i++ * m_width * 4];
}

BYTE* image::pixelData() const { return m_pixels.data(); }

texture::texture( const std::string &fileName )
			: resource{ fileName }, m_tex{ nullptr }
{
	std::vector<BYTE> img;
	FILE *F;
	F = fopen(fileName.c_str(), "rb");
	unsigned int len;
	fseek(F, 0, SEEK_END);
	len = ftell(F);
	fseek(F, 0, SEEK_SET);
	fread(&img[0], sizeof(BYTE), len, F);
	fclose(F);
	int BytesPerPixel;
	BYTE *pixels;
		m_width = img[0] + (img[1] << 8),
		m_height = img[2] + (img[3] << 8);
	if (img.size() == 4 + m_width * m_height * 3)
		BytesPerPixel = 3;
	else
		BytesPerPixel = 4;
	pixels = &img[4];
	texture::texture(fileName, m_width, m_height, BytesPerPixel, pixels);
}
texture::texture( const std::string &name, const int &width, const int &height, const int &bytesPerPixel, BYTE *pixels )
	: resource(name), m_width{ width }, m_height{ height }
{
	D3D11_TEXTURE2D_DESC TexDescr = {0};

	TexDescr.Width = width;
	TexDescr.Height = height;
	TexDescr.MipLevels = 1;
	TexDescr.ArraySize = 1;
	TexDescr.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	TexDescr.Usage = D3D11_USAGE_DEFAULT;
	TexDescr.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TexDescr.SampleDesc.Count = 1;
	TexDescr.SampleDesc.Quality = 0;

	D3D11_SUBRESOURCE_DATA Data;
	Data.pSysMem = (void *)pixels;
	Data.SysMemPitch = width * bytesPerPixel;
	Data.SysMemSlicePitch = 0;
	if (anim::instance()->m_device->CreateTexture2D(&TexDescr, &Data, &m_tex))
		throw std::exception("Error create texture interface");
}

texture::texture( const image &img )
	: resource{ img.name() + "_tex_from_image" }
	, m_width{ img.width() }, m_height{ img.height() }
{
	D3D11_TEXTURE2D_DESC TexDescr = {0};

	TexDescr.Width = m_width;
	TexDescr.Height = m_height;
	TexDescr.MipLevels = 1;
	TexDescr.ArraySize = 1;
	TexDescr.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	TexDescr.Usage = D3D11_USAGE_DEFAULT;
	TexDescr.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TexDescr.SampleDesc.Count = 1;
	TexDescr.SampleDesc.Quality = 0;

	D3D11_SUBRESOURCE_DATA Data;
	Data.pSysMem = (void *)img.pixelData();
	Data.SysMemPitch = m_width * 4 * sizeof BYTE;
	Data.SysMemSlicePitch = 0;
	if (anim::instance()->m_device->CreateTexture2D(&TexDescr, &Data, &m_tex))
		throw std::exception("Error create texture interface");
}

texture::~texture()
{
	if (m_tex != nullptr)
		m_tex->Release();
}

void texture::apply()
{
	///
}
