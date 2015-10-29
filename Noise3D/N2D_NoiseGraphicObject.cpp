
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
	for (UINT i = 0;i < 5;i++)
	{
		mVB_ByteSize_GPU[i]		=0;

		m_pVB_GPU[i]					=nullptr;

		mCanUpdateToGpu[i]		= FALSE;

		m_pVB_Mem[i]					= new std::vector<N_SimpleVertex>;
	}

	m_pRegionList_TriangleID_Common	= new std::vector<UINT>;
	m_pRegionList_TriangleID_Rect		= new std::vector<N_RegionInfo>;	
	m_pRegionList_TriangleID_Ellipse	= new std::vector<N_RegionInfo>;
	m_pRegionList_LineID_Common	= new std::vector<UINT>;
	m_pRegionList_LineID_Triangle		= new std::vector<N_RegionInfo>;
	m_pRegionList_LineID_Rect			= new std::vector<N_RegionInfo>;
	m_pRegionList_LineID_Ellipse			= new std::vector<N_RegionInfo>;
}

void NoiseGraphicObject::SelfDestruction()
{
	ReleaseCOM(m_pVB_GPU[0]);
	ReleaseCOM(m_pVB_GPU[1]);
	ReleaseCOM(m_pVB_GPU[2]);
	ReleaseCOM(m_pVB_GPU[3]);
	ReleaseCOM(m_pVB_GPU[4]);
}

UINT NoiseGraphicObject::AddLine3D(NVECTOR3 v1, NVECTOR3 v2, NVECTOR4 color1, NVECTOR4 color2)
{
	//.....................
	N_SimpleVertex tmpVertex;
	tmpVertex.Pos = v1;
	tmpVertex.Color = color1;
	m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_LINE_3D]->push_back(tmpVertex);

	tmpVertex.Pos = v2;
	tmpVertex.Color = color2;
	m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_LINE_3D]->push_back(tmpVertex);

	//now it is allowed to update because of modification
	mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_LINE_3D] = TRUE;
	return GetLine3DCount() - 1;
}

UINT NoiseGraphicObject::AddLine2D(NVECTOR2 v1, NVECTOR2 v2, NVECTOR4 color1, NVECTOR4 color2)
{
	//only add COMMON LINES !!!!! 

	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(v1,v1);
	mFunction_ConvertPixelVec2FloatVec(v2,v2);

	UINT newLineID = 0;
	newLineID = mFunction_AddLine2D_GlobalVB(v1, v2, color1, color2);

	//update region list
	m_pRegionList_LineID_Common->push_back(newLineID);
	return newLineID;
}

UINT NoiseGraphicObject::AddPoint3D(NVECTOR3 v, NVECTOR4 color)
{
	//.....................
	N_SimpleVertex tmpVertex;
	tmpVertex.Pos = v;
	tmpVertex.Color = color;
	m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_POINT_3D]->push_back(tmpVertex);

	//now it is allowed to update because of modification
	mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_POINT_3D] = TRUE;
	return GetPoint3DCount() - 1;
}

UINT NoiseGraphicObject::AddPoint2D(NVECTOR2 v, NVECTOR4 color)
{
	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(v, v);

	//.....................
	N_SimpleVertex tmpVertex;
	tmpVertex.Pos = NVECTOR3(v.x,v.y,0);
	tmpVertex.Color = color;
	m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D]->push_back(tmpVertex);

	//now it is allowed to update because of modification
	mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D] = TRUE;
	return GetPoint2DCount() - 1;
}

UINT NoiseGraphicObject::AddTriangle2D(NVECTOR2 v1, NVECTOR2 v2, NVECTOR2 v3, NVECTOR4 color1, NVECTOR4 color2, NVECTOR4 color3)
{
	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(v1, v1);
	mFunction_ConvertPixelVec2FloatVec(v2, v2);
	mFunction_ConvertPixelVec2FloatVec(v3, v3);

	mFunction_AddSolidTriangle2D_GlobalVB(v1, v2, v3, color1, color2, color3, NVECTOR2(0,0), NVECTOR2(0, 0), NVECTOR2(0, 0),TRUE);
	return GetTriangle2DCount() - 1;
}

UINT NoiseGraphicObject::AddRectangle(NVECTOR2 vTopLeft, NVECTOR2 vBottomRight, NVECTOR4 color, UINT texID)
{
	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(vTopLeft, vTopLeft);
	mFunction_ConvertPixelVec2FloatVec(vBottomRight, vBottomRight);


	//information to be pushed into region info list
	UINT startTriangleID = GetTriangle2DCount();

		//we will use triangle to make up complex shapes
		mFunction_AddSolidTriangle2D_GlobalVB(
			vTopLeft,
			NVECTOR2(vBottomRight.x, vTopLeft.y),
			NVECTOR2(vTopLeft.x, vBottomRight.y),
			color,
			color,
			color,
			NVECTOR2(0,0),
			NVECTOR2(1,0),
			NVECTOR2(0, 1),
			FALSE);

		mFunction_AddSolidTriangle2D_GlobalVB(
			NVECTOR2(vBottomRight.x, vTopLeft.y),
			vBottomRight,
			NVECTOR2(vTopLeft.x, vBottomRight.y),
			color,
			color,
			color,
			NVECTOR2(1, 0),
			NVECTOR2(1, 1),
			NVECTOR2(0, 1),
			FALSE);

	//information to be pushed into region info list
	UINT endTriangleID = GetTriangle2DCount() - 1;

	//write down region info (to show this block of triangles compose of a rectangle)
	N_RegionInfo regionInfo;
	regionInfo.texID = texID;
	regionInfo.startID = startTriangleID;
	regionInfo.elememtCount = endTriangleID - startTriangleID+1;
	m_pRegionList_TriangleID_Rect->push_back(regionInfo);

	//return ID of Rectangle
	return GetRectCount()-1;
}

