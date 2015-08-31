
/***********************************************************************

					h：NoiseLineBuffer
				主要功能 ：添加各种线

************************************************************************/
#pragma once

public class _declspec(dllexport) NoiseGraphicObject
{
	friend NoiseRenderer;
	friend NoiseScene;

public:
	//构造函数
	NoiseGraphicObject();
	~NoiseGraphicObject();

	UINT		AddLine3D(NVECTOR3 v1, NVECTOR3 v2, NVECTOR4 color1 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4 color2 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	UINT		AddLine2D(NVECTOR2 v1, NVECTOR2 v2, NVECTOR4 color1 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4 color2 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	UINT		AddPoint3D(NVECTOR3 v, NVECTOR4 color = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	UINT		AddPoint2D(NVECTOR2 v, NVECTOR4 color = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	void		SetLine3D(UINT index, NVECTOR3 v1, NVECTOR3 v2, NVECTOR4 color1 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4 color2 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	void		SetLine2D(UINT index, NVECTOR2 v1, NVECTOR2 v2, NVECTOR4 color1 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4 color2 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	void		SetPoint3D(UINT index, NVECTOR3 v, NVECTOR4 color = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	void		SetPoint2D(UINT index, NVECTOR2 v, NVECTOR4 color = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	void		DeleteLine3D(UINT index);

	void		DeleteLine2D(UINT index);

	void		DeletePoint3D(UINT index);

	void		DeletePoint2D(UINT index);

	BOOL	AddToRenderList();

	UINT		GetLine3DCount();

	UINT		GetLine2DCount();

	UINT		GetPoint3DCount();

	UINT		GetPoint2DCount();


private:
	BOOL		mFunction_InitVB(UINT objType_ID);

	void			mFunction_UpdateToGpu(UINT objType_ID);

	void			mFunction_EraseVertices(std::vector<N_SimpleVertex>* pList,UINT iVertexStartID, UINT iVertexCount);

	void			mFunction_SetVertices(std::vector<N_SimpleVertex>* pList, N_SimpleVertex* pSourceArray,UINT iVertexStartID, UINT iVertexCount);

private:
	NoiseScene*			m_pFatherScene;

	UINT						mVB_ByteSize_GPU[5];

	ID3D11Buffer*		m_pVB_GPU[5];

	BOOL					mCanUpdateToGpu[5];

	std::vector<N_SimpleVertex>*	m_pVB_Mem[5];


};