
/***********************************************************************

						类：NOISE Graphic Object

							简述：负责渲染

************************************************************************/
#include "Noise3D.h"

//God...i feel that i am a laborer

NoiseGraphicObject::NoiseGraphicObject()
{
	//there are several variables needed for one graphic object type
	//and for the time being ,there are 5 graphic object types
	//see enumerations in ".h" file
	for (UINT i = 0;i < NOISE_GRAPHIC_OBJECT_BUFFER_COUNT;i++)
	{
		mVB_ByteSize_GPU[i]		=0;

		m_pVB_GPU[i]					=nullptr;

		mCanUpdateToGpu[i]		= FALSE;

		m_pVB_Mem[i]					= new std::vector<N_SimpleVertex>;
	}

	m_pTextureList_Rect = new std::vector<UINT>;
	m_pRectSubsetInfoList= new std::vector<N_GraphicObject_SubsetInfo>;
	SetStatusToBeInitialized();
}

void NoiseGraphicObject::Destroy()
{
	for (UINT i = 0;i < NOISE_GRAPHIC_OBJECT_BUFFER_COUNT;i++)
	{
		ReleaseCOM(m_pVB_GPU[i]);
	}
}

UINT NoiseGraphicObject::AddLine3D(NVECTOR3 v1, NVECTOR3 v2, NVECTOR4 color1, NVECTOR4 color2)
{
	mFunction_AddVertices3D(
		NOISE_GRAPHIC_OBJECT_TYPE_LINE_3D, 
		{ v1,v2 },
		{ color1,color2 }, 
		{ NVECTOR2(0,0),NVECTOR2(0,0) }
	);
	return GetLine3DCount() - 1;
}

UINT NoiseGraphicObject::AddLine2D(NVECTOR2 v1, NVECTOR2 v2, NVECTOR4 color1, NVECTOR4 color2)
{
	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(v1,v1);
	mFunction_ConvertPixelVec2FloatVec(v2,v2);

	mFunction_AddVertices2D(
		NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D,
		{ v1,v2 },
		{ color1,color2 },
		{ NVECTOR2(0,0),NVECTOR2(0,0) }
	);

	return GetLine2DCount()-1;
}

UINT NoiseGraphicObject::AddPoint3D(NVECTOR3 v, NVECTOR4 color)
{
	mFunction_AddVertices3D(
		NOISE_GRAPHIC_OBJECT_TYPE_POINT_3D,
		{ v },
		{ color },
		{ NVECTOR2(0,0) }
	);
	return GetPoint3DCount() - 1;
}

UINT NoiseGraphicObject::AddPoint2D(NVECTOR2 v, NVECTOR4 color)
{
	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(v, v);

	mFunction_AddVertices2D(
		NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D,
		{ v },
		{ color },
		{ NVECTOR2(0,0) }
	);

	return GetPoint2DCount() - 1;
}

UINT NoiseGraphicObject::AddTriangle2D(NVECTOR2 v1, NVECTOR2 v2, NVECTOR2 v3, NVECTOR4 color1, NVECTOR4 color2, NVECTOR4 color3)
{
	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(v1, v1);
	mFunction_ConvertPixelVec2FloatVec(v2, v2);
	mFunction_ConvertPixelVec2FloatVec(v3, v3);

	mFunction_AddVertices2D(
		NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D,
		{ v1,v2,v3 },
		{ color1,color2,color3 },
		{ NVECTOR2(0,0),NVECTOR2(0, 0),NVECTOR2(0, 0) }
	);

	return GetTriangle2DCount() - 1;
}

UINT NoiseGraphicObject::AddRectangle(NVECTOR2 vTopLeft, NVECTOR2 vBottomRight, NVECTOR4 color, UINT texID)
{
	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(vTopLeft, vTopLeft);
	mFunction_ConvertPixelVec2FloatVec(vBottomRight, vBottomRight);


	NVECTOR2 vTopRight = NVECTOR2(vBottomRight.x, vTopLeft.y);
	NVECTOR2 vBottomLeft = NVECTOR2(vTopLeft.x, vBottomRight.y);

	//....initializer_list nit
	mFunction_AddVertices2D(
		NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D,
		{ vTopLeft,vTopRight,vBottomLeft,vTopRight,vBottomRight,vBottomLeft },
		{color,color ,color ,color ,color ,color },
		{ NVECTOR2(0,0),NVECTOR2(1,0),NVECTOR2(0, 1),NVECTOR2(1, 0),NVECTOR2(1, 1),NVECTOR2(0, 1) }
	);

	//add TextureID
	m_pTextureList_Rect->push_back(texID);

	//return ID of Rectangle
	return GetRectCount()-1;
}