UINT NoiseGraphicObject::AddRectangle(NVECTOR2 vCenter, float fWidth, float fHeight, NVECTOR4 color, UINT texID)
{
	//dont use coord conversion here , because in the other overload , conversion will be applied
	UINT newRectID = 0;
	newRectID = AddRectangle(vCenter - NVECTOR2(fWidth / 2, fHeight / 2), vCenter + NVECTOR2(fWidth / 2, fHeight / 2), color, texID);
	return newRectID;
}

UINT NoiseGraphicObject::AddEllipse(float a, float b, NVECTOR2 vCenter, NVECTOR4 color, UINT stepCount , UINT texID)
{
	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(vCenter, vCenter);
	a = mFunction_ConvertPixelLength2FloatLength(a, TRUE);
	b = mFunction_ConvertPixelLength2FloatLength(b, FALSE);

	//information to be pushed into region info list
	UINT startTriangleID = GetTriangle2DCount();

	//well, step counts cannot too small
	//stepCount =3 means 
	if (stepCount < 3) stepCount = 3;

	//check "a" (semi-major axis) ,"b" (semi-minor axis)
	if (a < 0) a = 0.5f;
	if (b < 0) b = 0.5f;

	//use X coord to derive Y coord
	float angleStep = MATH_PI / (stepCount - 1);
	float currAngle = 0.0f;
	float nextAngle = currAngle + angleStep;

	//define 2 temporary vertices which are right on the Ellipse (interpolation)
	NVECTOR2 tmpV;
	NVECTOR2 tmpNextV;
	NVECTOR2 tmpTexcoord;
	NVECTOR2 tmpNextTexcoord;

	//loop to generate vertices
	for (UINT i = 0;i < stepCount;i++)
	{

		//this euqation can be derived from ellipse standard equation
		tmpV.x = a * cosf(currAngle);
		tmpV.y = sqrtf((a*a*b*b - b*b *tmpV.x*tmpV.x) / (a*a));
		//map a rectangle on this ellipse
		tmpTexcoord = NVECTOR2((tmpV.x / a + 1) / 2, (-tmpV.y / b + 1) / 2);


		tmpNextV.x = a * cosf(nextAngle);
		tmpNextV.y = sqrtf((a*a*b*b - b*b *tmpNextV.x*tmpNextV.x) / (a*a));
		tmpNextTexcoord = NVECTOR2((tmpNextV.x / a + 1) / 2, (-tmpNextV.y / b + 1) / 2);

		//so we can generate triangles using "radial triangulation" (well , a word created by myself hhhhhh)
		mFunction_AddSolidTriangle2D_GlobalVB(vCenter, tmpV + vCenter, tmpNextV + vCenter, color, color, color,
			NVECTOR2(0.5, 0.5),
			tmpTexcoord,
			tmpNextTexcoord,
			FALSE);

		//symmetric triangle about X AXIS
		mFunction_AddSolidTriangle2D_GlobalVB(vCenter, -tmpV + vCenter, -tmpNextV + vCenter, color, color, color,
			NVECTOR2(0.5, 0.5),
			NVECTOR2(1-tmpTexcoord.x, 1 - tmpTexcoord.y),
			NVECTOR2(1-tmpNextTexcoord.x, 1 - tmpNextTexcoord.y),
			FALSE);

		//update slopes
		currAngle += angleStep;
		nextAngle += angleStep;
	}
	//information to be pushed into region info list
	UINT endTriangleID = GetTriangle2DCount() - 1;

	//write down region info (to show this block of vertices compose of a rectangle)
	N_RegionInfo regionInfo;
	regionInfo.texID = texID;
	regionInfo.startID = startTriangleID;
	regionInfo.elememtCount = endTriangleID - startTriangleID+1;
	m_pRegionList_TriangleID_Ellipse->push_back(regionInfo);

	return GetEllipseCount()-1;

	//now it is allowed to update because of modification
	mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D] = TRUE;
}

UINT NoiseGraphicObject::AddTriangleOutline(NVECTOR2 v1, NVECTOR2 v2, NVECTOR2 v3, NVECTOR4 color1, NVECTOR4 color2, NVECTOR4 color3)
{
	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(v1, v1);
	mFunction_ConvertPixelVec2FloatVec(v2, v2);
	mFunction_ConvertPixelVec2FloatVec(v3, v3);

	//information to be pushed into region info list
	UINT startLineID = GetLine2DCount();

	//.....................
	mFunction_AddLine2D_GlobalVB(v1, v2, color1, color2);
	mFunction_AddLine2D_GlobalVB(v1, v3, color1, color3);
	mFunction_AddLine2D_GlobalVB(v2, v3, color2, color3);


	//information to be pushed into region info list
	UINT endLineID = GetLine2DCount() - 1;

	//write down region info (to show this block of triangles compose of a rectangle)
	N_RegionInfo regionInfo;
	regionInfo.texID = NOISE_MACRO_INVALID_TEXTURE_ID;
	regionInfo.startID = startLineID;
	regionInfo.elememtCount = endLineID - startLineID + 1;
	m_pRegionList_LineID_Triangle->push_back(regionInfo);

	//now it is allowed to update because of modification
	mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D] = TRUE;

	return GetTriangleOutlineCount()-1;
}

