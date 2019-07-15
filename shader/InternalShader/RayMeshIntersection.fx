
GS_INPUT_DEFAULT_MESH VS_RayMeshIntersection(VS_INPUT_DRAW_MESH input)
{
	//initialize
	GS_INPUT_DEFAULT_MESH outVert;
	//W transformation. intersection will be done in world space
	outVert.posW = mul(float4(input.posL, 1.0f), gWorldMatrix);
	outVert.normalW = mul(float4(input.normalL, 0.0f), gWorldInvTransposeMatrix).xyz;
	outVert.texcoord =  input.texcoord;

	return outVert;
}


[maxvertexcount(3)]
void GS_RayMeshIntersection(triangle GS_INPUT_DEFAULT_MESH  inputGeometry[3], inout PointStream<GS_OUTPUT_DEFAULT_MESH> pointStream)
{
	float u = 0.0f, v = 0.0f, t = 0.0f;

	bool isIntersected = IntersectTriangle(
		gIntersectingRayOrigin,
		gIntersectingRayDir,
		inputGeometry[0].posW.xyz,
		inputGeometry[1].posW.xyz,
		inputGeometry[2].posW.xyz,
		t, u, v);

	//if ray intersects triangle, compute the exact coordinate of that point according to t,u,v
	if (isIntersected)
	{
		GS_OUTPUT_DEFAULT_MESH outVert;

		//intersected point is in world space
		float3 intersectPointW = t * gIntersectingRayDir.xyz;

		//outVert.pos = intersectPointW;
		//outVert.pos = inputGeometry[1].posW.xyz;
		outVert.pos = float4(intersectPointW.xyz, t);
		//outVert.t = t;
		outVert.normal = (1 - u - v)*inputGeometry[0].normalW + u * inputGeometry[1].normalW + v*inputGeometry[2].normalW;
		outVert.texcoord = (1 - u - v)*inputGeometry[0].texcoord + u * inputGeometry[1].texcoord + v*inputGeometry[2].texcoord;
		pointStream.Append(outVert);

	}
}

//ray-triangle intersection
bool IntersectTriangle(float3 origin, float3 dir, float3 v0, float3 v1, float3 v2, out float t, out float u, out float v)
{
	//[Reference for the implementation]
	//Tomas M?ller, Trumbore B . Fast, Minimum Storage Ray-Triangle Intersection[J]. 2005.
	//the method can be describe as follow:
	//**Triangle(V0, V1, V2)
	//**Ray p=O+tD (O for origin, D for direction)
	//**Point in Triangle R(u,v) = (1-u-v)V0 + uV1 + vV2
	//then use CRAMER's RULE to solve the linear equation system for the unknown (t,u,v):
	//		O+tD = (1-u-v)V0 + uV1 + vV2		******(u >0, v>0, u+v<=1)
	// for more info, plz refer to document [Notes]Intersection between Ray and Shapes.docx
	//the code here won't be intuitive, because it's mainly algebraic inductions.

	t = u = v = 0.0f;
	//origin 	--- start of picking ray (world)
	//dir 		--- direction of picking ray (world)
	//v0,v1,v2	--- 3 vertices of triangle
	//t 			--- the vector ratio of intersection point
	// u,v 		--- ratio coordinate in a triangle on the triangle base

	float3 E1 = v1 - v0;
	float3 E2 = v2 - v0;
	float3 P = cross(dir, E2);
	float det = dot(E1, P);

	//in this case float EPSILON = 0.001f , preventing from floating point error
	if (det<0.001f && det>-0.001f)return false;
	float invDet = 1.0f / det;

	//evaluate u
	float3 M = origin - v0;
	u = dot(M, P)*invDet;

	//return as early as possible if even u is un-qualified
	if (u<0.0f || u>1.0f)return false;
	float3 Q = cross(M, E1);
	//evaluate v
	v = dot(dir, Q)*invDet;

	if (v < 0.0f || (u + v > 1.0f))return false;
	t = invDet * dot(E2, Q);

	return true;
}

/*bool IntersectTriangle(float3 origin, float3 dir, float3 v0, float3 v1, float3 v2, out float t, out float u, out float v)
{
t = u = v = 0.0f;
//origin 	--- start of picking ray (world)
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
}*/
