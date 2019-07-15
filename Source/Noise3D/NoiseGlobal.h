
/***********************************************************************

                           h：NoiseGlobal

************************************************************************/


#pragma once

//#define NOISE_DLL_EXPORT /*_declspec(dllexport)*/

namespace Noise3D
{

	namespace D3D
	{

		//g_cXXX for Global Const XXX 
		const  UINT g_cVBstride_Default = sizeof(N_DefaultVertex);		//VertexBuffer的每个元素的字节跨度

		const  UINT g_cVBstride_Simple = sizeof(N_SimpleVertex);

		const  UINT g_cVBoffset = 0;				//VertexBuffer index offset ( 0 for start from the beginning)

		const UINT	g_VertexDesc_Default_ElementCount = 5;

		const UINT	g_VertexDesc_Simple_ElementCount = 3;

		//主渲染缓存的像素尺寸

		extern D3D_DRIVER_TYPE			g_Device_driverType;
		//
		extern D3D_FEATURE_LEVEL		g_Device_featureLevel;

		//顶点描述
		extern D3D11_INPUT_ELEMENT_DESC g_VertexDesc_Default[g_VertexDesc_Default_ElementCount];

		extern D3D11_INPUT_ELEMENT_DESC g_VertexDesc_Simple[g_VertexDesc_Simple_ElementCount];

		//—————————Global D3D Interface—————————
		extern ID3D11Device*					g_pd3dDevice11;

		extern ID3D11DeviceContext*	g_pImmediateContext;

		extern ID3DX11Effect*				g_pFX;//shader management framework's interface ,could be used by multiple classes

		//顶点布局
		extern ID3D11InputLayout*			g_pVertexLayout_Default;

		extern ID3D11InputLayout*			g_pVertexLayout_Simple;

	}
	
	//Global utility function
	namespace Ut
	{
		constexpr float PI = 3.141592653f;

		constexpr float INV_PI = 1.0f / PI;

		extern /*_declspec(dllexport)*/ bool IsPointInRect2D(Vec2 v, Vec2 vTopLeft, Vec2 vBottomRight);

		extern /*_declspec(dllexport)*/ int	GetCharAlignmentOffsetPixelY(UINT boundaryPxHeight, UINT charRealHeight, wchar_t inputChar);

		extern /*_declspec(dllexport)*/ std::string GetFileFolderFromPath(std::string completeFilePath);

		extern	/*_declspec(dllexport)*/  std::string GetFileNameFromPath(std::string completeFilePath);

		extern	/*_declspec(dllexport)*/  std::string GetFileSubFixFromPath(std::string path);

		extern /*_declspec(dllexport)*/  std::wstring ConvertAnsiStrToWStr(std::string srcStr);

		extern /*_declspec(dllexport)*/ Vec3 PixelCoordToDirection_SphericalMapping(int px, int py, int pixelWidth, int pixelHeight);//spherical mapping

		extern /*_declspec(dllexport)*/ void DirectionToPixelCoord_SphericalMapping(Vec3 dir, int pixelWidth, int pixelHeight,uint32_t& outPixelX, uint32_t& outPixelY);//spherical mapping

		extern /*_declspec(dllexport)*/void DirectionToYawPitch(Vec3 dir,float& outYaw, float& outPitch);

		extern /*_declspec(dllexport)*/ Vec3 YawPitchToDirection(float yaw, float pitch);

		extern /*_declspec(dllexport)*/ uint32_t Factorial32(uint32_t x);

		extern /*_declspec(dllexport)*/ uint64_t Factorial64(uint32_t x);

		extern /*_declspec(dllexport)*/ float ReciprocalOfFactorial(uint32_t x);//1/(x!)

		extern /*_declspec(dllexport)*/ float Lerp(float a, float b, float t);

		extern /*_declspec(dllexport)*/ Vec2 Lerp(Vec2 v1, Vec2 v2, float t);

		extern /*_declspec(dllexport)*/ Vec3 Lerp(Vec3 v1, Vec3 v2, float t);

		extern /*_declspec(dllexport)*/ float Clamp(float val, float min, float max);

		extern /*_declspec(dllexport)*/ int	Clamp(int val, int min, int max);

		extern /*declspec(dllexport)*/ double Clamp(double val, double min, double max);

		extern /*_declspec(dllexport)*/ Vec3 Clamp(const Vec3& target, const Vec3& min, const Vec3& max);

		extern /*_declspec(dllexport)*/ Vec4 Clamp(const Vec4& target, const Vec4& min, const Vec4& max);

		extern /*_declspec(dllexport)*/ Color4f Clamp(const Color4f& target, const Color4f& min, const Color4f& max);

		extern /*_declspec(dllexport)*/ Vec3 CubicHermite(const Vec3 & v1, const Vec3 & v2, const Vec3 & t1, const Vec3 & t2, float t);
	
		extern /*_declspec(dllexport)*/ bool TolerantEqual(float lhs, float rhs, float errorLimit = std::numeric_limits<float>::epsilon());

		extern /*_declspec(dllexport)*/uint32_t ComputeMipMapChainPixelCount(uint32_t mipLevel, uint32_t width, uint32_t height);
	
		extern /*_declspec(dllexport)*/bool Debug_ComPtrBatchDestructionWithHResultDebug(HRESULT hr, const std::string& MsgText,int ptrCount, IUnknown* ptrArr... );

		extern /*_declspec(dllexport)*/void Debug_ComPtrBatchDestruction(int ptrCount, IUnknown* ptrArr...);
	}


}