UINT NoiseGraphicObject::AddRectangle(NVECTOR2 vCenter, float fWidth, float fHeight, NVECTOR4 color, UINT texID)
{
	//dont use coord conversion here , because in the other overload , conversion will be applied
	UINT newRectID = NOISE_MACRO_INVALID_ID;
	newRectID = AddRectangle(vCenter - NVECTOR2(fWidth / 2, fHeight / 2), vCenter + NVECTOR2(fWidth / 2, fHeight / 2), color, texID);
	return newRectID;
}

void NoiseGraphicObject::SetLine3D(UINT index, NVECTOR3 v1, NVECTOR3 v2, NVECTOR4 color1, NVECTOR4 color2)
{
	if (index > GetLine3DCount())
	{
		DEBUG_MSG1("Line 3D index invalid!!");
		return;
	}

	mFunction_SetVertices3D(
		NOISE_GRAPHIC_OBJECT_TYPE_LINE_3D,
		index*2,
		{ v1,v2 },
		{ color1,color2 },
		{ NVECTOR2(0,0),NVECTOR2(0,0) }
	);

}

void NoiseGraphicObject::SetLine2D(UINT index, NVECTOR2 v1, NVECTOR2 v2, NVECTOR4 color1, NVECTOR4 color2)
{
	if (index >=GetLine2DCount())
	{
		DEBUG_MSG1("Line 2D index invalid!!");
		return;
	}

	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(v1, v1);
	mFunction_ConvertPixelVec2FloatVec(v2, v2);

	mFunction_SetVertices2D(
		NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D,
		index*2,
		{ v1,v2 },
		{ color1,color2 },
		{ NVECTOR2(0,0),NVECTOR2(0,0) }
	);

}

void NoiseGraphicObject::SetPoint3D(UINT index, NVECTOR3 v, NVECTOR4 color)
{
	if (index > GetPoint3DCount())
	{
		DEBUG_MSG1("Point 3D index invalid!!");
		return;
	}

	mFunction_SetVertices3D(
		NOISE_GRAPHIC_OBJECT_TYPE_POINT_3D,
		index,
		{ v },
		{ color },
		{ NVECTOR2(0,0) }
	);

}

void NoiseGraphicObject::SetPoint2D(UINT index, NVECTOR2 v, NVECTOR4 color)
{
	if (index > GetPoint2DCount())
	{
		DEBUG_MSG1("Point 2D index invalid!!");
		return;
	}

	mFunction_SetVertices2D(
		NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D,
		index,
		{ v },
		{ color },
		{ NVECTOR2(0,0) }
	);
}

void NoiseGraphicObject::SetTriangle2D(UINT index, NVECTOR2 v1, NVECTOR2 v2, NVECTOR2 v3, NVECTOR4 color1, NVECTOR4 color2, NVECTOR4 color3)
{
	if (index >= GetTriangle2DCount())
	{
		DEBUG_MSG1("Rectangle Index Invalid !!");
		return;
	}

	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(v1, v1);
	mFunction_ConvertPixelVec2FloatVec(v2, v2);
	mFunction_ConvertPixelVec2FloatVec(v3, v3);

	mFunction_SetVertices2D(
		NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D,
		index*3,
		{ v1,v2,v3 },
		{ color1,color2,color3 },
		{ NVECTOR2(0,0),NVECTOR2(0, 0),NVECTOR2(0, 0) }
	);
}