UINT NoiseGraphicObject::AddRectangleOutline(NVECTOR2 vTopLeft, NVECTOR2 vBottomRight, NVECTOR4 color)
{
	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(vTopLeft, vTopLeft);
	mFunction_ConvertPixelVec2FloatVec(vBottomRight, vBottomRight);

	//information to be pushed into region info list
	UINT startLineID = GetLine2DCount();

	//.....................
	NVECTOR2 tmpV1, tmpV2;

	tmpV1 = vTopLeft;
	tmpV2 = NVECTOR2(vBottomRight.x, vTopLeft.y);
	mFunction_AddLine2D_GlobalVB(tmpV1, tmpV2, color, color);

	tmpV1 = NVECTOR2(vBottomRight.x, vTopLeft.y);
	tmpV2 = vBottomRight;
	mFunction_AddLine2D_GlobalVB(tmpV1, tmpV2, color, color);

	tmpV1 = vBottomRight;
	tmpV2 = NVECTOR2(vTopLeft.x, vBottomRight.y);
	mFunction_AddLine2D_GlobalVB(tmpV1, tmpV2, color, color);

	tmpV1 = NVECTOR2(vTopLeft.x, vBottomRight.y);
	tmpV2 = vTopLeft;
	mFunction_AddLine2D_GlobalVB(tmpV1, tmpV2, color, color);


	//information to be pushed into region info list
	UINT endLineID = GetLine2DCount() - 1;

	//write down region info (to show this block of triangles compose of a rectangle)
	N_RegionInfo regionInfo;
	regionInfo.texID = NOISE_MACRO_INVALID_TEXTURE_ID;
	regionInfo.startID = startLineID;
	regionInfo.elememtCount = endLineID - startLineID + 1;
	m_pRegionList_LineID_Rect->push_back(regionInfo);

	//now it is allowed to update because of modification
	mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D] = TRUE;

	return GetRectOutlineCount() - 1;
}

UINT NoiseGraphicObject::AddEllipseOutline(float a, float b, NVECTOR2 vCenter, NVECTOR4 color, UINT stepCount)
{
	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(vCenter, vCenter);
	a = mFunction_ConvertPixelLength2FloatLength(a, TRUE);
	b = mFunction_ConvertPixelLength2FloatLength(b, FALSE);

	//information to be pushed into region info list
	UINT startLineID = GetLine2DCount();

	//well, step counts cannot too small
	//stepCount =3 means 
	if (stepCount < 3) stepCount = 3;

	//check "a" (semi-major axis) ,"b" (semi-minor axis)
	if (a < 0) a = 1;
	if (b < 0) b = 1;


	//use X coord to derive Y coord
	float angleStep = MATH_PI / (stepCount - 1);
	float currAngle = 0.0f;
	float nextAngle = currAngle + angleStep;

	//define 2 temporary vertices which are right on the Ellipse (interpolation)
	NVECTOR2 tmpV;
	NVECTOR2 tmpNextV;
	NVECTOR2 tmpTexcoord;
	NVECTOR2 tmpNextTexcoord;

	//loop to generate vertices
	for (UINT i = 0;i < stepCount;i++)
	{

		//this euqation can be derived from ellipse standard equation
		tmpV.x = a * cosf(currAngle);
		tmpV.y = sqrtf((a*a*b*b - b*b *tmpV.x*tmpV.x) / (a*a));
		//map a rectangle on this ellipse
		tmpTexcoord = NVECTOR2((tmpV.x / a + 1) / 2, (-tmpV.y / b + 1) / 2);


		tmpNextV.x = a * cosf(nextAngle);
		tmpNextV.y = sqrtf((a*a*b*b - b*b *tmpNextV.x*tmpNextV.x) / (a*a));
		tmpNextTexcoord = NVECTOR2((tmpNextV.x / a + 1) / 2, (-tmpNextV.y / b + 1) / 2);

		//so we can generate triangles using "radial triangulation" (well , a word created by myself hhhhhh)
		mFunction_AddLine2D_GlobalVB( tmpV + vCenter, tmpNextV + vCenter, color, color);

		//symmetric triangle about X AXIS
		mFunction_AddLine2D_GlobalVB( -tmpV + vCenter, -tmpNextV + vCenter, color, color);

		//update slopes
		currAngle += angleStep;
		nextAngle += angleStep;
	}

	//information to be pushed into region info list
	UINT endLineID = GetLine2DCount() - 1;

	//write down region info (to show this block of triangles compose of a rectangle)
	N_RegionInfo regionInfo;
	regionInfo.texID = NOISE_MACRO_INVALID_TEXTURE_ID;
	regionInfo.startID = startLineID;
	regionInfo.elememtCount = endLineID - startLineID + 1;
	m_pRegionList_LineID_Ellipse->push_back(regionInfo);

	//now it is allowed to update because of modification
	mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D] = TRUE;

	return GetEllipseOutlineCount() - 1;
}

void NoiseGraphicObject::SetLine3D(UINT index, NVECTOR3 v1, NVECTOR3 v2, NVECTOR4 color1, NVECTOR4 color2)
{
	if (index > m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_LINE_3D]->size())
	{
		DEBUG_MSG1("Line 3D index invalid!!");
		return;
	}

	//source vertex array
	N_SimpleVertex simpleV[2];
	simpleV[0].Pos = v1;
	simpleV[0].Color = color1;
	simpleV[0].TexCoord = NVECTOR2(0, 0);
	simpleV[1].Pos = v2;
	simpleV[1].Color = color2;
	simpleV[1].TexCoord = NVECTOR2(0, 0);

	//a clearer index expression
	UINT vertexStartID = index * 2;
	UINT	 vertexCount = 2;

	//transmit an array ,the function do the error check
	mFunction_SetVertices(m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_LINE_3D], simpleV, vertexStartID, vertexCount);

	//now it is allowed to update because of modification
	mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_LINE_3D] = TRUE;
}

void NoiseGraphicObject::SetLine2D(UINT index, NVECTOR2 v1, NVECTOR2 v2, NVECTOR4 color1, NVECTOR4 color2)
{
	if (index >= m_pRegionList_LineID_Common->size())
	{
		DEBUG_MSG1("Line 2D index invalid!!");
		return;
	}

	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(v1, v1);
	mFunction_ConvertPixelVec2FloatVec(v2, v2);

	//vertex id in GLOBAL VB of line2d
	UINT lineID = m_pRegionList_LineID_Common->at(index);

	mFunction_SetLine2D_GlobalVB(lineID, v1, v2, color1, color2);
}

