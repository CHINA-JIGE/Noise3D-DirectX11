/**********************************************************************

	File:shading2D.fx
	Author: Jige

**************************************************************************/
SamplerState sampler2D_ANISOTROPIC
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 2;
	AddressU = Wrap;
	AddressV = Wrap;
	AddressW = Wrap;
};

cbuffer cbDrawText2D
{
	float4 	g2D_TextColor;
	float4	g2D_TextGlowColor;
}

Texture2D g2D_DiffuseMap;