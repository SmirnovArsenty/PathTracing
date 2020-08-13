#pragma once

#include "camera.h"
#include "triangle.h"

#include <exception>
#include <array>
#include <vector>

#pragma comment(lib, "d3d11")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dxguid")

namespace pt
{
	class renderer
	{
	public:
		/* Direct3D interfaces */
		ID3D11Device *m_device;          // Direct3D 11 device interface
		ID3D11DeviceContext *m_context;  // Direct3D 11 device context interface
		IDXGISwapChain *m_swapChain;     // DXGI swap chain interface

	private:
		/* Output render tagret */
		ID3D11RenderTargetView *m_renderTargetView; // Direct3D 11 render tagret view interface

		ID3D11Buffer *m_triangleBuffer;
		ID3D11Buffer *m_octoTreeBuffer;
		
		ID3D11Buffer *m_addData;
		ID3D11ShaderResourceView *m_shaderRVTree;
		ID3D11ShaderResourceView *m_shaderRVOcto;

		std::vector<triangle> m_triangles;

	public:
		class camera m_camera;

		renderer( HWND hWnd, unsigned int W, unsigned int H );
		~renderer();
		void makeFrame();
#if 0
		/***
		 * Resource management
		 ***/

		/* Resourses */
		resource_manager<shader> Shaders;
		resource_manager<texture> Textures;
		resource_manager<material> Materials;

		/* Create shader function.
		 * ARGUMENTS:
		 *   - texture file name prefix:
		 *       const std::string &FileNamePrefix;
		 * RETURNS:
		 *   (shader *) shader pointer.
		 */
		shader * AddShader( const std::string &FileNamePrefix )
		{
		  shader *shd;
		  if ((shd = Shaders.Find(FileNamePrefix)) != nullptr)
			return shd;
		  Shaders.Add(shd = new shader(FileNamePrefix));
		  return shd;
		} /* End of 'AddShader' function */

		/* Create texture function.
		 * ARGUMENTS:
		 *   - texture name:
		 *       const std::string &Name;
		 *   - image size:
		 *       int W, H;
		 *   - components count:
		 *       int BytesPerPixel;
		 *   - image pixel data:
		 *       BYTE *Pixels;
		 * RETURNS:
		 *   (texture *) texture pointer.
		 */
		texture * AddTexture( const std::string &Name, int W, int H, int BytesPerPixel, BYTE *Pixels )
		{
		  texture *tex;
		  if ((tex = Textures.Find(Name)) != nullptr)
			return tex;
		  Textures.Add(tex = new texture(Name, W, H, BytesPerPixel, Pixels));
		  return tex;
		} /* End of 'AddTexture' function */

		/* Create texture function.
		 * ARGUMENTS:
		 *   - texture G32/G24 file name:
		 *       const std::string &FileName;
		 * RETURNS:
		 *   (texture *) texture pointer.
		 */
		texture * AddTexture( const std::string &FileName )
		{
		  texture *tex;
		  if ((tex = Textures.Find(FileName)) != nullptr)
			return tex;
		  Textures.Add(tex = new texture(FileName));
		  return tex;
		} /* End of 'AddTexture' function */

		/* Create texture function.
		 * ARGUMENTS:
		 *   - texture name:
		 *       const std::string &Name;
		 *   - texture image:
		 *       const image &Img;
		 * RETURNS:
		 *   (texture *) texture pointer.
		 */
		texture * AddTexture( const std::string &Name, const image &Img )
		{
		  texture *tex;
		  if ((tex = Textures.Find(Name)) != nullptr)
			return tex;
		  Textures.Add(tex = new texture(Name, Img));
		  return tex;
		} /* End of 'AddTexture' function */

		/* Create material function.
		 * ARGUMENTS:
		 *   - material name:
		 *       const std::string &NewName;
		 *   - illumination coefficients (ambient. diffuse, specular):
		 *       const color3 &Ka, &Kd, &Ks;
		 *   - Phong shininess coefficient:
		 *       FLT Ph;
		 *   - transparency coefficient:
		 *       FLT Trans;
		 *   - shader pointer:
		 *       shader *Shd;
		 *   - texture 0 pointer:
		 *       texture *Tex0;
		 *   - texture 1 pointer:
		 *       texture *Tex1;
		 *   - texture 2 pointer:
		 *       texture *Tex1;
		 *   - texture 3 pointer:
		 *       texture *Tex1;
		 * RETURNS:
		 *   (material *) material pointer.
		 */
		material * AddMaterial( const std::string &Name,
								const color3 &Ka = color3(0.1), const color3 &Kd = color3(0.9), const color3 &Ks = color3(0.0),
								FLT Ph = 30, FLT Trans = 0,
								shader *Shd = nullptr,
								std::initializer_list<texture *> Textures = {} )
		{
		  material *mtl;
		  if ((mtl = Materials.Find(Name)) != nullptr)
		  {
			mtl->Ka = Ka;
			mtl->Kd = Kd;
			mtl->Ks = Ks;
			mtl->Ph = Ph;
			mtl->Trans = Trans;
			mtl->Shd = Shd;
			int i = 0;
			for (auto t : Textures)
			  mtl->Tex[i++] = t;
			return mtl;
		  }
		  Materials.Add(mtl = new material(Name, Ka, Kd, Ks, Ph, Trans, Shd, Textures));
		  return mtl;
		} /* End of 'AddMaterial' function */
#endif // 0
	};
}