void NoiseGraphicObject::SetRectangle(UINT index, NVECTOR2 vTopLeft, NVECTOR2 vBottomRight, NVECTOR4 color, UINT texID)
{
	//index mean the 'index'th rectangle

	if (index >= GetRectCount())
	{
		DEBUG_MSG1("Rectangle Index Invalid !!");
		return;
	}

	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(vTopLeft, vTopLeft);
	mFunction_ConvertPixelVec2FloatVec(vBottomRight, vBottomRight);

	//.............
	auto pList = m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D];
	NVECTOR2 vTopRight = NVECTOR2(vBottomRight.x, vTopLeft.y);
	NVECTOR2 vBottomLeft = NVECTOR2(vTopLeft.x, vBottomRight.y);

	//....initializer_list nit
	mFunction_SetVertices2D(
		NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D,
		index*6,
		{ vTopLeft,vTopRight,vBottomLeft,vTopRight,vBottomRight,vBottomLeft },
		{ color,color ,color ,color ,color ,color },
		{ pList->at(index * 6).TexCoord,pList->at(index * 6 +1).TexCoord, pList->at(index * 6 +2).TexCoord,
		pList->at(index * 6 +3).TexCoord,pList->at(index * 6 + 4).TexCoord, pList->at(index * 6 + 5).TexCoord }
	);

	//add TextureID
	m_pTextureList_Rect->at(index)=texID;

}

void NoiseGraphicObject::SetRectangle(UINT index, NVECTOR2 vCenter, float fWidth, float fHeight, NVECTOR4 color, UINT texID)
{
	//dont use coord conversion here , because in the other overload , conversion will be applied
	SetRectangle(index,vCenter - NVECTOR2(fWidth/2,fHeight/2),vCenter+ NVECTOR2(fWidth / 2, fHeight / 2),color,texID);
}

void NoiseGraphicObject::SetRectangleTexCoord(UINT index, NVECTOR2 texCoordTopLeft,NVECTOR2 texCoordBottomRight)
{
	//index mean the 'index'th rectangle

	if (index >=GetRectCount())
	{
		DEBUG_MSG1("Rectangle Index Invalid !!");
		return;
	}

	//after getting triangle ID (in global buffer) , compute 6 vertex ID of these 2 tri (in global buffer)
	UINT vertexID[6] =
	{
		index *6,
		index * 6	+ 1,
		index * 6	+ 2,
		index * 6	+3,
		index * 6	+ 4,
		index * 6	+ 5
	};

	m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D]->at(vertexID[0]).TexCoord = texCoordTopLeft;
	m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D]->at(vertexID[1]).TexCoord = NVECTOR2(texCoordBottomRight.x,texCoordTopLeft.y);
	m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D]->at(vertexID[2]).TexCoord = NVECTOR2(texCoordTopLeft.x, texCoordBottomRight.y);
	m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D]->at(vertexID[3]).TexCoord = NVECTOR2(texCoordBottomRight.x, texCoordTopLeft.y);
	m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D]->at(vertexID[4]).TexCoord = texCoordBottomRight;
	m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D]->at(vertexID[5]).TexCoord = NVECTOR2(texCoordTopLeft.x, texCoordBottomRight.y);
	
	//now it is allowed to update because of modification
	mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D] = TRUE;
}

void NoiseGraphicObject::DeleteLine3D(UINT index)
{
	//to explain codes in a  clearer way
	UINT vertexStartIndex = index * 2;
	UINT vertexCount = 2;	//1 line consist of 2 vertices

	//erase function
	mFunction_EraseVertices(NOISE_GRAPHIC_OBJECT_TYPE_LINE_3D, vertexStartIndex, vertexCount);

}

void NoiseGraphicObject::DeleteLine2D(UINT index)
{
	if (index < GetLine2DCount())
	{

		//to explain codes in a  clearer way
		UINT vertexStartIndex = index * 2;
		UINT vertexCount = 2;	//1 line consist of 2 vertices


		//erase function
		mFunction_EraseVertices(NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D, vertexStartIndex, vertexCount);

	}
	else
	{
		DEBUG_MSG1("delete line 2d :invalid index");
	}
}

void NoiseGraphicObject::DeletePoint3D(UINT index)
{
	//to explain codes in a  clearer way
	UINT vertexStartIndex = index;
	UINT vertexCount = 1;	//1 line consist of 2 vertices

							//erase function
	mFunction_EraseVertices(NOISE_GRAPHIC_OBJECT_TYPE_POINT_3D, vertexStartIndex, vertexCount);

}

