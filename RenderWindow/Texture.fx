/**********************************************************************

File:Texture.fx
Author: Jige

**************************************************************************/
#ifndef TEXTURE_H
#define TEXTURE_H


Texture2D gDiffuseMap;
Texture2D gNormalMap;
Texture2D gSpecularMap;

SamplerState sampler_ANISOTROPIC
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AdressU = Wrap;
	AddressV = Wrap;
	AddressW = Wrap;
};

#endif