void NoiseGraphicObject::SetPoint3D(UINT index, NVECTOR3 v, NVECTOR4 color)
{
	if (index > m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_POINT_3D]->size())
	{
		DEBUG_MSG1("Point 3D index invalid!!");
		return;
	}

	//source vertex array
	N_SimpleVertex simpleV[1];
	simpleV[0].Pos = v;
	simpleV[0].Color = color;
	simpleV[0].TexCoord = NVECTOR2(0, 0);


	//a clearer index expression
	UINT vertexStartID = index;
	UINT	 vertexCount = 1;

	//transmit an array ,the function do the error check
	mFunction_SetVertices(m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_POINT_3D], simpleV, vertexStartID, vertexCount);

	//now it is allowed to update because of modification
	mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_POINT_3D] = TRUE;
}

void NoiseGraphicObject::SetPoint2D(UINT index, NVECTOR2 v, NVECTOR4 color)
{
	if (index > m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D]->size())
	{
		DEBUG_MSG1("Point 2D index invalid!!");
		return;
	}

	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(v, v);

	//source vertex array
	N_SimpleVertex simpleV[1];
	simpleV[0].Pos = NVECTOR3(v.x, v.y, 0);
	simpleV[0].Color = color;
	simpleV[0].TexCoord = NVECTOR2(0, 0);


	//a clearer index expression
	UINT vertexStartID = index;
	UINT	 vertexCount = 1;

	//transmit an array ,the function do the error check
	mFunction_SetVertices(m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D], simpleV, vertexStartID, vertexCount);
	
	//now it is allowed to update because of modification
	mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D] = TRUE;
}

void NoiseGraphicObject::SetTriangle2D(UINT index, NVECTOR2 v1, NVECTOR2 v2, NVECTOR2 v3, NVECTOR4 color1, NVECTOR4 color2, NVECTOR4 color3)
{
	if (index >= m_pRegionList_TriangleID_Common->size())
	{
		DEBUG_MSG1("Rectangle Index Invalid !!");
		return;
	}

	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(v1, v1);
	mFunction_ConvertPixelVec2FloatVec(v2, v2);
	mFunction_ConvertPixelVec2FloatVec(v3, v3);

	mFunction_SetSolidTriangle2D_GlobalVB(m_pRegionList_TriangleID_Common->at(index), v1, v2, v3, color1, color2, color3,NVECTOR2(0,0),NVECTOR2(0,0), NVECTOR2(0, 0));
}

void NoiseGraphicObject::SetRectangle(UINT index, NVECTOR2 vTopLeft, NVECTOR2 vBottomRight, NVECTOR4 color, UINT texID)
{
	if (index >= m_pRegionList_TriangleID_Rect->size())
	{
		DEBUG_MSG1("Rectangle Index Invalid !!");
		return;
	}

	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(vTopLeft, vTopLeft);
	mFunction_ConvertPixelVec2FloatVec(vBottomRight, vBottomRight);

	UINT triangleID1 = m_pRegionList_TriangleID_Rect->at(index).startID;
	UINT triangleID2 = triangleID1 + 1;

	//2 triangles for 1 rect
	mFunction_SetSolidTriangle2D_GlobalVB(
		triangleID1,
		vTopLeft,
		NVECTOR2(vBottomRight.x, vTopLeft.y),
		NVECTOR2(vTopLeft.x, vBottomRight.y),
		color,
		color,
		color,
		NVECTOR2(0, 0),
		NVECTOR2(1, 0),
		NVECTOR2(0, 1)
		);

	mFunction_SetSolidTriangle2D_GlobalVB(
		triangleID2,
		NVECTOR2(vBottomRight.x, vTopLeft.y),
		vBottomRight,
		NVECTOR2(vTopLeft.x, vBottomRight.y),
		color,
		color,
		color,
		NVECTOR2(1, 0),
		NVECTOR2(1, 1),
		NVECTOR2(0, 1)
		);

	//modify texID of REGION_INFO 
	m_pRegionList_TriangleID_Rect->at(index).texID = texID;

	//now it is allowed to update because of modification
	mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D] = TRUE;
}

void NoiseGraphicObject::SetRectangle(UINT index, NVECTOR2 vCenter, float fWidth, float fHeight, NVECTOR4 color, UINT texID)
{
	//dont use coord conversion here , because in the other overload , conversion will be applied
	SetRectangle(index,vCenter - NVECTOR2(fWidth/2,fHeight/2),vCenter+ NVECTOR2(fWidth / 2, fHeight / 2),color,texID);
}

