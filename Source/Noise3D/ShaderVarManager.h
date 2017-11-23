
/***********************************************************************

                           h£ºShader Variable Manager
			Desc: WHenever I want to create a new constant buffer
			in shader(s), i had to create a struct with identical memory 
			layout in c++, then define one of this structure in Renderer
			then fill up those cbuffer struct in memory. Moreover,
			modification of shader cbuffer requires an even more 
			complicated cascaded modification in c++.s

			Thus I want to develop a shader variable manager, making 
			full use of Effect11 framework and ignore the concept of 
			"cbuffer" in c++. then variable is the unit for updating data
			to gpu. No more "identical struct" in c++ needs to define. 

			1, inherited by classes (PRIVATE inherit) which need to manipulate shaders
			2, provide shader variable update/retrieval service
			3, internal/fixed variables can be updated via given interfaces

************************************************************************/

#pragma once

namespace Noise3D
{

	class /*_declspec(dllexport)*/ IShaderVariableManager
	{
	public:
		
		//init must be POSTPONED because the initialization of renderer is triggered
		//by user, hence init op will not be done in constructor
		bool	Init();

		//set general variables
		void SetVar(const char* var, void* pVal,int size);

		//set Array
		void	SetStaticDirLight(int index, const N_DirLightDesc& staticLightDesc );

		void	SetStaticPointLight(int index, const N_PointLightDesc& staticLightDesc);

		void	SetStaticSpotLight(int index, const N_SpotLightDesc& staticLightDesc);

		void	SetDynamicDirLight(int index, const N_DirLightDesc& dynamicLightDesc);

		void	SetDynamicPointLight(int index, const N_PointLightDesc& dynamicLightDesc);

		void	SetDynamicSpotLight(int index, const N_SpotLightDesc& dynamicLightDesc);


	private:

		friend IRenderer;

		IShaderVariableManager();

		~IShaderVariableManager();

	protected:
		//all the effect variables are interfaces via which we can communicate with GPU
		//and the data will be updated to variable in GPU with certain name.

#define SHADER_VAR_MATRIX(name) ID3DX11EffectMatrixVariable* m_pFxMatrix_##name##;

		SHADER_VAR_MATRIX(World)
		SHADER_VAR_MATRIX(WorldInvTranspose)
		SHADER_VAR_MATRIX(Projection)
		SHADER_VAR_MATRIX(View)
		SHADER_VAR_MATRIX(ViewInv)


#define SHADER_VAR_GENERAL(name) ID3DX11EffectVariable* m_pFxVar_##name##;

		SHADER_VAR_GENERAL(DynamicDirLightArray)
		SHADER_VAR_GENERAL(DynamicPointLightArray)
		SHADER_VAR_GENERAL(DynamicSpotLightArray)
		SHADER_VAR_GENERAL(StaticDirLightArray)
		SHADER_VAR_GENERAL(StaticPointLightArray)
		SHADER_VAR_GENERAL(StaticSpotLightArray)

#define SHADER_VAR_SCALAR(name,type) ID3DX11EffectVariable* m_pFxScalar_##type##_##name##;
	
		SHADER_VAR_SCALAR(DynamicLightEnabled,int)
		SHADER_VAR_SCALAR(DynamicDirLightCount,int)
		SHADER_VAR_SCALAR(DynamicPointLightCount,int)
		SHADER_VAR_SCALAR(DynamicSpotLightCount, int)
		SHADER_VAR_SCALAR(StaticLightEnabled, int)
		SHADER_VAR_SCALAR(StaticDirLightCount, int)
		SHADER_VAR_SCALAR(StaticPointLightCount, int)
		SHADER_VAR_SCALAR(StaticSpotLightCount, int)
		SHADER_VAR_SCALAR(FogEnabled, int)
		SHADER_VAR_SCALAR(FogNear,float)
		SHADER_VAR_SCALAR(FogFar, float)
		SHADER_VAR_SCALAR(SkyBoxWidth, float)
		SHADER_VAR_SCALAR(SkyBoxHeight, float)
		SHADER_VAR_SCALAR(SkyBoxDepth, float)



#define SHADER_VAR_VECTOR(name,dimension) ID3DX11EffectVectorVariable* m_pFxVector##dimension##_##name##;

		SHADER_VAR_VECTOR(CamPos,3)
		SHADER_VAR_VECTOR(FogColor,3)
		SHADER_VAR_VECTOR(TextColor,4)
		SHADER_VAR_VECTOR(TextGlowColor,4)

#define SHADER_VAR_SAMPLER(name) ID3DX11EffectSamplerVariable* m_pFxSampler_##name##;

		SHADER_VAR_SAMPLER(DefaultDraw)
		SHADER_VAR_SAMPLER(Draw2D)

#define SHADER_VAR_TEXTURE(name) ID3DX11EffectShaderResourceVariable* m_pFxTexture_##name##;

		SHADER_VAR_TEXTURE(DiffuseMap)
		SHADER_VAR_TEXTURE(NormalMap)
		SHADER_VAR_TEXTURE(SpecularMap)
		SHADER_VAR_TEXTURE(CubeMap)//environment mapping
		SHADER_VAR_TEXTURE(ColorMap2D)//for 2d texturing

	};
}