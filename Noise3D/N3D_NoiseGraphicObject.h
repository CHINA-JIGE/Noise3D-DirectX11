
/***********************************************************************

					h：NoiseGraphicObject
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
	
	void		SelfDestruction();

	UINT		AddLine3D(NVECTOR3 v1, NVECTOR3 v2, NVECTOR4 color1 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4 color2 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	UINT		AddLine2D(NVECTOR2 v1, NVECTOR2 v2, NVECTOR4 color1 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4 color2 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	UINT		AddPoint3D(NVECTOR3 v, NVECTOR4 color = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	UINT		AddPoint2D(NVECTOR2 v, NVECTOR4 color = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	UINT		AddTriangle2D(NVECTOR2 v1, NVECTOR2 v2, NVECTOR2 v3, NVECTOR4 color1= NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4 color2= NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4 color3= NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	UINT		AddRectangle(NVECTOR2 vTopLeft, NVECTOR2 vBottomRight, NVECTOR4 color,UINT texID = NOISE_MACRO_INVALID_TEXTURE_ID);

	UINT		AddEllipse( float a, float b,NVECTOR2 vCenter, NVECTOR4 color,UINT stepCount = 30, UINT texID = NOISE_MACRO_INVALID_TEXTURE_ID);

	void		SetLine3D(UINT index, NVECTOR3 v1, NVECTOR3 v2, NVECTOR4 color1 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4 color2 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	void		SetLine2D(UINT index, NVECTOR2 v1, NVECTOR2 v2, NVECTOR4 color1 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4 color2 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	void		SetPoint3D(UINT index, NVECTOR3 v, NVECTOR4 color = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	void		SetPoint2D(UINT index, NVECTOR2 v, NVECTOR4 color = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	void		SetTriangle2D(UINT index, NVECTOR2 v1, NVECTOR2 v2, NVECTOR2 v3, NVECTOR4 color1 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4 color2 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4 color3 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	void		SetRectangle(UINT index, NVECTOR2 vTopLeft, NVECTOR2 vBottomRight, NVECTOR4 color, UINT texID = NOISE_MACRO_INVALID_TEXTURE_ID);

	void		SetEllipse(UINT index, float a, float b,NVECTOR2 vCenter, NVECTOR4 color,UINT texID = NOISE_MACRO_INVALID_TEXTURE_ID);

	void		DeleteLine3D(UINT index);

	void		DeleteLine2D(UINT index);

	void		DeletePoint3D(UINT index);

	void		DeletePoint2D(UINT index);

	void		DeleteTriangle2D(UINT index);

	void		DeleteRetangle(UINT index);

	void		DeleteEllipse(UINT index);

	UINT		GetLine3DCount();

	UINT		GetLine2DCount();

	UINT		GetPoint3DCount();

	UINT		GetPoint2DCount();

	UINT		GetTriangle2DCount();

	UINT		GetRectCount();

	UINT		GetEllipseCount();

	BOOL	AddToRenderList();


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
	
	//used to locate vertices block in triangle 2D VB
	std::vector<N_RegionInfo>*		m_pRegionList_TriangleID_Rect;
	std::vector<N_RegionInfo>*		m_pRegionList_TriangleID_Ellipse;

	//used to locate vertices block in line 2D VB
	std::vector<N_RegionInfo>*		m_pRegionList_LineID_Rect;
	std::vector<N_RegionInfo>*		m_pRegionList_LineID_Ellipse;

};