void NoiseGraphicObject::DeletePoint2D(UINT index)
{
	//to explain codes in a  clearer way
	UINT vertexStartIndex = index;
	UINT vertexCount = 1;	//1 line consist of 2 vertices

	//erase function
	mFunction_EraseVertices(NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D, vertexStartIndex, vertexCount);

}

void NoiseGraphicObject::DeleteTriangle2D(UINT index)
{
	//delete the index_th single Triangle
	UINT vertexStartIndex = 0;
	UINT	 vertexCount = 0;

	if (index < GetTriangle2DCount())
	{
		//sum up triangle counts , because the step count of Ellipse might not be the same
		vertexStartIndex = index* 3;
		vertexCount = 3;

		//erase from vector
		mFunction_EraseVertices(NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D, vertexStartIndex, vertexCount);

	}
	else
	{
		DEBUG_MSG1("delete triangle : Triangle Index Invalid!!");
		return;
	}

}

void NoiseGraphicObject::DeleteRectangle(UINT index)
{
	//delete the index_th Rectangle

	UINT vertexStartIndex = 0;
	UINT	 vertexCount = 0;

	if (index < GetRectCount())
	{
		vertexStartIndex = index * 6;
		vertexCount = 6;
		//erase vertices
 		mFunction_EraseVertices(NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D, vertexStartIndex, vertexCount);

		//erase texture info
		auto tmpIter = m_pTextureList_Rect->begin()+index;
		m_pTextureList_Rect->erase(tmpIter);
	}
	else
	{
		DEBUG_MSG1("delete rectangle : Rectangle Index Invalid!!");
		return;
	}

}

UINT NoiseGraphicObject::GetLine3DCount()
{
	return m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_LINE_3D]->size()/2;
};

UINT NoiseGraphicObject::GetLine2DCount()
{
	return m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D]->size() / 2;
};

UINT NoiseGraphicObject::GetPoint3DCount()
{
	return m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_POINT_3D]->size();
};

UINT NoiseGraphicObject::GetPoint2DCount()
{
	return m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D]->size();
};

UINT NoiseGraphicObject::GetTriangle2DCount()
{
	return m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D]->size()/3;
}

UINT NoiseGraphicObject::GetRectCount()
{
	return m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D]->size()/6;
}


/***********************************************************************
						PRIVATE
***********************************************************************/

BOOL	NoiseGraphicObject::mFunction_InitVB(UINT objType_ID)
{
	//get VB (in memory) byte size of corresponding graphic object type
	UINT vertexCount = m_pVB_Mem[objType_ID]->size();

	//Create VERTEX BUFFER
	D3D11_BUFFER_DESC vbd;
	vbd.ByteWidth = vertexCount * sizeof(N_SimpleVertex);
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0; 
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;


	D3D11_SUBRESOURCE_DATA tmpInitData;
	tmpInitData.pSysMem = vertexCount>0?&m_pVB_Mem[objType_ID]->at(0):nullptr;
	tmpInitData.SysMemPitch = 0;
	tmpInitData.SysMemSlicePitch = 0;

	//update the byte size of GPU vertex buffer
	mVB_ByteSize_GPU[objType_ID] = vbd.ByteWidth;

	//Create Buffers
	HRESULT hr = 0;
	hr = g_pd3dDevice11->CreateBuffer(&vbd, &tmpInitData, &m_pVB_GPU[objType_ID]);
	//ReleaseCOM(g_pd3dDevice11);
	HR_DEBUG(hr, "VERTEX BUFFER创建失败");

	return TRUE;
}

