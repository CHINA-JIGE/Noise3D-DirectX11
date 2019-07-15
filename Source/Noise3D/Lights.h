/***********************************************************************

                           h£ºILight interafces && lightDesc 

************************************************************************/
#pragma once

namespace Noise3D
{
	//LIGHT description : just combination of data
	//LIGHT interfaces : provide interfaces for the user to interact
	struct N_CommonLightDesc
	{
		N_CommonLightDesc():
			ambientColor(Vec3( 0, 0, 0 )),
			specularIntensity(0.0f),
			diffuseColor(Vec3(0, 0, 0)),
			diffuseIntensity( 0.0f),
			specularColor(Vec3(0, 0, 0))
			{}
		Vec3	ambientColor;		float				specularIntensity;
		Vec3	diffuseColor;		float				diffuseIntensity;
		Vec3	specularColor;	//4 bytes left to pad to fulfill 128 bytes alignment
	};


	//don't forget graphic memory's 128 bit alignment
	struct N_DirLightDesc
		:public N_CommonLightDesc
	{
		N_DirLightDesc() {  };

		/*Vec3	ambientColor;		float				specularIntensity;
		Vec3	diffuseColor;				float				diffuseIntensity;
		Vec3	specularColor;*/	float		mPad2;
		Vec3 direction;				float		mPad3;
	};


	//don't forget graphic memory's 128 bit alignment
	struct N_PointLightDesc 
		:public N_CommonLightDesc
	{
		N_PointLightDesc() { }

		/*Vec3	ambientColor;		float				specularIntensity;
		Vec3	diffuseColor;				float				diffuseIntensity;
		Vec3	specularColor;*/		float		attenuationFactor;
		Vec3 position;					float		lightingRange;

	};


	//don't forget graphic memory's 128 bit alignment
	struct N_SpotLightDesc
		:public N_CommonLightDesc
	{
		N_SpotLightDesc(){}

		/*Vec3 ambientColor;		float specularIntensity;
		Vec3 diffuseColor;			float diffuseIntensity;
		Vec3 specularColor;*/	float attenuationFactor;
		Vec3 lookAt;					float lightingAngle;
		Vec3 position;			float lightingRange;
	};


	//----------------------BASE LIGHT-------------------------------
	class IBaseLight
	{
	public:

		IBaseLight();

		void SetAmbientColor(const Vec3& color);

		void SetDiffuseColor(const Vec3& color);

		void SetSpecularColor(const Vec3& color);

		void SetSpecularIntensity(float specInt);

		void SetDiffuseIntensity(float diffInt);


	protected:

		//invoked by derived Light,not by user
		void	SetDesc(const N_CommonLightDesc& desc);

		//invoked by derived Light,not by user
		void GetDesc(N_CommonLightDesc& outDesc);

	private:
		N_CommonLightDesc mBaseLightDesc;

	};


	//---------------------Dynamic Directional Light------------------
	class DirLight : 
		public IBaseLight,
		public IShadowCaster,//container of DSV of shadow map
		public ISceneObject
	{
	public:

		//set local direction (which can be rotated and transform to world space)
		void	SetDirection(const Vec3& dir);

		//get local direction
		Vec3 GetDirection();

		//get world space direction (which can be rotated to transform to world space)
		Vec3 GetDirection_WorldSpace();

		//many CLAMP op happens in this
		void SetDesc(const N_DirLightDesc& desc);

		N_DirLightDesc GetDesc();

		//get desc with geometric info transformed to world space
		N_DirLightDesc GetDesc_TransformedToWorld();

		//ISceneObject::
		virtual	N_AABB GetLocalAABB() override;

		//ISceneObject::
		virtual	N_AABB ComputeWorldAABB_Accurate() override;

		//ISceneObject::
		virtual N_AABB ComputeWorldAABB_Fast() override;

		//ISceneObject::
		virtual N_BoundingSphere ComputeWorldBoundingSphere_Accurate() override;

		//ISceneObject::
		virtual	NOISE_SCENE_OBJECT_TYPE GetObjectType()const override;

		//SceneNode::
		AffineTransform& GetLocalTransform()=delete;

		//SceneNode::
		Matrix EvalWorldAffineTransformMatrix() = delete;

		//SceneNode::
		void EvalWorldAffineTransformMatrix(Matrix& outWorldMat, Matrix& outWorldInvTranspose) = delete;


	protected:

		//override SM init function. invoked by LightManager
		virtual bool mFunction_InitShadowMap(N_SHADOW_MAPPING_PARAM smParam) override final;

	private:

		//to init
		friend DirLight* Noise3D::LightManager::CreateDynamicDirLight(SceneNode*,N_UID,N_SHADOW_MAPPING_PARAM );
		
		friend IFactory<DirLight>;

		DirLight();

		~DirLight();

		N_DirLightDesc mLightDesc;

	};


