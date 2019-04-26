/******************************************

	Module: picking(impl)
	Author: Jige
	
	picking ray & triangle intersection
	(GS involved, accurate)

******************************************/

//----------------------PICKING : VS + GS-----------------------
GS_INPUT_MINIMIZED VS_Picking(VS_INPUT_DRAW_MESH input)//SV_POSITION is semantic
{
	//initialize
	GS_INPUT_MINIMIZED outputPosV;
	//W/V transformation
	outputPosV.pos = mul(mul(float4(input.posL, 1.0f), gWorldMatrix),gViewMatrix);

	return outputPosV;
}


[maxvertexcount(3)]
void GS_Picking(triangle GS_INPUT_MINIMIZED  inputGeometry[3], inout PointStream<GS_OUTPUT_MINIMIZED> pointStream )
{
	//tan(FovY) & tan(FovX) can be acquired from certain elements of projection matrix
	float tanHalfFovX = gProjMatrix[0][0];// 1 / (aspectRatio * tan(FovY/2))
	float tanHalfFovY = gProjMatrix[1][1];// 1 / tan(FovY/2)
	float4 rayDir = float4(gPickingRayNormalizedDirXY.x /tanHalfFovX , gPickingRayNormalizedDirXY.y / tanHalfFovY, 1.0f,1.0f);

	float u = 0.0f, v = 0.0f, t = 0.0f;

	bool isIntersected = IntersectTriangle( float3(0,0,0), rayDir.xyz, inputGeometry[0].pos.xyz, inputGeometry[1].pos.xyz,inputGeometry[2].pos.xyz, t, u, v);

	//if ray intersects triangle, compute the exact coordinate of that point according to t,u,v
	if (isIntersected)
	//if(true)
	{
		GS_OUTPUT_MINIMIZED outVert;

		//point in  view space
		float4 intersectPointV = float4(t * rayDir.xyz ,1.0f);

		//transform back to world space
		float3 intersectPointW = mul(intersectPointV, gViewInvMatrix).xyz;

		outVert.pos = intersectPointW;
		pointStream.Append(outVert);

	}
}