void		NoiseGraphicObject::mFunction_UpdateVerticesToGpu(UINT objType_ID)
{
	//nothing to update ,so exit (in InitVB() , create a  
	if (m_pVB_Mem[objType_ID]->size() == 0)return;


	//if a GPU buffer has not been created then create a new one
	if (m_pVB_GPU[objType_ID] == NULL)
	{
		mFunction_InitVB(objType_ID);
		return;
	}


	//calculate byte size of ALL VERTEX BUFFER
	UINT mVB_Byte_Size_Memory =	m_pVB_Mem[objType_ID]->size()*	sizeof(N_SimpleVertex);

	//decide whether we should expand the capacity of GPU Vertex Buffer
	if (mVB_Byte_Size_Memory  > mVB_ByteSize_GPU[objType_ID])
	{
		//If VB was once created , destroy it to remake a new one
		if (m_pVB_GPU[objType_ID])
		{
			m_pVB_GPU[objType_ID]->Release();
			
			//create a new VB
			mFunction_InitVB(objType_ID);
		}
		else
		{
			mFunction_InitVB(objType_ID);
		}
	}


	//update new data to GPU
	g_pImmediateContext->UpdateSubresource(m_pVB_GPU[objType_ID], 0, 0, &m_pVB_Mem[objType_ID]->at(0), 0, 0);

}

void		NoiseGraphicObject::mFunction_AddVertices2D(NOISE_GRAPHIC_OBJECT_TYPE buffType, std::initializer_list<NVECTOR2> vertexList, std::initializer_list<NVECTOR4> colorList, std::initializer_list<NVECTOR2> texcoordList)
{

	N_SimpleVertex tmpVertex;
	auto vertexIter = vertexList.begin();
	auto colorIter = colorList.begin();
	auto texcoordIter = texcoordList.begin();

	//construct a N_SimpleVertex
	for (UINT i = 0;i < vertexList.size();i++)
	{

		tmpVertex = 
		{
			NVECTOR3(vertexIter->x,vertexIter->y,0.0f),
			*colorIter++,
			*texcoordIter++
		};
		vertexIter++;

		m_pVB_Mem[buffType]->push_back(tmpVertex);
	}

	//now it is allowed to update because of modification
	mCanUpdateToGpu[buffType] = TRUE;
}

void		NoiseGraphicObject::mFunction_AddVertices3D(NOISE_GRAPHIC_OBJECT_TYPE buffType, std::initializer_list<NVECTOR3> vertexList, std::initializer_list<NVECTOR4> colorList, std::initializer_list<NVECTOR2> texcoordList)
{

	N_SimpleVertex tmpVertex;
	auto vertexIter = vertexList.begin();
	auto colorIter = colorList.begin();
	auto texcoordIter = texcoordList.begin();

	//construct a N_SimpleVertex
	for (UINT i = 0;i < vertexList.size();i++)
	{
		tmpVertex =
		{
			*vertexIter++,
			*colorIter++,
			*texcoordIter++
		};
		m_pVB_Mem[buffType]->push_back(tmpVertex);
	}

	//now it is allowed to update because of modification
	mCanUpdateToGpu[buffType] = TRUE;
}

void		NoiseGraphicObject::mFunction_SetVertices2D(NOISE_GRAPHIC_OBJECT_TYPE buffType, UINT iVertexStartID, std::initializer_list<NVECTOR2> vertexList, std::initializer_list<NVECTOR4> colorList, std::initializer_list<NVECTOR2> texcoordList)
{
	//check boundary (check the tail ,if the tail is within boundary , then it's valid
	if (iVertexStartID + vertexList.size() >m_pVB_Mem[buffType]->size())
	{
		DEBUG_MSG1("Noise Graphic Object : Vertex ID Out of boundary!");
		return;
	}
	
	N_SimpleVertex tmpVertex;
	auto vertexIter = vertexList.begin();
	auto colorIter = colorList.begin();
	auto texcoordIter = texcoordList.begin();

	//construct a N_SimpleVertex
	for (UINT i = 0;i < vertexList.size();i++)
	{

		tmpVertex =
		{
			NVECTOR3(vertexIter->x,vertexIter->y,0.0f),
			*colorIter++,
			*texcoordIter++
		};
		vertexIter++;

		m_pVB_Mem[buffType]->at(iVertexStartID+i)=tmpVertex;
	}

	//now it is allowed to update because of modification
	mCanUpdateToGpu[buffType] = TRUE;

}

