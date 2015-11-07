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



Texture2D g2D_DiffuseMap;