void NoiseGraphicObject::SetEllipse(UINT index, float a, float b,NVECTOR2 vCenter, NVECTOR4 color, UINT texID)
{
	if (index >= m_pRegionList_TriangleID_Ellipse->size())
	{
		DEBUG_MSG1("SetEllipse: index invalid");
		return;
	}


	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(vCenter, vCenter);
	a = mFunction_ConvertPixelLength2FloatLength(a, TRUE);
	b = mFunction_ConvertPixelLength2FloatLength(b, FALSE);

	//define an array to use mFunction_SetVertices
	UINT triangleCount = m_pRegionList_TriangleID_Ellipse->at(index).elememtCount;

	//......
	UINT	 stepCount = triangleCount / 2 ;

	UINT startTriangleID = m_pRegionList_TriangleID_Ellipse->at(index).startID;

	//check "a" (semi-major axis) ,"b" (semi-minor axis)
	if (a < 0) a = 0.5f;
	if (b < 0) b = 0.5f;

	//use X coord to derive Y coord
	float angleStep = MATH_PI / (stepCount-1);
	float currAngle = 0.0f;
	float nextAngle = currAngle + angleStep;

	//define 2 temporary vertices which are right on the Ellipse (interpolation)
	NVECTOR2 tmpV;
	NVECTOR2 tmpNextV;
	NVECTOR2 tmpTexcoord;
	NVECTOR2 tmpNextTexcoord;


	//loop to generate vertices
	for (UINT i = 0;i < stepCount;i++)
	{

		//this euqation can be derived from ellipse standard equation
		tmpV.x = a * cosf(currAngle);
		tmpV.y = sqrtf((a*a*b*b - b*b *tmpV.x*tmpV.x) / (a*a));
		//map a rectangle on this ellipse
		tmpTexcoord = NVECTOR2((tmpV.x / a + 1) / 2, (-tmpV.y / b + 1) / 2);


		tmpNextV.x = a * cosf(nextAngle);
		tmpNextV.y = sqrtf((a*a*b*b - b*b *tmpNextV.x*tmpNextV.x) / (a*a));
		tmpNextTexcoord = NVECTOR2((tmpNextV.x / a + 1) / 2, (-tmpNextV.y / b + 1) / 2);

		//so we can generate triangles using "radial triangulation" (well , a word created by myself hhhhhh)

		mFunction_SetSolidTriangle2D_GlobalVB(startTriangleID + i * 2 + 1, vCenter, -tmpV + vCenter, -tmpNextV + vCenter, color, color, color,
			NVECTOR2(0.5, 0.5),
			tmpTexcoord,
			tmpNextTexcoord
			);


		//SET TRIANGLES IN GLOBAL VERTEX BUFFER 
		mFunction_SetSolidTriangle2D_GlobalVB(startTriangleID + i * 2, vCenter, tmpV + vCenter, tmpNextV + vCenter,
			color, color, color,
			NVECTOR2(0.5, 0.5),
			NVECTOR2(1 - tmpTexcoord.x, 1 - tmpTexcoord.y),
			NVECTOR2(1 - tmpNextTexcoord.x, 1 - tmpNextTexcoord.y)
			);

		//update slopes
		currAngle += angleStep;
		nextAngle += angleStep;

	}

	//modify texID of REGION_INFO 
	m_pRegionList_TriangleID_Ellipse->at(index).texID = texID;

	//now it is allowed to update because of modification
	mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D] = TRUE;
}

void NoiseGraphicObject::SetTriangleOutline(UINT index, NVECTOR2 v1, NVECTOR2 v2, NVECTOR2 v3, NVECTOR4 color1, NVECTOR4 color2, NVECTOR4 color3)
{
	if (index >= m_pRegionList_LineID_Triangle->size())
	{
		DEBUG_MSG1("Triangle Outline ID invalid!!");
		return;
	}

	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(v1, v1);
	mFunction_ConvertPixelVec2FloatVec(v2, v2);
	mFunction_ConvertPixelVec2FloatVec(v3, v3);


	UINT firstLineIndex = m_pRegionList_LineID_Triangle->at(index).startID;

	//.....................
	mFunction_SetLine2D_GlobalVB(firstLineIndex, v1, v2, color1, color2);

	mFunction_SetLine2D_GlobalVB(firstLineIndex+1, v2, v3, color2, color3);

	mFunction_SetLine2D_GlobalVB(firstLineIndex+2, v3, v1, color3, color1);

}

void NoiseGraphicObject::SetRectangleOutline(UINT index, NVECTOR2 vTopLeft, NVECTOR2 vBottomRight, NVECTOR4 color)
{
	if (index >= m_pRegionList_LineID_Rect->size())
	{
		DEBUG_MSG1("Triangle Outline ID invalid!!");
		return;
	}

	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(vTopLeft, vTopLeft);
	mFunction_ConvertPixelVec2FloatVec(vBottomRight, vBottomRight);

	//............
	UINT firstLineIndex = m_pRegionList_LineID_Rect->at(index).startID;

	//.....................
	NVECTOR2 v1, v2;

	v1 = vTopLeft;
	v2 = NVECTOR2(vBottomRight.x, vTopLeft.y);
	mFunction_SetLine2D_GlobalVB(firstLineIndex, v1, v2, color, color);

	v1 = NVECTOR2(vBottomRight.x, vTopLeft.y);
	v2 = vBottomRight;
	mFunction_SetLine2D_GlobalVB(firstLineIndex + 1, v1, v2, color, color);

	v1 = vBottomRight;
	v2 = NVECTOR2(vTopLeft.x, vBottomRight.y);
	mFunction_SetLine2D_GlobalVB(firstLineIndex + 2, v1, v2, color, color);

	v1 = NVECTOR2(vTopLeft.x, vBottomRight.y);
	v2 = vTopLeft;
	mFunction_SetLine2D_GlobalVB(firstLineIndex + 3, v1, v2, color, color);
}

void NoiseGraphicObject::SetRectangleOutline(UINT index, NVECTOR2 vCenter, float fWidth, float fHeight, NVECTOR4 color)
{
	//no need for unit conversion
	SetRectangleOutline(index, vCenter - NVECTOR2(fWidth / 2, fHeight / 2), vCenter + NVECTOR2(fWidth / 2, fHeight / 2), color);

}