	//-----------------------Dynamic Point Light--------------------
	class PointLight : 
		public IBaseLight,
		public  ISceneObject
		//shadow map init not implemented
	{
	public:

		//local space
		void SetPosition(const Vec3& pos);

		//local space
		Vec3 GetPostion();

		//world space
		Vec3 GetPosition_WorldSpace();

		void SetAttenuationFactor(float attFactor);

		void	SetLightingRange(float range);

		void SetDesc(const N_PointLightDesc& desc);//many CLAMP op happens in this

		N_PointLightDesc GetDesc();

		//get desc with geometric info transformed to world space
		N_PointLightDesc GetDesc_TransformedToWorld();


		//ISceneObject::
		virtual	N_AABB GetLocalAABB() override;

		//ISceneObject::
		virtual	N_AABB ComputeWorldAABB_Accurate() override;

		//ISceneObject::
		virtual N_AABB ComputeWorldAABB_Fast() override;

		//ISceneObject::
		virtual N_BoundingSphere ComputeWorldBoundingSphere_Accurate() override;

		//ISceneObject::
		virtual	NOISE_SCENE_OBJECT_TYPE GetObjectType()const override;

	private:

		friend PointLight* LightManager::CreateDynamicPointLight(SceneNode*, N_UID);
		friend IFactory<PointLight>;

		PointLight();

		~PointLight();

		N_PointLightDesc mLightDesc;
	};


	//-----------------------Dynamic Spot Light------------------
	class SpotLight:
		public IBaseLight,
		public ISceneObject
	{
	public:

		void SetPosition(const Vec3& pos);

		Vec3 GetPosition();

		Vec3 GetPosition_WorldSpace();

		void SetAttenuationFactor(float attFactor);

		void	SetLookAt(const Vec3& vLitAt);

		Vec3 GetLookAt();

		Vec3 GetLookAt_WorldSpace();

		void	SetLightingAngle(float coneAngle_Rad);

		void	SetLightingRange(float range);

		void SetDesc(const N_SpotLightDesc& desc);//many CLAMP op happens in this

		N_SpotLightDesc GetDesc();

		//get desc with geometric info transformed to world space
		N_SpotLightDesc GetDesc_TransformedToWorld();

		//ISceneObject::
		virtual	N_AABB GetLocalAABB() override;

		//ISceneObject::
		virtual	N_AABB ComputeWorldAABB_Accurate() override;

		//ISceneObject::
		virtual N_AABB ComputeWorldAABB_Fast() override;

		//ISceneObject::
		virtual	NOISE_SCENE_OBJECT_TYPE GetObjectType()const override;

		//ISceneObject::
		virtual N_BoundingSphere ComputeWorldBoundingSphere_Accurate() override;

		//SceneNode::
		AffineTransform& GetLocalTransform() = delete;

		//SceneNode::
		Matrix EvalWorldAffineTransformMatrix() = delete;

		//SceneNode::
		void EvalWorldAffineTransformMatrix(Matrix& outWorldMat, Matrix& outWorldInvTranspose) = delete;

	private:

		friend SpotLight*	LightManager::CreateDynamicSpotLight(SceneNode*,N_UID);
		friend IFactory<SpotLight>;

		SpotLight();

		~SpotLight();

		N_SpotLightDesc mLightDesc;

	};


};