
/***********************************************************************

								h£ºCollision Testor
				provide all kinds of collision detection
				between all kinds of objects

************************************************************************/

#pragma once

namespace Noise3D
{
	class IShaderVariableManager;

	class /*_declspec(dllexport)*/ ICollisionTestor
	{
	public:

		//normalized Coord frame(Cartesian) centered at the middle of screen ,with X & Y both valued in [-1,1]
		void Picking_GpuBased(IMesh* pMesh, const NVECTOR2& mouseNormalizedCoord,std::vector<NVECTOR3>& outCollidedPointList);

		//normalized Coord frame(Cartesian) centered at the middle of screen ,with X & Y both valued in [-1,1]
		UINT Picking_GpuBased(IMesh* pMesh, const NVECTOR2& mouseNormalizedCoord);

	private:
		
		ICollisionTestor();

		~ICollisionTestor();

		friend IFactory<ICollisionTestor>;

		friend IScene;

		bool mFunction_Init();

		//depth stencil state
		bool mFunction_InitDSS();

		//-------PICKING-----------

		static const UINT c_maxSOVertexCount = 200;

		IShaderVariableManager* m_pRefShaderVarMgr;

		ID3D11Buffer*			m_pSOGpuWriteableBuffer;
		ID3D11Buffer*			m_pSOCpuReadableBuffer;//this buffer will be used only when concrete collision point pos is needed
		ID3D11Query*			m_pSOQuery;//Inherited from ID3D11Async which is used to query SO information
		ID3DX11EffectTechnique*			m_pFX_Tech_Picking;//gpu acceleration picking intersection
		ID3D11DepthStencilState*			m_pDSS_DisableDepthTest;
	};
}