void NoiseGraphicObject::SetEllipseOutline(UINT index, float a, float b, NVECTOR2 vCenter, NVECTOR4 color)
{
	if (index >= m_pRegionList_LineID_Ellipse->size())
	{
		DEBUG_MSG1("SetEllipseOutine : index invalid");
		return;
	}

	//coord unit conversion
	mFunction_ConvertPixelVec2FloatVec(vCenter, vCenter);
	a = mFunction_ConvertPixelLength2FloatLength(a, TRUE);
	b = mFunction_ConvertPixelLength2FloatLength(b, FALSE);


	//define an array to use mFunction_SetVertices
	UINT lineCount = m_pRegionList_LineID_Ellipse->at(index).elememtCount;

	//about why stepCount is divided by 2 ,see AddEllipse
	UINT	 stepCount = lineCount / 2;

	UINT startLineID = m_pRegionList_LineID_Ellipse->at(index).startID;

	//check "a" (semi-major axis) ,"b" (semi-minor axis)
	if (a < 0) a = 1;
	if (b < 0) b = 1;

	//use X coord to derive Y coord
	float angleStep = MATH_PI / (stepCount - 1);
	float currAngle = 0.0f;
	float nextAngle = currAngle + angleStep;

	//define 2 temporary vertices which are right on the Ellipse (interpolation)
	NVECTOR2 tmpV;
	NVECTOR2 tmpNextV;

	//loop to generate vertices
	for (UINT i = 0;i < stepCount;i++)
	{

		//this euqation can be derived from ellipse standard equation
		tmpV.x = a * cosf(currAngle);
		tmpV.y = sqrtf((a*a*b*b - b*b *tmpV.x*tmpV.x) / (a*a));

		tmpNextV.x = a * cosf(nextAngle);
		tmpNextV.y = sqrtf((a*a*b*b - b*b *tmpNextV.x*tmpNextV.x) / (a*a));

		//so we can generate triangles using "radial triangulation" (well , a word created by myself hhhhhh)
		mFunction_SetLine2D_GlobalVB(startLineID + i * 2, tmpV + vCenter, tmpNextV + vCenter,color, color);

		//symmetric triangle about X AXIS
		mFunction_SetLine2D_GlobalVB(startLineID + i * 2 + 1, -tmpV + vCenter, -tmpNextV + vCenter, color, color);

		//update slopes
		currAngle += angleStep;
		nextAngle += angleStep;

	}

	//modify texID of REGION_INFO 
	m_pRegionList_LineID_Ellipse->at(index).texID = NOISE_MACRO_INVALID_TEXTURE_ID;

};


void NoiseGraphicObject::DeleteLine3D(UINT index)
{
	//to explain codes in a  clearer way
	UINT vertexStartIndex = index * 2;
	UINT vertexCount = 2;	//1 line consist of 2 vertices

	//erase function
	mFunction_EraseVertices(m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_LINE_3D], vertexStartIndex, vertexCount);
	
	//now it is allowed to update because of modification
	mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_LINE_3D] = TRUE;
}

void NoiseGraphicObject::DeleteLine2D(UINT index)
{
	if (index < m_pRegionList_LineID_Common->size())
	{

		//to explain codes in a  clearer way
		UINT vertexStartIndex = m_pRegionList_LineID_Common->at(index) * 2;
		UINT vertexCount = 2;	//1 line consist of 2 vertices


		//erase function
		mFunction_EraseVertices(m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D], vertexStartIndex, vertexCount);

		//now it is allowed to update because of modification
		mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D] = TRUE;
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
	mFunction_EraseVertices(m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_POINT_3D], vertexStartIndex, vertexCount);

	//now it is allowed to update because of modification
	mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_POINT_3D] = TRUE;
}

void NoiseGraphicObject::DeletePoint2D(UINT index)
{
	//to explain codes in a  clearer way
	UINT vertexStartIndex = index;
	UINT vertexCount = 1;	//1 line consist of 2 vertices

	//erase function
	mFunction_EraseVertices(m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D], vertexStartIndex, vertexCount);

	//now it is allowed to update because of modification
	mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D] = TRUE;
}

void NoiseGraphicObject::DeleteTriangle2D(UINT index)
{
	//delete the index_th single Triangle
	UINT vertexStartIndex = 0;
	UINT	 vertexCount = 0;

	if (index < m_pRegionList_TriangleID_Common->size())
	{
		//sum up triangle counts , because the step count of Ellipse might not be the same
		vertexStartIndex = m_pRegionList_TriangleID_Common->at(index) * 3;
		vertexCount = 3;

		//erase from vector
		mFunction_EraseVertices(m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D], vertexStartIndex, vertexCount);

		//now it is allowed to update because of modification
		mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D] = TRUE;

		//also remember to delete corresponding region info
		auto iter_thisRegionInfo = m_pRegionList_TriangleID_Common->begin();
		iter_thisRegionInfo += index;
		m_pRegionList_TriangleID_Common->erase(iter_thisRegionInfo);

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

	if (index < m_pRegionList_TriangleID_Rect->size())
	{
		vertexStartIndex = m_pRegionList_TriangleID_Rect->at(index).startID * 3;
		vertexCount = m_pRegionList_TriangleID_Rect->at(index).elememtCount*3;
		//erase from vector
		mFunction_EraseVertices(m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D], vertexStartIndex, vertexCount);
		
		//now it is allowed to update because of modification
		mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D] = TRUE;

		//also remember to delete corresponding region info
		auto iter_thisRegionInfo = m_pRegionList_TriangleID_Rect->begin();
		iter_thisRegionInfo += index;
		m_pRegionList_TriangleID_Rect->erase(iter_thisRegionInfo);
	
	}
	else
	{
		DEBUG_MSG1("delete rectangle : Rectangle Index Invalid!!");
		return;
	}

}

void NoiseGraphicObject::DeleteEllipse(UINT index)
{
	//delete the index_th Ellipse

	UINT vertexStartIndex = 0;
	UINT	 vertexCount = 0;

	if (index < m_pRegionList_TriangleID_Ellipse->size())
	{
		//sum up triangle counts , because the step count of Ellipse might not be the same
		vertexStartIndex = m_pRegionList_TriangleID_Ellipse->at(index).startID * 3;
		vertexCount = m_pRegionList_TriangleID_Ellipse->at(index).elememtCount * 3;

		//erase from vector
		mFunction_EraseVertices(m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D], vertexStartIndex, vertexCount);

		//now it is allowed to update because of modification
		mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D] = TRUE;

		//also remember to delete corresponding region info
		auto iter_thisRegionInfo = m_pRegionList_TriangleID_Ellipse->begin();
		iter_thisRegionInfo += index;
		m_pRegionList_TriangleID_Ellipse->erase(iter_thisRegionInfo);

	}
	else
	{
		DEBUG_MSG1("delete Ellipse : Ellipse Index Invalid!!");
		return;
	}
}

