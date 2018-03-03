
/***********************************************************************

                           h£ºShader Variable Manager
			Desc: Whenever I want to create a new constant buffer
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

		enum NOISE_SHADER_VAR_MATRIX
		{
			WORLD,
			WORLD_INV_TRANSPOSE,
			PROJECTION,
			VIEW,
			VIEW_INV,

			NOISE_SHADER_VAR_MATRIX_ELEMENT_COUNT
		};

		enum NOISE_SHADER_VAR_GENERAL
		{
			DYNAMIC_DIRLIGHT,
			DYNAMIC_POINTLIGHT,
			DYNAMIC_SPOTLIGHT,
			MATERIAL_BASIC,

			NOISE_SHADER_VAR_GENERAL_ELEMENT_COUNT
		};

		enum NOISE_SHADER_VAR_SCALAR
		{
			DYNAMIC_LIGHT_ENABLED,
			DYNAMIC_DIRLIGHT_COUNT,
			DYNAMIC_POINTLIGHT_COUNT,
			DYNAMIC_SPOTLIGHT_COUNT,
			//STATIC_LIGHT_ENABLED,
			//STATIC_DIRLIGHT_COUNT,
			//STATIC_POINTLIGHT_COUNT,
			//STATIC_SPOTLIGHT_COUNT,
			FOG_ENABLED,
			FOG_NEAR,
			FOG_FAR,
			SKYBOX_WIDTH,
			SKYBOX_HEIGHT,
			SKYBOX_DEPTH,

			NOISE_SHADER_VAR_SCALAR_ELEMENT_COUNT
		};

		enum NOISE_SHADER_VAR_VECTOR
		{
			CAMERA_POS3,
			FOG_COLOR3,
			TEXT_COLOR4,
			TEXT_GLOW_COLOR4,
			PICKING_RAY_NORMALIZED_DIR_XY,

			NOISE_SHADER_VAR_VECTOR_ELEMENT_COUNT
		};

		enum NOISE_SHADER_VAR_SAMPLER
		{
			DEFAULT_SAMPLER,
			DRAW_2D_SAMPLER,

			NOISE_SHADER_VAR_SAMPLER_ELEMENT_COUNT
		};

		enum NOISE_SHADER_VAR_TEXTURE
		{
			DIFFUSE_MAP,
			NORMAL_MAP,
			SPECULAR_MAP,
			CUBE_MAP,
			COLOR_MAP_2D,
			POST_PROCESS_PREV_RT,

			NOISE_SHADER_VAR_TEXTURE_ELEMENT_COUNT
		};

	public:

		//init must be POSTPONED because the initialization of renderer is triggered
		//by user, hence init op will not be done in constructor
		static IShaderVariableManager* GetSingleton();

		//set general variables
		void SetVar(const char* var, void* pVal,int size);

		void SetMatrix(NOISE_SHADER_VAR_MATRIX var, const NMATRIX& data);

		void SetVector2(NOISE_SHADER_VAR_VECTOR var, const NVECTOR2& data);

		void SetVector3(NOISE_SHADER_VAR_VECTOR var, const NVECTOR3& data);

		void SetVector4(NOISE_SHADER_VAR_VECTOR var, const NVECTOR4& data);

		void SetFloat(NOISE_SHADER_VAR_SCALAR var, float val);

		void SetInt(NOISE_SHADER_VAR_SCALAR var, int val);

		void SetSampler(NOISE_SHADER_VAR_SAMPLER var,int index, ID3D11SamplerState* pState);

		void SetTexture(NOISE_SHADER_VAR_TEXTURE var, ID3D11ShaderResourceView* pSRV);


		//set Array
		void	SetDynamicDirLight(int index, const N_DirLightDesc& dynamicLightDesc);

		void	SetDynamicPointLight(int index, const N_PointLightDesc& dynamicLightDesc);

		void	SetDynamicSpotLight(int index, const N_SpotLightDesc& dynamicLightDesc);

		void	SetMaterial(N_BasicMaterialDesc mat);


	private:

		//only specific friend class can inherit shader var manager
		friend class IRenderInfrastructure;

		friend class ICollisionTestor;

		IShaderVariableManager();

		~IShaderVariableManager();

		static IShaderVariableManager* m_pSingleton;

		//all the effect variables are interfaces via which we can communicate with GPU
		//and the data will be updated to variable in GPU with certain name.
		ID3DX11EffectMatrixVariable*			m_pFxMatrix[NOISE_SHADER_VAR_MATRIX_ELEMENT_COUNT];
		ID3DX11EffectVariable*						m_pFxVar[NOISE_SHADER_VAR_GENERAL_ELEMENT_COUNT];
		ID3DX11EffectScalarVariable*				m_pFxScalar[NOISE_SHADER_VAR_SCALAR_ELEMENT_COUNT];
		ID3DX11EffectVectorVariable*			m_pFxVector[NOISE_SHADER_VAR_VECTOR_ELEMENT_COUNT];
		ID3DX11EffectSamplerVariable*			m_pFxSampler[NOISE_SHADER_VAR_SAMPLER_ELEMENT_COUNT];
		ID3DX11EffectShaderResourceVariable* m_pFxTexture[NOISE_SHADER_VAR_TEXTURE_ELEMENT_COUNT];
	};
};