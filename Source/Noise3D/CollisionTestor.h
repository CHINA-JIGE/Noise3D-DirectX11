
/***********************************************************************

								h£ºCollision Testor
				provide all kinds of collision detection
				between all kinds of objects

************************************************************************/


#pragma once


namespace Noise3D
{

	struct N_CbPicking
	{
		/*(x,y,1) for the picking ray , and this is the xy*/
		NVECTOR2 pickingRayNormalizedDirXY;    NVECTOR2 mPad1;
	};

	class /*_declspec(dllexport)*/ ICollisionTestor
	{
	public:

		void Picking(IMesh* pMesh, const NVECTOR2& mousePixelCoord, const NVECTOR2& windowPixelSize,std::vector<NVECTOR3>& outCollidedPointList);

		UINT Picking(IMesh* pMesh, const NVECTOR2& mousePixelCoord, const NVECTOR2& windowPixelSize);

	private:
		
		ICollisionTestor();

		~ICollisionTestor();

		friend IFactory<ICollisionTestor>;

		friend IScene;

		BOOL mFunction_Init();

		//-------PICKING-----------

		static const UINT c_maxSOVertexCount = 100;

		ID3D11Buffer*			m_pSOBuffer;
		ID3D11Query*			m_pSOQuery;//Inherited from ID3D11Async which is used to query SO information
		ID3DX11EffectTechnique*			m_pFX_Tech_Picking;//gpu acceleration picking intersection
		ID3DX11EffectConstantBuffer*	m_pFX_CbPicking;
		ID3DX11EffectConstantBuffer*	m_pFX_CbCameraInfo;
	};
}