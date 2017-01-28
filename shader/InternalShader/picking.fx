/**********************************************************************

								File:Picking.fx
	implement of PICKING RAY-TRIANGLE MESH INTERSECTION

**************************************************************************/

cbuffer cbPicking
{
	float2 gPickingRayNormalizedDirXY;/*(x,y,1) for the picking ray , and this is the xy*/
	float2 gPadPicking123;
}


//----------------------PICKING : VS + GS-----------------------

float4 VS_Picking(VS_INPUT_DEFAULT input):SV_POSITION//SV_POSITION is semantic
{
	//initialize
	float4 outputPosV = float4(0, 0, 0, 1.0f);
	//World/View transformation
	outputPosV = mul(mul(float4(input.posL, 1.0f), gWorldMatrix),gViewMatrix);

	return outputPosV;
}

bool IntersectTriangle(float3 origin, float3 dir, float3 v0, float3 v1, float3 v2, out float t, out float u, out float v)
{
	t = u = v = 0.0f;
	//origin 		--- start of picking ray (world)
	//dir 		--- direction of picking ray (world)
	//v0,v1,v2	--- 3 vertices of triangle
	//t 			--- the vector ratio of intersection point
	// u,v 		--- ratio coordinate in a triangle on the triangle base

	float3 edge1 = v1 - v0;
	float3 edge2 = v2 - v0;
	float3 qvec = cross(dir, edge2);
	float det = dot(edge1, qvec);

	//in this case float EPSILON = 0.001f , preventing from floating point error
	if (det<0.001f && det>-0.001f)return false;
	float invdet = 1.0f / det;

	//evaluate u
	float3 tvec = origin - v0;
	u = dot(tvec, qvec);
	u = u*invdet;

	//return as early as possible if even u is un-qualified
	if (u<0.0f || u>1.0f)return false;
	float3 pvec = cross(tvec, edge1);

	//evaluate v
	v = dot(dir, pvec);
	v = v*invdet;

	if (v < 0.0f || (u + v > 1.0f))return false;
	float3 nvec = cross(edge1, edge2);

	//evaluate t (ratio of intersect point on the vector)
	t = dot(tvec, nvec);
	t = t*invdet;

	return true;
}


[maxvertexcount(3)]
void GS_Picking(triangle GS_INPUT_MINIZED  inputGeometry[3], inout PointStream<GS_OUTPUT_MINIZED> pointStream )
{
	//Intersection is performed in VIEW SPACE

	float3 rayDir = float3(gPickingRayNormalizedDirXY.xy, 1.0f);

	float u = 0.0f, v = 0.0f, t = 0.0f;

	bool isIntersected = IntersectTriangle( float3(0, 0, 0), rayDir, inputGeometry[0].pos.xyz, inputGeometry[1].pos.xyz,inputGeometry[2].pos.xyz, t, u, v);

	//if ray intersects triangle, compute the exact coordinate of that point according to t,u,v
	if (isIntersected)
	{
		//point in  view space
		float4 intersectPointV = float4(t * rayDir.xyz,1.0f);

		//transform back to world space
		float4 intersectPointW = mul(intersectPointV, gInvViewMatrix);

		GS_OUTPUT_MINIZED outVert;
		outVert.pos = intersectPointW;

		pointStream.Append(outVert);
	}
}