void NoiseGraphicObject::DeleteTriangleOutline(UINT index)
{
	//delete the index_th single Triangle
	UINT vertexStartIndex = 0;
	UINT	 vertexCount = 0;

	if (index < m_pRegionList_LineID_Triangle->size())
	{
		//
		vertexStartIndex = m_pRegionList_LineID_Triangle->at(index).startID * 2;

		//3 lines  ,  6 vertices
		vertexCount = 6;

		//erase from vector
		mFunction_EraseVertices(m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D], vertexStartIndex, vertexCount);

		//now it is allowed to update because of modification
		mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D] = TRUE;

		//also remember to delete corresponding region info
		auto iter_thisRegionInfo = m_pRegionList_LineID_Triangle->begin();
		iter_thisRegionInfo += index;
		m_pRegionList_LineID_Triangle->erase(iter_thisRegionInfo);

	}
	else
	{
		DEBUG_MSG1("delete triangle Outline : Triangle Index Invalid!!");
		return;
	}
}

void NoiseGraphicObject::DeleteRectOutline(UINT index)
{
	//delete the index_th single Triangle
	UINT vertexStartIndex = 0;
	UINT	 vertexCount = 0;

	if (index < m_pRegionList_LineID_Rect->size())
	{
		//
		vertexStartIndex = m_pRegionList_LineID_Rect->at(index).startID * 2;

		//3 lines  ,  6 vertices
		vertexCount =8;

		//erase from vector
		mFunction_EraseVertices(m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D], vertexStartIndex, vertexCount);

		//now it is allowed to update because of modification
		mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D] = TRUE;

		//also remember to delete corresponding region info
		auto iter_thisRegionInfo = m_pRegionList_LineID_Rect->begin();
		iter_thisRegionInfo += index;
		m_pRegionList_LineID_Rect->erase(iter_thisRegionInfo);

	}
	else
	{
		DEBUG_MSG1("delete rectangle Outline : Triangle Index Invalid!!");
		return;
	}
}

void NoiseGraphicObject::DeleteEllipseOutline(UINT index)
{
	//delete the index_th single Triangle
	UINT vertexStartIndex = 0;
	UINT	 vertexCount = 0;

	if (index < m_pRegionList_LineID_Ellipse->size())
	{
		//
		vertexStartIndex = m_pRegionList_LineID_Ellipse->at(index).startID * 2;

		//n lines ,2n vertices
		vertexCount = m_pRegionList_LineID_Ellipse->at(index).elememtCount*2;

		//erase from vector
		mFunction_EraseVertices(m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D], vertexStartIndex, vertexCount);

		//now it is allowed to update because of modification
		mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D] = TRUE;

		//also remember to delete corresponding region info
		auto iter_thisRegionInfo = m_pRegionList_LineID_Ellipse->begin();
		iter_thisRegionInfo += index;
		m_pRegionList_LineID_Ellipse->erase(iter_thisRegionInfo);
	}
	else
	{
		DEBUG_MSG1("delete rectangle Outline : Triangle Index Invalid!!");
		return;
	}

};


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
	return m_pRegionList_TriangleID_Rect->size();
}

UINT NoiseGraphicObject::GetEllipseCount()
{
	return m_pRegionList_TriangleID_Ellipse->size();
}

UINT NoiseGraphicObject::GetTriangleOutlineCount()
{
	return m_pRegionList_LineID_Triangle->size();
};
	
UINT NoiseGraphicObject::GetRectOutlineCount()
{
	return m_pRegionList_LineID_Rect->size();
};

UINT NoiseGraphicObject::GetEllipseOutlineCount()
{
	return m_pRegionList_LineID_Ellipse->size();
};

BOOL NoiseGraphicObject::AddToRenderList()
{
	
	if (m_pFatherScene == NULL)
	{
		DEBUG_MSG1("Noise Graphic Object: NoiseScene Has Not been created!");
		return FALSE;
	}
	m_pFatherScene->m_pChildRenderer->m_pRenderList_GraphicObject->push_back(this);

	//Update Data to GPU if data is not up to date , 5 object types for now
	for (UINT i = 0;i < 5;i++)
	{
		if (mCanUpdateToGpu[i])
		{
			mFunction_UpdateVerticesToGpu(i);
			mCanUpdateToGpu[i] = FALSE;
		}
	}
	return TRUE;
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
	tmpInitData.pSysMem = &m_pVB_Mem[objType_ID]->at(0);
	tmpInitData.SysMemPitch = 0;
	tmpInitData.SysMemSlicePitch = 0;

	//update the byte size of GPU vertex buffer
	mVB_ByteSize_GPU[objType_ID] = vbd.ByteWidth;

	//Create Buffers
	HRESULT hr = 0;
	hr = g_pd3dDevice11->CreateBuffer(&vbd, &tmpInitData, &m_pVB_GPU[objType_ID]);
	HR_DEBUG(hr, "VERTEX BUFFER创建失败");

	return TRUE;
}

