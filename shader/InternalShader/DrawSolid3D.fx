/******************************************

	Module: Draw Solid 3D(impl)
	Author: Jige
	
	draw solid 3D mesh 
	(pure color, simplified vertex)

******************************************/

//-----------------------------------Draw 3D Lines/Points------------------------------
VS_OUTPUT_SIMPLE VS_Solid3D(VS_INPUT_SIMPLE input)
{
	VS_OUTPUT_SIMPLE output = (VS_OUTPUT_SIMPLE)0;
	output.posH = mul(mul(float4(input.posL, 1.0f), gViewMatrix), gProjMatrix);
	output.color = input.color;
	output.texcoord = input.texcoord;
	return output;
}

PS_OUTPUT_SIMPLE PS_Solid3D(VS_OUTPUT_SIMPLE input)
{
	PS_OUTPUT_SIMPLE output;
	output.color = input.color;
	return output;
}
