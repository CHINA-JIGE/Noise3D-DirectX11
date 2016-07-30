
/***********************************************************************

                           h£ºNoiseScene

************************************************************************/

#pragma once

namespace Noise3D
{
		class /*_declspec(dllexport)*/ IScene:
		public IFactory<IRenderer>,
		public IFactory<ICamera>,
		public IFactory<IMeshManager>,
		public IFactory<ILightManager>,
		public IFactory<ITextureManager>,
		public IFactory<IMaterialManager>,
		public IFactory<IGraphicObjectManager>,
		public IFactory<IAtmosphere>
	{
		/*friend class IMesh;
		friend class IRenderer;
		friend class ICamera;
		friend class IMaterialManager;
		friend class ITextureManager;
		friend class IAtmosphere;*/

	public:

		void				ReleaseAllChildObject();

		IRenderer*				GetRenderer();

		ICamera*					GetCamera();

		IMeshManager*		GetMeshMgr();

		ILightManager*			GetLightMgr();

		ITextureManager*		GetTextureMgr();

		IMaterialManager*	GetMaterialMgr();

		IAtmosphere*			GetAtmosphere();

		IGraphicObjectManager*		GetGraphicObjMgr();

	private:

		friend class IFactory<IScene>;

		IScene();

		~IScene();

	};



}