void		NoiseGraphicObject::mFunction_UpdateVerticesToGpu(UINT objType_ID)
{
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

void		NoiseGraphicObject::mFunction_EraseVertices(std::vector<N_SimpleVertex>* pList, UINT iVertexStartID,UINT iVertexCount)
{
	//define iterators
	std::vector<N_SimpleVertex>::iterator iter_Start;
	std::vector<N_SimpleVertex>::iterator iter_End;

	if (pList)
	{
		//initialize iterators
		iter_Start = pList->begin();
		iter_End = pList->begin();
	}
	else
	{
		DEBUG_MSG1("Noise Graphic Object : vector ptr 'pList' invalid ");
		return;
	}


	//check boundary (check the tail ,if the tail is within boundary , then it's valid
	if ((iVertexStartID + iVertexCount) >= pList->size())
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

}

void		NoiseGraphicObject::mFunction_SetVertices(std::vector<N_SimpleVertex>* pList, N_SimpleVertex* pSourceArray, UINT iVertexStartID, UINT iVertexCount)
{
	//validate pList
	if (!pList)
	{
		DEBUG_MSG1("Noise Graphic Object : vector ptr 'pList' invalid ");
		return;
	}

	//check boundary (check the tail ,if the tail is within boundary , then it's valid
	if (iVertexStartID + iVertexCount > pList->size())
	{
		DEBUG_MSG1("Noise Graphic Object : Vertex ID Out of boundary!");
		return;
	}
	
	//value assignment
	for (UINT i = 0;i < iVertexCount;i++)
	{
		pList->at(iVertexStartID + i) = pSourceArray[i];
	}

}

UINT		NoiseGraphicObject::mFunction_AddSolidTriangle2D_GlobalVB(NVECTOR2 v1, NVECTOR2 v2, NVECTOR2 v3, NVECTOR4 c1, NVECTOR4 c2, NVECTOR4 c3, NVECTOR2 texcoord1, NVECTOR2 texcoord2, NVECTOR2 texcoord3,BOOL isCommonTriangle)
{
	//I Create this private function to cover the awkward  isCommonTriangle
	//because updating RegionInfo need to know whether AddTriangle2D is invoked by 
	//AddRect / AddEllipse

	//new Common triangle ID
	UINT newTriangleID = m_pRegionList_TriangleID_Common->size();

	//.....................
	N_SimpleVertex tmpVertex;
	tmpVertex.Pos = NVECTOR3(v1.x, v1.y, 0);
	tmpVertex.Color = c1;
	tmpVertex.TexCoord = texcoord1;
	m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D]->push_back(tmpVertex);

	tmpVertex.Pos = NVECTOR3(v2.x, v2.y, 0);
	tmpVertex.Color = c2;
	tmpVertex.TexCoord = texcoord2;
	m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D]->push_back(tmpVertex);

	tmpVertex.Pos = NVECTOR3(v3.x, v3.y, 0);
	tmpVertex.Color = c3;
	tmpVertex.TexCoord = texcoord3;
	m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D]->push_back(tmpVertex);

	//update common triangle List , just record Triangle ID of this single triangle
	//if : (this triangle is not triangle in Complex Shape ,just a single one)
	if (isCommonTriangle)
	{
		m_pRegionList_TriangleID_Common->push_back(newTriangleID);
	}

	//now it is allowed to update because of modification
	mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D] = TRUE;

	return newTriangleID;
}

void		NoiseGraphicObject::mFunction_SetSolidTriangle2D_GlobalVB(UINT index,NVECTOR2 v1, NVECTOR2 v2, NVECTOR2 v3, NVECTOR4 c1, NVECTOR4 c2, NVECTOR4 c3, NVECTOR2 texcoord1, NVECTOR2 texcoord2, NVECTOR2 texcoord3)
{
	N_SimpleVertex simpleV[3];
	simpleV[0].Pos = NVECTOR3(v1.x, v1.y, 0);
	simpleV[0].Color = c1;
	simpleV[0].TexCoord =texcoord1;
	simpleV[1].Pos = NVECTOR3(v2.x, v2.y, 0);
	simpleV[1].Color = c2;
	simpleV[1].TexCoord = texcoord2;
	simpleV[2].Pos = NVECTOR3(v3.x, v3.y, 0);
	simpleV[2].Color = c3;
	simpleV[2].TexCoord = texcoord3;


	//a clearer index expression
	UINT vertexStartID = index * 3;
	UINT	 vertexCount = 3;

	//transmit an array ,the function do the error check
	mFunction_SetVertices(m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D], simpleV, vertexStartID, vertexCount);

	//now it is allowed to update because of modification
	mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D] = TRUE;;
}

UINT		NoiseGraphicObject::mFunction_AddLine2D_GlobalVB(NVECTOR2 v1, NVECTOR2 v2, NVECTOR4 color1, NVECTOR4 color2)
{
	//.....................
	N_SimpleVertex tmpVertex;
	tmpVertex.Pos = NVECTOR3(v1.x, v1.y, 0);
	tmpVertex.Color = color1;
	m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D]->push_back(tmpVertex);

	tmpVertex.Pos = NVECTOR3(v2.x, v2.y, 0);
	tmpVertex.Color = color2;
	m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D]->push_back(tmpVertex);

	//now it is allowed to update because of modification
	mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D] = TRUE;
	return GetLine2DCount() - 1;
}

void		NoiseGraphicObject::mFunction_SetLine2D_GlobalVB(UINT index, NVECTOR2 v1, NVECTOR2 v2, NVECTOR4 color1, NVECTOR4 color2)
{
	//source vertex array
	N_SimpleVertex simpleV[2];
	simpleV[0].Pos = NVECTOR3(v1.x, v1.y, 0);
	simpleV[0].Color = color1;
	simpleV[0].TexCoord = NVECTOR2(0, 0);
	simpleV[1].Pos = NVECTOR3(v2.x, v2.y, 0);
	simpleV[1].Color = color2;
	simpleV[1].TexCoord = NVECTOR2(0, 0);

	//a clearer index expression
	UINT vertexStartID = index * 2;
	UINT	 vertexCount = 2;

	//transmit an array ,the function do the error check
	mFunction_SetVertices(m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D], simpleV, vertexStartID, vertexCount);

	//now it is allowed to update because of modification
	mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D] = TRUE;
}

void		NoiseGraphicObject::mFunction_ConvertPixelVec2FloatVec(NVECTOR2 pxCoord,NVECTOR2& outVec2)
{
	//first ,  get the pixel size of main back buffer (global var)
	float halfW= (float)gMainBufferPixelWidth / 2.0f;
	float halfH = (float)gMainBufferPixelHeight / 2.0f;
	outVec2 = NVECTOR2((pxCoord.x /halfW)-1,		1-(pxCoord.y / halfH));
}

float		NoiseGraphicObject::mFunction_ConvertPixelLength2FloatLength(float pxLen, BOOL isWidth)
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

void		NoiseGraphicObject::mFunction_GenerateTriangleDrawCallList()
{
}