void		NoiseGraphicObject::mFunction_SetVertices3D(NOISE_GRAPHIC_OBJECT_TYPE buffType, UINT iVertexStartID, std::initializer_list<NVECTOR3> vertexList, std::initializer_list<NVECTOR4> colorList, std::initializer_list<NVECTOR2> texcoordList)
{
	//check boundary (check the tail ,if the tail is within boundary , then it's valid
	if (iVertexStartID + vertexList.size() >m_pVB_Mem[buffType]->size())
	{
		DEBUG_MSG1("Noise Graphic Object : Vertex ID Out of boundary!");
		return;
	}

	N_SimpleVertex tmpVertex;
	auto vertexIter = vertexList.begin();
	auto colorIter = colorList.begin();
	auto texcoordIter = texcoordList.begin();

	//construct a N_SimpleVertex
	for (UINT i = 0;i < vertexList.size();i++)
	{

		tmpVertex =
		{
			*vertexIter++,
			*colorIter++,
			*texcoordIter++
		};

		m_pVB_Mem[buffType]->at(i) = tmpVertex;
	}

	//now it is allowed to update because of modification
	mCanUpdateToGpu[buffType] = TRUE;

}

void		NoiseGraphicObject::mFunction_EraseVertices(NOISE_GRAPHIC_OBJECT_TYPE buffType, UINT iVertexStartID, UINT iVertexCount)
{
	auto pList			= m_pVB_Mem[buffType];
	auto iter_Start	= pList->begin();
	auto iter_End	= pList->begin();


	//check boundary (check the tail ,if the tail is within boundary , then it's valid
	if ((iVertexStartID + iVertexCount) > pList->size())
	{
		DEBUG_MSG1("Noise Graphic Object : Vertex ID Out of boundary!");
		return;
	}


	//vector :: erase  ---------  delete elements in [a,b) 
	//(note that if I apply vector.erase(begin()+1,begin()+3) , then the 2nd & 3rd elements will be erased
	iter_Start += iVertexStartID;
	iter_End = iter_Start + iVertexCount;

	//delte elements
	pList->erase(iter_Start, iter_End);

	mCanUpdateToGpu[buffType] = TRUE;
};

inline void  NoiseGraphicObject::mFunction_ConvertPixelVec2FloatVec(NVECTOR2 pxCoord,NVECTOR2& outVec2)
{
	//first ,  get the pixel size of main back buffer (global var)
	float halfW= (float)gMainBufferPixelWidth / 2.0f;
	float halfH = (float)gMainBufferPixelHeight / 2.0f;
	outVec2 = NVECTOR2((pxCoord.x /halfW)-1,		1-(pxCoord.y / halfH));
}

inline float NoiseGraphicObject::mFunction_ConvertPixelLength2FloatLength(float pxLen, BOOL isWidth)
{
	float outLength = 0;
	//we need to know the pixel length is on X or Y direction
	if (isWidth)
	{
		outLength = pxLen*2.0f / (float)gMainBufferPixelWidth;
	}
	else
	{
		outLength = pxLen * 2.0f / (float)gMainBufferPixelHeight;
	}
	return outLength;
}

void		NoiseGraphicObject::mFunction_GenerateRectSubsetInfo()
{
	//clear old record
	m_pRectSubsetInfoList->clear();

	//ignore empty vertex buffer
	if (m_pTextureList_Rect->size() == 0)return;


	//init the first subset (at least 1 element)
	N_GraphicObject_SubsetInfo newSubset;
	newSubset.vertexCount = 6;//6 ver for 1 rect
	newSubset.startID = 0;
	newSubset.texID = m_pTextureList_Rect->at(0);
	m_pRectSubsetInfoList->push_back(newSubset);

	//1 UINT for 1 Rect,6 vertices
	for (UINT i = 1;i < m_pTextureList_Rect->size();i++)
	{
		//region growing
		if (m_pTextureList_Rect->at(i) == m_pRectSubsetInfoList->back().texID)
		{
			m_pRectSubsetInfoList->back().vertexCount+=6;
		}
		else
		{
			newSubset.startID = i*6;
			newSubset.vertexCount = 6;
			newSubset.texID = m_pTextureList_Rect->at(i);
			m_pRectSubsetInfoList->push_back(newSubset);
		}


	}

}
