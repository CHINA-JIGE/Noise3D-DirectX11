
/***********************************************************************

					h：NoiseLineBuffer
				主要功能 ：添加各种线

************************************************************************/
#pragma once

public class _declspec(dllexport) NoiseLineBuffer
{
	friend NoiseRenderer;
	friend NoiseScene;

public:
	//构造函数
	NoiseLineBuffer();
	~NoiseLineBuffer();

	void AddLine3D(NVECTOR3 v1, NVECTOR3 v2, NVECTOR4 color1 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4 color2 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	void	DeleteLine3D(UINT index);

	BOOL	AddToRenderList();

	UINT		GetLineCount();


private:
	BOOL					mFunction_InitVB(UINT mVertexCount);
	void						mFunction_UpdateToGpu();

	NoiseScene*			m_pFatherScene;
	ID3D11Buffer*		m_pVertexBuffer;
	BOOL					mCanUpdateToGpu;
	UINT						mVertexBufferByteSize;
	std::vector<N_SimpleVertex>* m_pVertexInMem;
};